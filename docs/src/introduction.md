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

NashmiC compiles through C:

```
.nsh source -> Lexer -> Parser -> AST -> C Codegen -> .c file -> cc -> native binary
```

Your `.nsh` source gets tokenized, parsed into an AST, transpiled to C11, then compiled to a native binary by your system's C compiler. The generated code links against `nsh_runtime.c` for built-in functions and easter eggs.

## File Extension

NashmiC source files use the `.nsh` extension.

## Editor Support

NashmiC has a VS Code extension with syntax highlighting, snippets, bracket matching, and code folding. Install it from the marketplace:

```bash
code --install-extension ziadstr.nashmic
```

Or search **"NashmiC"** in the VS Code Extensions panel (`Ctrl+Shift+X`).

Ready to start? See [Getting Started](getting-started.md).
