/*
 * NashmiC — sema.h
 * Phase 2: Semantic Analysis
 *
 * Three-pass analysis between parsing and codegen:
 *   Pass 1 — Declaration: register top-level types and functions
 *   Pass 2 — Resolution: resolve type annotations, check variable refs
 *   Pass 3 — Type checking: binary ops, calls, returns, field access
 */

#ifndef NSH_SEMA_H
#define NSH_SEMA_H

#include "ast.h"
#include "span.h"

/* ── Type System ─────────────────────────────────────────────── */

typedef enum {
    NSH_TYPE_INT32,
    NSH_TYPE_INT64,
    NSH_TYPE_FLOAT,
    NSH_TYPE_DOUBLE,
    NSH_TYPE_BOOL,
    NSH_TYPE_CHAR,
    NSH_TYPE_STRING,
    NSH_TYPE_VOID,
    NSH_TYPE_STRUCT,
    NSH_TYPE_ENUM,
    NSH_TYPE_RESULT,
    NSH_TYPE_OPTIONAL,
    NSH_TYPE_FUNC,
    NSH_TYPE_UNKNOWN,     /* unresolved / error recovery */
} NshTypeKind;

typedef struct NshType NshType;

struct NshType {
    NshTypeKind kind;
    union {
        /* NSH_TYPE_STRUCT */
        struct {
            char *name;
        } struct_type;

        /* NSH_TYPE_ENUM */
        struct {
            char *name;
        } enum_type;

        /* NSH_TYPE_RESULT — natije<T> */
        struct {
            NshType *inner;
        } result_type;

        /* NSH_TYPE_OPTIONAL — yimkin<T> */
        struct {
            NshType *inner;
        } optional_type;

        /* NSH_TYPE_FUNC */
        struct {
            NshType **param_types;
            int param_count;
            NshType *return_type;
        } func_type;
    } as;
};

NshType *nsh_type_new(NshTypeKind kind);
NshType *nsh_type_clone(const NshType *type);
void nsh_type_free(NshType *type);
int nsh_type_equal(const NshType *a, const NshType *b);
const char *nsh_type_name(const NshType *type);

/* ── Symbol Table ────────────────────────────────────────────── */

typedef enum {
    SYM_VAR,
    SYM_CONST,
    SYM_FUNC,
    SYM_TYPE,
    SYM_ENUM_VARIANT,
} NshSymKind;

typedef struct {
    char *name;
    NshSymKind kind;
    NshType *type_info;
    NshSpan span;
    int is_used;
} NshSymbol;

#define SCOPE_BUCKET_COUNT 64

typedef struct NshScope NshScope;

typedef struct {
    NshSymbol *sym;
    unsigned hash;
} ScopeBucket;

struct NshScope {
    ScopeBucket buckets[SCOPE_BUCKET_COUNT];
    int count;
    NshScope *parent;
};

NshScope *scope_new(NshScope *parent);
void scope_free(NshScope *scope);
int scope_insert(NshScope *scope, NshSymbol *sym);
NshSymbol *scope_lookup(NshScope *scope, const char *name);
NshSymbol *scope_lookup_local(NshScope *scope, const char *name);

/* ── Semantic Checker ────────────────────────────────────────── */

/*
 * Run semantic analysis on a parsed AST.
 * Returns 0 if no errors, or the number of semantic errors found.
 *
 * This is additive — it emits diagnostics but does NOT prevent codegen
 * for valid programs. The caller decides whether to abort based on the
 * return value.
 */
int sema_check(NshNode *program, const char *source, const char *filename);

#endif /* NSH_SEMA_H */
