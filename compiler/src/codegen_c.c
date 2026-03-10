/*
 * NashmiC — codegen_c.c
 * C transpiler backend
 *
 * Translates NashmiC AST into C source code, then the host C compiler
 * (gcc/cc) compiles it into a native binary.
 */

#define _GNU_SOURCE
#include "codegen_c.h"
#include "diagnostics.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

/* ── Variant Registry (enum variant → enum info) ────────────── */

#define MAX_VARIANTS 256

typedef struct {
    char *variant_name;
    char *enum_name;
    char *payload_type;  /* NULL if no payload */
    int tag_index;
} VariantInfo;

/* ── Code Emitter State ──────────────────────────────────────── */

#define MAX_DEFERS 64

typedef struct {
    FILE *out;
    int indent;
    /* defer stack per function */
    int defer_count;
    char *defer_labels[MAX_DEFERS];
    int in_function; /* track if we're inside a function for defer */
    /* variant registry — populated from enum declarations */
    VariantInfo variants[MAX_VARIANTS];
    int variant_count;
    /* result type tracking */
    char *current_result_type;  /* inner type of current function's natije<T>, or NULL */
    /* collected natije<T> types for struct generation */
    char *result_types[64];
    int result_type_count;
} Emitter;

static void emit(Emitter *e, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(e->out, fmt, args);
    va_end(args);
}

static void emit_indent(Emitter *e) {
    for (int i = 0; i < e->indent; i++) {
        fprintf(e->out, "    ");
    }
}

static void emit_line(Emitter *e, const char *fmt, ...) {
    emit_indent(e);
    va_list args;
    va_start(args, fmt);
    vfprintf(e->out, fmt, args);
    va_end(args);
    fprintf(e->out, "\n");
}

/* Forward declarations for result type helpers */
static const char *extract_natije_inner(const char *type);
static const char *result_struct_name(const char *inner_nsh);

/* ── Type Mapping (Franco-only) ──────────────────────────────── */

/* map_type buffer for generating "struct Foo" strings.
 * We use a rotating set of static buffers to avoid overwrites. */
#define MAP_TYPE_BUFS 8
#define MAP_TYPE_BUF_SIZE 256
static char map_type_bufs[MAP_TYPE_BUFS][MAP_TYPE_BUF_SIZE];
static int map_type_idx = 0;

static const char *map_type(const char *nsh_type) {
    if (!nsh_type) return "void";

    if (strcmp(nsh_type, "adad") == 0) return "int32_t";
    if (strcmp(nsh_type, "adad64") == 0) return "int64_t";
    if (strcmp(nsh_type, "fasle") == 0) return "float";
    if (strcmp(nsh_type, "fasle64") == 0) return "double";
    if (strcmp(nsh_type, "mante2") == 0) return "int";
    if (strcmp(nsh_type, "harf") == 0) return "char";
    if (strcmp(nsh_type, "bait") == 0) return "uint8_t";
    if (strcmp(nsh_type, "nass") == 0) return "const char*";
    if (strcmp(nsh_type, "fadi") == 0) return "void";

    /* natije<T> → struct nsh_result_T */
    if (strncmp(nsh_type, "natije<", 7) == 0) {
        const char *inner = extract_natije_inner(nsh_type);
        if (inner) {
            char *buf = map_type_bufs[map_type_idx];
            map_type_idx = (map_type_idx + 1) % MAP_TYPE_BUFS;
            snprintf(buf, MAP_TYPE_BUF_SIZE, "struct %s", result_struct_name(inner));
            return buf;
        }
    }

    /* yimkin<T> → struct nsh_option_T (handled later) */
    if (strncmp(nsh_type, "yimkin<", 7) == 0) {
        /* placeholder — will be wired in Phase 1.4 */
        return "int";
    }

    /* User-defined types: prepend "struct " */
    /* Check if it starts with an uppercase letter (convention for types) */
    if (nsh_type[0] >= 'A' && nsh_type[0] <= 'Z') {
        char *buf = map_type_bufs[map_type_idx];
        map_type_idx = (map_type_idx + 1) % MAP_TYPE_BUFS;
        snprintf(buf, MAP_TYPE_BUF_SIZE, "struct %s", nsh_type);
        return buf;
    }

    /* fallback: pass through */
    return nsh_type;
}

/* ── Variant Registry Helpers ────────────────────────────────── */

static void register_variant(Emitter *e, const char *variant, const char *enum_name,
                             const char *payload_type, int tag) {
    if (e->variant_count >= MAX_VARIANTS) return;
    VariantInfo *v = &e->variants[e->variant_count++];
    v->variant_name = strdup(variant);
    v->enum_name = strdup(enum_name);
    v->payload_type = payload_type ? strdup(payload_type) : NULL;
    v->tag_index = tag;
}

static VariantInfo *find_variant(Emitter *e, const char *name) {
    for (int i = 0; i < e->variant_count; i++) {
        if (strcmp(e->variants[i].variant_name, name) == 0)
            return &e->variants[i];
    }
    return NULL;
}

