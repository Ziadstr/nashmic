# NashmiC

**nashmi** (نشمي) = brave, noble, valiant. The ultimate Jordanian compliment.

NashmiC is a programming language from Jordan. Easy like Python, powerful like Go, Jordanian to the bone.

## What is NashmiC?

NashmiC is a compiled programming language with Franco-Arab keywords. You code the way Jordanians text - not textbook Arabic, real Jordanian dialect. Every keyword is a word you'd actually say walking through downtown Amman.

```
yalla() {
    khalli name: nass = "Ziad"
    itba3("marhaba ya {name}!\n")
}
```

That's a complete program. `yalla` means "let's go" - it's your entry point. `khalli` means "let there be" - it declares a variable. `itba3` means "spit it out" - it prints. `nass` means "text" - it's a string.

Arabic script works perfectly in strings and comments. Keywords are Franco-Arab only: `iza` not `اذا`, because that's how Jordanians actually type.

## Philosophy

NashmiC is built on five principles:

1. **Easy like Python** - no semicolons, string interpolation, clean syntax. If it feels like boilerplate, it doesn't belong.

2. **Powerful like Go** - strong types, explicit error handling, no hidden magic. You always know what your code is doing.

3. **No null** - `yimkin<T>` for optionals, `natije<T>` for errors. The billion-dollar mistake doesn't exist here.

4. **Compiler with personality** - errors come with Jordanian proverbs. Build successes get celebrated. The compiler is your teammate, not a cold tool.

5. **Cultural identity** - easter eggs, build messages, and proverbs are baked into the language. `drobi()` gives you a random Jordanian proverb. `mansaf()` prints the national dish recipe in ASCII art. This isn't decoration; it's identity.

## Feature Status

Everything listed below is fully implemented, compiles, and runs.

| Feature | Status | Keyword(s) |
|---------|--------|-------------|
| Variables & constants | Working | `khalli`, `thabet` |
| Functions & recursion | Working | `dalle`, `rajje3`, `yalla` |
| Control flow (if/else, while, for-each, loop) | Working | `iza`, `wala`, `wala_iza`, `tool_ma`, `lakol`, `liff` |
| Break & continue | Working | `khalas`, `kammel` |
| Range iteration | Working | `lakol i fi 0..20` |
| String interpolation | Working | `"hello {name}"` |
| String concatenation | Working | `"hello " + name` |
| Structs | Working | `haikal` |
| Enums (tagged unions) | Working | `ta3dad` |
| Pattern matching | Working | `hasab`, `hale`, `3adi` |
| Result type + error propagation | Working | `natije<T>`, `tamam`, `ghalat`, `wala?` |
| Optional type | Working | `yimkin<T>`, `fi`, `mafi`, `iza fi` |
| Methods & impl blocks | Working | `tabbe2`, `had` |
| Defer (cleanup on scope exit) | Working | `ba3dain` |
| Arrays | Working | `saff<T>`, `.zeed()`, `.toul()` |
| Cultural easter eggs | Working | `drobi()`, `mansaf()`, `sahteen()`, `nashmi()` |
| Compiler messages with proverbs | Working | Jordanian proverbs on errors |

## How It Works

NashmiC compiles through C in six stages:

```
.nsh source -> Lexer -> Parser -> AST -> Sema -> C Codegen -> cc -> native binary
```

1. **Lexer** — Tokenizes your `.nsh` source into tokens. Handles UTF-8, Franco-Arab keywords, string interpolation, and operators.
2. **Parser** — Recursive descent + Pratt parser builds an Abstract Syntax Tree (AST). Handles operator precedence, generics (`natije<T>`, `saff<T>`), and block scoping.
3. **Semantic Analysis (Sema)** — Three-pass analysis: declares symbols, resolves types, then type-checks. Catches undeclared variables, type mismatches, invalid field access, wrong argument counts, and unused variables — all with Jordanian proverb error messages.
4. **C Code Generation** — Walks the AST and emits equivalent C11 code. Structs become C structs, enums become tagged unions, methods become mangled functions (`Point_distance`), and `natije<T>` becomes a result struct with `_is_ok` flag.
5. **C Compilation** — Your system's C compiler (`gcc` or `clang`) compiles the generated C with `-std=gnu11 -O2` and links against `nsh_runtime.c`.
6. **Binary** — You get a native binary. No VM, no interpreter, no garbage collector. Just machine code.

The entire compiler is ~5,000 lines of C11. No dependencies beyond libc and libm.

## File Extension

NashmiC source files use the `.nsh` extension.

## Editor Support

NashmiC has a VS Code extension with syntax highlighting, snippets, bracket matching, and code folding. Install it from the marketplace:

```bash
code --install-extension ziadstr.nashmic
```

Or search **"NashmiC"** in the VS Code Extensions panel (`Ctrl+Shift+X`).

## Why Franco-Arab?

Every previous Arabic programming language (Qalb, Alif, ABJAD, Arablan) used Arabic script. They all faced the same problems:

- **RTL/LTR mixing** — Arabic is right-to-left, but code structure (braces, operators, numbers) is left-to-right. Editors struggle with this.
- **No standard keyboard** — Arabic keyboard layouts vary by country. Programming symbols like `{`, `[`, `=` require awkward key combos.
- **Tooling gaps** — Syntax highlighters, formatters, linters, and debuggers don't handle Arabic script well.

Franco-Arab (Arabizi) sidesteps all of these. It's the script 400M+ Arabs already use every day in texts, social media, and chat. It's left-to-right, uses the Latin alphabet, and works with every tool ever built for programming. Numbers replace Arabic letters that don't exist in Latin: `3` = ع (ain), `2` = ء (hamza), `7` = ح (ha), `5` = خ (kha).

NashmiC keywords aren't transliterated Modern Standard Arabic — they're Jordanian dialect. The way a CS student in Amman would actually talk. `yalla` not `ibda2`, `khalas` not `iqaf`, `rajje3` not `irja3`. If a Jordanian wouldn't say it in conversation, it doesn't belong in NashmiC.

## Try It

- **[Web Playground](playground/)** — try NashmiC in your browser, no install needed
- **[VS Code Extension](https://marketplace.visualstudio.com/items?itemName=ziadstr.nashmic)** — syntax highlighting, snippets, bracket matching
- **[Getting Started](getting-started.md)** — install and write your first program
