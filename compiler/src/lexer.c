/*
 * NashmiC — lexer.c
 * UTF-8 tokenizer with dual-mode Arabic + Franco keyword support
 *
 * Handles:
 * - Arabic script identifiers/keywords (Unicode range 0x0600-0x06FF + 0xFE70-0xFEFF)
 * - Franco-Arab Latin identifiers (a-z, A-Z, 0-9, _, 2, 3, 7 in ident positions)
 * - Numbers (int and float literals)
 * - String literals (double-quoted, with escape sequences)
 * - All punctuation and operators
 * - Single-line (//) and multi-line comments
 */

#include "lexer.h"
#include "keywords.h"
#include "utf8.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* ── Helpers ─────────────────────────────────────────────────── */

static int is_arabic_cp(uint32_t cp) {
    return (cp >= 0x0600 && cp <= 0x06FF) ||   /* Arabic block */
           (cp >= 0xFE70 && cp <= 0xFEFF) ||   /* Arabic Presentation Forms-B */
           (cp >= 0x0660 && cp <= 0x0669) ||   /* Arabic-Indic digits ٠-٩ */
           cp == 0x0640;                        /* Tatweel */
}

static int is_ident_start_cp(uint32_t cp) {
    return is_arabic_cp(cp) ||
           (cp >= 'a' && cp <= 'z') ||
           (cp >= 'A' && cp <= 'Z') ||
           cp == '_';
}

static int is_ident_cont_cp(uint32_t cp) {
    return is_ident_start_cp(cp) ||
           (cp >= '0' && cp <= '9') ||
           (cp >= 0x0660 && cp <= 0x0669);  /* Arabic-Indic digits */
}

static int at_end(NshLexer *lex) {
    return lex->pos >= lex->source_len;
}

static char peek_byte(NshLexer *lex) {
    if (at_end(lex)) return '\0';
    return lex->source[lex->pos];
}

static char peek_byte_at(NshLexer *lex, size_t offset) {
    size_t idx = lex->pos + offset;
    if (idx >= lex->source_len) return '\0';
    return lex->source[idx];
}

static char advance_byte(NshLexer *lex) {
    if (at_end(lex)) return '\0';
    char c = lex->source[lex->pos++];
    if (c == '\n') {
        lex->line++;
        lex->col = 1;
    } else {
        lex->col++;
    }
    return c;
}

static NshToken make_token(NshLexer *lex, NshTokenType type,
                           const char *start, int length,
                           int start_line, int start_col) {
    NshToken tok;
    tok.type = type;
    tok.start = start;
    tok.length = length;
    tok.span.file = lex->filename;
    tok.span.offset = start - lex->source;
    tok.span.length = length;
    tok.span.line = start_line;
    tok.span.col = start_col;
    tok.value.int_val = 0;
    return tok;
}

static NshToken error_token(NshLexer *lex, const char *msg) {
    NshToken tok;
    tok.type = TOK_ERROR;
    tok.start = msg;
    tok.length = (int)strlen(msg);
    tok.span.file = lex->filename;
    tok.span.offset = lex->pos;
    tok.span.length = 0;
    tok.span.line = lex->line;
    tok.span.col = lex->col;
    tok.value.int_val = 0;
    return tok;
}

/* ── Skip whitespace and comments ────────────────────────────── */

static void skip_whitespace_and_comments(NshLexer *lex) {
    for (;;) {
        /* skip whitespace */
        while (!at_end(lex) && (peek_byte(lex) == ' ' ||
                                 peek_byte(lex) == '\t' ||
                                 peek_byte(lex) == '\r' ||
                                 peek_byte(lex) == '\n')) {
            advance_byte(lex);
        }

        /* single-line comment: // */
        if (peek_byte(lex) == '/' && peek_byte_at(lex, 1) == '/') {
            while (!at_end(lex) && peek_byte(lex) != '\n') {
                advance_byte(lex);
            }
            continue;
        }

        /* multi-line comment */
        if (peek_byte(lex) == '/' && peek_byte_at(lex, 1) == '*') {
            advance_byte(lex); /* / */
            advance_byte(lex); /* * */
            int depth = 1;
            while (!at_end(lex) && depth > 0) {
                if (peek_byte(lex) == '/' && peek_byte_at(lex, 1) == '*') {
                    advance_byte(lex);
                    advance_byte(lex);
                    depth++;
                } else if (peek_byte(lex) == '*' && peek_byte_at(lex, 1) == '/') {
                    advance_byte(lex);
                    advance_byte(lex);
                    depth--;
                } else {
                    advance_byte(lex);
                }
            }
            continue;
        }

        break;
    }
}

