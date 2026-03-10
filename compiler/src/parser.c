/*
 * NashmiC — parser.c
 * Recursive descent parser with Pratt precedence climbing
 *
 * Grammar (NashmiC — its own language, not a C reskin):
 *   program     = (func_decl | struct_decl | enum_decl | impl_block)*
 *   func_decl   = "dalle" IDENT "(" params ")" ("->" type)? block
 *               | "yalla" "(" ")" block
 *   struct_decl = "haikal" IDENT "{" (IDENT ":" type ",")* "}"
 *   enum_decl   = "ta3dad" IDENT "{" (IDENT ("(" type ")")? ",")* "}"
 *   impl_block  = "tabbe2" IDENT "{" func_decl* "}"
 *   params      = (IDENT ":" type ("," IDENT ":" type)*)?
 *   block       = "{" stmt* "}"
 *   stmt        = var_decl | return_stmt | if_stmt | while_stmt
 *               | for_each | loop | match_stmt | break | continue
 *               | defer | expr_stmt
 *   var_decl    = "khalli" IDENT (":" type)? "=" expr
 *   return_stmt = "rajje3" expr?
 *   if_stmt     = "iza" ("fi" IDENT "=")? expr block ("wala" block)?
 *   while_stmt  = "tool_ma" expr block
 *   for_each    = "lakol" IDENT "fi" expr block
 *   match_stmt  = "hasab" expr "{" (match_arm)* "}"
 *   match_arm   = "hale" pattern "=>" (block | expr)
 *   defer       = "ba3dain" (block | expr)
 *   expr        = assignment
 *   assignment  = or_expr ("=" assignment)?
 *   or_expr     = and_expr ("aw" and_expr)*
 *   and_expr    = equality ("w" equality)*
 *   equality    = comparison (("==" | "!=") comparison)*
 *   comparison  = addition (("<" | ">" | "<=" | ">=") addition)*
 *   addition    = range (("+" | "-") range)*
 *   range       = multiply (".." multiply)?
 *   multiply    = unary (("*" | "/" | "%") unary)*
 *   unary       = ("-" | "mish" | "!") unary | postfix
 *   postfix     = primary ("(" args ")" | "." IDENT | "[" expr "]")*
 *                 ("wala?" string?)?
 *   primary     = INT | FLOAT | STRING (with interpolation) | "ah" | "la"
 *               | IDENT ("{" struct_lit "}")? | "(" expr ("," expr)* ")"
 *               | "tamam" "(" expr ")" | "ghalat" "(" expr ")"
 *               | "fi" "(" expr ")" | "mafi"
 */

#define _GNU_SOURCE
#include "parser.h"
#include "diagnostics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Parser Helpers ──────────────────────────────────────────── */

static void advance(NshParser *p) {
    p->previous = p->current;
    for (;;) {
        p->current = lexer_next(&p->lexer);
        if (p->current.type != TOK_ERROR) break;

        /* report error and keep going */
        diag_error_at(p->filename, p->source, p->current.span,
                      p->current.start);
        p->had_error = 1;
    }
}

static int check(NshParser *p, NshTokenType type) {
    return p->current.type == type;
}

static int match(NshParser *p, NshTokenType type) {
    if (!check(p, type)) return 0;
    advance(p);
    return 1;
}

static void expect(NshParser *p, NshTokenType type, const char *msg) {
    if (check(p, type)) {
        advance(p);
        return;
    }

    diag_error_at(p->filename, p->source, p->current.span, msg);
    p->had_error = 1;
}

static char *token_to_string(NshToken *tok) {
    char *s = malloc(tok->length + 1);
    memcpy(s, tok->start, tok->length);
    s[tok->length] = '\0';
    return s;
}

static NshSpan token_span(NshToken *tok) {
    return tok->span;
}

/* ── Forward Declarations ────────────────────────────────────── */

static NshNode *parse_block(NshParser *p);
static NshNode *parse_statement(NshParser *p);
static NshNode *parse_expression(NshParser *p);

/* ── String Interpolation Parser ──────────────────────────────── */

/* Parse a string literal that may contain {expr} interpolations.
 * Input token includes quotes: "hello {name} world"
 * Returns NODE_STRING_LIT for plain strings, NODE_INTERP_STRING for interpolated. */
