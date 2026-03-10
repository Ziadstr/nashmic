/*
 * NashmiC — parser.c
 * Recursive descent parser with Pratt precedence climbing
 *
 * Phase 0 Grammar:
 *   program     = (func_decl)*
 *   func_decl   = "dalle" IDENT "(" params ")" ("->" type)? block
 *               | "yalla" "(" ")" block
 *   params      = (IDENT ":" type ("," IDENT ":" type)*)?
 *   block       = "{" stmt* "}"
 *   stmt        = var_decl | return_stmt | if_stmt | while_stmt
 *               | for_each | break | continue | expr_stmt
 *   var_decl    = "khalli" IDENT (":" type)? "=" expr
 *   return_stmt = "rajje3" expr?
 *   if_stmt     = "iza" expr block ("wala" block)?
 *   while_stmt  = "tool_ma" expr block
 *   for_each    = "lakol" IDENT "fi" expr block
 *   expr_stmt   = expr
 *   expr        = assignment
 *   assignment  = or_expr ("=" assignment)?
 *   or_expr     = and_expr ("aw" and_expr)*
 *   and_expr    = equality ("w" equality)*
 *   equality    = comparison (("==" | "!=") comparison)*
 *   comparison  = addition (("<" | ">" | "<=" | ">=") addition)*
 *   addition    = multiply (("+" | "-") multiply)*
 *   multiply    = unary (("*" | "/" | "%") unary)*
 *   unary       = ("-" | "mish" | "!") unary | call
 *   call        = primary ("(" args ")" | "." IDENT | "[" expr "]")*
 *   primary     = INT | FLOAT | STRING | "ah" | "la" | "wala_ishi"
 *               | IDENT | "(" expr ")"
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

    /* String literal */
    if (check(p, TOK_STRING_LIT)) {
        NshNode *n = node_string_lit(p->current.start, p->current.length, sp);
        advance(p);
        return n;
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

    /* Null: wala_ishi */
    if (check(p, TOK_WALA_ISHI)) {
        advance(p);
        return node_null_lit(sp);
    }

    /* Identifier */
    if (check(p, TOK_IDENT)) {
        NshNode *n = node_ident(p->current.start, p->current.length, sp);
        advance(p);
        return n;
    }

    /* itba3 — treat as identifier so it can be called */
    if (check(p, TOK_ITBA3)) {
        NshNode *n = node_ident(p->current.start, p->current.length, sp);
        advance(p);
        return n;
    }

    /* Parenthesized expression */
    if (check(p, TOK_LPAREN)) {
        advance(p);
        NshNode *expr = parse_expression(p);
        expect(p, TOK_RPAREN, "expected ')' — وين القوس يا زلمة؟");
        return expr;
    }

    diag_error_at(p->filename, p->source, sp,
                  "expected expression — شو هاد الحكي؟");
    p->had_error = 1;
    advance(p); /* skip bad token */
    return node_int_lit(0, sp); /* error recovery: return dummy */
}

static NshNode *parse_call(NshParser *p) {
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
            expect(p, TOK_IDENT, "expected field name after '.'");
            NshNode *mem = node_new(NODE_MEMBER, expr->span);
            mem->as.member.object = expr;
            mem->as.member.field = token_to_string(&p->previous);
            expr = mem;
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
    return parse_call(p);
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

static NshNode *parse_addition(NshParser *p) {
    NshNode *left = parse_multiply(p);

    while (check(p, TOK_PLUS) || check(p, TOK_MINUS)) {
        NshBinOp op = check(p, TOK_PLUS) ? BIN_ADD : BIN_SUB;
        advance(p);
        NshNode *right = parse_multiply(p);
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
    /* For Phase 0, type annotations are just identifiers we pass through */
    if (check(p, TOK_IDENT) || check(p, TOK_ADAD) || check(p, TOK_ADAD64) ||
        check(p, TOK_FASLE) || check(p, TOK_FASLE64) || check(p, TOK_MANTE2) ||
        check(p, TOK_HARF) || check(p, TOK_BAIT) || check(p, TOK_NASS) ||
        check(p, TOK_FADI)) {
        char *type = token_to_string(&p->current);
        advance(p);
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

    expect(p, TOK_IDENT, "expected variable name — شو اسم المتغير؟");
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
                      "ثابت must have an initializer — ثابت بدون قيمة؟");
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

    /* expect "fi" (في) — check for identifier "fi" */
    if (check(p, TOK_IDENT) &&
        p->current.length == 2 &&
        memcmp(p->current.start, "fi", 2) == 0) {
        advance(p);
    } else {
        expect(p, TOK_IDENT, "expected 'fi' after variable name — lakol x fi ...");
    }

    NshNode *iterable = parse_expression(p);
    NshNode *body = parse_block(p);

    NshNode *node = node_new(NODE_FOR_EACH, sp);
    node->as.for_each.var_name = var_name;
    node->as.for_each.iterable = iterable;
    node->as.for_each.body = body;
    return node;
}

static NshNode *parse_block(NshParser *p) {
    NshSpan sp = token_span(&p->current);
    expect(p, TOK_LBRACE, "expected '{' — وين القوس؟");

    NshNode *block = node_new(NODE_BLOCK, sp);
    nodelist_init(&block->as.block.stmts);

    while (!check(p, TOK_RBRACE) && !check(p, TOK_EOF)) {
        NshNode *stmt = parse_statement(p);
        if (stmt) nodelist_push(&block->as.block.stmts, stmt);
    }

    expect(p, TOK_RBRACE, "expected '}' — سكّر القوس يا زلمة");
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

    /* Break: khalas */
    if (match(p, TOK_KHALAS)) {
        return node_new(NODE_BREAK, token_span(&p->previous));
    }

    /* Continue: kammel */
    if (match(p, TOK_KAMMEL)) {
        return node_new(NODE_CONTINUE, token_span(&p->previous));
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
        expect(p, TOK_IDENT, "expected function name — شو اسم الدالة؟");
        name = token_to_string(&p->previous);
    }

    expect(p, TOK_LPAREN, "expected '(' after function name");

    NshParamList params;
    paramlist_init(&params);

    if (!check(p, TOK_RPAREN)) {
        do {
            expect(p, TOK_IDENT, "expected parameter name");
            char *pname = token_to_string(&p->previous);
            expect(p, TOK_COLON, "expected ':' after parameter name");
            char *ptype = parse_type_annotation(p);

            NshParam param = {.name = pname, .type_name = ptype};
            paramlist_push(&params, param);
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

static NshNode *parse_top_level(NshParser *p) {
    if (match(p, TOK_YALLA)) {
        return parse_func_decl(p, 1);
    }
    if (match(p, TOK_DALLE)) {
        return parse_func_decl(p, 0);
    }

    diag_error_at(p->filename, p->source, p->current.span,
                  "expected function declaration (dalle/yalla) at top level");
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
