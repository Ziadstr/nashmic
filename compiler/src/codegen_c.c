/*
 * NashmiC — codegen_c.c
 * C transpiler backend (Phase 1)
 *
 * Translates NashmiC AST into C source code, then the host C compiler
 * (gcc/cc) compiles it into a native binary.
 */

#include "codegen_c.h"
#include "diagnostics.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

/* ── Code Emitter State ──────────────────────────────────────── */

typedef struct {
    FILE *out;
    int indent;
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

/* ── Type Mapping ────────────────────────────────────────────── */

static const char *map_type(const char *nsh_type) {
    if (!nsh_type) return "void";

    /* Franco types */
    if (strcmp(nsh_type, "adad") == 0) return "int32_t";
    if (strcmp(nsh_type, "adad64") == 0) return "int64_t";
    if (strcmp(nsh_type, "fasle") == 0) return "float";
    if (strcmp(nsh_type, "fasle64") == 0) return "double";
    if (strcmp(nsh_type, "mante2") == 0) return "int";
    if (strcmp(nsh_type, "harf") == 0) return "char";
    if (strcmp(nsh_type, "bait") == 0) return "uint8_t";
    if (strcmp(nsh_type, "nass") == 0) return "const char*";
    if (strcmp(nsh_type, "fadi") == 0) return "void";

    /* Arabic types */
    if (strcmp(nsh_type, "عدد") == 0) return "int32_t";
    if (strcmp(nsh_type, "عدد٦٤") == 0) return "int64_t";
    if (strcmp(nsh_type, "فاصلة") == 0) return "float";
    if (strcmp(nsh_type, "فاصلة٦٤") == 0) return "double";
    if (strcmp(nsh_type, "منطق") == 0) return "int";
    if (strcmp(nsh_type, "حرف") == 0) return "char";
    if (strcmp(nsh_type, "بايت") == 0) return "uint8_t";
    if (strcmp(nsh_type, "نص") == 0) return "const char*";
    if (strcmp(nsh_type, "فاضي") == 0) return "void";

    /* fallback: pass through */
    return nsh_type;
}

/* ── Expression Codegen ──────────────────────────────────────── */

static void emit_expr(Emitter *e, NshNode *node);

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
        if (strcmp(node->as.ident.name, "itba3") == 0 ||
            strcmp(node->as.ident.name, "اطبع") == 0) {
            emit(e, "nsh_itba3");
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

    default:
        emit(e, "/* unknown expr */0");
        break;
    }
}

/* ── Statement Codegen ───────────────────────────────────────── */

static void emit_stmt(Emitter *e, NshNode *node);

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
        if (node->as.ret.value) {
            emit(e, "return ");
            emit_expr(e, node->as.ret.value);
            emit(e, ";\n");
        } else {
            emit(e, "return;\n");
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
                /* inline the if — don't add extra indent */
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

    case NODE_FOR_EACH:
        /* Phase 0: for-each over range only (0..N) */
        /* For now, emit a simple for loop — we'll handle iterators later */
        emit_indent(e);
        emit(e, "/* lakol %s fi ... */\n", node->as.for_each.var_name);
        emit_indent(e);
        /* If iterable is a range (binary ..), emit proper for loop */
        if (node->as.for_each.iterable->type == NODE_BINARY) {
            /* TODO: handle range expressions properly in Phase 1 */
            emit(e, "for (int64_t %s = 0; %s < 10; %s++) ",
                 node->as.for_each.var_name,
                 node->as.for_each.var_name,
                 node->as.for_each.var_name);
        } else {
            emit(e, "for (int64_t %s = 0; %s < 10; %s++) ",
                 node->as.for_each.var_name,
                 node->as.for_each.var_name,
                 node->as.for_each.var_name);
        }
        emit_block(e, node->as.for_each.body);
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

    default:
        emit_line(e, "/* unhandled statement */");
        break;
    }
}

/* ── Top-Level Codegen ───────────────────────────────────────── */

static void emit_func_decl(Emitter *e, NshNode *node) {
    const char *ret = map_type(node->as.func_decl.return_type);

    if (node->as.func_decl.is_yalla) {
        /* yalla() becomes main() */
        emit(e, "int main(void) ");
        emit_block(e, node->as.func_decl.body);
        /* ensure main returns 0 */
        /* (codegen adds it to the block) */
        emit(e, "\n");
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
}

/* ── Public API ──────────────────────────────────────────────── */

int codegen_c_emit(NshNode *program, FILE *out) {
    if (!program || program->type != NODE_PROGRAM) {
        diag_error("codegen: expected program node");
        return -1;
    }

    Emitter e = {.out = out, .indent = 0};

    /* C preamble */
    emit(&e, "/* Generated by mansaf — NashmiC Compiler 🇯🇴 */\n");
    emit(&e, "#include <stdio.h>\n");
    emit(&e, "#include <stdint.h>\n");
    emit(&e, "#include <stddef.h>\n");
    emit(&e, "#include \"nsh_runtime.h\"\n");
    emit(&e, "\n");

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
    emit(&e, "\n");

    /* Emit all functions */
    for (int i = 0; i < program->as.program.decls.count; i++) {
        NshNode *decl = program->as.program.decls.items[i];
        if (decl->type == NODE_FUNC_DECL) {
            emit_func_decl(&e, decl);
        }
    }

    return 0;
}
