# NashmiC — Language Design & Roadmap

> **نشمي** (Nashmi) = brave, noble, valiant — the ultimate Jordanian compliment.
>
> A programming language from Jordan. Easy like Python, powerful like Go, Jordanian to the bone.
> Franco-Arab keywords — code the way you text. Not textbook Arabic, real Jordanian dialect.

---

## Table of Contents

1. [Philosophy](#1-philosophy)
2. [Language Design](#2-language-design)
3. [Keyword Reference](#3-keyword-reference)
4. [Compiler Architecture](#4-compiler-architecture)
5. [Runtime](#5-runtime)
6. [Status & Roadmap](#6-status--roadmap)
7. [Compiler Output Style](#7-compiler-output-style)
8. [Future Vision](#8-future-vision)

---

## 1. Philosophy

### Identity

- **Franco-Arab only** — no Arabic script keywords. `iza` not `اذا`. Code the way Jordanians text.
- Arabic script works fine in strings and comments — just not as keywords.
- Every keyword is a word you'd actually say in Amman.

### Design Principles

1. **Easy like Python** — no semicolons, string interpolation, clean syntax
2. **Powerful like Go** — strong types, explicit error handling, no hidden magic
3. **No null** — `yimkin<T>` for optionals, `natije<T>` for errors
4. **Compiler with personality** — errors come with Jordanian proverbs
5. **Cultural identity** — easter eggs, build messages, and proverbs baked in

### File Extension

- `.nsh` — NashmiC source files

---

## 2. Language Design

### Error Model — `natije<T>` + `wala?`

NashmiC's unique error handling. No exceptions, no try/catch. Errors are values.

```
dalle read_config(path: nass) -> natije<nass> {
    khalli f = iftah(path) wala? "ما قدرت افتح الملف"
    khalli content = i2ra_kol(f) wala?
    rajje3 tamam(content)
}
```

- `natije<T>` = Result type. Every function that can fail returns one.
- `tamam(value)` = success (like Rust's `Ok`)
- `ghalat(msg)` = error (like Rust's `Err`)
- `wala?` = unwrap or propagate. If error, return it up the chain.
- `wala? "msg"` = unwrap or propagate with context wrapping
- **Mandatory handling** — ignoring a `natije` is a compiler error, not a warning

### No Null — `yimkin<T>`

NashmiC has no null. Optional values use `yimkin<T>`:

```
dalle find_user(id: adad) -> yimkin<User> {
    iza id == 0 { rajje3 mafi }     // "there isn't"
    rajje3 fi(users[id])             // "there is"
}

// Must check before use:
iza fi u = find_user(42) {
    itba3("لقيناه: {u.ism}\n")
} wala {
    itba3("ما لقيناه\n")
}
```

- `fi(value)` = Some — "there is"
- `mafi` = None — "there isn't"
- `iza fi x = expr` = optional binding — must unwrap before use
- Accessing `.field` on `yimkin<T>` without checking = compiler error

### String Interpolation

No `printf`-style format strings. Curly braces inside strings:

```
khalli name: nass = "Ziad"
khalli age: adad64 = 23
itba3("marhaba ya {name}, age {age}, next year {age + 1}\n")
```

- `{expr}` evaluates any expression inline
- Type-aware: automatically uses the right format for strings, ints, floats
- `{{` for literal brace

### Structs — `haikal`

```
haikal Point {
    x: fasle64,
    y: fasle64,
}

khalli p: Point = Point{ x: 3.0, y: 4.0 }
itba3("Point: ({p.x}, {p.y})\n")
```

### Methods — `tabbe2` (impl blocks)

```
tabbe2 Point {
    dalle distance(had, other: Point) -> fasle64 {
        khalli dx = had.x - other.x
        khalli dy = had.y - other.y
        rajje3 sqrt(dx * dx + dy * dy)
    }
}

khalli d = p.distance(q)
```

- `had` = self/this — "this thing"
- Methods become `TypeName_methodname(TypeName *had, ...)` in generated C

### Tagged Unions — `ta3dad` (enum)

```
ta3dad Shape {
    Da2ira(fasle64),
    Mustateel(fasle64, fasle64),
    Noqta,
}
```

### Pattern Matching — `hasab`/`hale`

```
hasab shape {
    hale Da2ira(r) => itba3("Circle: radius {r}\n")
    hale Mustateel(w, h) => itba3("Rect: {w}x{h}\n")
    hale Noqta => itba3("Point\n")
    3adi => itba3("Unknown\n")
}
```

- Exhaustiveness checking — compiler error if not all variants covered and no `3adi` default

### Defer — `ba3dain`

"Later" — procrastination as a feature. Runs on scope exit, LIFO order.

```
khalli f = iftah("data.txt")
ba3dain sakker(f)
// ... use f ...
// sakker(f) runs automatically when scope exits
```

### Range Iteration

```
lakol i fi 0..20 {
    itba3("{i}\n")
}
```

### Control Flow

```
// if/else
iza x > 0 {
    itba3("positive\n")
} wala_iza x == 0 {
    itba3("zero\n")
} wala {
    itba3("negative\n")
}

// while
tool_ma running {
    // ...
}

// infinite loop
liff {
    iza done { khalas }    // break
    kammel               // continue
}
```

---

## 3. Keyword Reference

### Control Flow

| NashmiC | Meaning | English equivalent |
|---------|---------|-------------------|
| `iza` | "if" | if |
| `wala` | "or else" | else |
| `wala_iza` | "or if" | else if |
| `tool_ma` | "as long as" | while |
| `lakol x fi` | "for each x in" | for each |
| `liff` | "go around" | loop (infinite) |
| `khalas` | "enough!" | break |
| `kammel` | "keep going" | continue |
| `rajje3` | "bring back" | return |
| `hasab` | "depending on" | match/switch |
| `hale` | "case" | match arm |
| `3adi` | "whatever" | default |

### Types

| NashmiC | C equivalent | Description |
|---------|-------------|-------------|
| `adad` | `int32_t` | 32-bit integer |
| `adad64` | `int64_t` | 64-bit integer |
| `fasle` | `float` | 32-bit float |
| `fasle64` | `double` | 64-bit float |
| `mante2` | `bool` | boolean (`ah`/`la`) |
| `harf` | `char` | character |
| `nass` | `const char*` | string |
| `fadi` | `void` | void |

### Declarations

| NashmiC | Meaning |
|---------|---------|
| `khalli` | let (variable) |
| `thabet` | const |
| `dalle` | function |
| `haikal` | struct |
| `ta3dad` | enum (tagged union) |
| `tabbe2` | impl (methods on a type) |

### Values

| NashmiC | Meaning |
|---------|---------|
| `ah` | true |
| `la` | false |
| `tamam(x)` | Ok(x) — success result |
| `ghalat(x)` | Err(x) — error result |
| `fi(x)` | Some(x) — optional has value |
| `mafi` | None — optional is empty |

### Error Handling

| NashmiC | Meaning |
|---------|---------|
| `natije<T>` | Result type |
| `wala?` | unwrap or propagate error |
| `yimkin<T>` | Optional type |
| `ba3dain` | defer (runs on scope exit) |
| `faz3a` | panic |

### I/O & Built-ins

| NashmiC | Meaning |
|---------|---------|
| `itba3(...)` | print to stdout |
| `i2ra()` | read line from stdin |
| `itla3(code)` | exit program |
| `drobi()` | random Jordanian proverb |
| `mansaf()` | mansaf recipe (ASCII art) |
| `sahteen()` | "bon appetit" |
| `nashmi()` | about NashmiC |

---

## 4. Compiler Architecture

### Pipeline

```
.nsh source → Lexer → Parser → AST → C Codegen → .c file → cc → native binary
```

### Components

| Component | File(s) | Description |
|-----------|---------|-------------|
| CLI | `compiler/src/main.c` | `mansaf build/run/lex` commands |
| Lexer | `compiler/src/lexer.c/h` | UTF-8 tokenizer, FNV-1a keyword hash table |
| Keywords | `compiler/src/keywords.c/h` | Franco-Arab keyword registration |
| Parser | `compiler/src/parser.c/h` | Recursive descent + Pratt precedence climbing |
| AST | `compiler/src/ast.c/h` | All node types including enums, impl, match, interpolation |
| Codegen | `compiler/src/codegen_c.c/h` | C transpiler — emits C11 code |
| Diagnostics | `compiler/src/diagnostics.c/h` | Errors with Jordanian proverbs |
| UTF-8 | `compiler/src/utf8.c/h` | UTF-8 encoding/decoding |
| Spans | `compiler/src/span.h` | Source locations |

### Lexer Details

- UTF-8 aware — handles Arabic identifiers and string content
- FNV-1a hash table for O(1) keyword lookup
- Franco-Arab keywords only (Arabic script words become regular identifiers)
- Compound token: `wala?` lexed as single `TOK_WALA_QUESTION`
- String interpolation: `{` inside `"..."` triggers expression sub-lexing

### Parser Details

- Recursive descent for statements and declarations
- Pratt precedence climbing for expressions
- Precedence: assignment < or < and < equality < comparison < addition < multiplication < unary < postfix
- Backtracking for struct literals (`Name{` vs block `{`)
- `iza fi x = expr` optional binding pattern

### Codegen Details

- C11 target — uses `_Generic` for type dispatch in string interpolation
- Statement expressions `({ ... })` for inline interpolation
- Struct types prefixed with `struct` for uppercase user-defined names
- `open_memstream` for defer statement capture
- Helper preamble: `_nsh_print_i64`, `_nsh_print_str`, `_nsh_print_f64`, etc.

---

## 5. Runtime

### `runtime/nsh_runtime.c/h`

| Function | NashmiC | Description |
|----------|---------|-------------|
| `nsh_itba3()` | `itba3()` | Print to stdout (format string) |
| `nsh_i2ra()` | `i2ra()` | Read line from stdin |
| `nsh_itla3()` | `itla3()` | Exit with code |
| `nsh_drobi()` | `drobi()` | Random proverb (rate-limited, roasts after 5 calls) |
| `nsh_mansaf()` | `mansaf()` | Mansaf recipe in ASCII art |
| `nsh_sahteen()` | `sahteen()` | "bon appetit" |
| `nsh_nashmi()` | `nashmi()` | NashmiC manifesto |

The runtime is linked at compile time. Generated C code `#include`s `nsh_runtime.h`.

---

## 6. Status & Roadmap

### Implemented (MVP)

- [x] Franco-only keywords (Arabic script → regular identifiers)
- [x] Functions, variables, constants (`dalle`, `khalli`, `thabet`)
- [x] Control flow (`iza`/`wala`/`wala_iza`, `tool_ma`, `lakol..fi`, `liff`, `khalas`, `kammel`)
- [x] Range iteration (`lakol i fi 0..20`) — reads actual range bounds
- [x] String interpolation (`"hello {name}, age {age + 1}"`) — C11 `_Generic` type dispatch
- [x] Structs (`haikal`) — declaration, literals, member access, function params
- [x] Recursive functions
- [x] Operator precedence (Pratt parsing)
- [x] Cultural diagnostics — errors with Jordanian proverbs
- [x] Build personality — success quips, first-build ASCII banner, `--tarab` flag
- [x] Easter egg runtime functions — `drobi()`, `mansaf()`, `sahteen()`, `nashmi()`
- [x] 6 working examples — marhaba, fibonacci, ranges, interpolation, structs, easter_eggs

### Parser Done, Codegen In Progress

- [ ] `natije<T>` + `wala?` — full C struct generation + propagation logic
- [ ] `yimkin<T>` — full C struct generation + `iza fi` binding codegen
- [ ] `ta3dad` enums — tagged union C codegen (scaffolded)
- [ ] `hasab`/`hale` pattern matching — switch on tag + payload extraction
- [ ] `tabbe2` impl blocks — method dispatch codegen (written, untested)
- [ ] `ba3dain` defer — goto-chain cleanup (partially implemented)

### Not Yet Started

- [ ] Multiple return values / tuple destructuring
- [ ] Semicolon-free syntax (auto-insertion)
- [ ] Semantic analysis (type checking, symbol tables)
- [ ] Arrays / slices
- [ ] Module system (`jeeb` imports)
- [ ] Standard library

---

## 7. Compiler Output Style

### Error Messages

Errors include source context and Jordanian proverbs:

```
خطأ[E0001]: expected expression — شو هاد الحكي؟
  --> src/main.nsh:12:18
     |
  12 |     khalli x = ???
     |                ^^^
     |
      = اصبر على الحصرم تاكله عنب — patience turns sour to sweet
```

### Proverb Categories

| Trigger | Proverb | Translation |
|---------|---------|-------------|
| Type mismatch | حبل الكذب قصير | "The rope of lies is short" |
| Invalid argument | عذر الراقصة إنو الأرض مايلة | "The dancer blames the floor" |
| Unreachable code | اللي ما بوصل للعنب بقول عنه حامض | "Sour grapes" |
| Stack overflow | اللي بحفر حفرة لأخوه بقع فيها | "Dig a pit, fall in it" |
| Unused variable | بلجيكي | "Belgian" = unnecessarily complex |
| Unhandled error | شايف حالك | "You think you're safe?" |

### Build Messages

Success quips rotate randomly:

```
✓ تم! زي الفل
✓ تم! نشمي والله
✓ تم! والله ما قصرت
✓ تم! اسرع من طلقة
```

First-ever build shows ASCII art banner. `--tarab` flag attempts to play celebratory audio.

---

## 8. Future Vision

These are long-term goals, not current priorities:

| Feature | Description |
|---------|-------------|
| Semantic analysis | Full type checking, inference, symbol tables |
| QBE backend | Native compilation without going through C |
| LLVM backend | Optimized native binaries, cross-compilation |
| Module system | `jeeb riyadiyat` imports, `mansaf.toml` manifests |
| Standard library | Collections, math, file I/O, networking, time |
| Package manager | `souq` — install, publish, search packages |
| Concurrency | Green threads, channels (`qanah`), `shaghghel`/`istanna` |
| Generics | Parametric polymorphism |
| Editor support | Tree-sitter grammar, VS Code extension, LSP |
| Self-hosting | Compiler written in NashmiC itself |

---

Built by Ziad Al-Sharif
