/*
 * NashmiC — keywords.c
 * Dual-mode keyword hash table: Arabic script + Franco-Arab Latin
 *
 * Simple open-addressing hash table. Each keyword is registered twice —
 * once in Arabic script, once in Franco transliteration.
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

/* Register a keyword with both Arabic and Franco spellings */
#define DUAL(arabic, franco, tok) do { \
    kw_insert(arabic, tok);            \
    kw_insert(franco, tok);            \
} while (0)

void keywords_init(void) {
    if (kw_initialized) return;
    memset(kw_table, 0, sizeof(kw_table));

    /* ── Control Flow ── */
    DUAL("اذا",       "iza",          TOK_IZA);
    DUAL("والا",      "wala",         TOK_WALA);
    DUAL("والا_اذا",  "wala_iza",     TOK_WALA_IZA);
    DUAL("طول_ما",    "tool_ma",      TOK_TOOL_MA);
    DUAL("لكل",       "lakol",        TOK_LAKOL);
    DUAL("لف",        "liff",         TOK_LIFF);
    DUAL("خلص",       "khalas",       TOK_KHALAS);
    DUAL("كمل",       "kammel",       TOK_KAMMEL);
    DUAL("رجع",       "rajje3",       TOK_RAJJE3);
    DUAL("حسب",       "hasab",        TOK_HASAB);
    DUAL("حالة",      "hale",         TOK_HALE);
    DUAL("عادي",      "3adi",         TOK_3ADI);

    /* ── Types ── */
    DUAL("عدد",       "adad",         TOK_ADAD);
    DUAL("عدد٦٤",    "adad64",       TOK_ADAD64);
    DUAL("فاصلة",     "fasle",        TOK_FASLE);
    DUAL("فاصلة٦٤",  "fasle64",      TOK_FASLE64);
    DUAL("منطق",      "mante2",       TOK_MANTE2);
    DUAL("حرف",       "harf",         TOK_HARF);
    DUAL("بايت",      "bait",         TOK_BAIT);
    DUAL("نص",        "nass",         TOK_NASS);
    DUAL("فاضي",      "fadi",         TOK_FADI);
    DUAL("مؤشر",      "mu2shr",       TOK_MU2SHR);
    DUAL("صف",        "saff",         TOK_SAFF);
    DUAL("هيكل",      "haikal",       TOK_HAIKAL);
    DUAL("تعداد",     "ta3dad",       TOK_TA3DAD);

    /* ── Values ── */
    DUAL("اه",        "ah",           TOK_AH);
    DUAL("لا",        "la",           TOK_LA);
    DUAL("ولا_اشي",   "wala_ishi",    TOK_WALA_ISHI);

    /* ── Declarations ── */
    DUAL("خلي",       "khalli",       TOK_KHALLI);
    DUAL("ثابت",      "thabet",       TOK_THABET);
    DUAL("دالة",      "dalle",        TOK_DALLE);
    DUAL("عام",       "aam",          TOK_AAM);
    DUAL("خاص",       "khas",         TOK_KHAS);
    DUAL("صدر",       "sadder",       TOK_SADDER);
    DUAL("جيب",       "jeeb",         TOK_JEEB);
    DUAL("قسم",       "qesm",         TOK_QESM);
    DUAL("خارجي",     "kharji",       TOK_KHARJI);
    DUAL("جديد",      "jdeed",        TOK_JDEED);
    DUAL("فك",        "fukk",         TOK_FUKK);

    /* ── Memory ── */
    DUAL("احجز",      "ihjez",        TOK_IHJEZ);
    DUAL("حارس",      "hares",        TOK_HARES);
    DUAL("عالعمياني",  "3al_3amyani",  TOK_3AL_3AMYANI);

    /* ── Error Handling ── */
    DUAL("جرب",       "jarreb",       TOK_JARREB);
    DUAL("امسك",      "imsek",        TOK_IMSEK);
    DUAL("ارمي",      "irmi",         TOK_IRMI);
    DUAL("فزعة",      "faz3a",        TOK_FAZ3A);
    DUAL("نتيجة",     "natije",       TOK_NATIJE);
    DUAL("تمام",      "tamam",        TOK_TAMAM);
    DUAL("غلط",       "ghalat",       TOK_GHALAT);
    DUAL("بعدين",     "ba3dain",      TOK_BA3DAIN);
    DUAL("بنهايته",   "binhayto",     TOK_BINHAYTO);

    /* ── Logical Operators ── */
    DUAL("و",         "w",            TOK_W);
    DUAL("او",        "aw",           TOK_AW);
    DUAL("مش",        "mish",         TOK_MISH);
    DUAL("حجم",       "hajm",         TOK_HAJM);
    DUAL("نوع",       "no3",          TOK_NO3);

    /* ── Concurrency ── */
    DUAL("شغل",       "shaghghel",    TOK_SHAGHGHEL);
    DUAL("استنى",     "istanna",      TOK_ISTANNA);
    DUAL("قناة",      "qanah",        TOK_QANAH);
    DUAL("ابعث",      "ib3ath",       TOK_IB3ATH);
    DUAL("استلم",     "istalam",      TOK_ISTALAM);
    DUAL("اقفل",      "i2fel",        TOK_I2FEL);
    DUAL("افتح",      "iftah",        TOK_IFTAH);
    DUAL("عالطاير",   "3al_tayer",    TOK_3AL_TAYER);
    DUAL("خيط",       "khait",        TOK_KHAIT);

    /* ── I/O & Common ── */
    DUAL("اطبع",      "itba3",        TOK_ITBA3);
    DUAL("اقرأ",      "i2ra",         TOK_I2RA);
    DUAL("استنى",     "istanna",      TOK_ISTANNA);
    DUAL("اطلع",      "itla3",        TOK_ITLA3);
    DUAL("اكد",       "akked",        TOK_AKKED);
    DUAL("فحص",       "fahas",        TOK_FAHAS);

    /* ── Extra Jordanian ── */
    DUAL("يلا",       "yalla",        TOK_YALLA);
    DUAL("هاد",       "had",          TOK_HAD);
    DUAL("طابق",      "tabe2",        TOK_TABE2);
    DUAL("سمي",       "sammi",        TOK_SAMMI);
    DUAL("طبق",       "tabbe2",       TOK_TABBE2);
    DUAL("صفة",       "sife",         TOK_SIFE);
    DUAL("حط",        "7utt",         TOK_7UTT);
    DUAL("شيل",       "sheel",        TOK_SHEEL);
    DUAL("زيد",       "zeed",         TOK_ZEED);
    DUAL("زنخ",       "zanakh",       TOK_ZANAKH);

    /* ── Collections (stdlib, but reserved as keywords) ── */
    kw_insert("fi", TOK_IDENT); /* "fi" used in for-each: lakol x fi list */
    /* We'll handle "fi" specially in the parser, not as a keyword token */

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
