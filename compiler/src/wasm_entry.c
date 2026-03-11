/*
 * NashmiC — wasm_entry.c
 * WebAssembly entry point for in-browser transpilation
 *
 * Exports:
 *   nsh_compile(source)    -> generated C code string (caller must free)
 *   nsh_get_errors()       -> diagnostic output string (caller must free)
 *
 * Diagnostics (parser errors, sema warnings) go to stderr, which
 * Emscripten routes to console.error in JS. We capture them on the
 * JS side via Module.printErr override.
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

/* ── Error buffer ────────────────────────────────────────────── */

static char *error_buffer = NULL;

/* ── Exported API ────────────────────────────────────────────── */

/*
 * Compile NashmiC source code to C.
 *
 * Returns a malloc'd string containing the generated C code,
 * or an empty string on failure. The caller (JS via cwrap) must
 * call free() on the returned pointer.
 *
 * Diagnostics are built from diag_error_count() and sema results.
 * Detailed stderr output is captured on the JS side via printErr.
 */
EMSCRIPTEN_KEEPALIVE
char *nsh_compile(const char *source) {
    /* Free previous error buffer */
    if (error_buffer) {
        free(error_buffer);
        error_buffer = NULL;
    }

    if (!source || *source == '\0') {
        error_buffer = strdup("No source code provided.\n");
        return strdup("");
    }

    size_t source_len = strlen(source);

    /* Initialize subsystems */
    keywords_init();
    diag_reset();

    /* ── Lex + Parse ──────────────────────────────────────────── */
    NshNode *ast = parser_parse(source, source_len, "playground.nsh");
    if (!ast) {
        char *msg = NULL;
        size_t msg_len = 0;
        FILE *msg_stream = open_memstream(&msg, &msg_len);
        if (msg_stream) {
            fprintf(msg_stream, "Parse failed — %d error(s)\n", diag_error_count());
            fclose(msg_stream);
        }
        error_buffer = msg ? msg : strdup("Parse failed.\n");
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

    /* Build summary message */
    char *diag_msg = NULL;
    size_t diag_len = 0;
    FILE *diag_stream = open_memstream(&diag_msg, &diag_len);
    if (diag_stream) {
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
 * Retrieve the diagnostic output from the last nsh_compile() call.
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
