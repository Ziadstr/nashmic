/*
 * NashmiC — diagnostics.c
 * Jordanian-flavored compiler diagnostics with source context
 *
 * Error format:
 *   خطأ[E####]: message
 *     --> file:line:col
 *      |
 *   12 |     offending line
 *      |     ^^^^^^^ hint
 *      |
 *      = proverb — explanation
 */

#include "diagnostics.h"
#include <stdio.h>
#include <string.h>

/* ANSI colors */
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[0;36m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"

static int error_count = 0;

/* Find the start of the line containing the given byte offset */
static const char *find_line_start(const char *source, size_t offset) {
    const char *p = source + offset;
    while (p > source && *(p - 1) != '\n') p--;
    return p;
}

/* Find the end of the line (exclusive, points to \n or end) */
static const char *find_line_end(const char *source, size_t source_len, size_t offset) {
    const char *end = source + source_len;
    const char *p = source + offset;
    while (p < end && *p != '\n') p++;
    return p;
}

static void print_source_context(const char *source, size_t source_len,
                                 NshSpan span) {
    if (!source || span.line == 0) return;

    const char *line_start = find_line_start(source, span.offset);
    const char *line_end = find_line_end(source, source_len, span.offset);
    int line_len = (int)(line_end - line_start);

    /* line number gutter */
    fprintf(stderr, BLUE "     |" RESET "\n");
    fprintf(stderr, BLUE " %3d | " RESET, span.line);

    /* print the source line */
    fwrite(line_start, 1, line_len, stderr);
    fprintf(stderr, "\n");

    /* underline */
    fprintf(stderr, BLUE "     | " RESET);
    for (int i = 1; i < span.col; i++) fprintf(stderr, " ");

    int underline_len = span.length > 0 ? (int)span.length : 1;
    if (underline_len > line_len) underline_len = line_len;

    fprintf(stderr, RED);
    for (int i = 0; i < underline_len; i++) fprintf(stderr, "^");
    fprintf(stderr, RESET "\n");

    fprintf(stderr, BLUE "     |" RESET "\n");
}

void diag_error_at(const char *filename, const char *source,
                   NshSpan span, const char *message) {
    error_count++;

    fprintf(stderr, RED "خطأ" RESET "[E%04d]: " BOLD "%s" RESET "\n",
            error_count, message);

    if (filename && span.line > 0) {
        fprintf(stderr, BLUE "  --> " RESET "%s:%d:%d\n",
                filename, span.line, span.col);
    }

    if (source && span.line > 0) {
        size_t source_len = strlen(source);
        print_source_context(source, source_len, span);
    }

    fprintf(stderr, "\n");
}

void diag_warning_at(const char *filename, const char *source,
                     NshSpan span, const char *message) {
    fprintf(stderr, YELLOW "تحذير" RESET ": " BOLD "%s" RESET "\n", message);

    if (filename && span.line > 0) {
        fprintf(stderr, BLUE "  --> " RESET "%s:%d:%d\n",
                filename, span.line, span.col);
    }

    if (source && span.line > 0) {
        size_t source_len = strlen(source);
        print_source_context(source, source_len, span);
    }

    fprintf(stderr, "\n");
}

void diag_error(const char *message) {
    error_count++;
    fprintf(stderr, RED "خطأ" RESET ": " BOLD "%s" RESET "\n\n", message);
}

int diag_error_count(void) {
    return error_count;
}

void diag_reset(void) {
    error_count = 0;
}