static NshNode *parse_string_with_interpolation(NshParser *p) {
    NshSpan sp = token_span(&p->current);
    const char *raw = p->current.start;
    int raw_len = p->current.length;
    advance(p); /* consume the string token */

    /* Check if there are any unescaped { inside (skip the quotes) */
    int has_interp = 0;
    for (int i = 1; i < raw_len - 1; i++) {
        if (raw[i] == '{' && (i + 1 < raw_len - 1 && raw[i + 1] != '{')) {
            has_interp = 1;
            break;
        }
        if (raw[i] == '\\') i++; /* skip escaped chars */
    }

    if (!has_interp) {
        /* Plain string — return as-is (including quotes) */
        return node_string_lit(raw, raw_len, sp);
    }

    /* Parse interpolated string into parts */
    NshNode *interp = node_new(NODE_INTERP_STRING, sp);
    nodelist_init(&interp->as.interp_string.parts);

    /* Walk through string content (between quotes) */
    int pos = 1; /* skip opening " */
    int end = raw_len - 1; /* before closing " */
    int frag_start = pos;

    while (pos < end) {
        if (raw[pos] == '\\') {
            pos += 2; /* skip escape sequence */
            continue;
        }
        if (raw[pos] == '{') {
            if (pos + 1 < end && raw[pos + 1] == '{') {
                /* Escaped {{ — include single { in fragment */
                pos += 2;
                continue;
            }
            /* Emit preceding string fragment */
            if (pos > frag_start) {
                int flen = pos - frag_start;
                NshNode *frag = node_new(NODE_STRING_LIT, sp);
                frag->as.string_lit.value = malloc(flen + 1);
                memcpy(frag->as.string_lit.value, raw + frag_start, flen);
                frag->as.string_lit.value[flen] = '\0';
                frag->as.string_lit.length = flen;
                nodelist_push(&interp->as.interp_string.parts, frag);
            }
            /* Find matching } */
            pos++; /* skip { */
            int expr_start = pos;
            int depth = 1;
            while (pos < end && depth > 0) {
                if (raw[pos] == '{') depth++;
                else if (raw[pos] == '}') depth--;
                if (depth > 0) pos++;
            }
            /* Parse the expression between { and } */
            int expr_len = pos - expr_start;
            if (expr_len > 0) {
                /* Create a sub-lexer for the expression */
                NshLexer sub_lex;
                lexer_init(&sub_lex, raw + expr_start, expr_len, p->filename);
                /* Save parser state */
                NshLexer saved_lex = p->lexer;
                NshToken saved_cur = p->current;
                NshToken saved_prev = p->previous;
                /* Switch to sub-lexer */
                p->lexer = sub_lex;
                advance(p); /* prime with first token */
                NshNode *expr = parse_expression(p);
                nodelist_push(&interp->as.interp_string.parts, expr);
                /* Restore parser state */
                p->lexer = saved_lex;
                p->current = saved_cur;
                p->previous = saved_prev;
            }
            if (pos < end) pos++; /* skip } */
            frag_start = pos;
        } else {
            pos++;
        }
    }

    /* Emit trailing string fragment */
    if (pos > frag_start) {
        int flen = pos - frag_start;
        NshNode *frag = node_new(NODE_STRING_LIT, sp);
        frag->as.string_lit.value = malloc(flen + 1);
        memcpy(frag->as.string_lit.value, raw + frag_start, flen);
        frag->as.string_lit.value[flen] = '\0';
        frag->as.string_lit.length = flen;
        nodelist_push(&interp->as.interp_string.parts, frag);
    }

    return interp;
}

/* ── Expression Parsing (Pratt-style precedence) ─────────────── */

