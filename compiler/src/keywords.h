/*
 * NashmiC — keywords.h
 * Dual-mode keyword lookup: Arabic script + Franco-Arab Latin
 * Every keyword has two spellings that map to the same token.
 */

#ifndef NSH_KEYWORDS_H
#define NSH_KEYWORDS_H

#include "lexer.h"

/* Initialize the keyword hash tables (Arabic + Franco). Call once at startup. */
void keywords_init(void);

/* Lookup an identifier string. Returns the token type if it's a keyword,
 * or TOK_IDENT if it's a regular identifier. */
NshTokenType keywords_lookup(const char *text, int len);

/* Free keyword tables. */
void keywords_free(void);

#endif /* NSH_KEYWORDS_H */