/* ── Result Type Helpers ─────────────────────────────────────── */

/* Extract inner type from "natije<T>" → "T", returns NULL if not natije */
static const char *extract_natije_inner(const char *type) {
    if (!type) return NULL;
    if (strncmp(type, "natije<", 7) != 0) return NULL;
    /* Find the closing > */
    const char *start = type + 7;
    const char *end = strchr(start, '>');
    if (!end) return NULL;
    static char inner_buf[128];
    int len = (int)(end - start);
    if (len >= 128) len = 127;
    memcpy(inner_buf, start, len);
    inner_buf[len] = '\0';
    return inner_buf;
}

/* Get C struct name for a natije type: "adad64" → "nsh_result_int64_t" */
static const char *result_struct_name(const char *inner_nsh) {
    static char buf[256];
    const char *c_type = map_type(inner_nsh);
    /* Sanitize C type for struct name (replace spaces and *) */
    char sanitized[128];
    int j = 0;
    for (int i = 0; c_type[i] && j < 126; i++) {
        if (c_type[i] == ' ') sanitized[j++] = '_';
        else if (c_type[i] == '*') sanitized[j++] = 'p';
        else sanitized[j++] = c_type[i];
    }
    sanitized[j] = '\0';
    snprintf(buf, sizeof(buf), "nsh_result_%s", sanitized);
    return buf;
}

/* Register a natije<T> type if not already registered */
static void register_result_type(Emitter *e, const char *inner_nsh) {
    for (int i = 0; i < e->result_type_count; i++) {
        if (strcmp(e->result_types[i], inner_nsh) == 0) return;
    }
    if (e->result_type_count < 64) {
        e->result_types[e->result_type_count++] = strdup(inner_nsh);
    }
}

/* Emit a result struct definition */
static void emit_result_struct(Emitter *e, const char *inner_nsh) {
    const char *c_type = map_type(inner_nsh);
    const char *sname = result_struct_name(inner_nsh);
    emit(e, "struct %s {\n", sname);
    emit(e, "    int _is_ok;\n");
    emit(e, "    union {\n");
    emit(e, "        %s _ok;\n", c_type);
    emit(e, "        const char* _err;\n");
    emit(e, "    } _v;\n");
    emit(e, "};\n\n");
}

/* ── Expression Codegen ──────────────────────────────────────── */

static void emit_expr(Emitter *e, NshNode *node);
static void emit_stmt(Emitter *e, NshNode *node);
static void emit_block(Emitter *e, NshNode *node);

static void emit_binop(Emitter *e, NshBinOp op) {
    switch (op) {
    case BIN_ADD: emit(e, " + "); break;
    case BIN_SUB: emit(e, " - "); break;
    case BIN_MUL: emit(e, " * "); break;
    case BIN_DIV: emit(e, " / "); break;
    case BIN_MOD: emit(e, " %% "); break;
    case BIN_EQ:  emit(e, " == "); break;
    case BIN_NEQ: emit(e, " != "); break;
    case BIN_LT:  emit(e, " < "); break;
    case BIN_GT:  emit(e, " > "); break;
    case BIN_LTE: emit(e, " <= "); break;
    case BIN_GTE: emit(e, " >= "); break;
    case BIN_AND: emit(e, " && "); break;
    case BIN_OR:  emit(e, " || "); break;
    case BIN_RANGE: break; /* handled specially */
    }
}

/* Check if an expression node already wraps itself in parentheses */
static int expr_is_parenthesized(NshNode *node) {
    return node->type == NODE_BINARY || node->type == NODE_UNARY;
}

/* Emit an expression wrapped in parens for use as a condition.
 * Avoids double-parens for expressions that already produce them. */
static void emit_condition(Emitter *e, NshNode *node) {
    if (expr_is_parenthesized(node)) {
        emit_expr(e, node);
    } else {
        emit(e, "(");
        emit_expr(e, node);
        emit(e, ")");
    }
}