static NshNode *parse_primary(NshParser *p) {
    NshSpan sp = token_span(&p->current);

    /* Integer literal */
    if (check(p, TOK_INT_LIT)) {
        int64_t val = p->current.value.int_val;
        advance(p);
        return node_int_lit(val, sp);
    }

    /* Float literal */
    if (check(p, TOK_FLOAT_LIT)) {
        double val = p->current.value.float_val;
        advance(p);
        return node_float_lit(val, sp);
    }

    /* String literal (may contain interpolation) */
    if (check(p, TOK_STRING_LIT)) {
        return parse_string_with_interpolation(p);
    }

    /* Boolean: ah (true) */
    if (check(p, TOK_AH)) {
        advance(p);
        return node_bool_lit(1, sp);
    }

    /* Boolean: la (false) */
    if (check(p, TOK_LA)) {
        advance(p);
        return node_bool_lit(0, sp);
    }

    /* Null: wala_ishi — still parseable but deprecated */
    if (check(p, TOK_WALA_ISHI)) {
        advance(p);
        return node_null_lit(sp);
    }

    /* tamam(expr) — Result Ok constructor */
    if (check(p, TOK_TAMAM)) {
        advance(p);
        if (check(p, TOK_LPAREN)) {
            advance(p);
            NshNode *val = parse_expression(p);
            expect(p, TOK_RPAREN, "expected ')' after tamam(...)");
            NshNode *wrap = node_new(NODE_RESULT_WRAP, sp);
            wrap->as.result_wrap.is_ok = 1;
            wrap->as.result_wrap.value = val;
            return wrap;
        }
        /* Bare tamam as identifier */
        return node_ident("tamam", 5, sp);
    }

    /* ghalat(expr) — Result Err constructor */
    if (check(p, TOK_GHALAT)) {
        advance(p);
        if (check(p, TOK_LPAREN)) {
            advance(p);
            NshNode *val = parse_expression(p);
            expect(p, TOK_RPAREN, "expected ')' after ghalat(...)");
            NshNode *wrap = node_new(NODE_RESULT_WRAP, sp);
            wrap->as.result_wrap.is_ok = 0;
            wrap->as.result_wrap.value = val;
            return wrap;
        }
        return node_ident("ghalat", 6, sp);
    }

    /* fi(expr) — Optional Some constructor */
    if (check(p, TOK_FI)) {
        advance(p);
        if (check(p, TOK_LPAREN)) {
            advance(p);
            NshNode *val = parse_expression(p);
            expect(p, TOK_RPAREN, "expected ')' after fi(...)");
            NshNode *wrap = node_new(NODE_RESULT_WRAP, sp);
            wrap->as.result_wrap.is_ok = 1; /* reuse: fi = Some */
            wrap->as.result_wrap.value = val;
            return wrap;
        }
        return node_ident("fi", 2, sp);
    }

    /* mafi — Optional None constructor */
    if (check(p, TOK_MAFI)) {
        advance(p);
        return node_null_lit(sp); /* mafi = None, represented as null internally */
    }

    /* Identifier (possibly followed by struct literal { ... }) */
    if (check(p, TOK_IDENT)) {
        NshNode *n = node_ident(p->current.start, p->current.length, sp);
        advance(p);

        /* Check for struct literal: Name{ field: val, ... } */
        /* Only if { is on the same line (no space ambiguity) */
        if (check(p, TOK_LBRACE) &&
            p->current.span.line == p->previous.span.line) {
            /* Could be struct literal — peek for field: pattern */
            /* Save state to backtrack if not struct lit */
            size_t saved_pos = p->lexer.pos;
            int saved_line = p->lexer.line;
            int saved_col = p->lexer.col;
            NshToken saved_cur = p->current;
            NshToken saved_prev = p->previous;

            advance(p); /* consume { */
            if (check(p, TOK_IDENT)) {
                /* Check if next is : (struct lit) or something else (block) */
                NshToken ident_tok = p->current;
                advance(p);
                if (check(p, TOK_COLON)) {
                    /* It's a struct literal! Parse it */
                    /* Backtrack to after { */
                    /* We already consumed IDENT and saw : */
                    char *struct_name = NULL;
                    if (n->as.ident.name) {
                        struct_name = strdup(n->as.ident.name);
                    }
                    node_free(n);

                    int capacity = 8;
                    char **field_names = malloc(sizeof(char*) * capacity);
                    NshNode **field_values = malloc(sizeof(NshNode*) * capacity);
                    int field_count = 0;

                    /* Parse first field (already consumed ident and saw :) */
                    field_names[field_count] = token_to_string(&ident_tok);
                    advance(p); /* consume : */
                    field_values[field_count] = parse_expression(p);
                    field_count++;

                    while (match(p, TOK_COMMA)) {
                        if (check(p, TOK_RBRACE)) break; /* trailing comma */
                        if (field_count >= capacity) {
                            capacity *= 2;
                            field_names = realloc(field_names, sizeof(char*) * capacity);
                            field_values = realloc(field_values, sizeof(NshNode*) * capacity);
                        }
                        expect(p, TOK_IDENT, "expected field name in struct literal");
                        field_names[field_count] = token_to_string(&p->previous);
                        expect(p, TOK_COLON, "expected ':' after field name");
                        field_values[field_count] = parse_expression(p);
                        field_count++;
                    }

                    expect(p, TOK_RBRACE, "expected '}' after struct literal");

                    NshNode *lit = node_new(NODE_STRUCT_LIT, sp);
                    lit->as.struct_lit.name = struct_name;
                    lit->as.struct_lit.field_names = field_names;
                    lit->as.struct_lit.field_values = field_values;
                    lit->as.struct_lit.field_count = field_count;
                    return lit;
                } else {
                    /* Not a struct literal — backtrack */
                    p->lexer.pos = saved_pos;
                    p->lexer.line = saved_line;
                    p->lexer.col = saved_col;
                    p->current = saved_cur;
                    p->previous = saved_prev;
                }
            } else {
                /* Not a struct literal — backtrack */
                p->lexer.pos = saved_pos;
                p->lexer.line = saved_line;
                p->lexer.col = saved_col;
                p->current = saved_cur;
                p->previous = saved_prev;
            }
        }

        return n;
    }

    /* itba3, i2ra, itla3, etc — built-in functions parsed as identifiers */
    if (check(p, TOK_ITBA3) || check(p, TOK_I2RA) || check(p, TOK_ITLA3) ||
        check(p, TOK_AKKED) || check(p, TOK_FAZ3A) || check(p, TOK_HAD)) {
        NshNode *n = node_ident(p->current.start, p->current.length, sp);
        advance(p);
        return n;
    }

    /* Parenthesized expression or tuple */
    if (check(p, TOK_LPAREN)) {
        advance(p);
        NshNode *first = parse_expression(p);

        /* Check for tuple: (a, b, ...) */
        if (check(p, TOK_COMMA)) {
            NshNode *tuple = node_new(NODE_TUPLE_LIT, sp);
            nodelist_init(&tuple->as.tuple_lit.elements);
            nodelist_push(&tuple->as.tuple_lit.elements, first);
            while (match(p, TOK_COMMA)) {
                if (check(p, TOK_RPAREN)) break; /* trailing comma */
                NshNode *elem = parse_expression(p);
                nodelist_push(&tuple->as.tuple_lit.elements, elem);
            }
            expect(p, TOK_RPAREN, "expected ')' after tuple");
            return tuple;
        }

        expect(p, TOK_RPAREN, "expected ')'");
        return first;
    }

    diag_error_at(p->filename, p->source, sp,
                  "expected expression — شو هاد الحكي؟");
    p->had_error = 1;
    advance(p); /* skip bad token */
    return node_int_lit(0, sp); /* error recovery: return dummy */
}

