/*
 * NashmiC — repl.c
 * Interactive Read-Eval-Print Loop
 *
 * Type NashmiC code line by line. Multi-line input is supported via
 * brace depth tracking. Top-level declarations (dalle, haikal, ta3dad,
 * tabbe2) accumulate across evaluations. Everything else gets wrapped
 * in yalla() { ... } and compiled+run on the spot.
 */

#define _GNU_SOURCE
#include "repl.h"
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
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <libgen.h>
#include <limits.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

/* Try to use readline for line editing + history.
 * HAVE_READLINE can be set via -DHAVE_READLINE=1 or auto-detected via __has_include. */
#ifndef HAVE_READLINE
#ifdef __has_include
#if __has_include(<readline/readline.h>)
#define HAVE_READLINE 1
#endif
#endif
#endif

#ifdef HAVE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

/* ── ANSI Colors ─────────────────────────────────────────────── */

#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define CYAN    "\033[0;36m"
#define YELLOW  "\033[1;33m"
#define MAGENTA "\033[1;35m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define RESET   "\033[0m"

/* ── Constants ───────────────────────────────────────────────── */

#define REPL_PROMPT      CYAN "nashmi> " RESET
#define REPL_CONT_PROMPT DIM  "...     " RESET
#define INPUT_BUF_INIT   1024
#define DECL_BUF_INIT    4096

/* Top-level declaration keywords (Franco-Arab) */
static const char *TOPLEVEL_KEYWORDS[] = {
    "dalle", "haikal", "ta3dad", "tabbe2",
    NULL
};

/* ── Signal Handling ─────────────────────────────────────────── */

static volatile sig_atomic_t repl_interrupted = 0;

static void repl_sigint_handler(int sig) {
    (void)sig;
    repl_interrupted = 1;
    /* Write a newline so the prompt reappears cleanly */
    ssize_t __attribute__((unused)) w = write(STDOUT_FILENO, "\n", 1);
#ifdef HAVE_READLINE
    rl_on_new_line();
    rl_redisplay();
#endif
}

/* ── Dynamic String Buffer ───────────────────────────────────── */

typedef struct {
    char *data;
    size_t len;
    size_t cap;
} StrBuf;

static void strbuf_init(StrBuf *buf, size_t initial_cap) {
    buf->cap = initial_cap > 0 ? initial_cap : 256;
    buf->data = malloc(buf->cap);
    buf->data[0] = '\0';
    buf->len = 0;
}

static void strbuf_append(StrBuf *buf, const char *str) {
    size_t slen = strlen(str);
    while (buf->len + slen + 1 > buf->cap) {
        buf->cap *= 2;
        buf->data = realloc(buf->data, buf->cap);
    }
    memcpy(buf->data + buf->len, str, slen + 1);
    buf->len += slen;
}

static void strbuf_clear(StrBuf *buf) {
    buf->data[0] = '\0';
    buf->len = 0;
}

static void strbuf_free(StrBuf *buf) {
    free(buf->data);
    buf->data = NULL;
    buf->len = 0;
    buf->cap = 0;
}

/* ── Helpers ─────────────────────────────────────────────────── */

/* Read a line from stdin. Returns allocated string or NULL on EOF.
 * Uses readline if available, fgets otherwise. */
static char *read_line(const char *prompt) {
#ifdef HAVE_READLINE
    char *line = readline(prompt);
    if (line && *line) {
        add_history(line);
    }
    return line;  /* NULL on EOF */
#else
    printf("%s", prompt);
    fflush(stdout);

    char buf[4096];
    if (!fgets(buf, sizeof(buf), stdin)) {
        return NULL;  /* EOF */
    }

    /* Strip trailing newline */
    size_t blen = strlen(buf);
    if (blen > 0 && buf[blen - 1] == '\n') {
        buf[blen - 1] = '\0';
    }

    return strdup(buf);
#endif
}

