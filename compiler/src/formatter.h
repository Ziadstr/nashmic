/*
 * NashmiC — formatter.h
 * Auto-formatter: pretty-prints AST back to NashmiC source
 */

#ifndef NSH_FORMATTER_H
#define NSH_FORMATTER_H

#include "ast.h"
#include <stdio.h>

/* Format an AST program node as NashmiC source.
 * Writes to the given FILE stream. Returns 0 on success, -1 on error. */
int formatter_format(NshNode *ast, FILE *out);

#endif /* NSH_FORMATTER_H */