static NshNode *parse_postfix(NshParser *p) {
    NshNode *expr = parse_primary(p);

    for (;;) {
        if (check(p, TOK_LPAREN)) {
            advance(p); /* ( */
            NshSpan sp = expr->span;
            NshNode *call_node = node_new(NODE_CALL, sp);
            call_node->as.call.callee = expr;
            nodelist_init(&call_node->as.call.args);

            if (!check(p, TOK_RPAREN)) {
                do {
                    NshNode *arg = parse_expression(p);
                    nodelist_push(&call_node->as.call.args, arg);
                } while (match(p, TOK_COMMA));
            }
            expect(p, TOK_RPAREN, "expected ')' after arguments");
            expr = call_node;
        } else if (check(p, TOK_DOT)) {
            advance(p); /* . */
            if (check(p, TOK_IDENT) || check(p, TOK_HAD)) {
                NshNode *mem = node_new(NODE_MEMBER, expr->span);
                mem->as.member.object = expr;
                mem->as.member.field = token_to_string(&p->current);
                advance(p);
                expr = mem;
            } else {
                expect(p, TOK_IDENT, "expected field name after '.'");
            }
        } else if (check(p, TOK_LBRACKET)) {
            advance(p); /* [ */
            NshNode *idx = parse_expression(p);
            expect(p, TOK_RBRACKET, "expected ']'");
            NshNode *index_node = node_new(NODE_INDEX, expr->span);
            index_node->as.index_expr.object = expr;
            index_node->as.index_expr.index = idx;
            expr = index_node;
        } else {
            break;
        }
    }

    /* Check for wala? (error propagation) */
    if (check(p, TOK_WALA_QUESTION)) {
        NshSpan sp = token_span(&p->current);
        advance(p);
        NshNode *wq = node_new(NODE_WALA_QUESTION, sp);
        wq->as.wala_question.expr = expr;
        wq->as.wala_question.error_msg = NULL;

        /* Optional error message string */
        if (check(p, TOK_STRING_LIT)) {
            wq->as.wala_question.error_msg =
                node_string_lit(p->current.start, p->current.length,
                                token_span(&p->current));
            advance(p);
        }
        expr = wq;
    }

    return expr;
}

static NshNode *parse_unary(NshParser *p) {
    if (check(p, TOK_MINUS)) {
        NshSpan sp = token_span(&p->current);
        advance(p);
        NshNode *operand = parse_unary(p);
        NshNode *n = node_new(NODE_UNARY, sp);
        n->as.unary.op = UNARY_NEG;
        n->as.unary.operand = operand;
        return n;
    }
    if (check(p, TOK_BANG) || check(p, TOK_MISH)) {
        NshSpan sp = token_span(&p->current);
        advance(p);
        NshNode *operand = parse_unary(p);
        NshNode *n = node_new(NODE_UNARY, sp);
        n->as.unary.op = UNARY_NOT;
        n->as.unary.operand = operand;
        return n;
    }
    return parse_postfix(p);
}

static NshNode *parse_multiply(NshParser *p) {
    NshNode *left = parse_unary(p);

    while (check(p, TOK_STAR) || check(p, TOK_SLASH) || check(p, TOK_PERCENT)) {
        NshBinOp op;
        if (check(p, TOK_STAR)) op = BIN_MUL;
        else if (check(p, TOK_SLASH)) op = BIN_DIV;
        else op = BIN_MOD;

        advance(p);
        NshNode *right = parse_unary(p);
        NshNode *bin = node_new(NODE_BINARY, left->span);
        bin->as.binary.op = op;
        bin->as.binary.left = left;
        bin->as.binary.right = right;
        left = bin;
    }

    return left;
}

/* Range: a..b — creates a range expression */
static NshNode *parse_range(NshParser *p) {
    NshNode *left = parse_multiply(p);

    if (check(p, TOK_DOTDOT)) {
        advance(p);
        NshNode *right = parse_multiply(p);
        NshNode *range = node_new(NODE_RANGE, left->span);
        range->as.range.start = left;
        range->as.range.end = right;
        return range;
    }

    return left;
}

static NshNode *parse_addition(NshParser *p) {
    NshNode *left = parse_range(p);

    while (check(p, TOK_PLUS) || check(p, TOK_MINUS)) {
        NshBinOp op = check(p, TOK_PLUS) ? BIN_ADD : BIN_SUB;
        advance(p);
        NshNode *right = parse_range(p);
        NshNode *bin = node_new(NODE_BINARY, left->span);
        bin->as.binary.op = op;
        bin->as.binary.left = left;
        bin->as.binary.right = right;
        left = bin;
    }

    return left;
}

