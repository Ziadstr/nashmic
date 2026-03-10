/*
 * NashmiC — ast.h
 * Abstract Syntax Tree node types
 *
 * Supports:
 * - yalla() entry point, dalle functions
 * - khalli/thabet variable declarations
 * - Control flow: iza/wala, tool_ma, lakol..fi, liff
 * - Range expressions (0..20), string interpolation
 * - ba3dain (defer), haikal (struct), ta3dad (enum)
 * - natije/tamam/ghalat Result type, wala? propagation
 * - yimkin<T> optional type, fi/mafi
 * - tabbe2 impl blocks (methods on structs)
 * - hasab/hale pattern matching
 * - Expressions, calls, member access, indexing
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
    NODE_STRUCT_DECL,    /* haikal Name { field: type, ... } */
    NODE_ENUM_DECL,      /* ta3dad Name { Variant1, Variant2(type), ... } */
    NODE_IMPL_BLOCK,     /* tabbe2 TypeName { dalle ... } */

    /* Statements */
    NODE_VAR_DECL,       /* khalli x = ... */
    NODE_CONST_DECL,     /* thabet x = ... */
    NODE_RETURN,         /* rajje3 expr */
    NODE_IF,             /* iza ... wala ... */
    NODE_WHILE,          /* tool_ma ... */
    NODE_FOR_EACH,       /* lakol x fi ... */
    NODE_LOOP,           /* liff { ... } — infinite loop */
    NODE_BLOCK,          /* { ... } */
    NODE_EXPR_STMT,      /* expression as statement */
    NODE_BREAK,          /* khalas */
    NODE_CONTINUE,       /* kammel */
    NODE_DEFER,          /* ba3dain expr — runs on scope exit */
    NODE_MATCH,          /* hasab expr { hale pattern => ... } */
    NODE_MATCH_ARM,      /* hale pattern => body */

    /* Expressions */
    NODE_INT_LIT,
    NODE_FLOAT_LIT,
    NODE_STRING_LIT,
    NODE_INTERP_STRING,  /* "marhaba ya {name}" — string interpolation */
    NODE_BOOL_LIT,
    NODE_NULL_LIT,
    NODE_IDENT,
    NODE_BINARY,         /* a + b, a == b, etc. */
    NODE_UNARY,          /* -x, mish x */
    NODE_CALL,           /* func(args...) */
    NODE_INDEX,          /* arr[i] */
    NODE_MEMBER,         /* obj.field */
    NODE_ASSIGN,         /* x = expr */
    NODE_RANGE,          /* 0..20 — range expression */
    NODE_STRUCT_LIT,     /* Name{ field: value, ... } */
    NODE_WALA_QUESTION,  /* expr wala? "msg" — unwrap-or-propagate */
    NODE_RESULT_WRAP,    /* tamam(expr) / ghalat(expr) */
    NODE_OPTIONAL_BIND,  /* iza fi x = expr { ... } wala { ... } */
    NODE_TUPLE_LIT,      /* (a, b, c) */
    NODE_ARRAY_LIT,      /* [expr, expr, ...] */
} NshNodeType;

/* ── Binary Operators ────────────────────────────────────────── */

typedef enum {
    BIN_ADD, BIN_SUB, BIN_MUL, BIN_DIV, BIN_MOD,
    BIN_EQ, BIN_NEQ, BIN_LT, BIN_GT, BIN_LTE, BIN_GTE,
    BIN_AND, BIN_OR,
    BIN_RANGE,  /* .. — range operator */
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
    char *type_name;    /* type annotation as string */
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
            char *value;         /* raw string content (no quotes for fragments) */
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

        /* NODE_RANGE — 0..20 */
        struct {
            NshNode *start;
            NshNode *end;
        } range;

        /* NODE_DEFER — ba3dain { ... } or ba3dain expr */
        struct {
            NshNode *body;
        } defer;

        /* NODE_LOOP — liff { ... } */
        struct {
            NshNode *body;
        } loop;

        /* NODE_MATCH — hasab expr { hale ... => ..., ... } */
        struct {
            NshNode *subject;
            NshNodeList arms;    /* list of NODE_MATCH_ARM */
        } match;

        /* NODE_MATCH_ARM — hale pattern => body */
        struct {
            char *pattern_name;      /* variant name or "_" for default */
            NshNodeList bindings;    /* extracted variable names (as NODE_IDENT) */
            NshNode *body;           /* block or expression */
        } match_arm;

        /* NODE_STRUCT_DECL — haikal Name { fields } */
        struct {
            char *name;
            NshParamList fields; /* reuse ParamList: name + type_name */
        } struct_decl;

        /* NODE_ENUM_DECL — ta3dad Name { variants } */
        struct {
            char *name;
            NshParamList variants; /* name = variant, type_name = payload type or NULL */
        } enum_decl;

        /* NODE_IMPL_BLOCK — tabbe2 TypeName { dalle ... } */
        struct {
            char *type_name;
            NshNodeList methods; /* list of NODE_FUNC_DECL */
        } impl_block;

        /* NODE_STRUCT_LIT — Name{ field: value, ... } */
        struct {
            char *name;
            char **field_names;
            NshNode **field_values;
            int field_count;
        } struct_lit;

        /* NODE_INTERP_STRING — "text {expr} more text" */
        struct {
            NshNodeList parts;  /* alternating string fragments and expressions */
        } interp_string;

        /* NODE_WALA_QUESTION — expr wala? "optional msg" */
        struct {
            NshNode *expr;
            NshNode *error_msg;  /* optional: string literal for error wrapping */
        } wala_question;

        /* NODE_RESULT_WRAP — tamam(expr) or ghalat(expr) */
        struct {
            int is_ok;          /* 1 = tamam, 0 = ghalat */
            NshNode *value;
        } result_wrap;

        /* NODE_OPTIONAL_BIND — iza fi x = expr { ... } wala { ... } */
        struct {
            char *var_name;
            NshNode *expr;
            NshNode *then_block;
            NshNode *else_block;
        } optional_bind;

        /* NODE_TUPLE_LIT — (a, b, c) */
        struct {
            NshNodeList elements;
        } tuple_lit;

        /* NODE_ARRAY_LIT — [expr, expr, ...] */
        struct {
            NshNodeList elements;
            char *elem_type;    /* inferred or annotated element type, e.g. "adad64" */
        } array_lit;
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
