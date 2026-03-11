/**
 * NashmiC Tree-sitter Grammar
 *
 * Franco-Arab programming language — keywords use Arabizi (Franco-Arab).
 * Based on analysis of compiler/src/parser.c, lexer.h, keywords.c, ast.h.
 */

const PREC = {
  ASSIGN: 1,
  OR: 2,
  AND: 3,
  EQUALITY: 4,
  COMPARISON: 5,
  RANGE: 6,
  ADDITIVE: 7,
  MULTIPLICATIVE: 8,
  UNARY: 9,
  CALL: 10,
  MEMBER: 11,
};

module.exports = grammar({
  name: "nashmic",

  extras: ($) => [/\s/, $.comment],

  word: ($) => $.identifier,

  conflicts: ($) => [],

  rules: {
    source_file: ($) => repeat($._definition),

    _definition: ($) =>
      choice(
        $.function_def,
        $.struct_def,
        $.enum_def,
        $.impl_block,
        $._statement
      ),

    // ── Declarations ──────────────────────────────────

    function_def: ($) =>
      seq(
        "dalle",
        field("name", $.identifier),
        "(",
        optional($.parameter_list),
        ")",
        optional(seq(":", field("return_type", $._type))),
        field("body", $.block)
      ),

    parameter_list: ($) =>
      seq($.parameter, repeat(seq(",", $.parameter)), optional(",")),

    parameter: ($) =>
      seq(
        field("name", $.identifier),
        ":",
        field("type", $._type)
      ),

    struct_def: ($) =>
      seq(
        "haikal",
        field("name", $.identifier),
        "{",
        optional($.field_list),
        "}"
      ),

    field_list: ($) =>
      seq($.field_def, repeat(seq(",", $.field_def)), optional(",")),

    field_def: ($) =>
      seq(
        field("name", $.identifier),
        ":",
        field("type", $._type)
      ),

    enum_def: ($) =>
      seq(
        "ta3dad",
        field("name", $.identifier),
        "{",
        optional($.variant_list),
        "}"
      ),

    variant_list: ($) =>
      seq($.variant, repeat(seq(",", $.variant)), optional(",")),

    variant: ($) =>
      seq(
        field("name", $.identifier),
        optional(seq("(", field("payload_type", $._type), ")"))
      ),

    impl_block: ($) =>
      seq(
        "tabbe2",
        field("type_name", $.identifier),
        "{",
        repeat($.function_def),
        "}"
      ),

    // ── Statements ────────────────────────────────────

    _statement: ($) =>
      choice(
        $.variable_decl,
        $.const_decl,
        $.if_statement,
        $.while_loop,
        $.for_loop,
        $.loop_statement,
        $.return_statement,
        $.defer_statement,
        $.match_statement,
        $.break_statement,
        $.continue_statement,
        $.expression_statement,
        $.block
      ),

    variable_decl: ($) =>
      seq(
        "khalli",
        field("name", $.identifier),
        optional(seq(":", field("type", $._type))),
        "=",
        field("value", $._expression)
      ),

    const_decl: ($) =>
      seq(
        "thabet",
        field("name", $.identifier),
        optional(seq(":", field("type", $._type))),
        "=",
        field("value", $._expression)
      ),

    if_statement: ($) =>
      seq(
        "iza",
        choice(
          seq(
            "(",
            field("condition", $._expression),
            ")",
            field("consequence", $.block)
          ),
          seq(
            "fi",
            field("binding", $.identifier),
            "=",
            field("value", $._expression),
            field("consequence", $.block)
          )
        ),
        repeat($.else_if_clause),
        optional($.else_clause)
      ),

    else_if_clause: ($) =>
      seq(
        choice("wala_iza", seq("wala", "iza")),
        "(",
        field("condition", $._expression),
        ")",
        field("consequence", $.block)
      ),

    else_clause: ($) => seq("wala", field("body", $.block)),

    while_loop: ($) =>
      seq(
        "tool_ma",
        "(",
        field("condition", $._expression),
        ")",
        field("body", $.block)
      ),

    for_loop: ($) =>
      seq(
        "lakol",
        field("variable", $.identifier),
        "fi",
        field("iterable", $._expression),
        field("body", $.block)
      ),

    loop_statement: ($) => seq("liff", field("body", $.block)),

    return_statement: ($) =>
      prec.right(seq("rajje3", optional(field("value", $._expression)))),

    defer_statement: ($) => seq("ba3dain", field("body", $.block)),

    match_statement: ($) =>
      seq(
        "hasab",
        field("subject", $.identifier),
        "{",
        repeat($.match_arm),
        "}"
      ),

    match_arm: ($) =>
      prec(1, seq(
        "hale",
        field("pattern", $.pattern),
        ":",
        field("body", choice($.block, $._expression))
      )),

    pattern: ($) =>
      choice(
        seq(
          $.identifier,
          optional(seq("(", $.identifier, ")"))
        ),
        "_"
      ),

    break_statement: ($) => "khalas",

    continue_statement: ($) => "kammel",

    expression_statement: ($) => $._expression,

    block: ($) => seq("{", repeat($._statement), "}"),

    // ── Types ─────────────────────────────────────────

    _type: ($) =>
      choice(
        $.builtin_type,
        $.generic_type,
        $.type_identifier
      ),

    builtin_type: ($) =>
      choice(
        "adad",
        "adad64",
        "fasle",
        "fasle64",
        "harf",
        "nass",
        "mante2",
        "fadi",
        "bait"
      ),

    generic_type: ($) =>
      seq(
        field("name", choice("natije", "yimkin", "saff")),
        "<",
        field("type_argument", $._type),
        ">"
      ),

    type_identifier: ($) => $.identifier,

    // ── Expressions ───────────────────────────────────

    _expression: ($) =>
      choice(
        $.assignment,
        $._non_assign_expr
      ),

    _non_assign_expr: ($) =>
      choice(
        $.binary_expr,
        $.unary_expr,
        $.call_expr,
        $.field_access,
        $.index_expr,
        $.range_expr,
        $.result_wrap,
        $._primary_expr
      ),

    assignment: ($) =>
      prec.right(
        PREC.ASSIGN,
        seq(
          field("target", $._non_assign_expr),
          "=",
          field("value", $._expression)
        )
      ),

    binary_expr: ($) =>
      choice(
        ...[
          [PREC.OR, "aw"],
          [PREC.AND, "w"],
          [PREC.EQUALITY, choice("==", "!=")],
          [PREC.COMPARISON, choice("<", ">", "<=", ">=")],
          [PREC.ADDITIVE, choice("+", "-")],
          [PREC.MULTIPLICATIVE, choice("*", "/", "%")],
        ].map(([prec_val, op]) =>
          prec.left(
            prec_val,
            seq(
              field("left", $._non_assign_expr),
              field("operator", op),
              field("right", $._non_assign_expr)
            )
          )
        )
      ),

    unary_expr: ($) =>
      prec(
        PREC.UNARY,
        seq(
          field("operator", choice("-", "mish", "!")),
          field("operand", $._non_assign_expr)
        )
      ),

    call_expr: ($) =>
      prec(
        PREC.CALL,
        seq(
          field("function", $._non_assign_expr),
          "(",
          optional($.argument_list),
          ")"
        )
      ),

    argument_list: ($) =>
      seq($._expression, repeat(seq(",", $._expression)), optional(",")),

    field_access: ($) =>
      prec.left(
        PREC.MEMBER,
        seq(
          field("object", $._non_assign_expr),
          ".",
          field("field", $.identifier)
        )
      ),

    index_expr: ($) =>
      prec(
        PREC.CALL,
        seq(
          field("object", $._non_assign_expr),
          "[",
          field("index", $._expression),
          "]"
        )
      ),

    range_expr: ($) =>
      prec.left(
        PREC.RANGE,
        seq(
          field("start", $._non_assign_expr),
          "..",
          field("end", $._non_assign_expr)
        )
      ),

    result_wrap: ($) =>
      seq(
        field("constructor", choice("tamam", "ghalat")),
        "(",
        field("value", $._expression),
        ")"
      ),

    _primary_expr: ($) =>
      choice(
        $.number_literal,
        $.string_literal,
        $.bool_literal,
        $.null_literal,
        $.identifier,
        $.struct_literal,
        $.array_literal,
        $.paren_expr
      ),

    paren_expr: ($) => seq("(", $._expression, ")"),

    // ── Literals ──────────────────────────────────────

    number_literal: ($) =>
      token(choice(/[0-9]+\.[0-9]+/, /[0-9]+/)),

    string_literal: ($) =>
      seq(
        '"',
        repeat(
          choice(
            /[^"\\{]+/,
            $.escape_sequence,
            $.interpolation
          )
        ),
        '"'
      ),

    interpolation: ($) => seq("{", $._expression, "}"),

    escape_sequence: ($) => token.immediate(seq("\\", /[\\nrt"0{]/)),

    bool_literal: ($) => choice("ah", "la"),

    null_literal: ($) => choice("mafi", "wala_ishi"),

    struct_literal: ($) =>
      prec(
        1,
        seq(
          field("name", $.identifier),
          "{",
          optional($.field_init_list),
          "}"
        )
      ),

    field_init_list: ($) =>
      seq($.field_init, repeat(seq(",", $.field_init)), optional(",")),

    field_init: ($) =>
      seq(
        field("name", $.identifier),
        ":",
        field("value", $._expression)
      ),

    array_literal: ($) =>
      seq("[", optional($.argument_list), "]"),

    // ── Identifiers & Comments ────────────────────────

    identifier: ($) => /[a-zA-Z_][a-zA-Z0-9_]*/,

    comment: ($) => token(seq("//", /.*/)),
  },
});