/* Count net brace depth change in a line (skipping strings/comments) */
static int count_brace_delta(const char *line) {
    int delta = 0;
    int in_string = 0;
    int in_line_comment = 0;

    for (const char *p = line; *p; p++) {
        if (in_line_comment) break;

        if (*p == '"' && !in_line_comment) {
            in_string = !in_string;
            continue;
        }

        if (in_string) {
            if (*p == '\\' && *(p + 1)) p++;  /* skip escaped char */
            continue;
        }

        if (*p == '/' && *(p + 1) == '/') {
            in_line_comment = 1;
            continue;
        }

        if (*p == '{') delta++;
        if (*p == '}') delta--;
    }

    return delta;
}

/* Check if a line starts with a top-level declaration keyword */
static int is_toplevel_decl(const char *line) {
    /* Skip leading whitespace */
    while (*line == ' ' || *line == '\t') line++;

    for (const char **kw = TOPLEVEL_KEYWORDS; *kw; kw++) {
        size_t kwlen = strlen(*kw);
        if (strncmp(line, *kw, kwlen) == 0) {
            char after = line[kwlen];
            if (after == ' ' || after == '\t' || after == '(' ||
                after == '{' || after == '\0' || after == '\n') {
                return 1;
            }
        }
    }

    return 0;
}

/* Check if a line is a REPL command (starts with :) */
static int is_repl_command(const char *line) {
    while (*line == ' ' || *line == '\t') line++;
    return *line == ':';
}

/* Check if a line is empty or whitespace-only */
static int is_blank(const char *line) {
    while (*line) {
        if (*line != ' ' && *line != '\t' && *line != '\n' && *line != '\r')
            return 0;
        line++;
    }
    return 1;
}

/* ── NASHMIC_ROOT detection (same as main.c) ────────────────── */

static char *detect_nashmic_root(void) {
    char exe_path[PATH_MAX];

#ifdef __linux__
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len <= 0) return NULL;
    exe_path[len] = '\0';
#elif defined(__APPLE__)
    uint32_t size = sizeof(exe_path);
    if (_NSGetExecutablePath(exe_path, &size) != 0) return NULL;
    char resolved[PATH_MAX];
    if (!realpath(exe_path, resolved)) return NULL;
    strncpy(exe_path, resolved, PATH_MAX - 1);
    exe_path[PATH_MAX - 1] = '\0';
#else
    return NULL;
#endif

    char *dir = dirname(exe_path);
    char *parent = dirname(dir);

    char check_path[PATH_MAX];
    snprintf(check_path, sizeof(check_path), "%s/runtime/nsh_runtime.c", parent);

    if (access(check_path, F_OK) == 0) {
        return strdup(parent);
    }

    snprintf(check_path, sizeof(check_path),
             "/usr/local/share/nashmic/runtime/nsh_runtime.c");
    if (access(check_path, F_OK) == 0) {
        return strdup("/usr/local/share/nashmic");
    }

    return NULL;
}

/* ── Compile and Run ─────────────────────────────────────────── */

/* Build a complete .nsh source from accumulated declarations + code,
 * parse, codegen, compile with cc, run, and clean up. Returns 0 on success. */