static void emit_expr(Emitter *e, NshNode *node) {
    switch (node->type) {
    case NODE_INT_LIT:
        emit(e, "%lld", (long long)node->as.int_lit.value);
        break;

    case NODE_FLOAT_LIT:
        emit(e, "%g", node->as.float_lit.value);
        break;

    case NODE_STRING_LIT:
        /* string_lit.value includes the quotes */
        fwrite(node->as.string_lit.value, 1, node->as.string_lit.length, e->out);
        break;

    case NODE_BOOL_LIT:
        emit(e, "%d", node->as.bool_lit.value);
        break;

    case NODE_NULL_LIT:
        emit(e, "NULL");
        break;

    case NODE_IDENT: {
        const char *id = node->as.ident.name;
        /* Map built-in function names */
        if (strcmp(id, "itba3") == 0) { emit(e, "nsh_itba3"); }
        else if (strcmp(id, "i2ra") == 0) { emit(e, "nsh_i2ra"); }
        else if (strcmp(id, "itla3") == 0) { emit(e, "nsh_itla3"); }
        else if (strcmp(id, "drobi") == 0) { emit(e, "nsh_drobi"); }
        else if (strcmp(id, "mansaf") == 0) { emit(e, "nsh_mansaf"); }
        else if (strcmp(id, "sahteen") == 0) { emit(e, "nsh_sahteen"); }
        else if (strcmp(id, "nashmi") == 0) { emit(e, "nsh_nashmi"); }
        else if (strcmp(id, "tamam") == 0) { emit(e, "nsh_tamam"); }
        else if (strcmp(id, "ghalat") == 0) { emit(e, "nsh_ghalat"); }
        else if (strcmp(id, "fi") == 0) { emit(e, "nsh_fi"); }
        else if (strcmp(id, "mafi") == 0) { emit(e, "nsh_mafi"); }
        else {
            /* Check if this is a bare enum variant (no payload) */
            VariantInfo *vi = find_variant(e, id);
            if (vi && !vi->payload_type) {
                emit(e, "(struct %s){._tag = %s_%s}",
                     vi->enum_name, vi->enum_name, vi->variant_name);
            } else {
                emit(e, "%s", id);
            }
        }
        break;
    }

    case NODE_BINARY:
        emit(e, "(");
        emit_expr(e, node->as.binary.left);
        emit_binop(e, node->as.binary.op);
        emit_expr(e, node->as.binary.right);
        emit(e, ")");
        break;

    case NODE_UNARY:
        if (node->as.unary.op == UNARY_NEG) emit(e, "(-");
        else emit(e, "(!");
        emit_expr(e, node->as.unary.operand);
        emit(e, ")");
        break;

    case NODE_CALL: {
        /* Special case: itba3("...{expr}...") — flatten interpolation into print */
        if (node->as.call.callee->type == NODE_IDENT &&
            (strcmp(node->as.call.callee->as.ident.name, "itba3") == 0) &&
            node->as.call.args.count == 1 &&
            node->as.call.args.items[0]->type == NODE_INTERP_STRING) {
            emit_expr(e, node->as.call.args.items[0]);
            break;
        }
        /* Check if this is an enum variant constructor: Da2ira(5.0) */
        if (node->as.call.callee->type == NODE_IDENT) {
            VariantInfo *vi = find_variant(e, node->as.call.callee->as.ident.name);
            if (vi && vi->payload_type) {
                emit(e, "(struct %s){._tag = %s_%s, ._v.%s._0 = ",
                     vi->enum_name, vi->enum_name, vi->variant_name,
                     vi->variant_name);
                if (node->as.call.args.count > 0)
                    emit_expr(e, node->as.call.args.items[0]);
                else
                    emit(e, "0");
                emit(e, "}");
                break;
            }
        }
        emit_expr(e, node->as.call.callee);
        emit(e, "(");
        for (int i = 0; i < node->as.call.args.count; i++) {
            if (i > 0) emit(e, ", ");
            emit_expr(e, node->as.call.args.items[i]);
        }
        emit(e, ")");
        break;
    }

    case NODE_INDEX:
        emit_expr(e, node->as.index_expr.object);
        emit(e, "[");
        emit_expr(e, node->as.index_expr.index);
        emit(e, "]");
        break;

    case NODE_MEMBER:
        emit_expr(e, node->as.member.object);
        emit(e, ".%s", node->as.member.field);
        break;

    case NODE_ASSIGN:
        emit_expr(e, node->as.assign.target);
        emit(e, " = ");
        emit_expr(e, node->as.assign.value);
        break;

    case NODE_INTERP_STRING:
        /* Emit as a statement expression: ({ frag; expr; frag; ... })
         * Each fragment is a printf, each expression uses a typed print helper. */
        emit(e, "({ ");
        for (int i = 0; i < node->as.interp_string.parts.count; i++) {
            NshNode *part = node->as.interp_string.parts.items[i];
            if (part->type == NODE_STRING_LIT) {
                emit(e, "printf(\"");
                fwrite(part->as.string_lit.value, 1,
                       part->as.string_lit.length, e->out);
                emit(e, "\"); ");
            } else if (part->type == NODE_INT_LIT) {
                emit(e, "_nsh_print_i64(");
                emit_expr(e, part);
                emit(e, "); ");
            } else if (part->type == NODE_FLOAT_LIT) {
                emit(e, "_nsh_print_f64(");
                emit_expr(e, part);
                emit(e, "); ");
            } else {
                /* For identifiers and expressions, emit a
                 * _Generic dispatch as a compound expression */
                emit(e, "_Generic((");
                emit_expr(e, part);
                emit(e, "), ");
                emit(e, "int: _nsh_print_i32, ");
                emit(e, "long: _nsh_print_i64, ");
                emit(e, "long long: _nsh_print_i64, ");
                emit(e, "float: _nsh_print_f64, ");
                emit(e, "double: _nsh_print_f64, ");
                emit(e, "char*: _nsh_print_str, ");
                emit(e, "const char*: _nsh_print_str, ");
                emit(e, "default: _nsh_print_i64");
                emit(e, ")(");
                emit_expr(e, part);
                emit(e, "); ");
            }
        }
        emit(e, "})");
        break;

    case NODE_STRUCT_LIT:
        emit(e, "(struct %s){", node->as.struct_lit.name);
        for (int i = 0; i < node->as.struct_lit.field_count; i++) {
            if (i > 0) emit(e, ", ");
            emit(e, ".%s = ", node->as.struct_lit.field_names[i]);
            emit_expr(e, node->as.struct_lit.field_values[i]);
        }
        emit(e, "}");
        break;

    case NODE_RESULT_WRAP: {
        /* tamam(expr) or ghalat(expr) */
        if (!e->current_result_type) {
            /* Fallback: no tracked result type */
            emit(e, "/* tamam/ghalat outside natije context */0");
            break;
        }
        const char *sname = result_struct_name(e->current_result_type);
        if (node->as.result_wrap.is_ok) {
            emit(e, "(struct %s){._is_ok = 1, ._v._ok = ", sname);
            emit_expr(e, node->as.result_wrap.value);
            emit(e, "}");
        } else {
            emit(e, "(struct %s){._is_ok = 0, ._v._err = ", sname);
            emit_expr(e, node->as.result_wrap.value);
            emit(e, "}");
        }
        break;
    }

    case NODE_WALA_QUESTION: {
        /* expr wala? → unwrap or propagate error */
        if (!e->current_result_type) {
            emit(e, "/* wala? outside natije context */");
            emit_expr(e, node->as.wala_question.expr);
            break;
        }
        const char *sname = result_struct_name(e->current_result_type);
        /* GNU C statement expression */
        emit(e, "({ struct %s _nsh_w = ", sname);
        emit_expr(e, node->as.wala_question.expr);
        emit(e, "; if (!_nsh_w._is_ok) return (struct %s){._is_ok = 0, ._v._err = _nsh_w._v._err}; _nsh_w._v._ok; })",
             sname);
        break;
    }

    default:
        emit(e, "/* unknown expr */0");
        break;
    }
}

