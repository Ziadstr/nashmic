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

/* ── Code Emitter State ──────────────────────────────────────── */

#define MAX_DEFERS 64

typedef struct {
    FILE *out;
    int indent;
    /* defer stack per function */
    int defer_count;
    char *defer_labels[MAX_DEFERS];
    int in_function; /* track if we're inside a function for defer */
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

    case NODE_IDENT:
        /* Map built-in function names */
        if (strcmp(node->as.ident.name, "itba3") == 0) {
            emit(e, "nsh_itba3");
        } else if (strcmp(node->as.ident.name, "i2ra") == 0) {
            emit(e, "nsh_i2ra");
        } else if (strcmp(node->as.ident.name, "itla3") == 0) {
            emit(e, "nsh_itla3");
        } else if (strcmp(node->as.ident.name, "drobi") == 0) {
            emit(e, "nsh_drobi");
        } else if (strcmp(node->as.ident.name, "mansaf") == 0) {
            emit(e, "nsh_mansaf");
        } else if (strcmp(node->as.ident.name, "sahteen") == 0) {
            emit(e, "nsh_sahteen");
        } else if (strcmp(node->as.ident.name, "nashmi") == 0) {
            emit(e, "nsh_nashmi");
        } else if (strcmp(node->as.ident.name, "tamam") == 0) {
            emit(e, "nsh_tamam");
        } else if (strcmp(node->as.ident.name, "ghalat") == 0) {
            emit(e, "nsh_ghalat");
        } else if (strcmp(node->as.ident.name, "fi") == 0) {
            emit(e, "nsh_fi");
        } else if (strcmp(node->as.ident.name, "mafi") == 0) {
            emit(e, "nsh_mafi");
        } else {
            emit(e, "%s", node->as.ident.name);
        }
        break;

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

    case NODE_CALL:
        /* Special case: itba3("...{expr}...") — flatten interpolation into print */
        if (node->as.call.callee->type == NODE_IDENT &&
            (strcmp(node->as.call.callee->as.ident.name, "itba3") == 0) &&
            node->as.call.args.count == 1 &&
            node->as.call.args.items[0]->type == NODE_INTERP_STRING) {
            /* Emit the interp string directly as a print call */
            emit_expr(e, node->as.call.args.items[0]);
            break;
        }
        emit_expr(e, node->as.call.callee);
        emit(e, "(");
        for (int i = 0; i < node->as.call.args.count; i++) {
            if (i > 0) emit(e, ", ");
            emit_expr(e, node->as.call.args.items[i]);
        }
        emit(e, ")");
        break;

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
        emit(e, "if (");
        emit_expr(e, node->as.if_stmt.condition);
        emit(e, ") ");
        emit_block(e, node->as.if_stmt.then_block);
        if (node->as.if_stmt.else_block) {
            emit_indent(e);
            if (node->as.if_stmt.else_block->type == NODE_IF) {
                emit(e, "else ");
                emit(e, "if (");
                emit_expr(e, node->as.if_stmt.else_block->as.if_stmt.condition);
                emit(e, ") ");
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
        emit(e, "while (");
        emit_expr(e, node->as.while_stmt.condition);
        emit(e, ") ");
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

    case NODE_MATCH:
        /* hasab expr { hale pattern => body, ... } */
        emit_indent(e);
        emit(e, "switch (");
        emit_expr(e, node->as.match.subject);
        emit(e, ") {\n");
        /* TODO: full pattern matching codegen */
        emit_indent(e);
        emit(e, "}\n");
        break;

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
}

/* ── Public API ──────────────────────────────────────────────── */

int codegen_c_emit(NshNode *program, FILE *out) {
    if (!program || program->type != NODE_PROGRAM) {
        diag_error("codegen: expected program node");
        return -1;
    }

    Emitter e = {.out = out, .indent = 0, .defer_count = 0, .in_function = 0};

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