static int repl_compile_and_run(const char *declarations, const char *code,
                                const char *nashmic_root) {
    /* Build full source: declarations + yalla() { code } */
    StrBuf source;
    strbuf_init(&source, 2048);

    if (declarations && *declarations) {
        strbuf_append(&source, declarations);
        strbuf_append(&source, "\n");
    }

    strbuf_append(&source, "yalla() {\n");
    strbuf_append(&source, code);
    strbuf_append(&source, "\n}\n");

    /* Write to temp .nsh file */
    char nsh_path[] = "/tmp/nsh_repl_XXXXXX.nsh";
    int fd = mkstemps(nsh_path, 4);
    if (fd < 0) {
        fprintf(stderr, RED "خطأ" RESET ": failed to create temp file\n");
        strbuf_free(&source);
        return 1;
    }

    FILE *nsh_file = fdopen(fd, "w");
    fprintf(nsh_file, "%s", source.data);
    fclose(nsh_file);

    /* Parse */
    keywords_init();
    diag_reset();

    NshNode *ast = parser_parse(source.data, source.len, "<repl>");
    if (!ast) {
        fprintf(stderr, RED "خطأ" RESET ": parse failed\n");
        strbuf_free(&source);
        keywords_free();
        remove(nsh_path);
        return 1;
    }

    /* Semantic analysis (soft — warn only) */
    sema_check(ast, source.data, "<repl>");

    /* Generate C */
    char c_path[] = "/tmp/nsh_repl_XXXXXX.c";
    int c_fd = mkstemps(c_path, 2);
    if (c_fd < 0) {
        fprintf(stderr, RED "خطأ" RESET ": failed to create temp C file\n");
        node_free(ast);
        strbuf_free(&source);
        keywords_free();
        remove(nsh_path);
        return 1;
    }

    FILE *c_file = fdopen(c_fd, "w");
    if (codegen_c_emit(ast, c_file) != 0) {
        fprintf(stderr, RED "خطأ" RESET ": codegen failed\n");
        fclose(c_file);
        node_free(ast);
        strbuf_free(&source);
        keywords_free();
        remove(nsh_path);
        remove(c_path);
        return 1;
    }
    fclose(c_file);

    /* Compile to binary */
    char bin_path[] = "/tmp/nsh_repl_XXXXXX";
    int bin_fd = mkstemp(bin_path);
    if (bin_fd < 0) {
        fprintf(stderr, RED "خطأ" RESET ": failed to create temp binary\n");
        node_free(ast);
        strbuf_free(&source);
        keywords_free();
        remove(nsh_path);
        remove(c_path);
        return 1;
    }
    close(bin_fd);

    char cmd[2048];
    snprintf(cmd, sizeof(cmd),
             "cc -std=gnu11 -O2 -o %s %s %s/runtime/nsh_runtime.c "
             "-I%s/runtime -lm 2>&1",
             bin_path, c_path, nashmic_root, nashmic_root);

    int cc_result = system(cmd);
    if (cc_result != 0) {
        fprintf(stderr, RED "خطأ" RESET ": C compilation failed\n");
        node_free(ast);
        strbuf_free(&source);
        keywords_free();
        remove(nsh_path);
        remove(c_path);
        remove(bin_path);
        return 1;
    }

    /* Run */
    int run_result = system(bin_path);
    (void)WEXITSTATUS(run_result);

    /* Clean up */
    remove(nsh_path);
    remove(c_path);
    remove(bin_path);
    node_free(ast);
    strbuf_free(&source);
    keywords_free();

    return 0;
}

/* ── Welcome Banner ──────────────────────────────────────────── */

static void print_repl_banner(void) {
    printf("\n");
    printf(YELLOW "  ╔═══════════════════════════════════════════╗\n" RESET);
    printf(YELLOW "  ║" RESET BOLD "   NashmiC Interactive REPL" RESET
           YELLOW "                ║\n" RESET);
    printf(YELLOW "  ║" RESET DIM "   اكتب كود و شوف النتيجة على طول" RESET
           YELLOW "       ║\n" RESET);
    printf(YELLOW "  ╚═══════════════════════════════════════════╝\n" RESET);
    printf("\n");
    printf(DIM "  Commands:  :q / :quit    Exit\n" RESET);
    printf(DIM "             :clear        Reset declarations\n" RESET);
    printf(DIM "             :decls        Show accumulated declarations\n" RESET);
    printf(DIM "             :help         Show this help\n" RESET);
    printf("\n");
}

/* ── REPL Loop ───────────────────────────────────────────────── */

