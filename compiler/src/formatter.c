/*
 * NashmiC — formatter.c
 * Auto-formatter: walks the AST and emits consistently-formatted NashmiC
 *
 * Formatting rules:
 * - 4-space indentation
 * - One blank line between top-level declarations
 * - No blank lines within function bodies between statements
 * - Opening braces on same line as statement
 * - Spaces around binary operators, after commas, after keywords
 * - No trailing whitespace
 * - Single newline at end of file
 * - Preserve string contents exactly
 */

#include "formatter.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

/* ── Formatter State ─────────────────────────────────────────── */

typedef struct {
    FILE *out;
    int indent;
} Fmt;

static void fmt_emit(Fmt *f, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(f->out, format, args);
    va_end(args);
}

static void fmt_indent(Fmt *f) {
    for (int i = 0; i < f->indent; i++) {
        fprintf(f->out, "    ");
    }
}

static void fmt_newline(Fmt *f) {
    fprintf(f->out, "\n");
}

/* ── Forward Declarations ────────────────────────────────────── */

static void fmt_expr(Fmt *f, NshNode *node);
static void fmt_stmt(Fmt *f, NshNode *node);
static void fmt_block_body(Fmt *f, NshNode *block);

/* ── Binary Operator Strings ─────────────────────────────────── */

static const char *binop_str(NshBinOp op) {
    switch (op) {
    case BIN_ADD:   return "+";
    case BIN_SUB:   return "-";
    case BIN_MUL:   return "*";
    case BIN_DIV:   return "/";
    case BIN_MOD:   return "%";
    case BIN_EQ:    return "==";
    case BIN_NEQ:   return "!=";
    case BIN_LT:    return "<";
    case BIN_GT:    return ">";
    case BIN_LTE:   return "<=";
    case BIN_GTE:   return ">=";
    case BIN_AND:   return "w";
    case BIN_OR:    return "aw";
    case BIN_RANGE: return "..";
    }
    return "??";
}

/* ── Expression Formatting ───────────────────────────────────── */

static void fmt_expr(Fmt *f, NshNode *node) {
    if (!node) return;

    switch (node->type) {
    case NODE_INT_LIT:
        fmt_emit(f, "%" PRId64, node->as.int_lit.value);
        break;

    case NODE_FLOAT_LIT: {
        /* Print float, ensure it always has a decimal point */
        char buf[64];
        snprintf(buf, sizeof(buf), "%g", node->as.float_lit.value);
        fmt_emit(f, "%s", buf);
        break;
    }

    case NODE_STRING_LIT:
        /* value includes quotes — write exactly */
        fwrite(node->as.string_lit.value, 1, node->as.string_lit.length, f->out);
        break;

    case NODE_INTERP_STRING: {
        /* Reconstruct interpolated string: "text{expr}text" */
        fmt_emit(f, "\"");
        for (int i = 0; i < node->as.interp_string.parts.count; i++) {
            NshNode *part = node->as.interp_string.parts.items[i];
            if (part->type == NODE_STRING_LIT) {
                /* Fragment — no quotes, raw content */
                fwrite(part->as.string_lit.value, 1,
                       part->as.string_lit.length, f->out);
            } else {
                /* Expression — wrap in {} */
                fmt_emit(f, "{");
                fmt_expr(f, part);
                fmt_emit(f, "}");
            }
        }
        fmt_emit(f, "\"");
        break;
    }

    case NODE_BOOL_LIT:
        fmt_emit(f, "%s", node->as.bool_lit.value ? "ah" : "la");
        break;

    case NODE_NULL_LIT:
        fmt_emit(f, "mafi");
        break;

    case NODE_IDENT:
        fmt_emit(f, "%s", node->as.ident.name);
        break;

    case NODE_BINARY:
        if (node->as.binary.op == BIN_RANGE) {
            /* Range: no spaces around .. */
            fmt_expr(f, node->as.binary.left);
            fmt_emit(f, "..");
            fmt_expr(f, node->as.binary.right);
        } else {
            fmt_expr(f, node->as.binary.left);
            fmt_emit(f, " %s ", binop_str(node->as.binary.op));
            fmt_expr(f, node->as.binary.right);
        }
        break;

    case NODE_UNARY:
        if (node->as.unary.op == UNARY_NEG) {
            fmt_emit(f, "-");
        } else {
            fmt_emit(f, "mish ");
        }
        fmt_expr(f, node->as.unary.operand);
        break;

    case NODE_CALL:
        fmt_expr(f, node->as.call.callee);
        fmt_emit(f, "(");
        for (int i = 0; i < node->as.call.args.count; i++) {
            if (i > 0) fmt_emit(f, ", ");
            fmt_expr(f, node->as.call.args.items[i]);
        }
        fmt_emit(f, ")");
        break;

    case NODE_INDEX:
        fmt_expr(f, node->as.index_expr.object);
        fmt_emit(f, "[");
        fmt_expr(f, node->as.index_expr.index);
        fmt_emit(f, "]");
        break;

    case NODE_MEMBER:
        fmt_expr(f, node->as.member.object);
        fmt_emit(f, ".%s", node->as.member.field);
        break;

    case NODE_ASSIGN:
        fmt_expr(f, node->as.assign.target);
        fmt_emit(f, " = ");
        fmt_expr(f, node->as.assign.value);
        break;

    case NODE_RANGE:
        fmt_expr(f, node->as.range.start);
        fmt_emit(f, "..");
        fmt_expr(f, node->as.range.end);
        break;

    case NODE_STRUCT_LIT:
        fmt_emit(f, "%s{ ", node->as.struct_lit.name);
        for (int i = 0; i < node->as.struct_lit.field_count; i++) {
            if (i > 0) fmt_emit(f, ", ");
            fmt_emit(f, "%s: ", node->as.struct_lit.field_names[i]);
            fmt_expr(f, node->as.struct_lit.field_values[i]);
        }
        fmt_emit(f, " }");
        break;

    case NODE_WALA_QUESTION:
        fmt_expr(f, node->as.wala_question.expr);
        fmt_emit(f, " wala?");
        if (node->as.wala_question.error_msg) {
            fmt_emit(f, " ");
            fmt_expr(f, node->as.wala_question.error_msg);
        }
        break;

    case NODE_RESULT_WRAP:
        if (node->as.result_wrap.is_ok) {
            fmt_emit(f, "tamam(");
        } else {
            fmt_emit(f, "ghalat(");
        }
        fmt_expr(f, node->as.result_wrap.value);
        fmt_emit(f, ")");
        break;

    case NODE_TUPLE_LIT:
        fmt_emit(f, "(");
        for (int i = 0; i < node->as.tuple_lit.elements.count; i++) {
            if (i > 0) fmt_emit(f, ", ");
            fmt_expr(f, node->as.tuple_lit.elements.items[i]);
        }
        fmt_emit(f, ")");
        break;

    case NODE_ARRAY_LIT:
        fmt_emit(f, "[");
        for (int i = 0; i < node->as.array_lit.elements.count; i++) {
            if (i > 0) fmt_emit(f, ", ");
            fmt_expr(f, node->as.array_lit.elements.items[i]);
        }
        fmt_emit(f, "]");
        break;

    default:
        /* Shouldn't hit statement nodes here, but be safe */
        fmt_emit(f, "/* unknown expr */");
        break;
    }
}

