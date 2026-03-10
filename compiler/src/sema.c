/*
 * NashmiC — sema.c
 * Phase 2: Semantic Analysis
 *
 * Three-pass analysis:
 *   Pass 1 — Register top-level declarations (haikal, ta3dad, dalle, yalla)
 *   Pass 2 — Resolve type annotations, check variable references
 *   Pass 3 — Type check expressions, calls, returns
 *
 * Diagnostics use the existing Jordanian proverb system.
 * Errors are soft — they don't block codegen for valid programs.
 */

#define _GNU_SOURCE
#include "sema.h"
#include "diagnostics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Type Helpers ────────────────────────────────────────────── */

NshType *nsh_type_new(NshTypeKind kind) {
    NshType *type = calloc(1, sizeof(NshType));
    type->kind = kind;
    return type;
}

NshType *nsh_type_clone(const NshType *type) {
    if (!type) return NULL;
    NshType *clone = nsh_type_new(type->kind);
    switch (type->kind) {
    case NSH_TYPE_STRUCT:
        clone->as.struct_type.name = strdup(type->as.struct_type.name);
        break;
    case NSH_TYPE_ENUM:
        clone->as.enum_type.name = strdup(type->as.enum_type.name);
        break;
    case NSH_TYPE_RESULT:
        clone->as.result_type.inner = nsh_type_clone(type->as.result_type.inner);
        break;
    case NSH_TYPE_OPTIONAL:
        clone->as.optional_type.inner = nsh_type_clone(type->as.optional_type.inner);
        break;
    case NSH_TYPE_FUNC: {
        int count = type->as.func_type.param_count;
        clone->as.func_type.param_count = count;
        clone->as.func_type.return_type =
            nsh_type_clone(type->as.func_type.return_type);
        if (count > 0) {
            clone->as.func_type.param_types =
                calloc(count, sizeof(NshType *));
            for (int i = 0; i < count; i++) {
                clone->as.func_type.param_types[i] =
                    nsh_type_clone(type->as.func_type.param_types[i]);
            }
        }
        break;
    }
    default:
        break;
    }
    return clone;
}

void nsh_type_free(NshType *type) {
    if (!type) return;
    switch (type->kind) {
    case NSH_TYPE_STRUCT:
        free(type->as.struct_type.name);
        break;
    case NSH_TYPE_ENUM:
        free(type->as.enum_type.name);
        break;
    case NSH_TYPE_RESULT:
        nsh_type_free(type->as.result_type.inner);
        break;
    case NSH_TYPE_OPTIONAL:
        nsh_type_free(type->as.optional_type.inner);
        break;
    case NSH_TYPE_FUNC:
        for (int i = 0; i < type->as.func_type.param_count; i++) {
            nsh_type_free(type->as.func_type.param_types[i]);
        }
        free(type->as.func_type.param_types);
        nsh_type_free(type->as.func_type.return_type);
        break;
    default:
        break;
    }
    free(type);
}

int nsh_type_equal(const NshType *a, const NshType *b) {
    if (!a || !b) return 0;
    if (a->kind == NSH_TYPE_UNKNOWN || b->kind == NSH_TYPE_UNKNOWN) return 1;
    if (a->kind != b->kind) return 0;

    switch (a->kind) {
    case NSH_TYPE_STRUCT:
        return strcmp(a->as.struct_type.name, b->as.struct_type.name) == 0;
    case NSH_TYPE_ENUM:
        return strcmp(a->as.enum_type.name, b->as.enum_type.name) == 0;
    case NSH_TYPE_RESULT:
        return nsh_type_equal(a->as.result_type.inner,
                              b->as.result_type.inner);
    case NSH_TYPE_OPTIONAL:
        return nsh_type_equal(a->as.optional_type.inner,
                              b->as.optional_type.inner);
    default:
        return 1; /* same primitive kind */
    }
}

/* Static buffer for type name display — not thread-safe but fine for
 * a single-threaded compiler. */
static char type_name_buf[256];

const char *nsh_type_name(const NshType *type) {
    if (!type) return "??";
    switch (type->kind) {
    case NSH_TYPE_INT32:    return "adad32";
    case NSH_TYPE_INT64:    return "adad64";
    case NSH_TYPE_FLOAT:    return "fasle32";
    case NSH_TYPE_DOUBLE:   return "fasle64";
    case NSH_TYPE_BOOL:     return "mante2";
    case NSH_TYPE_CHAR:     return "harf";
    case NSH_TYPE_STRING:   return "nass";
    case NSH_TYPE_VOID:     return "fara3'";
    case NSH_TYPE_STRUCT:
        return type->as.struct_type.name ? type->as.struct_type.name : "haikal";
    case NSH_TYPE_ENUM:
        return type->as.enum_type.name ? type->as.enum_type.name : "ta3dad";
    case NSH_TYPE_RESULT:
        snprintf(type_name_buf, sizeof(type_name_buf), "natije<%s>",
                 nsh_type_name(type->as.result_type.inner));
        return type_name_buf;
    case NSH_TYPE_OPTIONAL:
        snprintf(type_name_buf, sizeof(type_name_buf), "yimkin<%s>",
                 nsh_type_name(type->as.optional_type.inner));
        return type_name_buf;
    case NSH_TYPE_FUNC:     return "dalle";
    case NSH_TYPE_UNKNOWN:  return "??";
    }
    return "??";
}

/* ── Scope / Symbol Table ────────────────────────────────────── */

static unsigned hash_string(const char *str) {
    unsigned h = 5381;
    while (*str) {
        h = ((h << 5) + h) + (unsigned char)*str;
        str++;
    }
    return h;
}

NshScope *scope_new(NshScope *parent) {
    NshScope *scope = calloc(1, sizeof(NshScope));
    scope->parent = parent;
    return scope;
}