/* ── Lex identifier or keyword ───────────────────────────────── */

static NshToken lex_identifier(NshLexer *lex) {
    const char *start = lex->source + lex->pos;
    int start_line = lex->line;
    int start_col = lex->col;
    size_t start_pos = lex->pos;

    /* consume identifier chars (multi-byte aware) */
    size_t probe = lex->pos;
    uint32_t cp = utf8_decode(lex->source, lex->source_len, &probe);

    if (!is_ident_start_cp(cp)) {
        advance_byte(lex);
        return error_token(lex, "unexpected character");
    }

    /* advance past first char */
    while (lex->pos < probe) advance_byte(lex);

    /* consume continuation chars */
    for (;;) {
        probe = lex->pos;
        if (at_end(lex)) break;
        cp = utf8_decode(lex->source, lex->source_len, &probe);
        if (!is_ident_cont_cp(cp)) break;
        while (lex->pos < probe) advance_byte(lex);
    }

    int length = (int)(lex->pos - start_pos);
    NshTokenType type = keywords_lookup(start, length);

    /* Check for wala? compound token */
    if (type == TOK_WALA && peek_byte(lex) == '?') {
        advance_byte(lex);
        length = (int)(lex->pos - start_pos);
        return make_token(lex, TOK_WALA_QUESTION, start, length,
                          start_line, start_col);
    }

    return make_token(lex, type, start, length, start_line, start_col);
}

/* ── Lex number ──────────────────────────────────────────────── */

static NshToken lex_number(NshLexer *lex) {
    const char *start = lex->source + lex->pos;
    int start_line = lex->line;
    int start_col = lex->col;
    size_t start_pos = lex->pos;
    int is_float = 0;

    /* consume digits */
    while (!at_end(lex) && isdigit((unsigned char)peek_byte(lex))) {
        advance_byte(lex);
    }

    /* check for decimal point */
    if (peek_byte(lex) == '.' && isdigit((unsigned char)peek_byte_at(lex, 1))) {
        is_float = 1;
        advance_byte(lex); /* '.' */
        while (!at_end(lex) && isdigit((unsigned char)peek_byte(lex))) {
            advance_byte(lex);
        }
    }

    int length = (int)(lex->pos - start_pos);
    NshToken tok = make_token(lex, is_float ? TOK_FLOAT_LIT : TOK_INT_LIT,
                              start, length, start_line, start_col);

    /* parse the value */
    char buf[64];
    int copy_len = length < 63 ? length : 63;
    memcpy(buf, start, copy_len);
    buf[copy_len] = '\0';

    if (is_float) {
        tok.value.float_val = strtod(buf, NULL);
    } else {
        tok.value.int_val = strtoll(buf, NULL, 10);
    }

    return tok;
}

/* ── Lex string ──────────────────────────────────────────────── */

static NshToken lex_string(NshLexer *lex) {
    int start_line = lex->line;
    int start_col = lex->col;
    const char *start = lex->source + lex->pos;

    advance_byte(lex); /* opening " */

    while (!at_end(lex) && peek_byte(lex) != '"') {
        if (peek_byte(lex) == '\\') {
            advance_byte(lex); /* backslash */
            if (!at_end(lex)) advance_byte(lex); /* escaped char */
        } else {
            advance_byte(lex);
        }
    }

    if (at_end(lex)) {
        return error_token(lex, "unterminated string — خلص الكلام بس ما سكرت");
    }

    advance_byte(lex); /* closing " */

    int length = (int)((lex->source + lex->pos) - start);
    return make_token(lex, TOK_STRING_LIT, start, length, start_line, start_col);
}

