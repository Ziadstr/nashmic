# NashmiC 🇯🇴

**نشمي** — A systems programming language with Jordanian soul.

NashmiC is a compiled systems language that speaks Jordanian Arabic. Every keyword is a word you'd actually say in Amman — not textbook MSA, real dialect. Write in Arabic script or Franco-Arab transliteration. Both work. Same compiler. Same binary.

```
yalla() {
    itba3("مرحبا يا عالم! 🇯🇴\n")
}
```

```
يلا() {
    اطبع("مرحبا يا عالم! 🇯🇴\n")
}
```

Same program. Same output. Your choice.

## Features

- **Dual-mode keywords** — Arabic script (`اذا`) and Franco (`iza`) are identical tokens
- **`natije` Result type** — `tamam(v)` / `ghalat(e)` for error handling, compiler-enforced
- **`hares` guard blocks** — pointer safety without a full borrow checker
- **`3al_3amyani` unsafe blocks** — opt into manual memory when you need it
- **`ba3dain` defer** — cleanup actions that run on scope exit
- **Pattern matching** — `hasab`/`hale` with destructuring
- **Built-in `nass` string type** — UTF-8, length-prefixed, Arabic-aware
- **Jordanian compiler messages** — errors come with proverbs

## Quick Start

```bash
# Build the compiler
make

# Run hello world
NASHMIC_ROOT=. build/mansaf run examples/marhaba.nsh

# Build a binary
NASHMIC_ROOT=. build/mansaf build examples/fibonacci.nsh
./fibonacci
```

## Examples

### Fibonacci

```
dalle fibonacci(n: adad64) -> adad64 {
    iza n <= 1 { rajje3 n }
    rajje3 fibonacci(n - 1) + fibonacci(n - 2)
}

yalla() {
    khalli i: adad64 = 0
    tool_ma i < 15 {
        itba3("fib(%lld) = %lld\n", i, fibonacci(i))
        i = i + 1
    }
}
```

### Pure Arabic Mode

```
دالة فيبوناتشي(ن: عدد٦٤) -> عدد٦٤ {
    اذا ن <= 1 { رجع ن }
    رجع فيبوناتشي(ن - 1) + فيبوناتشي(ن - 2)
}

يلا() {
    خلي ع: عدد٦٤ = 0
    طول_ما ع < 15 {
        اطبع("fib(%lld) = %lld\n", ع, فيبوناتشي(ع))
        ع = ع + 1
    }
}
```

## Toolchain

| Tool | Command | What |
|------|---------|------|
| **mansaf** (المنسف) | `mansaf build/run/lex` | Compiler |
| **diwan** (ديوان) | `mansaf diwan` | REPL |
| **souq** (سوق) | `souq install/search` | Package manager |

## Status

Phase 0 complete — lexer, parser, C codegen, runtime. See [PLAN.md](PLAN.md) for the full roadmap.

## License

MIT