void scope_free(NshScope *scope) {
    if (!scope) return;
    for (int i = 0; i < SCOPE_BUCKET_COUNT; i++) {
        if (scope->buckets[i].sym) {
            free(scope->buckets[i].sym->name);
            nsh_type_free(scope->buckets[i].sym->type_info);
            free(scope->buckets[i].sym);
        }
    }
    free(scope);
}

int scope_insert(NshScope *scope, NshSymbol *sym) {
    unsigned h = hash_string(sym->name);
    unsigned idx = h % SCOPE_BUCKET_COUNT;

    /* Linear probe for open addressing */
    for (int attempt = 0; attempt < SCOPE_BUCKET_COUNT; attempt++) {
        unsigned slot = (idx + (unsigned)attempt) % SCOPE_BUCKET_COUNT;
        if (!scope->buckets[slot].sym) {
            scope->buckets[slot].sym = sym;
            scope->buckets[slot].hash = h;
            scope->count++;
            return 1;
        }
        if (scope->buckets[slot].hash == h &&
            strcmp(scope->buckets[slot].sym->name, sym->name) == 0) {
            return 0; /* duplicate */
        }
    }
    return 0; /* table full */
}

NshSymbol *scope_lookup_local(NshScope *scope, const char *name) {
    unsigned h = hash_string(name);
    unsigned idx = h % SCOPE_BUCKET_COUNT;

    for (int attempt = 0; attempt < SCOPE_BUCKET_COUNT; attempt++) {
        unsigned slot = (idx + (unsigned)attempt) % SCOPE_BUCKET_COUNT;
        if (!scope->buckets[slot].sym) return NULL;
        if (scope->buckets[slot].hash == h &&
            strcmp(scope->buckets[slot].sym->name, name) == 0) {
            return scope->buckets[slot].sym;
        }
    }
    return NULL;
}

NshSymbol *scope_lookup(NshScope *scope, const char *name) {
    for (NshScope *s = scope; s; s = s->parent) {
        NshSymbol *sym = scope_lookup_local(s, name);
        if (sym) return sym;
    }
    return NULL;
}

/* ── Semantic Checker Context ────────────────────────────────── */

typedef struct {
    NshScope *global_scope;
    NshScope *current_scope;
    const char *source;
    const char *filename;
    int error_count;
    NshType *current_return_type; /* expected return type of current function */
} SemaCtx;

static void sema_error(SemaCtx *ctx, NshSpan span, const char *msg) {
    ctx->error_count++;
    diag_error_at(ctx->filename, ctx->source, span, msg);
}

static void sema_warn(SemaCtx *ctx, NshSpan span, const char *msg) {
    diag_warning_at(ctx->filename, ctx->source, span, msg);
}

/* ── Type Resolution ─────────────────────────────────────────── */

/* Resolve a type annotation string to an NshType. */
static NshType *resolve_type_name(SemaCtx *ctx, const char *name) {
    if (!name) return nsh_type_new(NSH_TYPE_VOID);

    /* Primitive types */
    if (strcmp(name, "adad32") == 0)  return nsh_type_new(NSH_TYPE_INT32);
    if (strcmp(name, "adad64") == 0)  return nsh_type_new(NSH_TYPE_INT64);
    if (strcmp(name, "fasle32") == 0) return nsh_type_new(NSH_TYPE_FLOAT);
    if (strcmp(name, "fasle64") == 0) return nsh_type_new(NSH_TYPE_DOUBLE);
    if (strcmp(name, "mante2") == 0)  return nsh_type_new(NSH_TYPE_BOOL);
    if (strcmp(name, "harf") == 0)    return nsh_type_new(NSH_TYPE_CHAR);
    if (strcmp(name, "nass") == 0)    return nsh_type_new(NSH_TYPE_STRING);
    if (strcmp(name, "fara3'") == 0 || strcmp(name, "void") == 0)
        return nsh_type_new(NSH_TYPE_VOID);

    /* natije<T> — result type */
    if (strncmp(name, "natije<", 7) == 0) {
        size_t len = strlen(name);
        if (len > 8 && name[len - 1] == '>') {
            char inner_name[128];
            size_t inner_len = len - 8; /* skip "natije<" and ">" */
            if (inner_len >= sizeof(inner_name)) inner_len = sizeof(inner_name) - 1;
            memcpy(inner_name, name + 7, inner_len);
            inner_name[inner_len] = '\0';

            NshType *result = nsh_type_new(NSH_TYPE_RESULT);
            result->as.result_type.inner = resolve_type_name(ctx, inner_name);
            return result;
        }
    }

    /* yimkin<T> — optional type */
    if (strncmp(name, "yimkin<", 7) == 0) {
        size_t len = strlen(name);
        if (len > 8 && name[len - 1] == '>') {
            char inner_name[128];
            size_t inner_len = len - 8;
            if (inner_len >= sizeof(inner_name)) inner_len = sizeof(inner_name) - 1;
            memcpy(inner_name, name + 7, inner_len);
            inner_name[inner_len] = '\0';

            NshType *opt = nsh_type_new(NSH_TYPE_OPTIONAL);
            opt->as.optional_type.inner = resolve_type_name(ctx, inner_name);
            return opt;
        }
    }

    /* User-defined types — check symbol table */
    NshSymbol *sym = scope_lookup(ctx->current_scope, name);
    if (sym && sym->kind == SYM_TYPE) {
        if (sym->type_info) return nsh_type_clone(sym->type_info);
    }

    /* Unknown type — return UNKNOWN for error recovery */
    return nsh_type_new(NSH_TYPE_UNKNOWN);
}

/* ── Helper: Create a symbol ─────────────────────────────────── */

static NshSymbol *make_symbol(const char *name, NshSymKind kind,
                              NshType *type_info, NshSpan span) {
    NshSymbol *sym = calloc(1, sizeof(NshSymbol));
    sym->name = strdup(name);
    sym->kind = kind;
    sym->type_info = type_info;
    sym->span = span;
    sym->is_used = 0;
    return sym;
}