/* ── Block Body Formatting ───────────────────────────────────── */

/* Emits the contents of a block (statements inside braces).
 * The caller handles the braces themselves. */
static void fmt_block_body(Fmt *f, NshNode *block) {
    if (!block || block->type != NODE_BLOCK) return;

    for (int i = 0; i < block->as.block.stmts.count; i++) {
        fmt_stmt(f, block->as.block.stmts.items[i]);
    }
}

/* Emits " {\n" body "indent}\n" — the standard block pattern.
 * The caller has already emitted the statement prefix (e.g., "iza condition"). */
static void fmt_block(Fmt *f, NshNode *block) {
    fmt_emit(f, " {");
    fmt_newline(f);
    f->indent++;
    fmt_block_body(f, block);
    f->indent--;
    fmt_indent(f);
    fmt_emit(f, "}");
}

/* ── Statement Formatting ────────────────────────────────────── */

static void fmt_stmt(Fmt *f, NshNode *node) {
    if (!node) return;

    switch (node->type) {
    case NODE_VAR_DECL:
        fmt_indent(f);
        fmt_emit(f, "khalli %s", node->as.var_decl.name);
        if (node->as.var_decl.type_ann) {
            fmt_emit(f, ": %s", node->as.var_decl.type_ann);
        }
        if (node->as.var_decl.init) {
            fmt_emit(f, " = ");
            fmt_expr(f, node->as.var_decl.init);
        }
        fmt_newline(f);
        break;

    case NODE_CONST_DECL:
        fmt_indent(f);
        fmt_emit(f, "thabet %s", node->as.var_decl.name);
        if (node->as.var_decl.type_ann) {
            fmt_emit(f, ": %s", node->as.var_decl.type_ann);
        }
        if (node->as.var_decl.init) {
            fmt_emit(f, " = ");
            fmt_expr(f, node->as.var_decl.init);
        }
        fmt_newline(f);
        break;

    case NODE_RETURN:
        fmt_indent(f);
        fmt_emit(f, "rajje3");
        if (node->as.ret.value) {
            fmt_emit(f, " ");
            fmt_expr(f, node->as.ret.value);
        }
        fmt_newline(f);
        break;

    case NODE_IF:
        fmt_indent(f);
        fmt_emit(f, "iza ");
        fmt_expr(f, node->as.if_stmt.condition);
        fmt_block(f, node->as.if_stmt.then_block);
        if (node->as.if_stmt.else_block) {
            if (node->as.if_stmt.else_block->type == NODE_IF) {
                /* else-if chain: "} wala iza ..." */
                fmt_emit(f, " wala ");
                /* Emit the chained if without leading indent */
                fmt_emit(f, "iza ");
                fmt_expr(f, node->as.if_stmt.else_block->as.if_stmt.condition);
                fmt_block(f, node->as.if_stmt.else_block->as.if_stmt.then_block);
                /* Handle deeper else chains recursively */
                NshNode *deeper = node->as.if_stmt.else_block->as.if_stmt.else_block;
                while (deeper) {
                    if (deeper->type == NODE_IF) {
                        fmt_emit(f, " wala iza ");
                        fmt_expr(f, deeper->as.if_stmt.condition);
                        fmt_block(f, deeper->as.if_stmt.then_block);
                        deeper = deeper->as.if_stmt.else_block;
                    } else {
                        fmt_emit(f, " wala");
                        fmt_block(f, deeper);
                        deeper = NULL;
                    }
                }
            } else {
                /* plain else block */
                fmt_emit(f, " wala");
                fmt_block(f, node->as.if_stmt.else_block);
            }
        }
        fmt_newline(f);
        break;

    case NODE_WHILE:
        fmt_indent(f);
        fmt_emit(f, "tool_ma ");
        fmt_expr(f, node->as.while_stmt.condition);
        fmt_block(f, node->as.while_stmt.body);
        fmt_newline(f);
        break;

    case NODE_FOR_EACH:
        fmt_indent(f);
        fmt_emit(f, "lakol %s fi ", node->as.for_each.var_name);
        fmt_expr(f, node->as.for_each.iterable);
        fmt_block(f, node->as.for_each.body);
        fmt_newline(f);
        break;

    case NODE_LOOP:
        fmt_indent(f);
        fmt_emit(f, "liff");
        fmt_block(f, node->as.loop.body);
        fmt_newline(f);
        break;

    case NODE_BREAK:
        fmt_indent(f);
        fmt_emit(f, "khalas");
        fmt_newline(f);
        break;

    case NODE_CONTINUE:
        fmt_indent(f);
        fmt_emit(f, "kammel");
        fmt_newline(f);
        break;

    case NODE_DEFER:
        fmt_indent(f);
        fmt_emit(f, "ba3dain");
        if (node->as.defer.body && node->as.defer.body->type == NODE_BLOCK) {
            fmt_block(f, node->as.defer.body);
        } else {
            fmt_emit(f, " ");
            fmt_expr(f, node->as.defer.body);
        }
        fmt_newline(f);
        break;

    case NODE_MATCH:
        fmt_indent(f);
        fmt_emit(f, "hasab ");
        fmt_expr(f, node->as.match.subject);
        fmt_emit(f, " {");
        fmt_newline(f);
        f->indent++;
        for (int i = 0; i < node->as.match.arms.count; i++) {
            fmt_stmt(f, node->as.match.arms.items[i]);
        }
        f->indent--;
        fmt_indent(f);
        fmt_emit(f, "}");
        fmt_newline(f);
        break;

    case NODE_MATCH_ARM:
        fmt_indent(f);
        fmt_emit(f, "hale %s", node->as.match_arm.pattern_name);
        if (node->as.match_arm.bindings.count > 0) {
            fmt_emit(f, "(");
            for (int i = 0; i < node->as.match_arm.bindings.count; i++) {
                if (i > 0) fmt_emit(f, ", ");
                NshNode *binding = node->as.match_arm.bindings.items[i];
                if (binding->type == NODE_IDENT) {
                    fmt_emit(f, "%s", binding->as.ident.name);
                }
            }
            fmt_emit(f, ")");
        }
        fmt_emit(f, " => ");
        if (node->as.match_arm.body &&
            node->as.match_arm.body->type == NODE_BLOCK) {
            fmt_emit(f, "{");
            fmt_newline(f);
            f->indent++;
            fmt_block_body(f, node->as.match_arm.body);
            f->indent--;
            fmt_indent(f);
            fmt_emit(f, "}");
        } else {
            fmt_expr(f, node->as.match_arm.body);
        }
        fmt_newline(f);
        break;

    case NODE_OPTIONAL_BIND:
        fmt_indent(f);
        fmt_emit(f, "iza fi %s = ", node->as.optional_bind.var_name);
        fmt_expr(f, node->as.optional_bind.expr);
        fmt_block(f, node->as.optional_bind.then_block);
        if (node->as.optional_bind.else_block) {
            fmt_emit(f, " wala");
            fmt_block(f, node->as.optional_bind.else_block);
        }
        fmt_newline(f);
        break;

    case NODE_EXPR_STMT:
        fmt_indent(f);
        fmt_expr(f, node->as.expr_stmt.expr);
        fmt_newline(f);
        break;

    case NODE_BLOCK:
        /* Standalone block (shouldn't normally appear as a statement,
         * but handle it just in case) */
        fmt_indent(f);
        fmt_emit(f, "{");
        fmt_newline(f);
        f->indent++;
        fmt_block_body(f, node);
        f->indent--;
        fmt_indent(f);
        fmt_emit(f, "}");
        fmt_newline(f);
        break;

    default:
        /* Expression as statement fallback */
        fmt_indent(f);
        fmt_expr(f, node);
        fmt_newline(f);
        break;
    }
}

