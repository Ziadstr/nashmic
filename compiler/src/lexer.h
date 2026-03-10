/*
 * NashmiC — lexer.h
 * UTF-8 tokenizer with dual-mode (Arabic + Franco) keyword support
 */

#ifndef NSH_LEXER_H
#define NSH_LEXER_H

#include <stddef.h>
#include <stdint.h>
#include "span.h"

/* ── Token Types ─────────────────────────────────────────────── */

typedef enum {
    /* Sentinel */
    TOK_EOF = 0,
    TOK_ERROR,

    /* Literals */
    TOK_IDENT,
    TOK_INT_LIT,
    TOK_FLOAT_LIT,
    TOK_STRING_LIT,

    /* ── Control Flow ──────────── */
    TOK_IZA,           /* اذا / iza          — if */
    TOK_WALA,          /* والا / wala         — else */
    TOK_WALA_IZA,      /* والا_اذا / wala_iza — else if */
    TOK_TOOL_MA,       /* طول_ما / tool_ma   — while */
    TOK_LAKOL,         /* لكل / lakol        — for each */
    TOK_LIFF,          /* لف / liff          — loop */
    TOK_KHALAS,        /* خلص / khalas       — break */
    TOK_KAMMEL,        /* كمل / kammel       — continue */
    TOK_RAJJE3,        /* رجع / rajje3       — return */
    TOK_HASAB,         /* حسب / hasab        — switch */
    TOK_HALE,          /* حالة / hale        — case */
    TOK_3ADI,          /* عادي / 3adi        — default */

    /* ── Types ─────────────────── */
    TOK_ADAD,          /* عدد / adad         — int32 */
    TOK_ADAD64,        /* عدد٦٤ / adad64    — int64 */
    TOK_FASLE,         /* فاصلة / fasle      — float */
    TOK_FASLE64,       /* فاصلة٦٤ / fasle64 — double */
    TOK_MANTE2,        /* منطق / mante2      — bool */
    TOK_HARF,          /* حرف / harf         — char */
    TOK_BAIT,          /* بايت / bait        — byte */
    TOK_NASS,          /* نص / nass          — string */
    TOK_FADI,          /* فاضي / fadi        — void */
    TOK_MU2SHR,        /* مؤشر / mu2shr     — pointer */
    TOK_SAFF,          /* صف / saff          — array */
    TOK_HAIKAL,        /* هيكل / haikal      — struct */
    TOK_TA3DAD,        /* تعداد / ta3dad     — enum */

    /* ── Values ────────────────── */
    TOK_AH,            /* اه / ah            — true */
    TOK_LA,            /* لا / la            — false */
    TOK_WALA_ISHI,     /* ولا_اشي / wala_ishi — null */

    /* ── Declarations ──────────── */
    TOK_KHALLI,        /* خلي / khalli       — let/var */
    TOK_THABET,        /* ثابت / thabet      — const */
    TOK_DALLE,         /* دالة / dalle       — function */
    TOK_AAM,           /* عام / aam          — public */
    TOK_KHAS,          /* خاص / khas         — private */
    TOK_SADDER,        /* صدر / sadder       — export */
    TOK_JEEB,          /* جيب / jeeb         — import */
    TOK_QESM,          /* قسم / qesm         — module */
    TOK_KHARJI,        /* خارجي / kharji     — extern */
    TOK_JDEED,         /* جديد / jdeed       — new */
    TOK_FUKK,          /* فك / fukk          — free */

    /* ── Memory ────────────────── */
    TOK_IHJEZ,         /* احجز / ihjez       — alloc */
    TOK_HARES,         /* حارس / hares       — guard */
    TOK_3AL_3AMYANI,   /* عالعمياني / 3al_3amyani — unsafe */

    /* ── Error Handling ────────── */
    TOK_JARREB,        /* جرب / jarreb       — try */
    TOK_IMSEK,         /* امسك / imsek       — catch */
    TOK_IRMI,          /* ارمي / irmi        — throw */
    TOK_FAZ3A,         /* فزعة / faz3a       — panic */
    TOK_NATIJE,        /* نتيجة / natije     — Result<T> */
    TOK_TAMAM,         /* تمام / tamam       — Ok(v) */
    TOK_GHALAT,        /* غلط / ghalat       — Err(e) */
    TOK_BA3DAIN,       /* بعدين / ba3dain    — defer */
    TOK_BINHAYTO,      /* بنهايته / binhayto — finally */

    /* ── Logical Operators ─────── */
    TOK_W,             /* و / w              — and */
    TOK_AW,            /* او / aw            — or */
    TOK_MISH,          /* مش / mish          — not */
    TOK_HAJM,          /* حجم / hajm         — sizeof */
    TOK_NO3,           /* نوع / no3          — typeof */

    /* ── Concurrency ───────────── */
    TOK_SHAGHGHEL,     /* شغل / shaghghel   — spawn */
    TOK_ISTANNA,       /* استنى / istanna    — wait/join */
    TOK_QANAH,         /* قناة / qanah       — channel */
    TOK_IB3ATH,        /* ابعث / ib3ath      — send */
    TOK_ISTALAM,       /* استلم / istalam    — receive */
    TOK_I2FEL,         /* اقفل / i2fel       — lock */
    TOK_IFTAH,         /* افتح / iftah       — unlock */
    TOK_3AL_TAYER,     /* عالطاير / 3al_tayer — async */
    TOK_KHAIT,         /* خيط / khait        — thread */

    /* ── I/O & Common ──────────── */
    TOK_ITBA3,         /* اطبع / itba3       — print */
    TOK_I2RA,          /* اقرأ / i2ra        — read/input */
    TOK_ITLA3,         /* اطلع / itla3       — exit */
    TOK_AKKED,         /* اكد / akked        — assert */
    TOK_FAHAS,         /* فحص / fahas        — test */

    /* ── Extra Jordanian ───────── */
    TOK_YALLA,         /* yalla              — main() */
    TOK_HAD,           /* had                — this/self */
    TOK_TABE2,         /* tabe2              — match */
    TOK_SAMMI,         /* sammi              — type alias */
    TOK_TABBE2,        /* tabbe2             — impl */
    TOK_SIFE,          /* sife               — trait */
    TOK_7UTT,          /* 7utt               — set/assign */
    TOK_SHEEL,         /* sheel              — remove */
    TOK_ZEED,          /* zeed               — add/push */
    TOK_ZANAKH,        /* zanakh             — deprecated */

    /* ── Optional Type ─────────── */
    TOK_YIMKIN,        /* yimkin              — Optional<T> (maybe) */
    TOK_FI,            /* fi                  — Some / "there is" */
    TOK_MAFI,          /* mafi                — None / "there isn't" */

    /* ── Error Propagation ─────── */
    TOK_WALA_QUESTION, /* wala?               — unwrap-or-propagate */

    /* ── String Interpolation ──── */
    TOK_INTERP_START,  /* { inside string     — start interpolation */
    TOK_INTERP_END,    /* } inside string     — end interpolation */

    /* ── Punctuation & Operators ── */
    TOK_LPAREN,        /* ( */
    TOK_RPAREN,        /* ) */
    TOK_LBRACE,        /* { */
    TOK_RBRACE,        /* } */
    TOK_LBRACKET,      /* [ */
    TOK_RBRACKET,      /* ] */
    TOK_SEMICOLON,     /* ; */
    TOK_COLON,         /* : */
    TOK_COMMA,         /* , */
    TOK_DOT,           /* . */
    TOK_ARROW,         /* -> */
    TOK_FAT_ARROW,     /* => */
    TOK_ASSIGN,        /* = */
    TOK_EQ,            /* == */
    TOK_NEQ,           /* != */
    TOK_LT,            /* < */
    TOK_GT,            /* > */
    TOK_LTE,           /* <= */
    TOK_GTE,           /* >= */
    TOK_PLUS,          /* + */
    TOK_MINUS,         /* - */
    TOK_STAR,          /* * */
    TOK_SLASH,         /* / */
    TOK_PERCENT,       /* % */
    TOK_AMP,           /* & */
    TOK_PIPE,          /* | */
    TOK_CARET,         /* ^ */
    TOK_TILDE,         /* ~ */
    TOK_BANG,          /* ! */
    TOK_DOTDOT,        /* .. */

    TOK_COUNT          /* total number of token types */
} NshTokenType;

/* ── Token ─────────────────────────────────────────────────── */

typedef struct {
    NshTokenType type;
    const char *start;   /* pointer into source buffer */
    int length;
    NshSpan span;

    /* literal values (populated for literal tokens) */
    union {
        int64_t int_val;
        double float_val;
    } value;
} NshToken;

/* ── Lexer State ───────────────────────────────────────────── */

typedef struct {
    const char *source;     /* full source text */
    size_t source_len;
    size_t pos;             /* current byte position */
    int line;               /* current line (1-based) */
    int col;                /* current column (1-based) */
    const char *filename;
} NshLexer;

/* Initialize a lexer on source text. */
void lexer_init(NshLexer *lex, const char *source, size_t len, const char *filename);

/* Get the next token. */
NshToken lexer_next(NshLexer *lex);

/* Peek at the next token without consuming it. */
NshToken lexer_peek(NshLexer *lex);

/* Return a human-readable name for a token type. */
const char *token_type_name(NshTokenType type);

#endif /* NSH_LEXER_H */