/* ── Built-in Functions ──────────────────────────────────────── */

/* Helper to register a variadic built-in function */
static void register_variadic_builtin(SemaCtx *ctx, const char *name,
                                       NshTypeKind return_kind) {
    NshType *ftype = nsh_type_new(NSH_TYPE_FUNC);
    ftype->as.func_type.param_types = NULL;
    ftype->as.func_type.param_count = -1; /* variadic sentinel */
    ftype->as.func_type.return_type = nsh_type_new(return_kind);

    NshSymbol *sym = make_symbol(name, SYM_FUNC, ftype, NSH_SPAN_NONE);
    sym->is_used = 1;
    scope_insert(ctx->global_scope, sym);
}

/* Helper to register a simple unary built-in function */
static void register_unary_builtin(SemaCtx *ctx, const char *name,
                                    NshTypeKind param_kind,
                                    NshTypeKind return_kind) {
    NshType *ftype = nsh_type_new(NSH_TYPE_FUNC);
    ftype->as.func_type.param_count = 1;
    ftype->as.func_type.param_types = calloc(1, sizeof(NshType *));
    ftype->as.func_type.param_types[0] = nsh_type_new(param_kind);
    ftype->as.func_type.return_type = nsh_type_new(return_kind);

    NshSymbol *sym = make_symbol(name, SYM_FUNC, ftype, NSH_SPAN_NONE);
    sym->is_used = 1;
    scope_insert(ctx->global_scope, sym);
}

/* Helper to register a nullary built-in function */
static void register_nullary_builtin(SemaCtx *ctx, const char *name,
                                      NshTypeKind return_kind) {
    NshType *ftype = nsh_type_new(NSH_TYPE_FUNC);
    ftype->as.func_type.param_count = 0;
    ftype->as.func_type.param_types = NULL;
    ftype->as.func_type.return_type = nsh_type_new(return_kind);

    NshSymbol *sym = make_symbol(name, SYM_FUNC, ftype, NSH_SPAN_NONE);
    sym->is_used = 1;
    scope_insert(ctx->global_scope, sym);
}

static void register_builtins(SemaCtx *ctx) {
    /* Core I/O */
    register_variadic_builtin(ctx, "itba3", NSH_TYPE_VOID);
    register_nullary_builtin(ctx, "i2ra", NSH_TYPE_STRING);
    register_unary_builtin(ctx, "itla3", NSH_TYPE_INT64, NSH_TYPE_VOID);
    register_unary_builtin(ctx, "toul", NSH_TYPE_STRING, NSH_TYPE_INT64);

    /* Easter egg functions */
    register_nullary_builtin(ctx, "drobi", NSH_TYPE_VOID);
    register_nullary_builtin(ctx, "mansaf", NSH_TYPE_VOID);
    register_nullary_builtin(ctx, "sahteen", NSH_TYPE_VOID);
    register_nullary_builtin(ctx, "nashmi", NSH_TYPE_VOID);

    /* C math pass-throughs (linked via -lm) */
    register_unary_builtin(ctx, "sqrt", NSH_TYPE_DOUBLE, NSH_TYPE_DOUBLE);
    register_unary_builtin(ctx, "abs", NSH_TYPE_INT64, NSH_TYPE_INT64);
    register_unary_builtin(ctx, "sin", NSH_TYPE_DOUBLE, NSH_TYPE_DOUBLE);
    register_unary_builtin(ctx, "cos", NSH_TYPE_DOUBLE, NSH_TYPE_DOUBLE);

    /* Result/Optional identifiers — these normally go through
     * NODE_RESULT_WRAP or NODE_NULL_LIT, but register as symbols
     * for cases where codegen uses them as plain idents. */
    NshSymbol *mafi_sym = make_symbol("mafi", SYM_ENUM_VARIANT,
                                       nsh_type_new(NSH_TYPE_UNKNOWN),
                                       NSH_SPAN_NONE);
    mafi_sym->is_used = 1;
    scope_insert(ctx->global_scope, mafi_sym);
}

/* ── Forward declarations ────────────────────────────────────── */

static NshType *check_expr(SemaCtx *ctx, NshNode *node);
static void check_stmt(SemaCtx *ctx, NshNode *node);
static void check_block(SemaCtx *ctx, NshNode *node);

/* ── Pass 1: Declaration Registration ────────────────────────── */