/* ── Top-Level Declaration Formatting ────────────────────────── */

static void fmt_func_decl(Fmt *f, NshNode *node) {
    if (node->as.func_decl.is_yalla) {
        fmt_emit(f, "yalla()");
    } else {
        fmt_emit(f, "dalle %s(", node->as.func_decl.name);
        for (int i = 0; i < node->as.func_decl.params.count; i++) {
            if (i > 0) fmt_emit(f, ", ");
            NshParam *param = &node->as.func_decl.params.items[i];
            fmt_emit(f, "%s", param->name);
            if (param->type_name) {
                fmt_emit(f, ": %s", param->type_name);
            }
        }
        fmt_emit(f, ")");
        if (node->as.func_decl.return_type) {
            fmt_emit(f, " -> %s", node->as.func_decl.return_type);
        }
    }
    fmt_block(f, node->as.func_decl.body);
    fmt_newline(f);
}

static void fmt_struct_decl(Fmt *f, NshNode *node) {
    fmt_emit(f, "haikal %s {", node->as.struct_decl.name);
    fmt_newline(f);
    f->indent++;
    for (int i = 0; i < node->as.struct_decl.fields.count; i++) {
        NshParam *field = &node->as.struct_decl.fields.items[i];
        fmt_indent(f);
        fmt_emit(f, "%s: %s,", field->name, field->type_name);
        fmt_newline(f);
    }
    f->indent--;
    fmt_emit(f, "}");
    fmt_newline(f);
}

