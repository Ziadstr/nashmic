/*
 * NashmiC Standard Library — malafat (Files)
 */

#define _GNU_SOURCE
#include "malafat.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

FILE *nsh_iftah_malaf(const char *path) {
    if (!path) return NULL;
    return fopen(path, "r");
}

FILE *nsh_iftah_malaf_ktaba(const char *path) {
    if (!path) return NULL;
    return fopen(path, "w");
}

void nsh_sakker_malaf(FILE *f) {
    if (f) fclose(f);
}

char *nsh_i2ra_kol(const char *path) {
    if (!path) return strdup("");
    FILE *f = fopen(path, "rb");
    if (!f) return strdup("");

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc((size_t)len + 1);
    if (!buf) { fclose(f); return strdup(""); }

    size_t read_bytes = fread(buf, 1, (size_t)len, f);
    buf[read_bytes] = '\0';
    fclose(f);
    return buf;
}

int nsh_uktub_malaf(const char *path, const char *content) {
    if (!path || !content) return 0;
    FILE *f = fopen(path, "w");
    if (!f) return 0;
    fputs(content, f);
    fclose(f);
    return 1;
}

int nsh_dahef_malaf(const char *path, const char *content) {
    if (!path || !content) return 0;
    FILE *f = fopen(path, "a");
    if (!f) return 0;
    fputs(content, f);
    fclose(f);
    return 1;
}

int nsh_mawjood(const char *path) {
    if (!path) return 0;
    return access(path, F_OK) == 0;
}

int nsh_imsah_malaf(const char *path) {
    if (!path) return 0;
    return remove(path) == 0;
}

int64_t nsh_hajm_malaf(const char *path) {
    if (!path) return -1;
    struct stat st;
    if (stat(path, &st) != 0) return -1;
    return (int64_t)st.st_size;
}

NshStringArray nsh_i2ra_sutoor(const char *path) {
    NshStringArray result = {NULL, 0, 0};
    if (!path) return result;

    char *content = nsh_i2ra_kol(path);
    if (!content || strlen(content) == 0) {
        free(content);
        return result;
    }

    result = nsh_qass(content, "\n");
    free(content);
    return result;
}