static void pass1_register_decls(SemaCtx *ctx, NshNode *program) {
    NshNodeList *decls = &program->as.program.decls;

    for (int i = 0; i < decls->count; i++) {
        NshNode *decl = decls->items[i];

        switch (decl->type) {
        case NODE_STRUCT_DECL: {
            NshType *stype = nsh_type_new(NSH_TYPE_STRUCT);
            stype->as.struct_type.name = strdup(decl->as.struct_decl.name);

            NshSymbol *sym = make_symbol(decl->as.struct_decl.name,
                                          SYM_TYPE, stype, decl->span);
            sym->is_used = 1; /* don't warn on unused types for now */
            if (!scope_insert(ctx->global_scope, sym)) {
                char msg[256];
                snprintf(msg, sizeof(msg),
                         "type '%s' is already defined",
                         decl->as.struct_decl.name);
                sema_error(ctx, decl->span, msg);
                free(sym->name);
                nsh_type_free(sym->type_info);
                free(sym);
            }
            break;
        }
        case NODE_ENUM_DECL: {
            NshType *etype = nsh_type_new(NSH_TYPE_ENUM);
            etype->as.enum_type.name = strdup(decl->as.enum_decl.name);

            NshSymbol *sym = make_symbol(decl->as.enum_decl.name,
                                          SYM_TYPE, etype, decl->span);
            sym->is_used = 1;
            if (!scope_insert(ctx->global_scope, sym)) {
                char msg[256];
                snprintf(msg, sizeof(msg),
                         "type '%s' is already defined",
                         decl->as.enum_decl.name);
                sema_error(ctx, decl->span, msg);
                free(sym->name);
                nsh_type_free(sym->type_info);
                free(sym);
            }

            /* Register enum variants as globals */
            NshParamList *variants = &decl->as.enum_decl.variants;
            for (int v = 0; v < variants->count; v++) {
                NshType *vtype = nsh_type_clone(etype);

                NshSymbol *vsym = make_symbol(variants->items[v].name,
                                               SYM_ENUM_VARIANT,
                                               vtype, decl->span);
                vsym->is_used = 1;
                scope_insert(ctx->global_scope, vsym);
            }
            break;
        }
        case NODE_FUNC_DECL: {
            NshType *ftype = nsh_type_new(NSH_TYPE_FUNC);
            int param_count = decl->as.func_decl.params.count;
            ftype->as.func_type.param_count = param_count;

            if (param_count > 0) {
                ftype->as.func_type.param_types =
                    calloc(param_count, sizeof(NshType *));
                for (int p = 0; p < param_count; p++) {
                    ftype->as.func_type.param_types[p] =
                        resolve_type_name(ctx,
                            decl->as.func_decl.params.items[p].type_name);
                }
            }

            ftype->as.func_type.return_type =
                resolve_type_name(ctx, decl->as.func_decl.return_type);

            const char *func_name = decl->as.func_decl.is_yalla
                                    ? "yalla"
                                    : decl->as.func_decl.name;

            NshSymbol *sym = make_symbol(func_name, SYM_FUNC,
                                          ftype, decl->span);
            sym->is_used = 1; /* don't warn on unused functions */
            if (!scope_insert(ctx->global_scope, sym)) {
                char msg[256];
                snprintf(msg, sizeof(msg),
                         "function '%s' is already defined", func_name);
                sema_error(ctx, decl->span, msg);
                free(sym->name);
                nsh_type_free(sym->type_info);
                free(sym);
            }
            break;
        }
        case NODE_IMPL_BLOCK: {
            /* Register methods as global functions with mangled names
             * (TypeName_method). This matches codegen behavior. */
            NshNodeList *methods = &decl->as.impl_block.methods;
            for (int m = 0; m < methods->count; m++) {
                NshNode *method = methods->items[m];
                if (method->type != NODE_FUNC_DECL) continue;

                char mangled[256];
                snprintf(mangled, sizeof(mangled), "%s_%s",
                         decl->as.impl_block.type_name,
                         method->as.func_decl.name);

                int param_count = method->as.func_decl.params.count;
                NshType *ftype = nsh_type_new(NSH_TYPE_FUNC);
                ftype->as.func_type.param_count = param_count;

                if (param_count > 0) {
                    ftype->as.func_type.param_types =
                        calloc(param_count, sizeof(NshType *));
                    for (int p = 0; p < param_count; p++) {
                        ftype->as.func_type.param_types[p] =
                            resolve_type_name(ctx,
                                method->as.func_decl.params.items[p].type_name);
                    }
                }

                ftype->as.func_type.return_type =
                    resolve_type_name(ctx, method->as.func_decl.return_type);

                NshSymbol *sym = make_symbol(mangled, SYM_FUNC,
                                              ftype, method->span);
                sym->is_used = 1;
                scope_insert(ctx->global_scope, sym);
            }
            break;
        }
        default:
            break;
        }
    }
}

/* ── Pass 2 & 3: Check Expressions ──────────────────────────── */

