/*
 * NashmiC Runtime — nsh_runtime.c
 * Phase 0: minimal runtime (init, print, exit)
 */

#define _GNU_SOURCE
#include "nsh_runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <locale.h>

void nsh_runtime_init(void) {
    /* Enable UTF-8 locale for Arabic output */
    setlocale(LC_ALL, "");
}

void nsh_runtime_shutdown(void) {
    fflush(stdout);
    fflush(stderr);
}

void nsh_itba3(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

char *nsh_i2ra(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read = getline(&line, &len, stdin);
    if (read == -1) {
        free(line);
        return NULL;
    }
    /* strip trailing newline */
    if (read > 0 && line[read - 1] == '\n') {
        line[read - 1] = '\0';
    }
    return line;
}

void nsh_itla3(int code) {
    nsh_runtime_shutdown();
    exit(code);
}