static NshNode *parse_comparison(NshParser *p) {
    NshNode *left = parse_addition(p);

    while (check(p, TOK_LT) || check(p, TOK_GT) ||
           check(p, TOK_LTE) || check(p, TOK_GTE)) {
        NshBinOp op;
        if (check(p, TOK_LT)) op = BIN_LT;
        else if (check(p, TOK_GT)) op = BIN_GT;
        else if (check(p, TOK_LTE)) op = BIN_LTE;
        else op = BIN_GTE;

        advance(p);
        NshNode *right = parse_addition(p);
        NshNode *bin = node_new(NODE_BINARY, left->span);
        bin->as.binary.op = op;
        bin->as.binary.left = left;
        bin->as.binary.right = right;
        left = bin;
    }

    return left;
}

static NshNode *parse_equality(NshParser *p) {
    NshNode *left = parse_comparison(p);

    while (check(p, TOK_EQ) || check(p, TOK_NEQ)) {
        NshBinOp op = check(p, TOK_EQ) ? BIN_EQ : BIN_NEQ;
        advance(p);
        NshNode *right = parse_comparison(p);
        NshNode *bin = node_new(NODE_BINARY, left->span);
        bin->as.binary.op = op;
        bin->as.binary.left = left;
        bin->as.binary.right = right;
        left = bin;
    }

    return left;
}

static NshNode *parse_and(NshParser *p) {
    NshNode *left = parse_equality(p);

    while (check(p, TOK_W)) {
        advance(p);
        NshNode *right = parse_equality(p);
        NshNode *bin = node_new(NODE_BINARY, left->span);
        bin->as.binary.op = BIN_AND;
        bin->as.binary.left = left;
        bin->as.binary.right = right;
        left = bin;
    }

    return left;
}

static NshNode *parse_or(NshParser *p) {
    NshNode *left = parse_and(p);

    while (check(p, TOK_AW)) {
        advance(p);
        NshNode *right = parse_and(p);
        NshNode *bin = node_new(NODE_BINARY, left->span);
        bin->as.binary.op = BIN_OR;
        bin->as.binary.left = left;
        bin->as.binary.right = right;
        left = bin;
    }

    return left;
}

static NshNode *parse_assignment(NshParser *p) {
    NshNode *expr = parse_or(p);

    if (check(p, TOK_ASSIGN)) {
        advance(p);
        NshNode *value = parse_assignment(p); /* right-associative */
        NshNode *assign = node_new(NODE_ASSIGN, expr->span);
        assign->as.assign.target = expr;
        assign->as.assign.value = value;
        return assign;
    }

    return expr;
}

static NshNode *parse_expression(NshParser *p) {
    return parse_assignment(p);
}

/* ── Statement Parsing ───────────────────────────────────────── */

static char *parse_type_annotation(NshParser *p) {
    /* Type annotations: identifiers, built-in type keywords, or generic<T> */
    if (check(p, TOK_IDENT) || check(p, TOK_ADAD) || check(p, TOK_ADAD64) ||
        check(p, TOK_FASLE) || check(p, TOK_FASLE64) || check(p, TOK_MANTE2) ||
        check(p, TOK_HARF) || check(p, TOK_BAIT) || check(p, TOK_NASS) ||
        check(p, TOK_FADI) || check(p, TOK_NATIJE) || check(p, TOK_YIMKIN)) {
        char *type = token_to_string(&p->current);
        advance(p);

        /* Handle generic: natije<T> or yimkin<T> */
        if (check(p, TOK_LT)) {
            advance(p); /* < */
            char *inner = parse_type_annotation(p);
            expect(p, TOK_GT, "expected '>' after generic type parameter");
            /* Build "natije<T>" string */
            if (inner) {
                size_t tlen = strlen(type) + 1 + strlen(inner) + 1 + 1;
                char *full = malloc(tlen);
                snprintf(full, tlen, "%s<%s>", type, inner);
                free(type);
                free(inner);
                type = full;
            }
        }

        return type;
    }
    diag_error_at(p->filename, p->source, p->current.span,
                  "expected type name");
    p->had_error = 1;
    return NULL;
}

static NshNode *parse_var_decl(NshParser *p, int is_const) {
    NshSpan sp = token_span(&p->previous); /* khalli or thabet already consumed */
    NshNodeType ntype = is_const ? NODE_CONST_DECL : NODE_VAR_DECL;

    /* Check for tuple destructuring: khalli (a, b) = expr */
    if (check(p, TOK_LPAREN)) {
        /* TODO: tuple destructuring — for now, parse as error */
        diag_error_at(p->filename, p->source, p->current.span,
                      "tuple destructuring not yet implemented");
        p->had_error = 1;
        advance(p);
        return node_int_lit(0, sp);
    }

    expect(p, TOK_IDENT, "expected variable name");
    char *name = token_to_string(&p->previous);

    char *type_ann = NULL;
    if (match(p, TOK_COLON)) {
        type_ann = parse_type_annotation(p);
    }

    NshNode *init = NULL;
    if (match(p, TOK_ASSIGN)) {
        init = parse_expression(p);
    } else if (is_const) {
        diag_error_at(p->filename, p->source, sp,
                      "thabet must have an initializer");
        p->had_error = 1;
    }

    NshNode *node = node_new(ntype, sp);
    node->as.var_decl.name = name;
    node->as.var_decl.type_ann = type_ann;
    node->as.var_decl.init = init;
    return node;
}