static NshType *check_expr(SemaCtx *ctx, NshNode *node) {
    if (!node) return nsh_type_new(NSH_TYPE_UNKNOWN);

    switch (node->type) {
    case NODE_INT_LIT:
        return nsh_type_new(NSH_TYPE_INT64);

    case NODE_FLOAT_LIT:
        return nsh_type_new(NSH_TYPE_DOUBLE);

    case NODE_STRING_LIT:
        return nsh_type_new(NSH_TYPE_STRING);

    case NODE_INTERP_STRING:
        /* Check sub-expressions inside interpolation */
        for (int i = 0; i < node->as.interp_string.parts.count; i++) {
            NshType *part_type = check_expr(ctx, node->as.interp_string.parts.items[i]);
            nsh_type_free(part_type);
        }
        return nsh_type_new(NSH_TYPE_STRING);

    case NODE_BOOL_LIT:
        return nsh_type_new(NSH_TYPE_BOOL);

    case NODE_NULL_LIT:
        return nsh_type_new(NSH_TYPE_UNKNOWN);

    case NODE_IDENT: {
        const char *name = node->as.ident.name;
        NshSymbol *sym = scope_lookup(ctx->current_scope, name);
        if (!sym) {
            char msg[256];
            snprintf(msg, sizeof(msg),
                     "undeclared variable '%s'", name);
            sema_error(ctx, node->span, msg);
            return nsh_type_new(NSH_TYPE_UNKNOWN);
        }
        sym->is_used = 1;
        if (sym->type_info) return nsh_type_clone(sym->type_info);
        return nsh_type_new(NSH_TYPE_UNKNOWN);
    }

    case NODE_BINARY: {
        NshType *left = check_expr(ctx, node->as.binary.left);
        NshType *right = check_expr(ctx, node->as.binary.right);

        NshType *result = NULL;
        NshBinOp op = node->as.binary.op;

        if (op == BIN_RANGE) {
            /* Range is always int..int */
            nsh_type_free(left);
            nsh_type_free(right);
            return nsh_type_new(NSH_TYPE_UNKNOWN); /* range type */
        }

        /* Comparison/logical operators return bool */
        if (op == BIN_EQ || op == BIN_NEQ || op == BIN_LT ||
            op == BIN_GT || op == BIN_LTE || op == BIN_GTE ||
            op == BIN_AND || op == BIN_OR) {
            result = nsh_type_new(NSH_TYPE_BOOL);
        } else {
            /* Arithmetic — result is the type of left operand */
            result = nsh_type_clone(left);
        }

        /* Type compatibility check for arithmetic ops (soft — warn only) */
        if (op >= BIN_ADD && op <= BIN_MOD) {
            if (left->kind != NSH_TYPE_UNKNOWN &&
                right->kind != NSH_TYPE_UNKNOWN &&
                !nsh_type_equal(left, right)) {
                /* Allow int/float mixing — common pattern */
                int is_numeric_left = (left->kind == NSH_TYPE_INT32 ||
                                       left->kind == NSH_TYPE_INT64 ||
                                       left->kind == NSH_TYPE_FLOAT ||
                                       left->kind == NSH_TYPE_DOUBLE);
                int is_numeric_right = (right->kind == NSH_TYPE_INT32 ||
                                        right->kind == NSH_TYPE_INT64 ||
                                        right->kind == NSH_TYPE_FLOAT ||
                                        right->kind == NSH_TYPE_DOUBLE);
                if (!is_numeric_left || !is_numeric_right) {
                    char msg[256];
                    snprintf(msg, sizeof(msg),
                             "type mismatch in binary operation: "
                             "%s vs %s",
                             nsh_type_name(left), nsh_type_name(right));
                    sema_warn(ctx, node->span, msg);
                }
            }
        }

        nsh_type_free(left);
        nsh_type_free(right);
        return result;
    }

    case NODE_UNARY: {
        NshType *operand = check_expr(ctx, node->as.unary.operand);
        if (node->as.unary.op == UNARY_NOT) {
            nsh_type_free(operand);
            return nsh_type_new(NSH_TYPE_BOOL);
        }
        return operand; /* negation preserves type */
    }

    case NODE_CALL: {
        /* Check callee */
        NshNode *callee = node->as.call.callee;
        NshType *callee_type = NULL;
        NshSymbol *func_sym = NULL;

        if (callee->type == NODE_IDENT) {
            func_sym = scope_lookup(ctx->current_scope, callee->as.ident.name);
            if (!func_sym) {
                /* Could be an enum variant constructor */
                char msg[256];
                snprintf(msg, sizeof(msg),
                         "undeclared function '%s'",
                         callee->as.ident.name);
                sema_error(ctx, callee->span, msg);

                /* Still check arguments */
                for (int i = 0; i < node->as.call.args.count; i++) {
                    NshType *arg_type = check_expr(ctx,
                        node->as.call.args.items[i]);
                    nsh_type_free(arg_type);
                }
                return nsh_type_new(NSH_TYPE_UNKNOWN);
            }
            func_sym->is_used = 1;

            /* Enum variant used as constructor — return enum type */
            if (func_sym->kind == SYM_ENUM_VARIANT) {
                for (int i = 0; i < node->as.call.args.count; i++) {
                    NshType *arg_type = check_expr(ctx,
                        node->as.call.args.items[i]);
                    nsh_type_free(arg_type);
                }
                return nsh_type_clone(func_sym->type_info);
            }

            callee_type = func_sym->type_info;
        } else if (callee->type == NODE_MEMBER) {
            /* method call — obj.method(args) */
            NshType *obj_type = check_expr(ctx, callee->as.member.object);
            nsh_type_free(obj_type);

            /* Check arguments */
            for (int i = 0; i < node->as.call.args.count; i++) {
                NshType *arg_type = check_expr(ctx,
                    node->as.call.args.items[i]);
                nsh_type_free(arg_type);
            }
            return nsh_type_new(NSH_TYPE_UNKNOWN);
        } else {
            callee_type = check_expr(ctx, callee);
            /* Check arguments */
            for (int i = 0; i < node->as.call.args.count; i++) {
                NshType *arg_type = check_expr(ctx,
                    node->as.call.args.items[i]);
                nsh_type_free(arg_type);
            }
            if (!callee_type || callee_type->kind != NSH_TYPE_FUNC) {
                nsh_type_free(callee_type);
                return nsh_type_new(NSH_TYPE_UNKNOWN);
            }
            NshType *ret = nsh_type_clone(callee_type->as.func_type.return_type);
            nsh_type_free(callee_type);
            return ret;
        }

        if (!callee_type || callee_type->kind != NSH_TYPE_FUNC) {
            for (int i = 0; i < node->as.call.args.count; i++) {
                NshType *arg_type = check_expr(ctx,
                    node->as.call.args.items[i]);
                nsh_type_free(arg_type);
            }
            return nsh_type_new(NSH_TYPE_UNKNOWN);
        }

        /* Check argument count (skip for variadic: param_count == -1) */
        int expected_params = callee_type->as.func_type.param_count;
        int actual_args = node->as.call.args.count;
        if (expected_params >= 0 && actual_args != expected_params) {
            char msg[256];
            snprintf(msg, sizeof(msg),
                     "function '%s' expects %d argument(s), got %d",
                     callee->as.ident.name, expected_params, actual_args);
            sema_error(ctx, node->span, msg);
        }

        /* Check arguments */
        for (int i = 0; i < actual_args; i++) {
            NshType *arg_type = check_expr(ctx, node->as.call.args.items[i]);
            nsh_type_free(arg_type);
        }

        /* Return the function's return type */
        return nsh_type_clone(callee_type->as.func_type.return_type);
    }

    case NODE_MEMBER: {
        NshType *obj_type = check_expr(ctx, node->as.member.object);
        /* We don't fully resolve field types yet — return unknown */
        nsh_type_free(obj_type);
        return nsh_type_new(NSH_TYPE_UNKNOWN);
    }

    case NODE_INDEX: {
        NshType *obj_type = check_expr(ctx, node->as.index_expr.object);
        NshType *idx_type = check_expr(ctx, node->as.index_expr.index);
        nsh_type_free(obj_type);
        nsh_type_free(idx_type);
        return nsh_type_new(NSH_TYPE_UNKNOWN);
    }

    case NODE_ASSIGN: {
        NshType *val_type = check_expr(ctx, node->as.assign.value);

        /* Check that target is an lvalue */
        NshNode *target = node->as.assign.target;
        if (target->type == NODE_IDENT) {
            NshSymbol *sym = scope_lookup(ctx->current_scope,
                                           target->as.ident.name);
            if (sym) {
                sym->is_used = 1;
                if (sym->kind == SYM_CONST) {
                    char msg[256];
                    snprintf(msg, sizeof(msg),
                             "cannot assign to constant '%s' (thabet)",
                             target->as.ident.name);
                    sema_error(ctx, node->span, msg);
                }
            }
        } else {
            /* member or index assign — check the sub-expression */
            NshType *tgt = check_expr(ctx, target);
            nsh_type_free(tgt);
        }

        return val_type;
    }

    case NODE_RANGE: {
        NshType *start_type = check_expr(ctx, node->as.range.start);
        NshType *end_type = check_expr(ctx, node->as.range.end);
        nsh_type_free(start_type);
        nsh_type_free(end_type);
        return nsh_type_new(NSH_TYPE_UNKNOWN);
    }

    case NODE_STRUCT_LIT: {
        /* Check that the struct type exists */
        NshSymbol *sym = scope_lookup(ctx->current_scope,
                                       node->as.struct_lit.name);
        if (!sym || sym->kind != SYM_TYPE) {
            char msg[256];
            snprintf(msg, sizeof(msg),
                     "undefined type '%s' in struct literal",
                     node->as.struct_lit.name);
            sema_warn(ctx, node->span, msg);
        } else {
            sym->is_used = 1;
        }

        /* Check field value expressions */
        for (int i = 0; i < node->as.struct_lit.field_count; i++) {
            NshType *field_type = check_expr(ctx,
                node->as.struct_lit.field_values[i]);
            nsh_type_free(field_type);
        }

        NshType *stype = nsh_type_new(NSH_TYPE_STRUCT);
        stype->as.struct_type.name = strdup(node->as.struct_lit.name);
        return stype;
    }

    case NODE_RESULT_WRAP: {
        NshType *inner = check_expr(ctx, node->as.result_wrap.value);
        NshType *result = nsh_type_new(NSH_TYPE_RESULT);
        result->as.result_type.inner = inner;
        return result;
    }

    case NODE_WALA_QUESTION: {
        NshType *expr_type = check_expr(ctx, node->as.wala_question.expr);
        if (node->as.wala_question.error_msg) {
            NshType *msg_type = check_expr(ctx,
                node->as.wala_question.error_msg);
            nsh_type_free(msg_type);
        }
        /* Unwraps result — return inner type */
        if (expr_type->kind == NSH_TYPE_RESULT) {
            NshType *inner = nsh_type_clone(expr_type->as.result_type.inner);
            nsh_type_free(expr_type);
            return inner;
        }
        return expr_type;
    }

    case NODE_OPTIONAL_BIND: {
        /* iza fi x = expr { ... } wala { ... } */
        NshType *expr_type = check_expr(ctx, node->as.optional_bind.expr);

        /* Create scope with bound variable for then block */
        if (node->as.optional_bind.then_block) {
            NshScope *bind_scope = scope_new(ctx->current_scope);
            NshScope *prev_scope = ctx->current_scope;
            ctx->current_scope = bind_scope;

            /* Register the bound variable — unwrap optional inner type */
            NshType *bound_type = nsh_type_new(NSH_TYPE_UNKNOWN);
            if (expr_type->kind == NSH_TYPE_OPTIONAL &&
                expr_type->as.optional_type.inner) {
                nsh_type_free(bound_type);
                bound_type = nsh_type_clone(expr_type->as.optional_type.inner);
            }

            NshSymbol *bsym = make_symbol(
                node->as.optional_bind.var_name, SYM_VAR,
                bound_type, node->span);
            bsym->is_used = 1;
            scope_insert(bind_scope, bsym);

            /* Check then body stmts in the bind scope */
            if (node->as.optional_bind.then_block->type == NODE_BLOCK) {
                NshNodeList *stmts =
                    &node->as.optional_bind.then_block->as.block.stmts;
                for (int i = 0; i < stmts->count; i++) {
                    check_stmt(ctx, stmts->items[i]);
                }
            } else {
                check_block(ctx, node->as.optional_bind.then_block);
            }

            ctx->current_scope = prev_scope;
            scope_free(bind_scope);
        }

        if (node->as.optional_bind.else_block) {
            check_block(ctx, node->as.optional_bind.else_block);
        }

        nsh_type_free(expr_type);
        return nsh_type_new(NSH_TYPE_VOID);
    }

    case NODE_TUPLE_LIT: {
        for (int i = 0; i < node->as.tuple_lit.elements.count; i++) {
            NshType *elem_type = check_expr(ctx,
                node->as.tuple_lit.elements.items[i]);
            nsh_type_free(elem_type);
        }
        return nsh_type_new(NSH_TYPE_UNKNOWN);
    }

    default:
        return nsh_type_new(NSH_TYPE_UNKNOWN);
    }
}

