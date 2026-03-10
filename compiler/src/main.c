/*
 * mansaf — NashmiC Compiler CLI
 *
 * Usage:
 *   mansaf build <file.nsh>     Compile to binary
 *   mansaf run <file.nsh>       Compile and run
 *   mansaf lex <file.nsh>       Debug: dump tokens
 *
 * NashmiC: its own language — easy like Python, powerful like Go,
 * Jordanian to the core.
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
#include <time.h>

/* ANSI colors */
#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define CYAN    "\033[0;36m"
#define YELLOW  "\033[1;33m"
#define MAGENTA "\033[1;35m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define RESET   "\033[0m"

/* ── Build Success Quips ─────────────────────────────────────── */

static const char *SUCCESS_QUIPS[] = {
    "اسرع من طلقة",
    "زي السيف",
    "والله ما قصرت",
    "زي الفل",
    "يا سلام عليك",
    "نشمي والله",
    "تمام التمام",
    "عنجد رهيب",
};
#define QUIP_COUNT 8

static const char *random_quip(void) {
    static int seeded = 0;
    if (!seeded) { srand((unsigned)time(NULL)); seeded = 1; }
    return SUCCESS_QUIPS[rand() % QUIP_COUNT];
}

/* ── First Build Banner ──────────────────────────────────────── */

static void print_first_build_banner(void) {
    printf("\n");
    printf(YELLOW "   ███╗   ██╗ █████╗ ███████╗██╗  ██╗███╗   ███╗██╗ ██████╗\n" RESET);
    printf(YELLOW "   ████╗  ██║██╔══██╗██╔════╝██║  ██║████╗ ████║██║██╔════╝\n" RESET);
    printf(YELLOW "   ██╔██╗ ██║███████║███████╗███████║██╔████╔██║██║██║     \n" RESET);
    printf(YELLOW "   ██║╚██╗██║██╔══██║╚════██║██╔══██║██║╚██╔╝██║██║██║     \n" RESET);
    printf(YELLOW "   ██║ ╚████║██║  ██║███████║██║  ██║██║ ╚═╝ ██║██║╚██████╗\n" RESET);
    printf(YELLOW "   ╚═╝  ╚═══╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═╝     ╚═╝╚═╝ ╚═════╝\n" RESET);
    printf("\n");
    printf(DIM "   مـنـسـف — NashmiC Compiler" RESET "\n");
    printf(DIM "   Easy like Python. Powerful like Go. Jordanian to the bone." RESET "\n");
    printf("\n");
}

/* Check if this is the first build ever (.nashmic_first_build marker) */
static int is_first_build(void) {
    const char *home = getenv("HOME");
    if (!home) return 0;
    char path[512];
    snprintf(path, sizeof(path), "%s/.nashmic_first_build", home);
    if (access(path, F_OK) == 0) return 0;
    /* Create marker */
    FILE *f = fopen(path, "w");
    if (f) { fprintf(f, "nashmi\n"); fclose(f); }
    return 1;
}

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

    size_t read_bytes = fread(buf, 1, len, f);
    buf[read_bytes] = '\0';
    fclose(f);

    if (out_len) *out_len = read_bytes;
    return buf;
}

/* ── Derive output name ──────────────────────────────────────── */

static char *derive_output_name(const char *input) {
    const char *base = strrchr(input, '/');
    base = base ? base + 1 : input;

    int len = (int)strlen(base);
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

static int cmd_build(const char *filename, int run_after, int tarab) {
    size_t len;
    char *source = read_file(filename, &len);
    if (!source) return 1;

    keywords_init();
    diag_reset();

    /* Parse */
    NshNode *ast = parser_parse(source, len, filename);
    if (!ast) {
        fprintf(stderr, RED "\nفشل البناء" RESET " — %d errors\n",
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

    const char *nashmic_root = getenv("NASHMIC_ROOT");
    if (!nashmic_root) {
        nashmic_root = ".";
    }

    char cmd[2048];
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

    /* Clean up temp C file */
    remove(c_path);

    /* Get binary size */
    struct stat st;
    stat(out_name, &st);

    if (!run_after) {
        /* First build banner */
        if (is_first_build()) {
            print_first_build_banner();
        }

        printf(GREEN "✓ تم!" RESET " %s\n", random_quip());
        printf("  Binary: ./%s (%.1f KB)\n", out_name, st.st_size / 1024.0);

        /* --tarab easter egg */
        if (tarab) {
            printf(MAGENTA "  🎵 طرب mode activated..." RESET "\n");
            int tarab_ret = system("which aplay >/dev/null 2>&1 && "
                                   "echo 'No audio file yet — but the spirit is there!' || "
                                   "echo '  aplay not found — install alsa-utils for tarab'");
            (void)tarab_ret;
        }
    }

    if (run_after) {
        char run_cmd[512];
        snprintf(run_cmd, sizeof(run_cmd), "./%s", out_name);
        int run_result = system(run_cmd);

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
    printf(BOLD "mansaf" RESET " — NashmiC Compiler\n\n");
    printf("Usage:\n");
    printf("  mansaf build <file.nsh>           Compile to binary\n");
    printf("  mansaf build --tarab <file.nsh>   Compile with celebration\n");
    printf("  mansaf run <file.nsh>             Compile and run\n");
    printf("  mansaf lex <file.nsh>             Dump tokens (debug)\n");
    printf("\n");
    printf("Examples:\n");
    printf("  mansaf run examples/marhaba.nsh\n");
    printf("  mansaf build src/main.nsh\n");
    printf("\n");
    printf("NashmiC: easy like Python, powerful like Go, Jordanian to the bone.\n");
    printf("يلا نبلش!\n");
}

/* ── Main ────────────────────────────────────────────────────── */

int main(int argc, char **argv) {
    if (argc < 2) {
        print_usage();
        return 0;
    }

    const char *command = argv[1];

    if (strcmp(command, "build") == 0) {
        int tarab = 0;
        const char *filename = NULL;

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "--tarab") == 0) {
                tarab = 1;
            } else {
                filename = argv[i];
            }
        }

        if (!filename) {
            fprintf(stderr, "Usage: mansaf build [--tarab] <file.nsh>\n");
            return 1;
        }
        return cmd_build(filename, 0, tarab);
    }

    if (strcmp(command, "run") == 0) {
        if (argc < 3) {
            fprintf(stderr, "Usage: mansaf run <file.nsh>\n");
            return 1;
        }
        return cmd_build(argv[2], 1, 0);
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