static NshNode *parse_return_stmt(NshParser *p) {
    NshSpan sp = token_span(&p->previous); /* rajje3 already consumed */
    NshNode *value = NULL;

    /* return value if not followed by } or EOF */
    if (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
        value = parse_expression(p);
    }

    NshNode *node = node_new(NODE_RETURN, sp);
    node->as.ret.value = value;
    return node;
}

static NshNode *parse_if_stmt(NshParser *p) {
    NshSpan sp = token_span(&p->previous); /* iza already consumed */

    /* Check for optional binding: iza fi x = expr */
    if (check(p, TOK_FI)) {
        advance(p); /* consume fi */
        expect(p, TOK_IDENT, "expected variable name after 'fi'");
        char *var_name = token_to_string(&p->previous);
        expect(p, TOK_ASSIGN, "expected '=' in optional binding");
        NshNode *expr = parse_expression(p);
        NshNode *then_block = parse_block(p);
        NshNode *else_block = NULL;

        if (match(p, TOK_WALA)) {
            else_block = parse_block(p);
        }

        NshNode *node = node_new(NODE_OPTIONAL_BIND, sp);
        node->as.optional_bind.var_name = var_name;
        node->as.optional_bind.expr = expr;
        node->as.optional_bind.then_block = then_block;
        node->as.optional_bind.else_block = else_block;
        return node;
    }

    NshNode *condition = parse_expression(p);
    NshNode *then_block = parse_block(p);
    NshNode *else_block = NULL;

    if (match(p, TOK_WALA_IZA)) {
        /* else if — recurse as if statement */
        else_block = parse_if_stmt(p);
    } else if (match(p, TOK_WALA)) {
        else_block = parse_block(p);
    }

    NshNode *node = node_new(NODE_IF, sp);
    node->as.if_stmt.condition = condition;
    node->as.if_stmt.then_block = then_block;
    node->as.if_stmt.else_block = else_block;
    return node;
}

static NshNode *parse_while_stmt(NshParser *p) {
    NshSpan sp = token_span(&p->previous); /* tool_ma already consumed */

    NshNode *condition = parse_expression(p);
    NshNode *body = parse_block(p);

    NshNode *node = node_new(NODE_WHILE, sp);
    node->as.while_stmt.condition = condition;
    node->as.while_stmt.body = body;
    return node;
}

static NshNode *parse_for_each(NshParser *p) {
    NshSpan sp = token_span(&p->previous); /* lakol already consumed */

    expect(p, TOK_IDENT, "expected variable name after lakol");
    char *var_name = token_to_string(&p->previous);

    /* expect "fi" keyword */
    if (check(p, TOK_FI)) {
        advance(p);
    } else if (check(p, TOK_IDENT) &&
               p->current.length == 2 &&
               memcmp(p->current.start, "fi", 2) == 0) {
        advance(p);
    } else {
        expect(p, TOK_FI, "expected 'fi' after variable name — lakol x fi ...");
    }

    NshNode *iterable = parse_expression(p);
    NshNode *body = parse_block(p);

    NshNode *node = node_new(NODE_FOR_EACH, sp);
    node->as.for_each.var_name = var_name;
    node->as.for_each.iterable = iterable;
    node->as.for_each.body = body;
    return node;
}

static NshNode *parse_match_stmt(NshParser *p) {
    NshSpan sp = token_span(&p->previous); /* hasab already consumed */

    NshNode *subject = parse_expression(p);
    expect(p, TOK_LBRACE, "expected '{' after hasab expression");

    NshNode *match_node = node_new(NODE_MATCH, sp);
    match_node->as.match.subject = subject;
    nodelist_init(&match_node->as.match.arms);

    while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
        if (match(p, TOK_HALE)) {
            NshSpan arm_sp = token_span(&p->previous);
            NshNode *arm = node_new(NODE_MATCH_ARM, arm_sp);

            /* Pattern: variant name, tamam, ghalat, fi, mafi, or 3adi (default) */
            if (check(p, TOK_3ADI)) {
                arm->as.match_arm.pattern_name = strdup("_");
                advance(p);
            } else if (check(p, TOK_TAMAM)) {
                arm->as.match_arm.pattern_name = strdup("tamam");
                advance(p);
            } else if (check(p, TOK_GHALAT)) {
                arm->as.match_arm.pattern_name = strdup("ghalat");
                advance(p);
            } else if (check(p, TOK_FI)) {
                arm->as.match_arm.pattern_name = strdup("fi");
                advance(p);
            } else if (check(p, TOK_MAFI)) {
                arm->as.match_arm.pattern_name = strdup("mafi");
                advance(p);
            } else {
                expect(p, TOK_IDENT, "expected pattern name after 'hale'");
                arm->as.match_arm.pattern_name = token_to_string(&p->previous);
            }

            /* Optional bindings: hale Variant(a, b) */
            nodelist_init(&arm->as.match_arm.bindings);
            if (match(p, TOK_LPAREN)) {
                if (!check(p, TOK_RPAREN)) {
                    do {
                        expect(p, TOK_IDENT, "expected binding name");
                        NshNode *binding = node_ident(p->previous.start,
                                                       p->previous.length,
                                                       token_span(&p->previous));
                        nodelist_push(&arm->as.match_arm.bindings, binding);
                    } while (match(p, TOK_COMMA));
                }
                expect(p, TOK_RPAREN, "expected ')' after bindings");
            }

            expect(p, TOK_FAT_ARROW, "expected '=>' after pattern");

            /* Body: block or expression */
            if (check(p, TOK_LBRACE)) {
                arm->as.match_arm.body = parse_block(p);
            } else {
                NshNode *expr = parse_expression(p);
                arm->as.match_arm.body = node_new(NODE_EXPR_STMT, expr->span);
                arm->as.match_arm.body->as.expr_stmt.expr = expr;
            }

            nodelist_push(&match_node->as.match.arms, arm);
        } else {
            diag_error_at(p->filename, p->source, p->current.span,
                          "expected 'hale' in hasab block");
            p->had_error = 1;
            advance(p);
        }
    }

    expect(p, TOK_RBRACE, "expected '}' after hasab block");
    return match_node;
}