/* ── Lex char literal ────────────────────────────────────────── */

static NshToken lex_char(NshLexer *lex) {
    int start_line = lex->line;
    int start_col = lex->col;
    const char *start = lex->source + lex->pos;

    advance_byte(lex); /* opening ' */

    if (peek_byte(lex) == '\\') {
        advance_byte(lex);
        advance_byte(lex);
    } else {
        /* might be multi-byte UTF-8 char */
        int seq = utf8_seq_len((unsigned char)peek_byte(lex));
        for (int i = 0; i < seq; i++) advance_byte(lex);
    }

    if (peek_byte(lex) != '\'') {
        return error_token(lex, "unterminated char literal");
    }
    advance_byte(lex); /* closing ' */

    int length = (int)((lex->source + lex->pos) - start);
    return make_token(lex, TOK_INT_LIT, start, length, start_line, start_col);
}

/* ── Main Lexer ──────────────────────────────────────────────── */

void lexer_init(NshLexer *lex, const char *source, size_t len, const char *filename) {
    lex->source = source;
    lex->source_len = len;
    lex->pos = 0;
    lex->line = 1;
    lex->col = 1;
    lex->filename = filename;
}

NshToken lexer_next(NshLexer *lex) {
    skip_whitespace_and_comments(lex);

    if (at_end(lex)) {
        return make_token(lex, TOK_EOF, lex->source + lex->pos, 0, lex->line, lex->col);
    }

    const char *start = lex->source + lex->pos;
    int start_line = lex->line;
    int start_col = lex->col;
    char c = peek_byte(lex);

    /* String literal */
    if (c == '"') return lex_string(lex);

    /* Char literal */
    if (c == '\'') return lex_char(lex);

    /* Number */
    if (isdigit((unsigned char)c)) return lex_number(lex);

    /* ASCII identifier */
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_') {
        return lex_identifier(lex);
    }

    /* UTF-8 identifier (Arabic and other multi-byte) */
    if ((unsigned char)c >= 0x80) {
        return lex_identifier(lex);
    }

    /* Punctuation & operators */
    advance_byte(lex);

    switch (c) {
    case '(': return make_token(lex, TOK_LPAREN, start, 1, start_line, start_col);
    case ')': return make_token(lex, TOK_RPAREN, start, 1, start_line, start_col);
    case '{': return make_token(lex, TOK_LBRACE, start, 1, start_line, start_col);
    case '}': return make_token(lex, TOK_RBRACE, start, 1, start_line, start_col);
    case '[': return make_token(lex, TOK_LBRACKET, start, 1, start_line, start_col);
    case ']': return make_token(lex, TOK_RBRACKET, start, 1, start_line, start_col);
    case ';': return make_token(lex, TOK_SEMICOLON, start, 1, start_line, start_col);
    case ':': return make_token(lex, TOK_COLON, start, 1, start_line, start_col);
    case ',': return make_token(lex, TOK_COMMA, start, 1, start_line, start_col);
    case '+': return make_token(lex, TOK_PLUS, start, 1, start_line, start_col);
    case '*': return make_token(lex, TOK_STAR, start, 1, start_line, start_col);
    case '/': return make_token(lex, TOK_SLASH, start, 1, start_line, start_col);
    case '%': return make_token(lex, TOK_PERCENT, start, 1, start_line, start_col);
    case '&': return make_token(lex, TOK_AMP, start, 1, start_line, start_col);
    case '|': return make_token(lex, TOK_PIPE, start, 1, start_line, start_col);
    case '^': return make_token(lex, TOK_CARET, start, 1, start_line, start_col);
    case '~': return make_token(lex, TOK_TILDE, start, 1, start_line, start_col);

    case '.':
        if (peek_byte(lex) == '.') {
            advance_byte(lex);
            return make_token(lex, TOK_DOTDOT, start, 2, start_line, start_col);
        }
        return make_token(lex, TOK_DOT, start, 1, start_line, start_col);

    case '-':
        if (peek_byte(lex) == '>') {
            advance_byte(lex);
            return make_token(lex, TOK_ARROW, start, 2, start_line, start_col);
        }
        return make_token(lex, TOK_MINUS, start, 1, start_line, start_col);

    case '=':
        if (peek_byte(lex) == '=') {
            advance_byte(lex);
            return make_token(lex, TOK_EQ, start, 2, start_line, start_col);
        }
        if (peek_byte(lex) == '>') {
            advance_byte(lex);
            return make_token(lex, TOK_FAT_ARROW, start, 2, start_line, start_col);
        }
        return make_token(lex, TOK_ASSIGN, start, 1, start_line, start_col);

    case '!':
        if (peek_byte(lex) == '=') {
            advance_byte(lex);
            return make_token(lex, TOK_NEQ, start, 2, start_line, start_col);
        }
        return make_token(lex, TOK_BANG, start, 1, start_line, start_col);

    case '<':
        if (peek_byte(lex) == '=') {
            advance_byte(lex);
            return make_token(lex, TOK_LTE, start, 2, start_line, start_col);
        }
        return make_token(lex, TOK_LT, start, 1, start_line, start_col);

    case '>':
        if (peek_byte(lex) == '=') {
            advance_byte(lex);
            return make_token(lex, TOK_GTE, start, 2, start_line, start_col);
        }
        return make_token(lex, TOK_GT, start, 1, start_line, start_col);

    default:
        return error_token(lex, "unexpected character — شو هاد؟");
    }
}

