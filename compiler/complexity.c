#include "complexity.h"
#include <string.h>
#include <stdio.h>

static const char *depth_to_complexity(int depth) {
    if (depth == 0) return "O(1)";
    if (depth == 1) return "O(n)";
    if (depth == 2) return "O(n\xc2\xb2)";  /* O(n²) */
    return "O(n\xc2\xb3)";                  /* O(n³) */
}

static int depth_to_scale(int depth) {
    if (depth == 0) return 0;
    if (depth == 1) return 2;
    if (depth == 2) return 4;
    return 5;
}

static const char *depth_to_notation(int depth) {
    if (depth == 0) return "constant";
    if (depth == 1) return "linear";
    if (depth == 2) return "quadratic";
    return "cubic";
}

static ComplexityResult *g_res;

static int analyze_node(const ASTNode *node, int cur_depth,
                         const char *block_name);

static int analyze_node(const ASTNode *node, int cur_depth,
                         const char *block_name) {
    if (!node) return cur_depth;

    int max_depth = cur_depth;

    if (strcmp(node->type, "WhileStmt") == 0 ||
        strcmp(node->type, "ForStmt")   == 0) {
        int inner = cur_depth + 1;
        for (int i = 0; i < node->child_count; i++) {
            int d = analyze_node(node->children[i], inner, block_name);
            if (d > max_depth) max_depth = d;
        }
        return max_depth;
    }

    if (strcmp(node->type, "FuncDecl") == 0) {
        char fname[128];
        snprintf(fname, sizeof(fname), "func %s", node->value);
        int func_max = 0;
        for (int i = 0; i < node->child_count; i++) {
            int d = analyze_node(node->children[i], 0, fname);
            if (d > func_max) func_max = d;
        }
        if (g_res->breakdown_count < MAX_BREAKDOWN) {
            BreakdownEntry *be = &g_res->breakdown[g_res->breakdown_count++];
            strncpy(be->block, fname, sizeof(be->block) - 1);
            be->block[sizeof(be->block) - 1] = '\0';
            be->loop_depth = func_max;
            strncpy(be->complexity, depth_to_complexity(func_max), sizeof(be->complexity) - 1);
            snprintf(be->reason, sizeof(be->reason),
                     func_max > 0 ? "%d nested loop(s) found" : "No loops found", func_max);
        }
        return cur_depth; /* function scope doesn't propagate */
    }

    for (int i = 0; i < node->child_count; i++) {
        int d = analyze_node(node->children[i], cur_depth, block_name);
        if (d > max_depth) max_depth = d;
    }
    return max_depth;
}

ComplexityResult complexity_analyze(const ASTNode *root) {
    ComplexityResult res;
    memset(&res, 0, sizeof(res));
    g_res = &res;

    int global_max = analyze_node(root, 0, "global");

    /* Insert global block first */
    BreakdownEntry global_be;
    snprintf(global_be.block, sizeof(global_be.block), "global");
    global_be.loop_depth = global_max;
    strncpy(global_be.complexity, depth_to_complexity(global_max), sizeof(global_be.complexity) - 1);
    snprintf(global_be.reason, sizeof(global_be.reason),
             global_max > 0 ? "%d nested loop(s) found" : "No loops found", global_max);

    /* Shift existing entries up by 1 */
    int total = res.breakdown_count + 1;
    if (total > MAX_BREAKDOWN) total = MAX_BREAKDOWN;
    for (int i = total - 1; i > 0; i--)
        res.breakdown[i] = res.breakdown[i - 1];
    res.breakdown[0] = global_be;
    res.breakdown_count = total;

    /* Find overall max depth */
    int overall_depth = 0;
    for (int i = 0; i < res.breakdown_count; i++)
        if (res.breakdown[i].loop_depth > overall_depth)
            overall_depth = res.breakdown[i].loop_depth;

    strncpy(res.overall,  depth_to_complexity(overall_depth), sizeof(res.overall) - 1);
    strncpy(res.notation, depth_to_notation(overall_depth),   sizeof(res.notation) - 1);
    res.scale_position = depth_to_scale(overall_depth);

    /* Generate explanation */
    if (overall_depth == 0)
        snprintf(res.explanation, sizeof(res.explanation),
                 "Your code contains no loops. All operations execute a fixed number of times "
                 "regardless of input size. Therefore time complexity is O(1) — constant time.");
    else if (overall_depth == 1)
        snprintf(res.explanation, sizeof(res.explanation),
                 "Your code contains a single loop that iterates n times. "
                 "Total operations \xe2\x89\x88 n. Therefore time complexity is O(n) — linear time.");
    else if (overall_depth == 2)
        snprintf(res.explanation, sizeof(res.explanation),
                 "Your code contains 2 nested loops. The outer loop runs n times, "
                 "the inner loop also runs n times for each outer iteration. "
                 "Total operations \xe2\x89\x88 n \xc3\x97 n = n\xc2\xb2. "
                 "Therefore time complexity is O(n\xc2\xb2) — quadratic time.");
    else
        snprintf(res.explanation, sizeof(res.explanation),
                 "Your code contains %d nested loops. "
                 "Each loop runs n times, giving total operations \xe2\x89\x88 n^%d. "
                 "Therefore time complexity is O(n\xc2\xb3).", overall_depth, overall_depth);

    g_res = NULL;
    return res;
}

void complexity_to_json(const ComplexityResult *res, StrBuf *out) {
    buf_append(out, "{");
    buf_append(out, "\"overall\":");       buf_append_json_str(out, res->overall);
    buf_append(out, ",\"notation\":");     buf_append_json_str(out, res->notation);
    buf_appendf(out, ",\"scale_position\":%d", res->scale_position);
    buf_append(out, ",\"explanation\":"); buf_append_json_str(out, res->explanation);
    buf_append(out, ",\"breakdown\":[");
    for (int i = 0; i < res->breakdown_count; i++) {
        if (i > 0) buf_append(out, ",");
        const BreakdownEntry *be = &res->breakdown[i];
        buf_append(out, "{");
        buf_append(out, "\"block\":");      buf_append_json_str(out, be->block);
        buf_appendf(out, ",\"loop_depth\":%d", be->loop_depth);
        buf_append(out, ",\"complexity\":"); buf_append_json_str(out, be->complexity);
        buf_append(out, ",\"reason\":");     buf_append_json_str(out, be->reason);
        buf_append(out, "}");
    }
    buf_append(out, "]}");
}