static NshNode *parse_block(NshParser *p) {
    NshSpan sp = token_span(&p->current);
    expect(p, TOK_LBRACE, "expected '{'");

    NshNode *block = node_new(NODE_BLOCK, sp);
    nodelist_init(&block->as.block.stmts);

    while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
        NshNode *stmt = parse_statement(p);
        if (stmt) nodelist_push(&block->as.block.stmts, stmt);
    }

    expect(p, TOK_RBRACE, "expected '}'");
    return block;
}

static NshNode *parse_statement(NshParser *p) {
    /* Variable declaration: khalli */
    if (match(p, TOK_KHALLI)) {
        return parse_var_decl(p, 0);
    }

    /* Constant declaration: thabet */
    if (match(p, TOK_THABET)) {
        return parse_var_decl(p, 1);
    }

    /* Return: rajje3 */
    if (match(p, TOK_RAJJE3)) {
        return parse_return_stmt(p);
    }

    /* If: iza */
    if (match(p, TOK_IZA)) {
        return parse_if_stmt(p);
    }

    /* While: tool_ma */
    if (match(p, TOK_TOOL_MA)) {
        return parse_while_stmt(p);
    }

    /* For each: lakol */
    if (match(p, TOK_LAKOL)) {
        return parse_for_each(p);
    }

    /* Loop: liff */
    if (match(p, TOK_LIFF)) {
        NshSpan sp = token_span(&p->previous);
        NshNode *body = parse_block(p);
        NshNode *node = node_new(NODE_LOOP, sp);
        node->as.loop.body = body;
        return node;
    }

    /* Break: khalas */
    if (match(p, TOK_KHALAS)) {
        return node_new(NODE_BREAK, token_span(&p->previous));
    }

    /* Continue: kammel */
    if (match(p, TOK_KAMMEL)) {
        return node_new(NODE_CONTINUE, token_span(&p->previous));
    }

    /* Defer: ba3dain */
    if (match(p, TOK_BA3DAIN)) {
        NshSpan sp = token_span(&p->previous);
        NshNode *body;
        if (check(p, TOK_LBRACE)) {
            body = parse_block(p);
        } else {
            NshNode *expr = parse_expression(p);
            body = node_new(NODE_EXPR_STMT, expr->span);
            body->as.expr_stmt.expr = expr;
        }
        NshNode *node = node_new(NODE_DEFER, sp);
        node->as.defer.body = body;
        return node;
    }

    /* Match: hasab */
    if (match(p, TOK_HASAB)) {
        return parse_match_stmt(p);
    }

    /* Expression statement */
    NshNode *expr = parse_expression(p);
    NshNode *stmt = node_new(NODE_EXPR_STMT, expr->span);
    stmt->as.expr_stmt.expr = expr;
    return stmt;
}

/* ── Top-Level Parsing ───────────────────────────────────────── */

static NshNode *parse_func_decl(NshParser *p, int is_yalla) {
    NshSpan sp = token_span(&p->previous);

    char *name;
    if (is_yalla) {
        name = strdup("yalla");
    } else {
        expect(p, TOK_IDENT, "expected function name");
        name = token_to_string(&p->previous);
    }

    expect(p, TOK_LPAREN, "expected '(' after function name");

    NshParamList params;
    paramlist_init(&params);

    if (!check(p, TOK_RPAREN)) {
        do {
            /* Allow 'had' as parameter name (self/this) */
            if (check(p, TOK_HAD)) {
                char *pname = strdup("had");
                advance(p);
                /* 'had' doesn't need a type annotation — it's the receiver */
                char *ptype = NULL;
                if (match(p, TOK_COLON)) {
                    ptype = parse_type_annotation(p);
                }
                NshParam param = {.name = pname, .type_name = ptype};
                paramlist_push(&params, param);
            } else {
                expect(p, TOK_IDENT, "expected parameter name");
                char *pname = token_to_string(&p->previous);
                expect(p, TOK_COLON, "expected ':' after parameter name");
                char *ptype = parse_type_annotation(p);

                NshParam param = {.name = pname, .type_name = ptype};
                paramlist_push(&params, param);
            }
        } while (match(p, TOK_COMMA));
    }

    expect(p, TOK_RPAREN, "expected ')' after parameters");

    char *return_type = NULL;
    if (match(p, TOK_ARROW)) {
        return_type = parse_type_annotation(p);
    }

    NshNode *body = parse_block(p);

    NshNode *func = node_new(NODE_FUNC_DECL, sp);
    func->as.func_decl.name = name;
    func->as.func_decl.params = params;
    func->as.func_decl.return_type = return_type;
    func->as.func_decl.body = body;
    func->as.func_decl.is_yalla = is_yalla;
    return func;
}

