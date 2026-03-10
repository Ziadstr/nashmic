/*
 * NashmiC — keywords.c
 * Franco-Arab keyword hash table
 *
 * NashmiC speaks Franco-Arab only. Arabic script identifiers are still
 * valid identifiers, but they're not keywords anymore.
 */

#include "keywords.h"
#include <string.h>
#include <stdlib.h>

/* ── Hash Table ─────────────────────────────────────────────── */

#define KW_TABLE_SIZE 512

typedef struct {
    const char *text;
    int text_len;
    NshTokenType type;
} KwEntry;

static KwEntry kw_table[KW_TABLE_SIZE];
static int kw_initialized = 0;

static uint32_t kw_hash(const char *text, int len) {
    uint32_t h = 2166136261u; /* FNV-1a */
    for (int i = 0; i < len; i++) {
        h ^= (unsigned char)text[i];
        h *= 16777619u;
    }
    return h;
}

static void kw_insert(const char *text, NshTokenType type) {
    int len = (int)strlen(text);
    uint32_t idx = kw_hash(text, len) % KW_TABLE_SIZE;

    /* linear probing */
    while (kw_table[idx].text != NULL) {
        idx = (idx + 1) % KW_TABLE_SIZE;
    }

    kw_table[idx].text = text;
    kw_table[idx].text_len = len;
    kw_table[idx].type = type;
}

void keywords_init(void) {
    if (kw_initialized) return;
    memset(kw_table, 0, sizeof(kw_table));

    /* ── Control Flow ── */
    kw_insert("iza",          TOK_IZA);
    kw_insert("wala",         TOK_WALA);
    kw_insert("wala_iza",     TOK_WALA_IZA);
    kw_insert("tool_ma",      TOK_TOOL_MA);
    kw_insert("lakol",        TOK_LAKOL);
    kw_insert("liff",         TOK_LIFF);
    kw_insert("khalas",       TOK_KHALAS);
    kw_insert("kammel",       TOK_KAMMEL);
    kw_insert("rajje3",       TOK_RAJJE3);
    kw_insert("hasab",        TOK_HASAB);
    kw_insert("hale",         TOK_HALE);
    kw_insert("3adi",         TOK_3ADI);

    /* ── Types ── */
    kw_insert("adad",         TOK_ADAD);
    kw_insert("adad64",       TOK_ADAD64);
    kw_insert("fasle",        TOK_FASLE);
    kw_insert("fasle64",      TOK_FASLE64);
    kw_insert("mante2",       TOK_MANTE2);
    kw_insert("harf",         TOK_HARF);
    kw_insert("bait",         TOK_BAIT);
    kw_insert("nass",         TOK_NASS);
    kw_insert("fadi",         TOK_FADI);
    kw_insert("mu2shr",       TOK_MU2SHR);
    kw_insert("saff",         TOK_SAFF);
    kw_insert("haikal",       TOK_HAIKAL);
    kw_insert("ta3dad",       TOK_TA3DAD);

    /* ── Values ── */
    kw_insert("ah",           TOK_AH);
    kw_insert("la",           TOK_LA);
    kw_insert("wala_ishi",    TOK_WALA_ISHI);

    /* ── Declarations ── */
    kw_insert("khalli",       TOK_KHALLI);
    kw_insert("thabet",       TOK_THABET);
    kw_insert("dalle",        TOK_DALLE);
    kw_insert("aam",          TOK_AAM);
    kw_insert("khas",         TOK_KHAS);
    kw_insert("sadder",       TOK_SADDER);
    kw_insert("jeeb",         TOK_JEEB);
    kw_insert("qesm",         TOK_QESM);
    kw_insert("kharji",       TOK_KHARJI);
    kw_insert("jdeed",        TOK_JDEED);
    kw_insert("fukk",         TOK_FUKK);

    /* ── Memory ── */
    kw_insert("ihjez",        TOK_IHJEZ);
    kw_insert("hares",        TOK_HARES);
    kw_insert("3al_3amyani",  TOK_3AL_3AMYANI);

    /* ── Error Handling ── */
    kw_insert("jarreb",       TOK_JARREB);
    kw_insert("imsek",        TOK_IMSEK);
    kw_insert("irmi",         TOK_IRMI);
    kw_insert("faz3a",        TOK_FAZ3A);
    kw_insert("natije",       TOK_NATIJE);
    kw_insert("tamam",        TOK_TAMAM);
    kw_insert("ghalat",       TOK_GHALAT);
    kw_insert("ba3dain",      TOK_BA3DAIN);
    kw_insert("binhayto",     TOK_BINHAYTO);

    /* ── Logical Operators ── */
    kw_insert("w",            TOK_W);
    kw_insert("aw",           TOK_AW);
    kw_insert("mish",         TOK_MISH);
    kw_insert("hajm",         TOK_HAJM);
    kw_insert("no3",          TOK_NO3);

    /* ── Concurrency ── */
    kw_insert("shaghghel",    TOK_SHAGHGHEL);
    kw_insert("istanna",      TOK_ISTANNA);
    kw_insert("qanah",        TOK_QANAH);
    kw_insert("ib3ath",       TOK_IB3ATH);
    kw_insert("istalam",      TOK_ISTALAM);
    kw_insert("i2fel",        TOK_I2FEL);
    kw_insert("iftah",        TOK_IFTAH);
    kw_insert("3al_tayer",    TOK_3AL_TAYER);
    kw_insert("khait",        TOK_KHAIT);

    /* ── I/O & Common ── */
    kw_insert("itba3",        TOK_ITBA3);
    kw_insert("i2ra",         TOK_I2RA);
    kw_insert("itla3",        TOK_ITLA3);
    kw_insert("akked",        TOK_AKKED);
    kw_insert("fahas",        TOK_FAHAS);

    /* ── Extra Jordanian ── */
    kw_insert("yalla",        TOK_YALLA);
    kw_insert("had",          TOK_HAD);
    kw_insert("tabe2",        TOK_TABE2);
    kw_insert("sammi",        TOK_SAMMI);
    kw_insert("tabbe2",       TOK_TABBE2);
    kw_insert("sife",         TOK_SIFE);
    kw_insert("7utt",         TOK_7UTT);
    kw_insert("sheel",        TOK_SHEEL);
    kw_insert("zeed",         TOK_ZEED);
    kw_insert("zanakh",       TOK_ZANAKH);

    /* ── Optional / Result extras ── */
    kw_insert("yimkin",       TOK_YIMKIN);
    kw_insert("fi",           TOK_FI);
    kw_insert("mafi",         TOK_MAFI);

    kw_initialized = 1;
}

NshTokenType keywords_lookup(const char *text, int len) {
    uint32_t idx = kw_hash(text, len) % KW_TABLE_SIZE;

    for (int probe = 0; probe < KW_TABLE_SIZE; probe++) {
        KwEntry *e = &kw_table[idx];
        if (e->text == NULL) return TOK_IDENT;

        if (e->text_len == len && memcmp(e->text, text, len) == 0) {
            return e->type;
        }

        idx = (idx + 1) % KW_TABLE_SIZE;
    }

    return TOK_IDENT;
}

void keywords_free(void) {
    /* static table — nothing to free, but reset for re-init */
    memset(kw_table, 0, sizeof(kw_table));
    kw_initialized = 0;
}
