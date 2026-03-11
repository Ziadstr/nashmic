/*
 * NashmiC Standard Library — malafat (Files)
 * File I/O operations.
 */

#ifndef NSH_MALAFAT_H
#define NSH_MALAFAT_H

#include <stdio.h>
#include <stdint.h>
#include "../nusoos/nusoos.h"

FILE *nsh_iftah_malaf(const char *path);
FILE *nsh_iftah_malaf_ktaba(const char *path);
void nsh_sakker_malaf(FILE *f);
char *nsh_i2ra_kol(const char *path);
int nsh_uktub_malaf(const char *path, const char *content);
int nsh_dahef_malaf(const char *path, const char *content);
int nsh_mawjood(const char *path);
int nsh_imsah_malaf(const char *path);
int64_t nsh_hajm_malaf(const char *path);
NshStringArray nsh_i2ra_sutoor(const char *path);

#endif /* NSH_MALAFAT_H */