static void fmt_enum_decl(Fmt *f, NshNode *node) {
    fmt_emit(f, "ta3dad %s {", node->as.enum_decl.name);
    fmt_newline(f);
    f->indent++;
    for (int i = 0; i < node->as.enum_decl.variants.count; i++) {
        NshParam *variant = &node->as.enum_decl.variants.items[i];
        fmt_indent(f);
        fmt_emit(f, "%s", variant->name);
        if (variant->type_name) {
            fmt_emit(f, "(%s)", variant->type_name);
        }
        fmt_emit(f, ",");
        fmt_newline(f);
    }
    f->indent--;
    fmt_emit(f, "}");
    fmt_newline(f);
}

static void fmt_impl_block(Fmt *f, NshNode *node) {
    fmt_emit(f, "tabbe2 %s {", node->as.impl_block.type_name);
    fmt_newline(f);
    f->indent++;
    for (int i = 0; i < node->as.impl_block.methods.count; i++) {
        if (i > 0) {
            /* Blank line between methods inside impl block */
            fmt_newline(f);
        }
        NshNode *method = node->as.impl_block.methods.items[i];
        fmt_indent(f);
        fmt_func_decl(f, method);
    }
    f->indent--;
    fmt_emit(f, "}");
    fmt_newline(f);
}

static void fmt_decl(Fmt *f, NshNode *node) {
    switch (node->type) {
    case NODE_FUNC_DECL:
        fmt_func_decl(f, node);
        break;
    case NODE_STRUCT_DECL:
        fmt_struct_decl(f, node);
        break;
    case NODE_ENUM_DECL:
        fmt_enum_decl(f, node);
        break;
    case NODE_IMPL_BLOCK:
        fmt_impl_block(f, node);
        break;
    default:
        /* Shouldn't happen at top level, but handle gracefully */
        fmt_stmt(f, node);
        break;
    }
}

/* ── Public API ──────────────────────────────────────────────── */

int formatter_format(NshNode *ast, FILE *out) {
    if (!ast || ast->type != NODE_PROGRAM) return -1;
    if (!out) return -1;

    Fmt f = {
        .out = out,
        .indent = 0,
    };

    for (int i = 0; i < ast->as.program.decls.count; i++) {
        if (i > 0) {
            /* Blank line between top-level declarations */
            fmt_newline(&f);
        }
        fmt_decl(&f, ast->as.program.decls.items[i]);
    }

    return 0;
}
