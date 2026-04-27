#ifndef HOTSPOT_H
#define HOTSPOT_H

#include "ast.h"
#include "symbol_table.h"
#include "json_output.h"

#define MAX_HOTSPOTS 128
#define HS_STR_LEN   512

typedef struct {
    int  line;
    char issue[HS_STR_LEN];
    char severity[16];   /* "High" | "Medium" | "Low" */
    char suggestion[HS_STR_LEN];
} Hotspot;

typedef struct {
    Hotspot items[MAX_HOTSPOTS];
    int     count;
} HotspotResult;

/* Detect performance hotspots in the AST */
HotspotResult hotspot_detect(const ASTNode *root, const SymTable *st);

/* Serialize to JSON array */
void hotspot_to_json(const HotspotResult *res, StrBuf *out);

#endif /* HOTSPOT_H */
