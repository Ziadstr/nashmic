/*
 * mansaf — NashmiC Compiler CLI 🇯🇴
 *
 * Usage:
 *   mansaf build <file.nsh>     Compile to binary
 *   mansaf run <file.nsh>       Compile and run
 *   mansaf lex <file.nsh>       Debug: dump tokens
 *
 * Phase 0: C transpiler backend (compile .nsh → .c → binary)
 */

#define _GNU_SOURCE
#include "lexer.h"
#include "keywords.h"
#include "parser.h"
#include "ast.h"
#include "codegen_c.h"
#include "diagnostics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

/* ANSI colors */
#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define CYAN    "\033[0;36m"
#define BOLD    "\033[1m"
#define RESET   "\033[0m"

/* ── File I/O ────────────────────────────────────────────────── */

static char *read_file(const char *path, size_t *out_len) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, RED "خطأ" RESET ": ما قدرت افتح الملف '%s'\n", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(len + 1);
    if (!buf) {
        fclose(f);
        fprintf(stderr, RED "خطأ" RESET ": ما فيه ذاكرة كافية\n");
        return NULL;
    }

    size_t read = fread(buf, 1, len, f);
    buf[read] = '\0';
    fclose(f);

    if (out_len) *out_len = read;
    return buf;
}

/* ── Derive output name ──────────────────────────────────────── */

static char *derive_output_name(const char *input) {
    /* "src/main.nsh" → "main" */
    const char *base = strrchr(input, '/');
    base = base ? base + 1 : input;

    int len = (int)strlen(base);
    /* strip .nsh extension */
    if (len > 4 && strcmp(base + len - 4, ".nsh") == 0) {
        len -= 4;
    }

    char *out = malloc(len + 1);
    memcpy(out, base, len);
    out[len] = '\0';
    return out;
}

/* ── Debug: Dump Tokens ──────────────────────────────────────── */

static int cmd_lex(const char *filename) {
    size_t len;
    char *source = read_file(filename, &len);
    if (!source) return 1;

    keywords_init();

    NshLexer lex;
    lexer_init(&lex, source, len, filename);

    printf("Tokens for %s:\n", filename);
    printf("%-6s %-25s %s\n", "LINE", "TYPE", "TEXT");
    printf("%-6s %-25s %s\n", "----", "----", "----");

    for (;;) {
        NshToken tok = lexer_next(&lex);
        if (tok.type == TOK_EOF) break;

        printf("%-6d %-25s %.*s\n",
               tok.span.line,
               token_type_name(tok.type),
               tok.length, tok.start);
    }

    free(source);
    keywords_free();
    return 0;
}

/* ── Build Command ───────────────────────────────────────────── */

static int cmd_build(const char *filename, int run_after) {
    size_t len;
    char *source = read_file(filename, &len);
    if (!source) return 1;

    keywords_init();
    diag_reset();

    /* Parse */
    NshNode *ast = parser_parse(source, len, filename);
    if (!ast) {
        fprintf(stderr, RED "\nفشل البناء" RESET " — %d أخطاء\n",
                diag_error_count());
        fprintf(stderr, "الله يهديك — Fix your code ya zalameh\n");
        free(source);
        keywords_free();
        return 1;
    }

    /* Generate C code to temp file */
    char c_path[] = "/tmp/nsh_XXXXXX.c";
    int fd = mkstemps(c_path, 2);
    if (fd < 0) {
        fprintf(stderr, RED "خطأ" RESET ": failed to create temp file\n");
        node_free(ast);
        free(source);
        keywords_free();
        return 1;
    }

    FILE *c_file = fdopen(fd, "w");
    if (codegen_c_emit(ast, c_file) != 0) {
        fclose(c_file);
        node_free(ast);
        free(source);
        keywords_free();
        return 1;
    }
    fclose(c_file);

    /* Compile C to binary */
    char *out_name = derive_output_name(filename);

    /* Find runtime directory — look relative to source or use env */
    char cmd[2048];
    const char *nashmic_root = getenv("NASHMIC_ROOT");
    if (!nashmic_root) {
        /* default: assume mansaf is run from project root or runtime is nearby */
        nashmic_root = ".";
    }

    snprintf(cmd, sizeof(cmd),
             "cc -std=c11 -O2 -o %s %s %s/runtime/nsh_runtime.c "
             "-I%s/runtime -lm 2>&1",
             out_name, c_path, nashmic_root, nashmic_root);

    int cc_result = system(cmd);
    if (cc_result != 0) {
        fprintf(stderr, RED "\nفشل التجميع" RESET " — C compiler failed\n");
        fprintf(stderr, "Generated C: %s\n", c_path);
        free(out_name);
        node_free(ast);
        free(source);
        keywords_free();
        return 1;
    }

    /* Clean up temp C file (keep on error for debugging) */
    remove(c_path);

    /* Get binary size */
    struct stat st;
    stat(out_name, &st);

    if (!run_after) {
        printf(GREEN "✓ تم!" RESET " Build succeeded — زي الفل 🇯🇴\n");
        printf("  Binary: ./%s (%.1f KB)\n", out_name, st.st_size / 1024.0);
    }

    if (run_after) {
        /* Run the binary */
        char run_cmd[512];
        snprintf(run_cmd, sizeof(run_cmd), "./%s", out_name);
        int run_result = system(run_cmd);

        /* Clean up binary after run */
        remove(out_name);

        free(out_name);
        node_free(ast);
        free(source);
        keywords_free();
        return WEXITSTATUS(run_result);
    }

    free(out_name);
    node_free(ast);
    free(source);
    keywords_free();
    return 0;
}

/* ── Usage ───────────────────────────────────────────────────── */

static void print_usage(void) {
    printf(BOLD "mansaf" RESET " — NashmiC Compiler 🇯🇴\n\n");
    printf("Usage:\n");
    printf("  mansaf build <file.nsh>    Compile to binary\n");
    printf("  mansaf run <file.nsh>      Compile and run\n");
    printf("  mansaf lex <file.nsh>      Dump tokens (debug)\n");
    printf("\n");
    printf("Examples:\n");
    printf("  mansaf run examples/marhaba.nsh\n");
    printf("  mansaf build src/main.nsh\n");
    printf("\n");
    printf("يلا نبلش! 🇯🇴\n");
}

/* ── Main ────────────────────────────────────────────────────── */

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage();
        return 0;
    }

    const char *command = argv[1];

    if (strcmp(command, "build") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: mansaf build <file.nsh>\n");
            return 1;
        }
        return cmd_build(argv[2], 0);
    }

    if (strcmp(command, "run") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: mansaf run <file.nsh>\n");
            return 1;
        }
        return cmd_build(argv[2], 1);
    }

    if (strcmp(command, "lex") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: mansaf lex <file.nsh>\n");
            return 1;
        }
        return cmd_lex(argv[2]);
    }

    if (strcmp(command, "help") == 0 || strcmp(command, "--help") == 0 ||
        strcmp(command, "-h") == 0) {
        print_usage();
        return 0;
    }

    fprintf(stderr, "Unknown command: %s\n", command);
    fprintf(stderr, "Run 'mansaf help' for usage.\n");
    return 1;
}
