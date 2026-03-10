/*
 * NashmiC Runtime — nsh_runtime.c
 * Core runtime + cultural easter eggs
 */

#define _GNU_SOURCE
#include "nsh_runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <locale.h>
#include <time.h>

void nsh_runtime_init(void) {
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
    ssize_t nread = getline(&line, &len, stdin);
    if (nread == -1) {
        free(line);
        return NULL;
    }
    if (nread > 0 && line[nread - 1] == '\n') {
        line[nread - 1] = '\0';
    }
    return line;
}

void nsh_itla3(int code) {
    nsh_runtime_shutdown();
    exit(code);
}

/* ── Easter Eggs ─────────────────────────────────────────────── */

static const char *PROVERBS[] = {
    "اللي ما بعرف يرقص بقول الأرض عوجة",
    "الصبر مفتاح الفرج",
    "اصبر على الحصرم تاكله عنب",
    "حبل الكذب قصير",
    "اللي بحفر حفرة لأخوه بقع فيها",
    "الجار قبل الدار",
    "اللي إيده بالمية مش زي اللي إيده بالنار",
    "عين الحسود فيها عود",
    "القرد بعين أمه غزال",
    "من طلع من داره قل مقداره",
    "الصديق وقت الضيق",
    "كل شي بوقته حلو",
    "العلم نور والجهل ظلام",
    "اللي بدو العسل بتحمل قرص النحل",
    "يا غريب كون أديب",
};
#define PROVERB_COUNT 15

static int drobi_count = 0;
static int drobi_seeded = 0;

void nsh_drobi(void) {
    if (!drobi_seeded) {
        srand((unsigned)time(NULL));
        drobi_seeded = 1;
    }

    drobi_count++;
    if (drobi_count > 5) {
        printf("يا زلمة كفاية ضحك — get back to coding\n");
        return;
    }

    printf("%s\n", PROVERBS[rand() % PROVERB_COUNT]);
}

void nsh_mansaf(void) {
    printf("\n");
    printf("  ╔══════════════════════════════════════╗\n");
    printf("  ║         🍚 وصفة المنسف 🍚           ║\n");
    printf("  ╠══════════════════════════════════════╣\n");
    printf("  ║  المقادير:                           ║\n");
    printf("  ║  • ٢ كيلو لحم غنم                   ║\n");
    printf("  ║  • ٤ أكواب جميد                     ║\n");
    printf("  ║  • ٣ أكواب أرز                      ║\n");
    printf("  ║  • خبز شراك                         ║\n");
    printf("  ║  • سمن بلدي، لوز وصنوبر             ║\n");
    printf("  ╠══════════════════════════════════════╣\n");
    printf("  ║  الطريقة:                            ║\n");
    printf("  ║  ١. اسلق اللحم مع البهارات          ║\n");
    printf("  ║  ٢. ذوّب الجميد واطبخه مع المرق     ║\n");
    printf("  ║  ٣. اطبخ الأرز بمرق اللحم           ║\n");
    printf("  ║  ٤. رتّب: شراك، أرز، لحم            ║\n");
    printf("  ║  ٥. زيّن باللوز والصنوبر المحمّص    ║\n");
    printf("  ║  ٦. صبّ الجميد على الوجه             ║\n");
    printf("  ╠══════════════════════════════════════╣\n");
    printf("  ║        صحتين وعافية 🇯🇴              ║\n");
    printf("  ╚══════════════════════════════════════╝\n");
    printf("\n");
}

void nsh_sahteen(void) {
    printf("صحتين — bon appetit! 🍽️\n");
}

void nsh_nashmi(void) {
    printf("\n");
    printf("  NashmiC — نشمي\n");
    printf("  ═══════════════\n");
    printf("  A programming language from Jordan.\n");
    printf("  Easy like Python. Powerful like Go.\n");
    printf("  Jordanian to the bone.\n");
    printf("\n");
    printf("  نشمي (Nashmi) = brave, noble, honorable.\n");
    printf("  The highest compliment in Jordanian culture.\n");
    printf("\n");
    printf("  Features:\n");
    printf("  • Franco-Arab keywords — code the way you text\n");
    printf("  • natije<T> + wala? — error handling with personality\n");
    printf("  • yimkin<T> — no null, ever\n");
    printf("  • String interpolation — \"marhaba ya {name}\"\n");
    printf("  • Compiler proverbs — learn wisdom with your errors\n");
    printf("  • drobi() — when you need a laugh\n");
    printf("\n");
    printf("  Built by Ziad Al-Sharif 🇯🇴\n");
    printf("\n");
}
