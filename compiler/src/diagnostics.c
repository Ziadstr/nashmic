/*
 * NashmiC — diagnostics.c
 * Jordanian-flavored compiler diagnostics with source context and proverbs
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
#include <stdlib.h>
#include <time.h>

/* ANSI colors */
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define CYAN    "\033[0;36m"
#define GREEN   "\033[1;32m"
#define MAGENTA "\033[1;35m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define RESET   "\033[0m"

static int error_count = 0;
static int proverb_seeded = 0;

/* ── Jordanian Proverbs for Errors ───────────────────────────── */

typedef struct {
    const char *keyword;     /* partial match in error message */
    const char *proverb;
} ProverbEntry;

static const ProverbEntry PROVERBS[] = {
    {"type",          "حبل الكذب قصير — the rope of lies is short"},
    {"mismatch",      "حبل الكذب قصير — the rope of lies is short"},
    {"argument",      "عذر الراقصة إنو الأرض مايلة — the dancer blames the floor"},
    {"parameter",     "عذر الراقصة إنو الأرض مايلة — the dancer blames the floor"},
    {"unreachable",   "اللي ما بوصل للعنب بقول عنه حامض — sour grapes"},
    {"overflow",      "اللي بحفر حفرة لأخوه بقع فيها — dig a pit, fall in it"},
    {"unused",        "بلجيكي — Belgian complexity, unnecessarily complex"},
    {"unhandled",     "شايف حالك — you think you're safe?"},
    {"error",         "شايف حالك — you think you're safe?"},
    {"catch",         "امسكت الغلط بس ما عملت اشي — caught the error but did nothing"},
    {"expected",      "اصبر على الحصرم تاكله عنب — patience turns sour to sweet"},
    {"undefined",     "اللي ما عنده قديم ما عنده جديد — no old? no new either"},
    {"expression",    "الحكي ببلاش والكلام ببطلاش — talk is cheap"},
    {NULL, NULL}
};

static const char *find_proverb(const char *message) {
    for (int i = 0; PROVERBS[i].keyword; i++) {
        if (strstr(message, PROVERBS[i].keyword)) {
            return PROVERBS[i].proverb;
        }
    }
    return NULL;
}

/* General-purpose error proverbs (random selection for unmatched errors) */
static const char *GENERAL_PROVERBS[] = {
    "اللي ما بعرف يرقص بقول الأرض عوجة",
    "الله يعين اللي ما عنده عين",
    "الصبر مفتاح الفرج",
    "من طلع من داره قل مقداره",
    "اللي بده يعيش بالجبل لازم يتحمل البرد",
};
#define GENERAL_PROVERB_COUNT 5

static const char *random_general_proverb(void) {
    if (!proverb_seeded) {
        srand((unsigned)time(NULL));
        proverb_seeded = 1;
    }
    return GENERAL_PROVERBS[rand() % GENERAL_PROVERB_COUNT];
}

/* ── Source Context ──────────────────────────────────────────── */

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

/* ── Public API ──────────────────────────────────────────────── */

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

    /* Proverb */
    const char *proverb = find_proverb(message);
    if (!proverb) proverb = random_general_proverb();
    fprintf(stderr, MAGENTA "      = %s" RESET "\n", proverb);

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
