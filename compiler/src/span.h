/*
 * NashmiC — span.h
 * Source location tracking for diagnostics
 */

#ifndef NSH_SPAN_H
#define NSH_SPAN_H

#include <stddef.h>

typedef struct {
    const char *file;   /* source filename */
    size_t offset;      /* byte offset in source */
    size_t length;      /* byte length of span */
    int line;           /* 1-based line */
    int col;            /* 1-based column */
} NshSpan;

#define NSH_SPAN_NONE ((NshSpan){NULL, 0, 0, 0, 0})

#endif /* NSH_SPAN_H */