/* ── Pass 2 & 3: Check Statements ───────────────────────────── */

static void check_block(SemaCtx *ctx, NshNode *node) {
    if (!node) return;
    if (node->type != NODE_BLOCK) {
        check_stmt(ctx, node);
        return;
    }

    NshScope *block_scope = scope_new(ctx->current_scope);
    NshScope *prev_scope = ctx->current_scope;
    ctx->current_scope = block_scope;

    for (int i = 0; i < node->as.block.stmts.count; i++) {
        check_stmt(ctx, node->as.block.stmts.items[i]);
    }

    /* Check for unused variables in this scope */
    for (int i = 0; i < SCOPE_BUCKET_COUNT; i++) {
        NshSymbol *sym = block_scope->buckets[i].sym;
        if (sym && !sym->is_used &&
            (sym->kind == SYM_VAR || sym->kind == SYM_CONST)) {
            char msg[256];
            snprintf(msg, sizeof(msg),
                     "unused variable '%s' — شو فايدة؟", sym->name);
            sema_warn(ctx, sym->span, msg);
        }
    }

    ctx->current_scope = prev_scope;
    scope_free(block_scope);
}

static void check_stmt(SemaCtx *ctx, NshNode *node) {
    if (!node) return;

    switch (node->type) {
    case NODE_VAR_DECL:
    case NODE_CONST_DECL: {
        NshType *init_type = NULL;
        if (node->as.var_decl.init) {
            init_type = check_expr(ctx, node->as.var_decl.init);
        }

        NshType *declared_type = NULL;
        if (node->as.var_decl.type_ann) {
            declared_type = resolve_type_name(ctx, node->as.var_decl.type_ann);
        }

        /* Use declared type if available, otherwise inferred */
        NshType *final_type = declared_type ? declared_type
                            : init_type ? nsh_type_clone(init_type)
                            : nsh_type_new(NSH_TYPE_UNKNOWN);

        NshSymKind sym_kind = (node->type == NODE_CONST_DECL)
                              ? SYM_CONST : SYM_VAR;

        NshSymbol *sym = make_symbol(node->as.var_decl.name, sym_kind,
                                      final_type, node->span);

        if (!scope_insert(ctx->current_scope, sym)) {
            char msg[256];
            snprintf(msg, sizeof(msg),
                     "variable '%s' already declared in this scope",
                     node->as.var_decl.name);
            sema_warn(ctx, node->span, msg);
            free(sym->name);
            nsh_type_free(sym->type_info);
            free(sym);
        }

        if (init_type) nsh_type_free(init_type);
        break;
    }

    case NODE_RETURN: {
        if (node->as.ret.value) {
            NshType *ret_type = check_expr(ctx, node->as.ret.value);
            nsh_type_free(ret_type);
        }
        break;
    }

    case NODE_IF: {
        NshType *cond_type = check_expr(ctx, node->as.if_stmt.condition);
        nsh_type_free(cond_type);

        check_block(ctx, node->as.if_stmt.then_block);
        if (node->as.if_stmt.else_block) {
            check_block(ctx, node->as.if_stmt.else_block);
        }
        break;
    }

    case NODE_WHILE: {
        NshType *cond_type = check_expr(ctx, node->as.while_stmt.condition);
        nsh_type_free(cond_type);
        check_block(ctx, node->as.while_stmt.body);
        break;
    }

    case NODE_FOR_EACH: {
        NshType *iter_type = check_expr(ctx, node->as.for_each.iterable);

        /* Create scope for loop variable */
        NshScope *loop_scope = scope_new(ctx->current_scope);
        NshScope *prev_scope = ctx->current_scope;
        ctx->current_scope = loop_scope;

        NshSymbol *loop_var = make_symbol(
            node->as.for_each.var_name, SYM_VAR,
            nsh_type_new(NSH_TYPE_INT64), /* loop var is int by default */
            node->span);
        loop_var->is_used = 1; /* loop vars are implicitly used */
        scope_insert(loop_scope, loop_var);

        /* Check body — but it's a block, so check_block will create
         * another scope. We need the loop var visible, so we check
         * the body's stmts directly in the loop_scope. */
        if (node->as.for_each.body &&
            node->as.for_each.body->type == NODE_BLOCK) {
            for (int i = 0;
                 i < node->as.for_each.body->as.block.stmts.count; i++) {
                check_stmt(ctx,
                    node->as.for_each.body->as.block.stmts.items[i]);
            }
        } else {
            check_block(ctx, node->as.for_each.body);
        }

        ctx->current_scope = prev_scope;
        scope_free(loop_scope);
        nsh_type_free(iter_type);
        break;
    }

    case NODE_LOOP: {
        check_block(ctx, node->as.loop.body);
        break;
    }

    case NODE_BLOCK: {
        check_block(ctx, node);
        break;
    }

    case NODE_EXPR_STMT: {
        NshType *expr_type = check_expr(ctx, node->as.expr_stmt.expr);
        nsh_type_free(expr_type);
        break;
    }

    case NODE_DEFER: {
        if (node->as.defer.body) {
            if (node->as.defer.body->type == NODE_BLOCK) {
                check_block(ctx, node->as.defer.body);
            } else {
                /* Defer with a single expression (e.g., ba3dain expr) */
                NshType *defer_type = check_expr(ctx, node->as.defer.body);
                nsh_type_free(defer_type);
            }
        }
        break;
    }

    case NODE_MATCH: {
        NshType *subject_type = check_expr(ctx, node->as.match.subject);

        for (int i = 0; i < node->as.match.arms.count; i++) {
            NshNode *arm = node->as.match.arms.items[i];
            if (arm->type != NODE_MATCH_ARM) continue;

            /* Create scope for arm bindings */
            NshScope *arm_scope = scope_new(ctx->current_scope);
            NshScope *prev_scope = ctx->current_scope;
            ctx->current_scope = arm_scope;

            /* Register bound variables from pattern */
            for (int b = 0; b < arm->as.match_arm.bindings.count; b++) {
                NshNode *binding = arm->as.match_arm.bindings.items[b];
                if (binding->type == NODE_IDENT) {
                    NshSymbol *bsym = make_symbol(
                        binding->as.ident.name, SYM_VAR,
                        nsh_type_new(NSH_TYPE_UNKNOWN),
                        binding->span);
                    bsym->is_used = 1; /* pattern bindings are used */
                    scope_insert(arm_scope, bsym);
                }
            }

            /* Check arm body */
            if (arm->as.match_arm.body) {
                check_block(ctx, arm->as.match_arm.body);
            }

            ctx->current_scope = prev_scope;
            scope_free(arm_scope);
        }

        nsh_type_free(subject_type);
        break;
    }

    case NODE_OPTIONAL_BIND: {
        /* iza fi x = expr { ... } wala { ... } — at statement level */
        NshType *expr_type = check_expr(ctx, node->as.optional_bind.expr);

        if (node->as.optional_bind.then_block) {
            NshScope *bind_scope = scope_new(ctx->current_scope);
            NshScope *prev_scope = ctx->current_scope;
            ctx->current_scope = bind_scope;

            NshType *bound_type = nsh_type_new(NSH_TYPE_UNKNOWN);
            if (expr_type->kind == NSH_TYPE_OPTIONAL &&
                expr_type->as.optional_type.inner) {
                nsh_type_free(bound_type);
                bound_type = nsh_type_clone(expr_type->as.optional_type.inner);
            }

            NshSymbol *bsym = make_symbol(
                node->as.optional_bind.var_name, SYM_VAR,
                bound_type, node->span);
            bsym->is_used = 1;
            scope_insert(bind_scope, bsym);

            if (node->as.optional_bind.then_block->type == NODE_BLOCK) {
                NshNodeList *stmts =
                    &node->as.optional_bind.then_block->as.block.stmts;
                for (int s = 0; s < stmts->count; s++) {
                    check_stmt(ctx, stmts->items[s]);
                }
            } else {
                check_block(ctx, node->as.optional_bind.then_block);
            }

            ctx->current_scope = prev_scope;
            scope_free(bind_scope);
        }

        if (node->as.optional_bind.else_block) {
            check_block(ctx, node->as.optional_bind.else_block);
        }

        nsh_type_free(expr_type);
        break;
    }

    case NODE_BREAK:
    case NODE_CONTINUE:
        break;

    default:
        break;
    }
}

