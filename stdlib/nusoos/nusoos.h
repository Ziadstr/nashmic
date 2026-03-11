/*
 * NashmiC Standard Library — nusoos (Strings)
 * String manipulation functions. All functions returning char*
 * return malloc'd strings — caller owns the memory.
 */

#ifndef NSH_NUSOOS_H
#define NSH_NUSOOS_H

#include <stdint.h>

typedef struct {
    char **items;
    int64_t len;
    int64_t cap;
} NshStringArray;

void nsh_string_array_free(NshStringArray *arr);

int64_t nsh_toul(const char *s);
NshStringArray nsh_qass(const char *s, const char *delim);
char *nsh_damj(NshStringArray arr, const char *sep);
char *nsh_badel(const char *s, const char *old_str, const char *new_str);
int nsh_yihtawi(const char *s, const char *sub);
int nsh_bdaya(const char *s, const char *prefix);
int nsh_nihaya(const char *s, const char *suffix);
char *nsh_a3la(const char *s);
char *nsh_asfal(const char *s);
char *nsh_qass_haddi(const char *s, int64_t start, int64_t length);
char nsh_harf_3ind(const char *s, int64_t i);
char *nsh_juz2(const char *s, int64_t start, int64_t end);
char *nsh_karrer(const char *s, int64_t n);

#endif /* NSH_NUSOOS_H */
