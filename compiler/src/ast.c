/*
 * NashmiC — ast.c
 * AST node allocation and management
 */

#include "ast.h"
#include <stdlib.h>
#include <string.h>

/* ── Node List ───────────────────────────────────────────────── */

void nodelist_init(NshNodeList *list) {
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

void nodelist_push(NshNodeList *list, NshNode *node) {
    if (list->count >= list->capacity) {
        list->capacity = list->capacity == 0 ? 8 : list->capacity * 2;
        list->items = realloc(list->items, sizeof(NshNode *) * list->capacity);
    }
    list->items[list->count++] = node;
}

void nodelist_free(NshNodeList *list) {
    for (int i = 0; i < list->count; i++) {
        node_free(list->items[i]);
    }
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

/* ── Param List ──────────────────────────────────────────────── */

void paramlist_init(NshParamList *list) {
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

void paramlist_push(NshParamList *list, NshParam param) {
    if (list->count >= list->capacity) {
        list->capacity = list->capacity == 0 ? 4 : list->capacity * 2;
        list->items = realloc(list->items, sizeof(NshParam) * list->capacity);
    }
    list->items[list->count++] = param;
}

void paramlist_free(NshParamList *list) {
    for (int i = 0; i < list->count; i++) {
        free(list->items[i].name);
        free(list->items[i].type_name);
    }
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
}

/* ── Node Allocation ─────────────────────────────────────────── */

NshNode *node_new(NshNodeType type, NshSpan span) {
    NshNode *node = calloc(1, sizeof(NshNode));
    node->type = type;
    node->span = span;
    return node;
}

void node_free(NshNode *node) {
    if (!node) return;

    switch (node->type) {
    case NODE_PROGRAM:
        nodelist_free(&node->as.program.decls);
        break;

    case NODE_FUNC_DECL:
        free(node->as.func_decl.name);
        paramlist_free(&node->as.func_decl.params);
        free(node->as.func_decl.return_type);
        node_free(node->as.func_decl.body);
        break;

    case NODE_VAR_DECL:
    case NODE_CONST_DECL:
        free(node->as.var_decl.name);
        free(node->as.var_decl.type_ann);
        node_free(node->as.var_decl.init);
        break;

    case NODE_RETURN:
        node_free(node->as.ret.value);
        break;

    case NODE_IF:
        node_free(node->as.if_stmt.condition);
        node_free(node->as.if_stmt.then_block);
        node_free(node->as.if_stmt.else_block);
        break;

    case NODE_WHILE:
        node_free(node->as.while_stmt.condition);
        node_free(node->as.while_stmt.body);
        break;

    case NODE_FOR_EACH:
        free(node->as.for_each.var_name);
        node_free(node->as.for_each.iterable);
        node_free(node->as.for_each.body);
        break;

    case NODE_BLOCK:
        nodelist_free(&node->as.block.stmts);
        break;

    case NODE_EXPR_STMT:
        node_free(node->as.expr_stmt.expr);
        break;

    case NODE_STRING_LIT:
        free(node->as.string_lit.value);
        break;

    case NODE_IDENT:
        free(node->as.ident.name);
        break;

    case NODE_BINARY:
        node_free(node->as.binary.left);
        node_free(node->as.binary.right);
        break;

    case NODE_UNARY:
        node_free(node->as.unary.operand);
        break;

    case NODE_CALL:
        node_free(node->as.call.callee);
        nodelist_free(&node->as.call.args);
        break;

    case NODE_INDEX:
        node_free(node->as.index_expr.object);
        node_free(node->as.index_expr.index);
        break;

    case NODE_MEMBER:
        node_free(node->as.member.object);
        free(node->as.member.field);
        break;

    case NODE_ASSIGN:
        node_free(node->as.assign.target);
        node_free(node->as.assign.value);
        break;

    case NODE_RANGE:
        node_free(node->as.range.start);
        node_free(node->as.range.end);
        break;

    case NODE_DEFER:
        node_free(node->as.defer.body);
        break;

    case NODE_LOOP:
        node_free(node->as.loop.body);
        break;

    case NODE_MATCH:
        node_free(node->as.match.subject);
        nodelist_free(&node->as.match.arms);
        break;

    case NODE_MATCH_ARM:
        free(node->as.match_arm.pattern_name);
        nodelist_free(&node->as.match_arm.bindings);
        node_free(node->as.match_arm.body);
        break;

    case NODE_STRUCT_DECL:
        free(node->as.struct_decl.name);
        paramlist_free(&node->as.struct_decl.fields);
        break;

    case NODE_ENUM_DECL:
        free(node->as.enum_decl.name);
        paramlist_free(&node->as.enum_decl.variants);
        break;

    case NODE_IMPL_BLOCK:
        free(node->as.impl_block.type_name);
        nodelist_free(&node->as.impl_block.methods);
        break;

    case NODE_STRUCT_LIT:
        free(node->as.struct_lit.name);
        for (int i = 0; i < node->as.struct_lit.field_count; i++) {
            free(node->as.struct_lit.field_names[i]);
            node_free(node->as.struct_lit.field_values[i]);
        }
        free(node->as.struct_lit.field_names);
        free(node->as.struct_lit.field_values);
        break;

    case NODE_INTERP_STRING:
        nodelist_free(&node->as.interp_string.parts);
        break;

    case NODE_WALA_QUESTION:
        node_free(node->as.wala_question.expr);
        node_free(node->as.wala_question.error_msg);
        break;

    case NODE_RESULT_WRAP:
        node_free(node->as.result_wrap.value);
        break;

    case NODE_OPTIONAL_BIND:
        free(node->as.optional_bind.var_name);
        node_free(node->as.optional_bind.expr);
        node_free(node->as.optional_bind.then_block);
        node_free(node->as.optional_bind.else_block);
        break;

    case NODE_TUPLE_LIT:
        nodelist_free(&node->as.tuple_lit.elements);
        break;

    case NODE_ARRAY_LIT:
        nodelist_free(&node->as.array_lit.elements);
        free(node->as.array_lit.elem_type);
        break;

    case NODE_INT_LIT:
    case NODE_FLOAT_LIT:
    case NODE_BOOL_LIT:
    case NODE_NULL_LIT:
    case NODE_BREAK:
    case NODE_CONTINUE:
        break;
    }

    free(node);
}

/* ── Convenience Constructors ────────────────────────────────── */

NshNode *node_int_lit(int64_t value, NshSpan span) {
    NshNode *n = node_new(NODE_INT_LIT, span);
    n->as.int_lit.value = value;
    return n;
}

NshNode *node_float_lit(double value, NshSpan span) {
    NshNode *n = node_new(NODE_FLOAT_LIT, span);
    n->as.float_lit.value = value;
    return n;
}

NshNode *node_string_lit(const char *text, int len, NshSpan span) {
    NshNode *n = node_new(NODE_STRING_LIT, span);
    n->as.string_lit.value = malloc(len + 1);
    memcpy(n->as.string_lit.value, text, len);
    n->as.string_lit.value[len] = '\0';
    n->as.string_lit.length = len;
    return n;
}

NshNode *node_bool_lit(int value, NshSpan span) {
    NshNode *n = node_new(NODE_BOOL_LIT, span);
    n->as.bool_lit.value = value;
    return n;
}

NshNode *node_null_lit(NshSpan span) {
    return node_new(NODE_NULL_LIT, span);
}

NshNode *node_ident(const char *name, int len, NshSpan span) {
    NshNode *n = node_new(NODE_IDENT, span);
    n->as.ident.name = malloc(len + 1);
    memcpy(n->as.ident.name, name, len);
    n->as.ident.name[len] = '\0';
    return n;
}
