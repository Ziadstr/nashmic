/*
 * NashmiC — wasm_entry.c
 * WebAssembly entry point for in-browser transpilation
 *
 * Exports:
 *   nsh_compile(source)    -> generated C code string (caller must free)
 *   nsh_get_errors()       -> diagnostic output string (caller must free)
 *
 * No system(), no fopen() on real files, no exec*().
 * All I/O goes through in-memory buffers.
 */

#define _GNU_SOURCE
#include "lexer.h"
#include "keywords.h"
#include "parser.h"
#include "ast.h"
#include "codegen_c.h"
#include "sema.h"
#include "diagnostics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

/* ── Error capture buffer ────────────────────────────────────── */

static char *error_buffer = NULL;
static size_t error_buffer_size = 0;

/* ── Stderr redirection helpers ──────────────────────────────── */

/*
 * Redirect stderr to an in-memory buffer so diagnostics, parser
 * errors, and sema warnings are all captured as a string.
 *
 * Strategy: save the real stderr, replace it with an open_memstream
 * FILE*, run the compiler pipeline, then restore stderr and read
 * the captured output.
 */

static FILE *real_stderr = NULL;
static FILE *capture_stream = NULL;
static char *capture_buf = NULL;
static size_t capture_len = 0;

static void begin_stderr_capture(void) {
    capture_buf = NULL;
    capture_len = 0;
    capture_stream = open_memstream(&capture_buf, &capture_len);
    if (capture_stream) {
        real_stderr = stderr;
        stderr = capture_stream;
    }
}

static char *end_stderr_capture(void) {
    if (capture_stream) {
        fflush(capture_stream);
        fclose(capture_stream);
        capture_stream = NULL;
        stderr = real_stderr;
        real_stderr = NULL;
    }
    /* Return ownership of the buffer to the caller */
    char *result = capture_buf;
    capture_buf = NULL;
    capture_len = 0;
    return result;
}

/* ── Exported API ────────────────────────────────────────────── */

/*
 * Compile NashmiC source code to C.
 *
 * Returns a malloc'd string containing the generated C code,
 * or an empty string on failure. The caller (JS via cwrap) must
 * call free() on the returned pointer.
 *
 * Errors and warnings are stored internally and retrieved via
 * nsh_get_errors().
 */
EMSCRIPTEN_KEEPALIVE
char *nsh_compile(const char *source) {
    /* Free previous error buffer */
    if (error_buffer) {
        free(error_buffer);
        error_buffer = NULL;
        error_buffer_size = 0;
    }

    if (!source || *source == '\0') {
        error_buffer = strdup("No source code provided.\n");
        return strdup("");
    }

    size_t source_len = strlen(source);

    /* Initialize subsystems */
    keywords_init();
    diag_reset();

    /* Start capturing stderr */
    begin_stderr_capture();

    /* ── Lex + Parse ──────────────────────────────────────────── */
    NshNode *ast = parser_parse(source, source_len, "playground.nsh");
    if (!ast) {
        char *captured = end_stderr_capture();

        /* Build error message */
        char *msg = NULL;
        size_t msg_len = 0;
        FILE *msg_stream = open_memstream(&msg, &msg_len);
        if (msg_stream) {
            fprintf(msg_stream, "Parse failed — %d error(s)\n", diag_error_count());
            if (captured && *captured) {
                fprintf(msg_stream, "%s", captured);
            }
            fclose(msg_stream);
        }

        error_buffer = msg ? msg : strdup("Parse failed.\n");
        free(captured);
        keywords_free();
        return strdup("");
    }

    /* ── Semantic Analysis ────────────────────────────────────── */
    int sema_errors = sema_check(ast, source, "playground.nsh");

    /* ── Code Generation ──────────────────────────────────────── */
    char *codegen_buf = NULL;
    size_t codegen_len = 0;
    FILE *codegen_stream = open_memstream(&codegen_buf, &codegen_len);

    int codegen_ok = 0;
    if (codegen_stream) {
        codegen_ok = (codegen_c_emit(ast, codegen_stream) == 0);
        fclose(codegen_stream);
    }

    /* Stop capturing stderr */
    char *captured = end_stderr_capture();

    /* Build error/warning output */
    char *diag_msg = NULL;
    size_t diag_len = 0;
    FILE *diag_stream = open_memstream(&diag_msg, &diag_len);
    if (diag_stream) {
        if (captured && *captured) {
            fprintf(diag_stream, "%s", captured);
        }
        if (sema_errors > 0) {
            fprintf(diag_stream, "%d semantic issue(s) found (warnings only — codegen proceeded)\n", sema_errors);
        }
        if (!codegen_ok) {
            fprintf(diag_stream, "Code generation failed.\n");
        }
        if (diag_error_count() == 0 && sema_errors == 0 && codegen_ok) {
            fprintf(diag_stream, "Compilation successful — no errors.\n");
        }
        fclose(diag_stream);
    }
    error_buffer = diag_msg ? diag_msg : strdup("");
    free(captured);

    /* Cleanup AST and keywords */
    node_free(ast);
    keywords_free();

    if (!codegen_ok || !codegen_buf) {
        free(codegen_buf);
        return strdup("");
    }

    return codegen_buf;
}

/*
 * Retrieve the error/warning output from the last nsh_compile() call.
 *
 * Returns a malloc'd string. The caller (JS via cwrap) must call
 * free() on the returned pointer.
 */
EMSCRIPTEN_KEEPALIVE
char *nsh_get_errors(void) {
    if (error_buffer) {
        return strdup(error_buffer);
    }
    return strdup("");
}
