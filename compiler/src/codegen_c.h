/*
 * NashmiC — codegen_c.h
 * Phase 1 backend: transpile AST to C code
 */

#ifndef NSH_CODEGEN_C_H
#define NSH_CODEGEN_C_H

#include "ast.h"
#include <stdio.h>

/* Generate C code from an AST program node.
 * Writes to the given FILE stream. Returns 0 on success, -1 on error. */
int codegen_c_emit(NshNode *program, FILE *out);

#endif /* NSH_CODEGEN_C_H */
