; NashmiC — Scope and Reference Tracking

; Scopes
(function_def) @scope
(block) @scope
(for_loop) @scope
(if_statement) @scope

; Definitions
(variable_decl
  name: (identifier) @definition.var)
(const_decl
  name: (identifier) @definition.var)
(parameter
  name: (identifier) @definition.parameter)
(function_def
  name: (identifier) @definition.function)
(struct_def
  name: (identifier) @definition.type)
(enum_def
  name: (identifier) @definition.type)
(for_loop
  variable: (identifier) @definition.var)

; References
(identifier) @reference