/* ── Statement Codegen ───────────────────────────────────────── */

static void emit_block(Emitter *e, NshNode *node) {
    emit(e, "{\n");
    e->indent++;
    for (int i = 0; i < node->as.block.stmts.count; i++) {
        emit_stmt(e, node->as.block.stmts.items[i]);
    }
    e->indent--;
    emit_indent(e);
    emit(e, "}\n");
}

/* Emit all deferred statements (LIFO order) into the function epilogue.
 * Called when the function body is done to emit the cleanup chain. */
static void emit_defer_cleanup(Emitter *e);
static void emit_defer_cleanup(Emitter *e) {
    if (e->defer_count == 0) return;
    emit_line(e, "goto _nsh_skip_cleanup;");
    emit_line(e, "_nsh_cleanup:");
    /* Defers execute in LIFO order */
    for (int i = e->defer_count - 1; i >= 0; i--) {
        emit_line(e, "/* ba3dain %d */", i);
        emit(e, "%s", e->defer_labels[i]);
    }
    emit_line(e, "return;");
    emit_line(e, "_nsh_skip_cleanup: ;");
}

static void emit_stmt(Emitter *e, NshNode *node) {
    switch (node->type) {
    case NODE_VAR_DECL:
    case NODE_CONST_DECL: {
        const char *c_type = "int64_t"; /* default to int64 for untyped */
        if (node->as.var_decl.type_ann) {
            c_type = map_type(node->as.var_decl.type_ann);
        } else if (node->as.var_decl.init) {
            /* infer type from initializer */
            NshNode *init = node->as.var_decl.init;
            if (init->type == NODE_INT_LIT) c_type = "int64_t";
            else if (init->type == NODE_FLOAT_LIT) c_type = "double";
            else if (init->type == NODE_STRING_LIT) c_type = "const char*";
            else if (init->type == NODE_BOOL_LIT) c_type = "int";
            else if (init->type == NODE_INTERP_STRING) c_type = "void";
        }

        /* Interpolated strings as statements (they call nsh_itba3 directly) */
        if (node->as.var_decl.init &&
            node->as.var_decl.init->type == NODE_INTERP_STRING) {
            emit_indent(e);
            emit_expr(e, node->as.var_decl.init);
            emit(e, ";\n");
            break;
        }

        emit_indent(e);
        if (node->type == NODE_CONST_DECL) emit(e, "const ");
        emit(e, "%s %s", c_type, node->as.var_decl.name);
        if (node->as.var_decl.init) {
            emit(e, " = ");
            emit_expr(e, node->as.var_decl.init);
        }
        emit(e, ";\n");
        break;
    }

    case NODE_RETURN:
        emit_indent(e);
        if (e->defer_count > 0) {
            /* With defers, we jump to cleanup chain */
            if (node->as.ret.value) {
                emit(e, "{ _nsh_retval = ");
                emit_expr(e, node->as.ret.value);
                emit(e, "; goto _nsh_cleanup; }\n");
            } else {
                emit(e, "goto _nsh_cleanup;\n");
            }
        } else {
            if (node->as.ret.value) {
                emit(e, "return ");
                emit_expr(e, node->as.ret.value);
                emit(e, ";\n");
            } else {
                emit(e, "return;\n");
            }
        }
        break;

    case NODE_IF:
        emit_indent(e);
        emit(e, "if ");
        emit_condition(e, node->as.if_stmt.condition);
        emit(e, " ");
        emit_block(e, node->as.if_stmt.then_block);
        if (node->as.if_stmt.else_block) {
            emit_indent(e);
            if (node->as.if_stmt.else_block->type == NODE_IF) {
                emit(e, "else ");
                emit(e, "if ");
                emit_condition(e, node->as.if_stmt.else_block->as.if_stmt.condition);
                emit(e, " ");
                emit_block(e, node->as.if_stmt.else_block->as.if_stmt.then_block);
                if (node->as.if_stmt.else_block->as.if_stmt.else_block) {
                    emit_indent(e);
                    emit(e, "else ");
                    emit_block(e, node->as.if_stmt.else_block->as.if_stmt.else_block);
                }
            } else {
                emit(e, "else ");
                emit_block(e, node->as.if_stmt.else_block);
            }
        }
        break;

    case NODE_WHILE:
        emit_indent(e);
        emit(e, "while ");
        emit_condition(e, node->as.while_stmt.condition);
        emit(e, " ");
        emit_block(e, node->as.while_stmt.body);
        break;

    case NODE_FOR_EACH: {
        emit_indent(e);
        NshNode *iter = node->as.for_each.iterable;

        if (iter->type == NODE_RANGE) {
            /* lakol i fi start..end — emit proper C for loop */
            emit(e, "for (int64_t %s = ", node->as.for_each.var_name);
            emit_expr(e, iter->as.range.start);
            emit(e, "; %s < ", node->as.for_each.var_name);
            emit_expr(e, iter->as.range.end);
            emit(e, "; %s++) ", node->as.for_each.var_name);
        } else {
            /* Fallback: iterate assuming array-like */
            emit(e, "for (int64_t %s = 0; %s < 10; %s++) ",
                 node->as.for_each.var_name,
                 node->as.for_each.var_name,
                 node->as.for_each.var_name);
        }
        emit_block(e, node->as.for_each.body);
        break;
    }

    case NODE_LOOP:
        emit_indent(e);
        emit(e, "for (;;) ");
        emit_block(e, node->as.loop.body);
        break;

    case NODE_BLOCK:
        emit_indent(e);
        emit_block(e, node);
        break;

    case NODE_EXPR_STMT:
        emit_indent(e);
        emit_expr(e, node->as.expr_stmt.expr);
        emit(e, ";\n");
        break;

    case NODE_BREAK:
        emit_line(e, "break;");
        break;

    case NODE_CONTINUE:
        emit_line(e, "continue;");
        break;

    case NODE_DEFER: {
        /* Capture the deferred code as a string to replay at cleanup */
        /* For now: just emit a comment — defers are collected and emitted at func end */
        if (e->defer_count < MAX_DEFERS) {
            /* Write the deferred statement to a temp buffer */
            char *buf = NULL;
            size_t buf_len = 0;
            FILE *tmp = open_memstream(&buf, &buf_len);
            if (tmp) {
                Emitter tmp_e = {.out = tmp, .indent = e->indent, .defer_count = 0};
                emit_stmt(&tmp_e, node->as.defer.body);
                fclose(tmp);
                e->defer_labels[e->defer_count++] = buf;
            }
        }
        emit_indent(e);
        emit(e, "/* ba3dain (deferred) */\n");
        break;
    }

    case NODE_MATCH: {
        /* hasab expr { hale Pattern(x) => body, ... } */

        /* Detect natije match: first non-default arm is tamam or ghalat */
        int is_natije_match = 0;
        for (int i = 0; i < node->as.match.arms.count; i++) {
            NshNode *arm = node->as.match.arms.items[i];
            const char *p = arm->as.match_arm.pattern_name;
            if (strcmp(p, "tamam") == 0 || strcmp(p, "ghalat") == 0) {
                is_natije_match = 1; break;
            }
        }

        if (is_natije_match) {
            /* natije match: emit if/else on _is_ok */
            emit_indent(e);
            emit(e, "{\n");
            e->indent++;
            /* We don't know the exact result struct type here, use auto keyword
             * — actually emit with typeof since we lack sema. Use a generic approach. */
            emit_indent(e);
            emit(e, "typeof(");
            emit_expr(e, node->as.match.subject);
            emit(e, ") _nsh_match = ");
            emit_expr(e, node->as.match.subject);
            emit(e, ";\n");
            for (int i = 0; i < node->as.match.arms.count; i++) {
                NshNode *arm = node->as.match.arms.items[i];
                const char *pat = arm->as.match_arm.pattern_name;
                emit_indent(e);
                if (strcmp(pat, "tamam") == 0) {
                    emit(e, "%sif (_nsh_match._is_ok) {\n", i > 0 ? "else " : "");
                    e->indent++;
                    if (arm->as.match_arm.bindings.count > 0) {
                        emit_indent(e);
                        emit(e, "typeof(_nsh_match._v._ok) %s = _nsh_match._v._ok;\n",
                             arm->as.match_arm.bindings.items[0]->as.ident.name);
                    }
                } else if (strcmp(pat, "ghalat") == 0) {
                    emit(e, "%sif (!_nsh_match._is_ok) {\n", i > 0 ? "else " : "");
                    e->indent++;
                    if (arm->as.match_arm.bindings.count > 0) {
                        emit_indent(e);
                        emit(e, "const char* %s = _nsh_match._v._err;\n",
                             arm->as.match_arm.bindings.items[0]->as.ident.name);
                    }
                } else {
                    emit(e, "else {\n");
                    e->indent++;
                }
                if (arm->as.match_arm.body->type == NODE_BLOCK) {
                    for (int j = 0; j < arm->as.match_arm.body->as.block.stmts.count; j++)
                        emit_stmt(e, arm->as.match_arm.body->as.block.stmts.items[j]);
                } else {
                    emit_stmt(e, arm->as.match_arm.body);
                }
                e->indent--;
                emit_indent(e);
                emit(e, "}\n");
            }
            e->indent--;
            emit_indent(e);
            emit(e, "}\n");
            break;
        }

        /* Find enum name from the first non-default arm's pattern */
        const char *enum_name = NULL;
        for (int i = 0; i < node->as.match.arms.count; i++) {
            NshNode *arm = node->as.match.arms.items[i];
            if (strcmp(arm->as.match_arm.pattern_name, "_") != 0) {
                VariantInfo *vi = find_variant(e, arm->as.match_arm.pattern_name);
                if (vi) { enum_name = vi->enum_name; break; }
            }
        }

        emit_indent(e);
        if (enum_name) {
            /* Enum match — use tagged union switch */
            emit(e, "{\n");
            e->indent++;
            emit_indent(e);
            emit(e, "struct %s _nsh_match = ", enum_name);
            emit_expr(e, node->as.match.subject);
            emit(e, ";\n");
            emit_indent(e);
            emit(e, "switch (_nsh_match._tag) {\n");
            for (int i = 0; i < node->as.match.arms.count; i++) {
                NshNode *arm = node->as.match.arms.items[i];
                const char *pat = arm->as.match_arm.pattern_name;
                if (strcmp(pat, "_") == 0) {
                    emit_indent(e);
                    emit(e, "default: {\n");
                } else {
                    emit_indent(e);
                    emit(e, "case %s_%s: {\n", enum_name, pat);
                }
                e->indent++;
                /* Extract bindings from payload */
                VariantInfo *vi = (strcmp(pat, "_") != 0) ? find_variant(e, pat) : NULL;
                if (vi && vi->payload_type && arm->as.match_arm.bindings.count > 0) {
                    emit_indent(e);
                    emit(e, "%s %s = _nsh_match._v.%s._0;\n",
                         map_type(vi->payload_type),
                         arm->as.match_arm.bindings.items[0]->as.ident.name,
                         vi->variant_name);
                }
                /* Emit body */
                if (arm->as.match_arm.body->type == NODE_BLOCK) {
                    for (int j = 0; j < arm->as.match_arm.body->as.block.stmts.count; j++)
                        emit_stmt(e, arm->as.match_arm.body->as.block.stmts.items[j]);
                } else {
                    emit_stmt(e, arm->as.match_arm.body);
                }
                emit_indent(e);
                emit(e, "break;\n");
                e->indent--;
                emit_indent(e);
                emit(e, "}\n");
            }
            emit_indent(e);
            emit(e, "}\n");
            e->indent--;
            emit_indent(e);
            emit(e, "}\n");
        } else {
            /* Non-enum match — plain switch on integer value */
            emit(e, "switch (");
            emit_expr(e, node->as.match.subject);
            emit(e, ") {\n");
            for (int i = 0; i < node->as.match.arms.count; i++) {
                NshNode *arm = node->as.match.arms.items[i];
                const char *pat = arm->as.match_arm.pattern_name;
                if (strcmp(pat, "_") == 0) {
                    emit_indent(e);
                    emit(e, "default: {\n");
                } else {
                    emit_indent(e);
                    emit(e, "case %s: {\n", pat);
                }
                e->indent++;
                if (arm->as.match_arm.body->type == NODE_BLOCK) {
                    for (int j = 0; j < arm->as.match_arm.body->as.block.stmts.count; j++)
                        emit_stmt(e, arm->as.match_arm.body->as.block.stmts.items[j]);
                } else {
                    emit_stmt(e, arm->as.match_arm.body);
                }
                emit_indent(e);
                emit(e, "break;\n");
                e->indent--;
                emit_indent(e);
                emit(e, "}\n");
            }
            emit_indent(e);
            emit(e, "}\n");
        }
        break;
    }

    default:
        emit_line(e, "/* unhandled statement */");
        break;
    }
}