NshToken lexer_peek(NshLexer *lex) {
    /* save state */
    size_t saved_pos = lex->pos;
    int saved_line = lex->line;
    int saved_col = lex->col;

    NshToken tok = lexer_next(lex);

    /* restore state */
    lex->pos = saved_pos;
    lex->line = saved_line;
    lex->col = saved_col;

    return tok;
}

const char *token_type_name(NshTokenType type) {
    switch (type) {
    case TOK_EOF: return "EOF";
    case TOK_ERROR: return "error";
    case TOK_IDENT: return "identifier";
    case TOK_INT_LIT: return "integer";
    case TOK_FLOAT_LIT: return "float";
    case TOK_STRING_LIT: return "string";
    case TOK_IZA: return "iza (if)";
    case TOK_WALA: return "wala (else)";
    case TOK_WALA_IZA: return "wala_iza (else if)";
    case TOK_TOOL_MA: return "tool_ma (while)";
    case TOK_LAKOL: return "lakol (for each)";
    case TOK_LIFF: return "liff (loop)";
    case TOK_KHALAS: return "khalas (break)";
    case TOK_KAMMEL: return "kammel (continue)";
    case TOK_RAJJE3: return "rajje3 (return)";
    case TOK_HASAB: return "hasab (switch)";
    case TOK_HALE: return "hale (case)";
    case TOK_3ADI: return "3adi (default)";
    case TOK_ADAD: return "adad (int32)";
    case TOK_ADAD64: return "adad64 (int64)";
    case TOK_FASLE: return "fasle (float)";
    case TOK_FASLE64: return "fasle64 (double)";
    case TOK_MANTE2: return "mante2 (bool)";
    case TOK_HARF: return "harf (char)";
    case TOK_BAIT: return "bait (byte)";
    case TOK_NASS: return "nass (string)";
    case TOK_FADI: return "fadi (void)";
    case TOK_MU2SHR: return "mu2shr (pointer)";
    case TOK_SAFF: return "saff (array)";
    case TOK_HAIKAL: return "haikal (struct)";
    case TOK_TA3DAD: return "ta3dad (enum)";
    case TOK_AH: return "ah (true)";
    case TOK_LA: return "la (false)";
    case TOK_WALA_ISHI: return "wala_ishi (null)";
    case TOK_KHALLI: return "khalli (let)";
    case TOK_THABET: return "thabet (const)";
    case TOK_DALLE: return "dalle (function)";
    case TOK_AAM: return "aam (public)";
    case TOK_KHAS: return "khas (private)";
    case TOK_SADDER: return "sadder (export)";
    case TOK_JEEB: return "jeeb (import)";
    case TOK_QESM: return "qesm (module)";
    case TOK_KHARJI: return "kharji (extern)";
    case TOK_JDEED: return "jdeed (new)";
    case TOK_FUKK: return "fukk (free)";
    case TOK_IHJEZ: return "ihjez (alloc)";
    case TOK_HARES: return "hares (guard)";
    case TOK_3AL_3AMYANI: return "3al_3amyani (unsafe)";
    case TOK_JARREB: return "jarreb (try)";
    case TOK_IMSEK: return "imsek (catch)";
    case TOK_IRMI: return "irmi (throw)";
    case TOK_FAZ3A: return "faz3a (panic)";
    case TOK_NATIJE: return "natije (Result)";
    case TOK_TAMAM: return "tamam (Ok)";
    case TOK_GHALAT: return "ghalat (Err)";
    case TOK_BA3DAIN: return "ba3dain (defer)";
    case TOK_BINHAYTO: return "binhayto (finally)";
    case TOK_W: return "w (and)";
    case TOK_AW: return "aw (or)";
    case TOK_MISH: return "mish (not)";
    case TOK_HAJM: return "hajm (sizeof)";
    case TOK_NO3: return "no3 (typeof)";
    case TOK_SHAGHGHEL: return "shaghghel (spawn)";
    case TOK_ISTANNA: return "istanna (wait)";
    case TOK_QANAH: return "qanah (channel)";
    case TOK_IB3ATH: return "ib3ath (send)";
    case TOK_ISTALAM: return "istalam (receive)";
    case TOK_I2FEL: return "i2fel (lock)";
    case TOK_IFTAH: return "iftah (unlock)";
    case TOK_3AL_TAYER: return "3al_tayer (async)";
    case TOK_KHAIT: return "khait (thread)";
    case TOK_ITBA3: return "itba3 (print)";
    case TOK_I2RA: return "i2ra (read)";
    case TOK_ITLA3: return "itla3 (exit)";
    case TOK_AKKED: return "akked (assert)";
    case TOK_FAHAS: return "fahas (test)";
    case TOK_YALLA: return "yalla (main)";
    case TOK_HAD: return "had (this)";
    case TOK_TABE2: return "tabe2 (match)";
    case TOK_SAMMI: return "sammi (type alias)";
    case TOK_TABBE2: return "tabbe2 (impl)";
    case TOK_SIFE: return "sife (trait)";
    case TOK_7UTT: return "7utt (set)";
    case TOK_SHEEL: return "sheel (remove)";
    case TOK_ZEED: return "zeed (add)";
    case TOK_ZANAKH: return "zanakh (deprecated)";
    case TOK_YIMKIN: return "yimkin (Optional)";
    case TOK_FI: return "fi (Some)";
    case TOK_MAFI: return "mafi (None)";
    case TOK_WALA_QUESTION: return "wala? (propagate)";
    case TOK_INTERP_START: return "{interp";
    case TOK_INTERP_END: return "interp}";
    case TOK_LPAREN: return "(";
    case TOK_RPAREN: return ")";
    case TOK_LBRACE: return "{";
    case TOK_RBRACE: return "}";
    case TOK_LBRACKET: return "[";
    case TOK_RBRACKET: return "]";
    case TOK_SEMICOLON: return ";";
    case TOK_COLON: return ":";
    case TOK_COMMA: return ",";
    case TOK_DOT: return ".";
    case TOK_ARROW: return "->";
    case TOK_FAT_ARROW: return "=>";
    case TOK_ASSIGN: return "=";
    case TOK_EQ: return "==";
    case TOK_NEQ: return "!=";
    case TOK_LT: return "<";
    case TOK_GT: return ">";
    case TOK_LTE: return "<=";
    case TOK_GTE: return ">=";
    case TOK_PLUS: return "+";
    case TOK_MINUS: return "-";
    case TOK_STAR: return "*";
    case TOK_SLASH: return "/";
    case TOK_PERCENT: return "%";
    case TOK_AMP: return "&";
    case TOK_PIPE: return "|";
    case TOK_CARET: return "^";
    case TOK_TILDE: return "~";
    case TOK_BANG: return "!";
    case TOK_DOTDOT: return "..";
    default: return "unknown";
    }
}