/* ── Check Function Body ─────────────────────────────────────── */

static void check_function(SemaCtx *ctx, NshNode *func) {
    if (!func || func->type != NODE_FUNC_DECL) return;

    NshScope *func_scope = scope_new(ctx->current_scope);
    NshScope *prev_scope = ctx->current_scope;
    ctx->current_scope = func_scope;

    /* Register parameters */
    for (int i = 0; i < func->as.func_decl.params.count; i++) {
        NshParam *param = &func->as.func_decl.params.items[i];
        NshType *param_type = resolve_type_name(ctx, param->type_name);

        NshSymbol *psym = make_symbol(param->name, SYM_VAR,
                                       param_type, func->span);
        psym->is_used = 1; /* don't warn on unused params */
        scope_insert(func_scope, psym);
    }

    /* Set return type context */
    NshType *prev_return = ctx->current_return_type;
    ctx->current_return_type = resolve_type_name(ctx,
        func->as.func_decl.return_type);

    /* Check body — check stmts directly in func scope to keep params visible */
    if (func->as.func_decl.body &&
        func->as.func_decl.body->type == NODE_BLOCK) {
        NshNodeList *stmts = &func->as.func_decl.body->as.block.stmts;
        for (int i = 0; i < stmts->count; i++) {
            check_stmt(ctx, stmts->items[i]);
        }
    }

    /* Check unused variables in function scope */
    for (int i = 0; i < SCOPE_BUCKET_COUNT; i++) {
        NshSymbol *sym = func_scope->buckets[i].sym;
        if (sym && !sym->is_used &&
            (sym->kind == SYM_VAR || sym->kind == SYM_CONST)) {
            char msg[256];
            snprintf(msg, sizeof(msg),
                     "unused variable '%s' — شو فايدة؟", sym->name);
            sema_warn(ctx, sym->span, msg);
        }
    }

    nsh_type_free(ctx->current_return_type);
    ctx->current_return_type = prev_return;
    ctx->current_scope = prev_scope;
    scope_free(func_scope);
}