/* ── Top-Level Codegen ───────────────────────────────────────── */

static void emit_struct_decl(Emitter *e, NshNode *node) {
    emit(e, "struct %s {\n", node->as.struct_decl.name);
    e->indent++;
    for (int i = 0; i < node->as.struct_decl.fields.count; i++) {
        NshParam *f = &node->as.struct_decl.fields.items[i];
        emit_indent(e);
        emit(e, "%s %s;\n", map_type(f->type_name), f->name);
    }
    e->indent--;
    emit(e, "};\n\n");
}

static void emit_enum_decl(Emitter *e, NshNode *node) {
    /* ta3dad Name { Variant1, Variant2(type), ... }
     * → tagged union:
     *   struct Name { int _tag; union { ... } _v; };
     *   enum Name_Tag { Name_Variant1 = 0, ... };
     */
    char *name = node->as.enum_decl.name;

    /* Register all variants in the variant registry */
    for (int i = 0; i < node->as.enum_decl.variants.count; i++) {
        NshParam *v = &node->as.enum_decl.variants.items[i];
        register_variant(e, v->name, name, v->type_name, i);
    }

    /* Tag enum */
    emit(e, "enum %s_Tag {\n", name);
    e->indent++;
    for (int i = 0; i < node->as.enum_decl.variants.count; i++) {
        NshParam *v = &node->as.enum_decl.variants.items[i];
        emit_indent(e);
        emit(e, "%s_%s = %d", name, v->name, i);
        if (i < node->as.enum_decl.variants.count - 1) emit(e, ",");
        emit(e, "\n");
    }
    e->indent--;
    emit(e, "};\n\n");

    /* Tagged union struct */
    emit(e, "struct %s {\n", name);
    e->indent++;
    emit_indent(e);
    emit(e, "int _tag;\n");
    emit_indent(e);
    emit(e, "union {\n");
    e->indent++;
    for (int i = 0; i < node->as.enum_decl.variants.count; i++) {
        NshParam *v = &node->as.enum_decl.variants.items[i];
        if (v->type_name) {
            emit_indent(e);
            emit(e, "struct { %s _0; } %s;\n", map_type(v->type_name), v->name);
        }
    }
    e->indent--;
    emit_indent(e);
    emit(e, "} _v;\n");
    e->indent--;
    emit(e, "};\n\n");
}

