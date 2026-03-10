/*
 * NashmiC — ast.h
 * Abstract Syntax Tree node types for Phase 0
 *
 * Phase 0 supports:
 * - yalla() { ... } entry point
 * - dalle name(params) -> type { ... } functions
 * - khalli name = expr / khalli name: type = expr
 * - itba3("...") print calls
 * - if/else, while, for-each, return
 * - binary/unary expressions, function calls
 * - integer/float/string literals, bool/null
 */

#ifndef NSH_AST_H
#define NSH_AST_H

#include "span.h"
#include <stdint.h>

/* ── Node Types ──────────────────────────────────────────────── */

typedef enum {
    /* Top level */
    NODE_PROGRAM,
    NODE_FUNC_DECL,

    /* Statements */
    NODE_VAR_DECL,       /* khalli x = ... */
    NODE_CONST_DECL,     /* thabet x = ... */
    NODE_RETURN,         /* rajje3 expr */
    NODE_IF,             /* iza ... wala ... */
    NODE_WHILE,          /* tool_ma ... */
    NODE_FOR_EACH,       /* lakol x fi ... */
    NODE_BLOCK,          /* { ... } */
    NODE_EXPR_STMT,      /* expression as statement */
    NODE_BREAK,          /* khalas */
    NODE_CONTINUE,       /* kammel */

    /* Expressions */
    NODE_INT_LIT,
    NODE_FLOAT_LIT,
    NODE_STRING_LIT,
    NODE_BOOL_LIT,
    NODE_NULL_LIT,
    NODE_IDENT,
    NODE_BINARY,         /* a + b, a == b, etc. */
    NODE_UNARY,          /* -x, mish x */
    NODE_CALL,           /* func(args...) */
    NODE_INDEX,          /* arr[i] */
    NODE_MEMBER,         /* obj.field */
    NODE_ASSIGN,         /* x = expr */
} NshNodeType;

/* ── Binary Operators ────────────────────────────────────────── */

typedef enum {
    BIN_ADD, BIN_SUB, BIN_MUL, BIN_DIV, BIN_MOD,
    BIN_EQ, BIN_NEQ, BIN_LT, BIN_GT, BIN_LTE, BIN_GTE,
    BIN_AND, BIN_OR,
} NshBinOp;

/* ── Unary Operators ─────────────────────────────────────────── */

typedef enum {
    UNARY_NEG,    /* - */
    UNARY_NOT,    /* mish / ! */
} NshUnaryOp;

/* ── Forward Declarations ────────────────────────────────────── */

typedef struct NshNode NshNode;

/* ── Node List (dynamic array) ───────────────────────────────── */

typedef struct {
    NshNode **items;
    int count;
    int capacity;
} NshNodeList;

void nodelist_init(NshNodeList *list);
void nodelist_push(NshNodeList *list, NshNode *node);
void nodelist_free(NshNodeList *list);

/* ── Parameter ───────────────────────────────────────────────── */

typedef struct {
    char *name;
    char *type_name;    /* type annotation as string for Phase 0 */
} NshParam;

typedef struct {
    NshParam *items;
    int count;
    int capacity;
} NshParamList;

void paramlist_init(NshParamList *list);
void paramlist_push(NshParamList *list, NshParam param);
void paramlist_free(NshParamList *list);

/* ── AST Node ────────────────────────────────────────────────── */

struct NshNode {
    NshNodeType type;
    NshSpan span;

    union {
        /* NODE_PROGRAM */
        struct {
            NshNodeList decls;
        } program;

        /* NODE_FUNC_DECL */
        struct {
            char *name;
            NshParamList params;
            char *return_type;   /* NULL = void */
            NshNode *body;       /* NODE_BLOCK */
            int is_yalla;        /* 1 if this is yalla() */
        } func_decl;

        /* NODE_VAR_DECL / NODE_CONST_DECL */
        struct {
            char *name;
            char *type_ann;      /* optional type annotation */
            NshNode *init;       /* initializer expression */
        } var_decl;

        /* NODE_RETURN */
        struct {
            NshNode *value;      /* NULL for bare return */
        } ret;

        /* NODE_IF */
        struct {
            NshNode *condition;
            NshNode *then_block;
            NshNode *else_block; /* NULL or another IF (else-if chain) or BLOCK */
        } if_stmt;

        /* NODE_WHILE */
        struct {
            NshNode *condition;
            NshNode *body;
        } while_stmt;

        /* NODE_FOR_EACH */
        struct {
            char *var_name;
            NshNode *iterable;
            NshNode *body;
        } for_each;

        /* NODE_BLOCK */
        struct {
            NshNodeList stmts;
        } block;

        /* NODE_EXPR_STMT */
        struct {
            NshNode *expr;
        } expr_stmt;

        /* NODE_INT_LIT */
        struct {
            int64_t value;
        } int_lit;

        /* NODE_FLOAT_LIT */
        struct {
            double value;
        } float_lit;

        /* NODE_STRING_LIT */
        struct {
            char *value;         /* includes quotes */
            int length;
        } string_lit;

        /* NODE_BOOL_LIT */
        struct {
            int value;           /* 1 = ah, 0 = la */
        } bool_lit;

        /* NODE_IDENT */
        struct {
            char *name;
        } ident;

        /* NODE_BINARY */
        struct {
            NshBinOp op;
            NshNode *left;
            NshNode *right;
        } binary;

        /* NODE_UNARY */
        struct {
            NshUnaryOp op;
            NshNode *operand;
        } unary;

        /* NODE_CALL */
        struct {
            NshNode *callee;     /* function expression */
            NshNodeList args;
        } call;

        /* NODE_INDEX */
        struct {
            NshNode *object;
            NshNode *index;
        } index_expr;

        /* NODE_MEMBER */
        struct {
            NshNode *object;
            char *field;
        } member;

        /* NODE_ASSIGN */
        struct {
            NshNode *target;
            NshNode *value;
        } assign;
    } as;
};

/* ── Node Constructors ───────────────────────────────────────── */

NshNode *node_new(NshNodeType type, NshSpan span);
void node_free(NshNode *node);

/* Convenience constructors */
NshNode *node_int_lit(int64_t value, NshSpan span);
NshNode *node_float_lit(double value, NshSpan span);
NshNode *node_string_lit(const char *text, int len, NshSpan span);
NshNode *node_bool_lit(int value, NshSpan span);
NshNode *node_null_lit(NshSpan span);
NshNode *node_ident(const char *name, int len, NshSpan span);

#endif /* NSH_AST_H */
