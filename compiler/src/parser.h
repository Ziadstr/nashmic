/*
 * NashmiC — parser.h
 * Recursive descent parser with Pratt precedence for expressions
 */

#ifndef NSH_PARSER_H
#define NSH_PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct {
    NshLexer lexer;
    NshToken current;
    NshToken previous;
    int had_error;
    int panic_mode;
    const char *source;      /* for error reporting */
    const char *filename;
} NshParser;

/* Parse a complete source file into an AST. Returns NULL on error. */
NshNode *parser_parse(const char *source, size_t len, const char *filename);

#endif /* NSH_PARSER_H */