static void emit_impl_block(Emitter *e, NshNode *node) {
    /* tabbe2 TypeName { dalle method(had, ...) -> T { ... } }
     * → TypeName_method(TypeName *had, ...) { ... }
     */
    char *type_name = node->as.impl_block.type_name;

    for (int i = 0; i < node->as.impl_block.methods.count; i++) {
        NshNode *method = node->as.impl_block.methods.items[i];
        if (method->type != NODE_FUNC_DECL) continue;

        const char *ret = map_type(method->as.func_decl.return_type);
        emit(e, "%s %s_%s(struct %s *had",
             ret, type_name, method->as.func_decl.name, type_name);

        /* Skip 'had' parameter if it's the first one */
        int start = 0;
        if (method->as.func_decl.params.count > 0 &&
            strcmp(method->as.func_decl.params.items[0].name, "had") == 0) {
            start = 1;
        }

        for (int j = start; j < method->as.func_decl.params.count; j++) {
            NshParam *p = &method->as.func_decl.params.items[j];
            emit(e, ", %s %s", map_type(p->type_name), p->name);
        }
        emit(e, ") ");
        emit_block(e, method->as.func_decl.body);
        emit(e, "\n");
    }
}

static void emit_func_decl(Emitter *e, NshNode *node) {
    const char *ret = map_type(node->as.func_decl.return_type);

    /* Reset defer stack for each function */
    int saved_defer_count = e->defer_count;
    e->defer_count = 0;

    /* Track natije<T> return type for tamam/ghalat/wala? codegen */
    char *saved_result_type = e->current_result_type;
    const char *inner = extract_natije_inner(node->as.func_decl.return_type);
    e->current_result_type = inner ? strdup(inner) : NULL;

    if (node->as.func_decl.is_yalla) {
        /* yalla() becomes main() */
        emit(e, "int main(void) {\n");
        e->indent++;
        for (int i = 0; i < node->as.func_decl.body->as.block.stmts.count; i++) {
            emit_stmt(e, node->as.func_decl.body->as.block.stmts.items[i]);
        }
        emit_defer_cleanup(e);
        e->indent--;
        emit_indent(e);
        emit(e, "}\n\n");
    } else {
        emit(e, "%s %s(", ret, node->as.func_decl.name);
        for (int i = 0; i < node->as.func_decl.params.count; i++) {
            if (i > 0) emit(e, ", ");
            NshParam *p = &node->as.func_decl.params.items[i];
            emit(e, "%s %s", map_type(p->type_name), p->name);
        }
        if (node->as.func_decl.params.count == 0) {
            emit(e, "void");
        }
        emit(e, ") ");
        emit_block(e, node->as.func_decl.body);
        emit(e, "\n");
    }

    /* Free any defer buffers */
    for (int i = 0; i < e->defer_count; i++) {
        free(e->defer_labels[i]);
        e->defer_labels[i] = NULL;
    }
    e->defer_count = saved_defer_count;

    /* Restore result type context */
    free(e->current_result_type);
    e->current_result_type = saved_result_type;
}

