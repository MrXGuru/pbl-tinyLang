#ifndef COMPLEXITY_H
#define COMPLEXITY_H

#include "ast.h"
#include "json_output.h"

#define MAX_BREAKDOWN 64
#define COMPL_STR_LEN 64

typedef struct {
    char block[128];
    int  loop_depth;
    char complexity[COMPL_STR_LEN];
    char reason[256];
} BreakdownEntry;

typedef struct {
    char           overall[COMPL_STR_LEN];
    char           notation[64];
    BreakdownEntry breakdown[MAX_BREAKDOWN];
    int            breakdown_count;
    char           explanation[1024];
    int            scale_position;
} ComplexityResult;

/* Analyze AST for loop nesting and return complexity info */
ComplexityResult complexity_analyze(const ASTNode *root);

/* Serialize to JSON */
void complexity_to_json(const ComplexityResult *res, StrBuf *out);

#endif /* COMPLEXITY_H */
