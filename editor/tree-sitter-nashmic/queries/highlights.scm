; NashmiC — Tree-sitter Highlight Queries

; Keywords
["dalle" "khalli" "thabet" "iza" "wala" "wala_iza" "lakol" "fi" "tool_ma"
 "rajje3" "haikal" "ta3dad" "tabbe2" "hasab" "hale"
 "ba3dain" "liff"] @keyword

(break_statement) @keyword
(continue_statement) @keyword

; Type keywords
["adad" "adad64" "fasle" "fasle64" "harf" "nass" "mante2" "fadi"
 "bait"] @type.builtin

; Generic type names
(generic_type
  name: _ @type.builtin)

; Boolean + null
(bool_literal) @constant.builtin
(null_literal) @constant.builtin

; Result constructors
["tamam" "ghalat"] @constructor

; Builtin functions
((identifier) @function.builtin
  (#any-of? @function.builtin
    "itba3" "i2ra" "itla3" "toul" "drobi" "mansaf" "sahteen" "nashmi"
    "jadhr" "qowa" "mutlaq" "ashwa2i" "jeta" "jeta_tamam" "dal"
    "ardiye" "sa2fiye" "da2reb" "aqall" "akthar" "log_tabi3i" "log10"
    "qass" "damj" "badel" "yihtawi" "bdaya" "nihaya" "a3la" "asfal"
    "qass_haddi" "harf_3ind" "juz2" "karrer"
    "iftah_malaf" "iftah_malaf_ktaba" "sakker_malaf" "i2ra_kol"
    "uktub_malaf" "dahef_malaf" "mawjood" "imsah_malaf" "hajm_malaf"
    "i2ra_sutoor"))

; Operators
["w" "aw" "mish"] @keyword.operator
["+" "-" "*" "/" "%" "==" "!=" "<" ">" "<=" ">=" "=" ".." "!"] @operator

; Function definitions
(function_def
  name: (identifier) @function)

; Function calls
(call_expr
  function: (identifier) @function.call)

; Struct/enum definitions
(struct_def
  name: (identifier) @type)
(enum_def
  name: (identifier) @type)
(impl_block
  type_name: (identifier) @type)

; Struct literals
(struct_literal
  name: (identifier) @type)

; Field access
(field_access
  field: (identifier) @property)

; Field definitions
(field_def
  name: (identifier) @property)
(field_init
  name: (identifier) @property)

; Parameters
(parameter
  name: (identifier) @variable.parameter)

; Type identifiers
(type_identifier
  (identifier) @type)

; Variants
(variant
  name: (identifier) @constructor)

; Match patterns
(pattern
  (identifier) @constructor)

; Literals
(string_literal) @string
(interpolation
  "{" @punctuation.special
  "}" @punctuation.special)
(escape_sequence) @string.escape
(number_literal) @number
(comment) @comment

; Punctuation
["(" ")" "{" "}" "[" "]"] @punctuation.bracket
["," ":" "."] @punctuation.delimiter

; Variable declarations
(variable_decl
  name: (identifier) @variable)
(const_decl
  name: (identifier) @variable)

; For loop variable
(for_loop
  variable: (identifier) @variable)

; Fallback
(identifier) @variable