/* ── Public API ──────────────────────────────────────────────── */

int codegen_c_emit(NshNode *program, FILE *out) {
    if (!program || program->type != NODE_PROGRAM) {
        diag_error("codegen: expected program node");
        return -1;
    }

    Emitter e = {.out = out, .indent = 0, .defer_count = 0, .in_function = 0,
                  .variant_count = 0, .current_result_type = NULL, .result_type_count = 0};

    /* C preamble */
    emit(&e, "/* Generated by mansaf — NashmiC Compiler */\n");
    emit(&e, "#include <stdio.h>\n");
    emit(&e, "#include <stdint.h>\n");
    emit(&e, "#include <stddef.h>\n");
    emit(&e, "#include <string.h>\n");
    emit(&e, "#include \"nsh_runtime.h\"\n");
    emit(&e, "\n");
    /* Helper macros for string interpolation type dispatch */
    emit(&e, "static inline void _nsh_print_i64(int64_t v) { printf(\"%%lld\", (long long)v); }\n");
    emit(&e, "static inline void _nsh_print_i32(int32_t v) { printf(\"%%d\", v); }\n");
    emit(&e, "static inline void _nsh_print_f64(double v) { printf(\"%%g\", v); }\n");
    emit(&e, "static inline void _nsh_print_str(const char *v) { printf(\"%%s\", v); }\n");
    emit(&e, "\n");

    /* Pre-scan: collect all natije<T> types used in function signatures */
    for (int i = 0; i < program->as.program.decls.count; i++) {
        NshNode *decl = program->as.program.decls.items[i];
        if (decl->type == NODE_FUNC_DECL && decl->as.func_decl.return_type) {
            const char *inner = extract_natije_inner(decl->as.func_decl.return_type);
            if (inner) register_result_type(&e, inner);
        }
    }

    /* Emit result struct definitions */
    for (int i = 0; i < e.result_type_count; i++) {
        emit_result_struct(&e, e.result_types[i]);
    }

    /* Emit struct declarations first (before functions that use them) */
    for (int i = 0; i < program->as.program.decls.count; i++) {
        NshNode *decl = program->as.program.decls.items[i];
        if (decl->type == NODE_STRUCT_DECL) {
            emit_struct_decl(&e, decl);
        }
        if (decl->type == NODE_ENUM_DECL) {
            emit_enum_decl(&e, decl);
        }
    }

    /* Forward declarations for all functions */
    for (int i = 0; i < program->as.program.decls.count; i++) {
        NshNode *decl = program->as.program.decls.items[i];
        if (decl->type == NODE_FUNC_DECL && !decl->as.func_decl.is_yalla) {
            const char *ret = map_type(decl->as.func_decl.return_type);
            emit(&e, "%s %s(", ret, decl->as.func_decl.name);
            for (int j = 0; j < decl->as.func_decl.params.count; j++) {
                if (j > 0) emit(&e, ", ");
                NshParam *p = &decl->as.func_decl.params.items[j];
                emit(&e, "%s %s", map_type(p->type_name), p->name);
            }
            if (decl->as.func_decl.params.count == 0) {
                emit(&e, "void");
            }
            emit(&e, ");\n");
        }
    }

    /* Forward declarations for impl block methods */
    for (int i = 0; i < program->as.program.decls.count; i++) {
        NshNode *decl = program->as.program.decls.items[i];
        if (decl->type == NODE_IMPL_BLOCK) {
            char *type_name = decl->as.impl_block.type_name;
            for (int j = 0; j < decl->as.impl_block.methods.count; j++) {
                NshNode *method = decl->as.impl_block.methods.items[j];
                if (method->type != NODE_FUNC_DECL) continue;
                const char *ret = map_type(method->as.func_decl.return_type);
                emit(&e, "%s %s_%s(struct %s *had",
                     ret, type_name, method->as.func_decl.name, type_name);
                int start = 0;
                if (method->as.func_decl.params.count > 0 &&
                    strcmp(method->as.func_decl.params.items[0].name, "had") == 0) {
                    start = 1;
                }
                for (int k = start; k < method->as.func_decl.params.count; k++) {
                    NshParam *p = &method->as.func_decl.params.items[k];
                    emit(&e, ", %s %s", map_type(p->type_name), p->name);
                }
                emit(&e, ");\n");
            }
        }
    }
    emit(&e, "\n");

    /* Emit all functions and impl blocks */
    for (int i = 0; i < program->as.program.decls.count; i++) {
        NshNode *decl = program->as.program.decls.items[i];
        if (decl->type == NODE_FUNC_DECL) {
            emit_func_decl(&e, decl);
        } else if (decl->type == NODE_IMPL_BLOCK) {
            emit_impl_block(&e, decl);
        }
    }

    return 0;
}
