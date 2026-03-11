/*
 * NashmiC Standard Library — nusoos (Strings)
 */

#include "nusoos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void nsh_string_array_free(NshStringArray *arr) {
    if (!arr) return;
    for (int64_t i = 0; i < arr->len; i++) {
        free(arr->items[i]);
    }
    free(arr->items);
    arr->items = NULL;
    arr->len = 0;
    arr->cap = 0;
}

int64_t nsh_toul(const char *s) {
    if (!s) return 0;
    return (int64_t)strlen(s);
}

NshStringArray nsh_qass(const char *s, const char *delim) {
    NshStringArray result = {NULL, 0, 0};
    if (!s || !delim) return result;

    size_t dlen = strlen(delim);
    if (dlen == 0) {
        /* Split each character */
        size_t slen = strlen(s);
        result.items = malloc(slen * sizeof(char *));
        result.cap = (int64_t)slen;
        for (size_t i = 0; i < slen; i++) {
            result.items[i] = malloc(2);
            result.items[i][0] = s[i];
            result.items[i][1] = '\0';
            result.len++;
        }
        return result;
    }

    result.cap = 8;
    result.items = malloc(result.cap * sizeof(char *));

    const char *start = s;
    const char *found;
    while ((found = strstr(start, delim)) != NULL) {
        if (result.len >= result.cap) {
            result.cap *= 2;
            result.items = realloc(result.items, result.cap * sizeof(char *));
        }
        size_t part_len = (size_t)(found - start);
        result.items[result.len] = malloc(part_len + 1);
        memcpy(result.items[result.len], start, part_len);
        result.items[result.len][part_len] = '\0';
        result.len++;
        start = found + dlen;
    }
    /* Last part */
    if (result.len >= result.cap) {
        result.cap *= 2;
        result.items = realloc(result.items, result.cap * sizeof(char *));
    }
    result.items[result.len] = strdup(start);
    result.len++;

    return result;
}

char *nsh_damj(NshStringArray arr, const char *sep) {
    if (arr.len == 0) return strdup("");
    if (!sep) sep = "";

    size_t sep_len = strlen(sep);
    size_t total = 0;
    for (int64_t i = 0; i < arr.len; i++) {
        total += strlen(arr.items[i]);
        if (i > 0) total += sep_len;
    }

    char *out = malloc(total + 1);
    char *p = out;
    for (int64_t i = 0; i < arr.len; i++) {
        if (i > 0) {
            memcpy(p, sep, sep_len);
            p += sep_len;
        }
        size_t len = strlen(arr.items[i]);
        memcpy(p, arr.items[i], len);
        p += len;
    }
    *p = '\0';
    return out;
}

char *nsh_badel(const char *s, const char *old_str, const char *new_str) {
    if (!s || !old_str || !new_str) return s ? strdup(s) : strdup("");

    size_t old_len = strlen(old_str);
    if (old_len == 0) return strdup(s);

    size_t new_len = strlen(new_str);

    /* Count occurrences */
    int count = 0;
    const char *p = s;
    while ((p = strstr(p, old_str)) != NULL) {
        count++;
        p += old_len;
    }

    size_t result_len = strlen(s) + count * ((int64_t)new_len - (int64_t)old_len);
    char *out = malloc(result_len + 1);
    char *dst = out;
    const char *src = s;

    while ((p = strstr(src, old_str)) != NULL) {
        size_t chunk = (size_t)(p - src);
        memcpy(dst, src, chunk);
        dst += chunk;
        memcpy(dst, new_str, new_len);
        dst += new_len;
        src = p + old_len;
    }
    strcpy(dst, src);
    return out;
}

int nsh_yihtawi(const char *s, const char *sub) {
    if (!s || !sub) return 0;
    return strstr(s, sub) != NULL;
}

int nsh_bdaya(const char *s, const char *prefix) {
    if (!s || !prefix) return 0;
    size_t plen = strlen(prefix);
    return strncmp(s, prefix, plen) == 0;
}

int nsh_nihaya(const char *s, const char *suffix) {
    if (!s || !suffix) return 0;
    size_t slen = strlen(s);
    size_t suflen = strlen(suffix);
    if (suflen > slen) return 0;
    return strcmp(s + slen - suflen, suffix) == 0;
}

char *nsh_a3la(const char *s) {
    if (!s) return strdup("");
    size_t len = strlen(s);
    char *out = malloc(len + 1);
    for (size_t i = 0; i < len; i++) {
        out[i] = (char)toupper((unsigned char)s[i]);
    }
    out[len] = '\0';
    return out;
}

char *nsh_asfal(const char *s) {
    if (!s) return strdup("");
    size_t len = strlen(s);
    char *out = malloc(len + 1);
    for (size_t i = 0; i < len; i++) {
        out[i] = (char)tolower((unsigned char)s[i]);
    }
    out[len] = '\0';
    return out;
}

char *nsh_qass_haddi(const char *s, int64_t start, int64_t length) {
    if (!s) return strdup("");
    int64_t slen = (int64_t)strlen(s);
    if (start < 0) start = 0;
    if (start >= slen) return strdup("");
    if (length < 0) length = 0;
    if (start + length > slen) length = slen - start;

    char *out = malloc((size_t)length + 1);
    memcpy(out, s + start, (size_t)length);
    out[length] = '\0';
    return out;
}

char nsh_harf_3ind(const char *s, int64_t i) {
    if (!s) return '\0';
    int64_t slen = (int64_t)strlen(s);
    if (i < 0 || i >= slen) {
        fprintf(stderr, "harf_3ind: index %lld out of bounds (length %lld)\n",
                (long long)i, (long long)slen);
        return '\0';
    }
    return s[i];
}

char *nsh_juz2(const char *s, int64_t start, int64_t end) {
    if (!s) return strdup("");
    int64_t slen = (int64_t)strlen(s);
    if (start < 0) start = 0;
    if (end > slen) end = slen;
    if (start >= end) return strdup("");

    int64_t length = end - start;
    char *out = malloc((size_t)length + 1);
    memcpy(out, s + start, (size_t)length);
    out[length] = '\0';
    return out;
}

char *nsh_karrer(const char *s, int64_t n) {
    if (!s || n <= 0) return strdup("");
    size_t slen = strlen(s);
    size_t total = slen * (size_t)n;
    char *out = malloc(total + 1);
    char *p = out;
    for (int64_t i = 0; i < n; i++) {
        memcpy(p, s, slen);
        p += slen;
    }
    *p = '\0';
    return out;
}