static NshNode *parse_struct_decl(NshParser *p) {
    NshSpan sp = token_span(&p->previous); /* haikal already consumed */

    expect(p, TOK_IDENT, "expected struct name");
    char *name = token_to_string(&p->previous);

    expect(p, TOK_LBRACE, "expected '{' after struct name");

    NshParamList fields;
    paramlist_init(&fields);

    while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
        expect(p, TOK_IDENT, "expected field name");
        char *fname = token_to_string(&p->previous);
        expect(p, TOK_COLON, "expected ':' after field name");
        char *ftype = parse_type_annotation(p);

        NshParam field = {.name = fname, .type_name = ftype};
        paramlist_push(&fields, field);

        match(p, TOK_COMMA); /* optional trailing comma */
    }

    expect(p, TOK_RBRACE, "expected '}'");

    NshNode *node = node_new(NODE_STRUCT_DECL, sp);
    node->as.struct_decl.name = name;
    node->as.struct_decl.fields = fields;
    return node;
}

static NshNode *parse_enum_decl(NshParser *p) {
    NshSpan sp = token_span(&p->previous); /* ta3dad already consumed */

    expect(p, TOK_IDENT, "expected enum name");
    char *name = token_to_string(&p->previous);

    expect(p, TOK_LBRACE, "expected '{' after enum name");

    NshParamList variants;
    paramlist_init(&variants);

    while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
        expect(p, TOK_IDENT, "expected variant name");
        char *vname = token_to_string(&p->previous);
        char *vtype = NULL;

        /* Optional payload: Variant(type) */
        if (match(p, TOK_LPAREN)) {
            vtype = parse_type_annotation(p);
            expect(p, TOK_RPAREN, "expected ')' after variant payload type");
        }

        NshParam variant = {.name = vname, .type_name = vtype};
        paramlist_push(&variants, variant);

        match(p, TOK_COMMA); /* optional trailing comma */
    }

    expect(p, TOK_RBRACE, "expected '}'");

    NshNode *node = node_new(NODE_ENUM_DECL, sp);
    node->as.enum_decl.name = name;
    node->as.enum_decl.variants = variants;
    return node;
}

static NshNode *parse_impl_block(NshParser *p) {
    NshSpan sp = token_span(&p->previous); /* tabbe2 already consumed */

    expect(p, TOK_IDENT, "expected type name after tabbe2");
    char *type_name = token_to_string(&p->previous);

    expect(p, TOK_LBRACE, "expected '{' after type name in tabbe2");

    NshNode *impl = node_new(NODE_IMPL_BLOCK, sp);
    impl->as.impl_block.type_name = type_name;
    nodelist_init(&impl->as.impl_block.methods);

    while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
        if (match(p, TOK_DALLE)) {
            NshNode *method = parse_func_decl(p, 0);
            nodelist_push(&impl->as.impl_block.methods, method);
        } else {
            diag_error_at(p->filename, p->source, p->current.span,
                          "expected 'dalle' in tabbe2 block");
            p->had_error = 1;
            advance(p);
        }
    }

    expect(p, TOK_RBRACE, "expected '}'");
    return impl;
}

static NshNode *parse_top_level(NshParser *p) {
    if (match(p, TOK_YALLA)) {
        return parse_func_decl(p, 1);
    }
    if (match(p, TOK_DALLE)) {
        return parse_func_decl(p, 0);
    }
    if (match(p, TOK_HAIKAL)) {
        return parse_struct_decl(p);
    }
    if (match(p, TOK_TA3DAD)) {
        return parse_enum_decl(p);
    }
    if (match(p, TOK_TABBE2)) {
        return parse_impl_block(p);
    }

    diag_error_at(p->filename, p->source, p->current.span,
                  "expected declaration (dalle/yalla/haikal/ta3dad/tabbe2) at top level");
    p->had_error = 1;
    advance(p);
    return NULL;
}

/* ── Public API ──────────────────────────────────────────────── */

NshNode *parser_parse(const char *source, size_t len, const char *filename) {
    NshParser parser;
    lexer_init(&parser.lexer, source, len, filename);
    parser.had_error = 0;
    parser.panic_mode = 0;
    parser.source = source;
    parser.filename = filename;

    /* prime the parser */
    advance(&parser);

    NshNode *program = node_new(NODE_PROGRAM, NSH_SPAN_NONE);
    nodelist_init(&program->as.program.decls);

    while (!check(&parser, TOK_EOF)) {
        NshNode *decl = parse_top_level(&parser);
        if (decl) nodelist_push(&program->as.program.decls, decl);
    }

    if (parser.had_error) {
        node_free(program);
        return NULL;
    }

    return program;
}