/* ── Main Entry Point ────────────────────────────────────────── */

int sema_check(NshNode *program, const char *source, const char *filename) {
    if (!program || program->type != NODE_PROGRAM) return 0;

    SemaCtx ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.source = source;
    ctx.filename = filename;

    ctx.global_scope = scope_new(NULL);
    ctx.current_scope = ctx.global_scope;
    ctx.current_return_type = NULL;

    /* Register built-in functions */
    register_builtins(&ctx);

    /* Pass 1: Register all top-level declarations */
    pass1_register_decls(&ctx, program);

    /* Pass 2 & 3: Check function bodies */
    NshNodeList *decls = &program->as.program.decls;
    for (int i = 0; i < decls->count; i++) {
        NshNode *decl = decls->items[i];

        switch (decl->type) {
        case NODE_FUNC_DECL:
            check_function(&ctx, decl);
            break;
        case NODE_IMPL_BLOCK: {
            /* Check each method */
            for (int m = 0; m < decl->as.impl_block.methods.count; m++) {
                NshNode *method = decl->as.impl_block.methods.items[m];

                /* Add 'had' (self) to scope before checking */
                NshScope *method_scope = scope_new(ctx.current_scope);
                NshScope *prev_scope = ctx.current_scope;
                ctx.current_scope = method_scope;

                NshType *self_type = nsh_type_new(NSH_TYPE_STRUCT);
                self_type->as.struct_type.name =
                    strdup(decl->as.impl_block.type_name);
                NshSymbol *self_sym = make_symbol("had", SYM_VAR,
                                                    self_type,
                                                    method->span);
                self_sym->is_used = 1;
                scope_insert(method_scope, self_sym);

                check_function(&ctx, method);

                ctx.current_scope = prev_scope;
                scope_free(method_scope);
            }
            break;
        }
        default:
            break;
        }
    }

    int errors = ctx.error_count;

    /* Clean up global scope */
    scope_free(ctx.global_scope);

    return errors;
}
