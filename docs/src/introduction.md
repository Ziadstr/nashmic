# NashmiC

**nashmi** (نشمي) = brave, noble, valiant -- the ultimate Jordanian compliment.

NashmiC is a programming language from Jordan. Easy like Python, powerful like Go, Jordanian to the bone.

## What is NashmiC?

NashmiC is a compiled programming language with Franco-Arab keywords. You code the way Jordanians text -- not textbook Arabic, real Jordanian dialect. Every keyword is a word you'd actually say walking through downtown Amman.

```
yalla() {
    khalli name: nass = "Ziad"
    itba3("marhaba ya {name}!\n")
}
```

That's a complete program. `yalla` means "let's go" -- it's your entry point. `khalli` means "let there be" -- it declares a variable. `itba3` means "print" -- literally "spit it out." `nass` means "text" -- it's a string.

Arabic script works perfectly in strings and comments. Keywords are Franco-Arab only -- `iza` not `اذا` -- because that's how Jordanians actually type.

## Philosophy

NashmiC is built on five principles:

1. **Easy like Python** -- no semicolons, string interpolation, clean syntax. If it feels like boilerplate, it doesn't belong.

2. **Powerful like Go** -- strong types, explicit error handling, no hidden magic. You always know what your code is doing.

3. **No null** -- `yimkin<T>` for optionals, `natije<T>` for errors. The billion-dollar mistake doesn't exist here.

4. **Compiler with personality** -- errors come with Jordanian proverbs. Build successes get celebrated. The compiler is not a cold tool; it's your teammate.

5. **Cultural identity** -- easter eggs, build messages, and proverbs are baked into the language. `drobi()` gives you a random Jordanian proverb. `mansaf()` prints the national dish recipe in ASCII art. This isn't decoration; it's identity.

## How It Works

NashmiC compiles through C:

```
.nsh source -> Lexer -> Parser -> AST -> C Codegen -> .c file -> cc -> native binary
```

Your `.nsh` source gets tokenized, parsed into an AST, transpiled to C11, then compiled to a native binary by your system's C compiler. The generated code links against `nsh_runtime.c` for built-in functions and easter eggs.

## File Extension

NashmiC source files use the `.nsh` extension.

## Status

NashmiC is in active development. The core language (functions, variables, control flow, structs, string interpolation, ranges) is fully working. Error handling, optionals, enums, pattern matching, methods, and defer are parsed and awaiting codegen completion. See [Getting Started](getting-started.md) to write your first program.
