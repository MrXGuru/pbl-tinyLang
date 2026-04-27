#include "hotspot.h"
#include <string.h>
#include <stdio.h>

static HotspotResult *g_hr;

/* ── Helper: expression to string ──────────── */
static void expr_to_str(const ASTNode *node, char *out, int outlen) {
    if (!node || outlen <= 0) return;
    if (strcmp(node->type, "Literal")    == 0 ||
        strcmp(node->type, "Identifier") == 0) {
        strncpy(out, node->value, (size_t)outlen - 1);
        out[outlen - 1] = '\0';
        return;
    }
    if (strcmp(node->type, "BinaryOp") == 0 && node->child_count >= 2) {
        char left[128] = "", right[128] = "";
        expr_to_str(node->children[0], left,  sizeof(left));
        expr_to_str(node->children[1], right, sizeof(right));
        snprintf(out, (size_t)outlen, "%s %s %s", left, node->value, right);
        return;
    }
    strncpy(out, "", (size_t)outlen); out[outlen - 1] = '\0';
}

static void add_hotspot(int line, const char *issue,
                        const char *sev, const char *suggestion) {
    if (!g_hr || g_hr->count >= MAX_HOTSPOTS) return;
    Hotspot *h = &g_hr->items[g_hr->count++];
    h->line = line;
    strncpy(h->issue,      issue,      HS_STR_LEN - 1);
    strncpy(h->severity,   sev,        15);
    strncpy(h->suggestion, suggestion, HS_STR_LEN - 1);
    h->issue[HS_STR_LEN - 1]      = '\0';
    h->severity[15]                = '\0';
    h->suggestion[HS_STR_LEN - 1] = '\0';
}

/* ── Nested loop detection ──────────────────── */
static void find_nested_loops(const ASTNode *node, int depth) {
    if (!node) return;
    if (strcmp(node->type, "WhileStmt") == 0 ||
        strcmp(node->type, "ForStmt")   == 0) {
        int new_depth = depth + 1;
        if (new_depth >= 2) {
            char issue[HS_STR_LEN], sug[HS_STR_LEN];
            snprintf(issue, sizeof(issue), "Nested loop detected");
            snprintf(sug, sizeof(sug),
                     "Consider if both loops are necessary. "
                     "Look for algorithmic improvements to reduce to O(n). "
                     "Inner loop body executes O(n%s) times.",
                     new_depth == 2 ? "\xc2\xb2" : "\xc2\xb3");
            add_hotspot(node->line, issue, "High", sug);
        }
        for (int i = 0; i < node->child_count; i++)
            find_nested_loops(node->children[i], new_depth);
    } else {
        for (int i = 0; i < node->child_count; i++)
            find_nested_loops(node->children[i], depth);
    }
}

/* ── Collect binary expressions inside a subtree ─── */
#define MAX_EXPRS 64
static char g_exprs[MAX_EXPRS][256];
static int  g_expr_count;

static void collect_exprs(const ASTNode *node) {
    if (!node) return;
    if (strcmp(node->type, "BinaryOp") == 0) {
        char s[256] = "";
        expr_to_str(node, s, sizeof(s));
        if (strlen(s) > 2 && g_expr_count < MAX_EXPRS) {
            strncpy(g_exprs[g_expr_count++], s, 255);
            g_exprs[g_expr_count - 1][255] = '\0';
        }
    }
    for (int i = 0; i < node->child_count; i++)
        collect_exprs(node->children[i]);
}

/* ── Repeated expression detection ──────────── */
static void find_repeated_exprs(const ASTNode *node) {
    if (!node) return;
    if (strcmp(node->type, "WhileStmt") == 0 ||
        strcmp(node->type, "ForStmt")   == 0) {
        g_expr_count = 0;
        for (int i = 0; i < node->child_count; i++)
            collect_exprs(node->children[i]);
        /* Count duplicates */
        for (int a = 0; a < g_expr_count; a++) {
            int cnt = 0;
            for (int b2 = 0; b2 < g_expr_count; b2++)
                if (strcmp(g_exprs[a], g_exprs[b2]) == 0) cnt++;
            if (cnt >= 2) {
                /* Check not already reported */
                int already = 0;
                for (int k = 0; k < g_hr->count; k++) {
                    if (strstr(g_hr->items[k].issue, g_exprs[a]) &&
                        g_hr->items[k].line == node->line) { already = 1; break; }
                }
                if (!already) {
                    char issue[HS_STR_LEN], sug[HS_STR_LEN];
                    snprintf(issue, sizeof(issue),
                             "Expression '%s' computed multiple times inside loop", g_exprs[a]);
                    snprintf(sug, sizeof(sug),
                             "Consider storing '%s' in a variable before the loop.", g_exprs[a]);
                    add_hotspot(node->line, issue, "Medium", sug);
                }
                break; /* only first dup per loop */
            }
        }
    }
    for (int i = 0; i < node->child_count; i++)
        find_repeated_exprs(node->children[i]);
}

/* ── Dead code after return ──────────────────── */
static void find_dead_code(const ASTNode *node) {
    if (!node) return;
    if (strcmp(node->type, "Block") == 0) {
        int found_return = 0, return_line = 0;
        for (int i = 0; i < node->child_count; i++) {
            if (found_return) {
                char issue[HS_STR_LEN], sug[HS_STR_LEN];
                int dead_line = node->children[i]->line > 0
                                ? node->children[i]->line
                                : return_line + 1;
                snprintf(issue, sizeof(issue),
                         "Code after return statement on line %d will never execute", return_line);
                snprintf(sug, sizeof(sug),
                         "Remove unreachable code after the return statement to improve clarity.");
                add_hotspot(dead_line, issue, "Medium", sug);
                break;
            }
            if (strcmp(node->children[i]->type, "ReturnStmt") == 0) {
                found_return = 1;
                return_line = node->children[i]->line;
            }
        }
    }
    for (int i = 0; i < node->child_count; i++)
        find_dead_code(node->children[i]);
}

/* ── Public API ──────────────────────────────── */
HotspotResult hotspot_detect(const ASTNode *root, const SymTable *st) {
    HotspotResult res;
    memset(&res, 0, sizeof(res));
    g_hr = &res;

    find_nested_loops(root, 0);
    find_repeated_exprs(root);

    /* Unused variables from symbol table */
    if (st) {
        for (int i = 0; i < st->count; i++) {
            const SymEntry *e = &st->entries[i];
            if (strcmp(e->type, "func") != 0 && e->references == 0) {
                char issue[HS_STR_LEN], sug[HS_STR_LEN];
                snprintf(issue, sizeof(issue),
                         "Variable '%s' is declared but never used", e->name);
                snprintf(sug, sizeof(sug),
                         "Remove the declaration of '%s' to keep the code clean and reduce memory usage.",
                         e->name);
                add_hotspot(e->line, issue, "Low", sug);
            }
        }
    }

    find_dead_code(root);

    g_hr = NULL;
    return res;
}

void hotspot_to_json(const HotspotResult *res, StrBuf *out) {
    buf_append(out, "[");
    for (int i = 0; i < res->count; i++) {
        if (i > 0) buf_append(out, ",");
        const Hotspot *h = &res->items[i];
        buf_append(out, "{");
        buf_appendf(out, "\"line\":%d", h->line);
        buf_append(out, ",\"issue\":");      buf_append_json_str(out, h->issue);
        buf_append(out, ",\"severity\":");   buf_append_json_str(out, h->severity);
        buf_append(out, ",\"suggestion\":"); buf_append_json_str(out, h->suggestion);
        buf_append(out, "}");
    }
    buf_append(out, "]");
}