int cmd_repl(void) {
    /* Detect NASHMIC_ROOT */
    const char *nashmic_root = getenv("NASHMIC_ROOT");
    char *detected_root = NULL;
    if (!nashmic_root) {
        detected_root = detect_nashmic_root();
        nashmic_root = detected_root ? detected_root : ".";
    }

    /* Verify runtime exists */
    char runtime_check[PATH_MAX];
    snprintf(runtime_check, sizeof(runtime_check),
             "%s/runtime/nsh_runtime.c", nashmic_root);
    if (access(runtime_check, F_OK) != 0) {
        fprintf(stderr, RED "خطأ" RESET ": runtime not found at %s\n",
                nashmic_root);
        fprintf(stderr, "Set NASHMIC_ROOT or run from the project directory.\n");
        free(detected_root);
        return 1;
    }

    /* Install signal handler for Ctrl+C */
    struct sigaction sa;
    sa.sa_handler = repl_sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    print_repl_banner();

    /* Accumulated top-level declarations */
    StrBuf declarations;
    strbuf_init(&declarations, DECL_BUF_INIT);

    /* Current input buffer */
    StrBuf input;
    strbuf_init(&input, INPUT_BUF_INIT);

    int brace_depth = 0;
    int collecting_multiline = 0;

    for (;;) {
        repl_interrupted = 0;

        const char *prompt = collecting_multiline ? REPL_CONT_PROMPT : REPL_PROMPT;
        char *line = read_line(prompt);

        /* NULL = EOF (Ctrl+D) */
        if (!line) {
            printf("\n" DIM "مع السلامة!" RESET "\n");
            break;
        }

        /* Ctrl+C was pressed during input */
        if (repl_interrupted) {
            free(line);
            strbuf_clear(&input);
            brace_depth = 0;
            collecting_multiline = 0;
            continue;
        }

        /* Handle REPL commands (only at top level, not mid-multiline) */
        if (!collecting_multiline && is_repl_command(line)) {
            const char *cmd = line;
            while (*cmd == ' ' || *cmd == '\t') cmd++;

            if (strcmp(cmd, ":q") == 0 || strcmp(cmd, ":quit") == 0) {
                free(line);
                printf(DIM "مع السلامة!" RESET "\n");
                break;
            }

            if (strcmp(cmd, ":clear") == 0) {
                strbuf_clear(&declarations);
                printf(DIM "تم مسح التعريفات" RESET "\n");
                free(line);
                continue;
            }

            if (strcmp(cmd, ":decls") == 0) {
                if (declarations.len == 0) {
                    printf(DIM "ما فيه تعريفات محفوظة" RESET "\n");
                } else {
                    printf(DIM "── Accumulated declarations ──" RESET "\n");
                    printf("%s\n", declarations.data);
                    printf(DIM "──────────────────────────────" RESET "\n");
                }
                free(line);
                continue;
            }

            if (strcmp(cmd, ":help") == 0) {
                printf(DIM "  :q / :quit    Exit the REPL\n" RESET);
                printf(DIM "  :clear        Reset accumulated declarations\n" RESET);
                printf(DIM "  :decls        Show accumulated declarations\n" RESET);
                printf(DIM "  :help         Show this help\n" RESET);
                free(line);
                continue;
            }

            fprintf(stderr, YELLOW "Unknown command" RESET ": %s\n", cmd);
            free(line);
            continue;
        }

        /* Skip blank lines at top level */
        if (!collecting_multiline && is_blank(line)) {
            free(line);
            continue;
        }

        /* Accumulate input */
        if (input.len > 0) {
            strbuf_append(&input, "\n");
        }
        strbuf_append(&input, line);

        /* Track brace depth */
        int delta = count_brace_delta(line);
        brace_depth += delta;

        free(line);

        /* If braces opened, we're in multiline mode */
        if (brace_depth > 0) {
            collecting_multiline = 1;
            continue;
        }

        /* If brace depth went negative, that's a syntax error — let the parser catch it */
        if (brace_depth < 0) {
            brace_depth = 0;
        }

        /* Input is complete — either single line or braces balanced */
        collecting_multiline = 0;

        /* Determine if this is a top-level declaration */
        if (is_toplevel_decl(input.data)) {
            strbuf_append(&declarations, input.data);
            strbuf_append(&declarations, "\n");
            printf(DIM "✓ declaration saved" RESET "\n");
        } else {
            /* Wrap in yalla() and compile+run */
            repl_compile_and_run(declarations.data, input.data, nashmic_root);
        }

        strbuf_clear(&input);
        brace_depth = 0;
    }

    strbuf_free(&input);
    strbuf_free(&declarations);
    free(detected_root);

    return 0;
}
