<p align="center">
  <img src="assets/logo-128.png" alt="NashmiC Logo" width="128">
</p>

<h1 align="center">NashmiC 🇯🇴</h1>

<p align="center">
  <strong>A Franco-Arab Programming Language</strong><br>
  Easy like Python. Powerful like Go. Jordanian to the bone.
</p>

<p align="center">
  <a href="https://github.com/Ziadstr/nashmic/actions/workflows/build.yml"><img src="https://github.com/Ziadstr/nashmic/actions/workflows/build.yml/badge.svg" alt="Build"></a>
  <a href="https://github.com/Ziadstr/nashmic/releases"><img src="https://img.shields.io/github/v/release/Ziadstr/nashmic?include_prereleases&label=version" alt="Version"></a>
  <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License"></a>
  <a href="https://ziadstr.github.io/nashmic/"><img src="https://img.shields.io/badge/docs-live-brightgreen.svg" alt="Docs"></a>
  <a href="https://ziadstr.github.io/nashmic/playground/"><img src="https://img.shields.io/badge/playground-try%20it-orange.svg" alt="Playground"></a>
  <a href="https://marketplace.visualstudio.com/items?itemName=ziadstr.nashmic"><img src="https://img.shields.io/visual-studio-marketplace/v/ziadstr.nashmic?label=VS%20Code&color=007ACC" alt="VS Code"></a>
</p>

---

NashmiC speaks **Franco-Arab** (Arabizi) — the way 400M+ Arabs actually text. No Arabic script, no RTL headaches. Every keyword is a word you'd say in Amman. Not textbook Arabic, real Jordanian dialect.

> Every Arabic-script programming language (Qalb, Alif, ABJAD) died from RTL/tooling issues. NashmiC sidesteps all of that by using the script Arabs actually type in.

```
yalla() {
    khalli name: nass = "Ziad"
    itba3("marhaba ya {name}!\n")
}
```

## Features

- **Franco-Arab keywords:** `iza`, `wala`, `khalli`, `dalle`, `rajje3`, `yalla`
- **String interpolation:** `"marhaba ya {name}"` with type-aware formatting
- **String concatenation:** `"hello " + name + "!"` with `+` operator
- **`natije<T>` + `wala?`:** mandatory error handling with inline propagation
- **`yimkin<T>`:** no null, ever. `fi`/`mafi` for optionals
- **`ba3dain` defer:** cleanup that runs on scope exit
- **Pattern matching:** `hasab`/`hale` with destructuring
- **Structs + methods:** `haikal` with `tabbe2` impl blocks
- **Tagged unions:** `ta3dad` with typed variants
- **Arrays:** `saff<T>` with `.zeed()` push, `.toul()` length, iteration
- **Semantic analysis:** type checking, field/method validation, unused variable warnings
- **Compiler personality:** errors come with Jordanian proverbs
- **Easter eggs:** `drobi()` for proverbs, `mansaf()` for the recipe
- **Standard library:** math (`riyadiyat`), strings (`nusoos`), file I/O (`malafat`)
- **VS Code extension:** syntax highlighting, snippets, bracket matching -- [install it](https://marketplace.visualstudio.com/items?itemName=ziadstr.nashmic)
- **Tree-sitter grammar:** syntax highlighting for Neovim, Helix, Zed
- **[Web playground](https://ziadstr.github.io/nashmic/playground/):** try NashmiC in your browser

## Installation

### Homebrew (macOS / Linux)

```bash
brew tap Ziadstr/nashmic
brew install nashmic
```

### AUR (Arch Linux)

```bash
yay -S nashmic-git
```

### Debian / Ubuntu (.deb)

Download the `.deb` from [Releases](https://github.com/Ziadstr/nashmic/releases):

```bash
sudo dpkg -i nashmic_*.deb
```

### One-liner

```bash
curl -fsSL https://raw.githubusercontent.com/Ziadstr/nashmic/main/install.sh | bash
```

### Build from source

```bash
git clone https://github.com/Ziadstr/nashmic.git
cd nashmic
make
sudo make install
```

### Requirements

- A C11 compiler (`gcc` or `clang`)
- `make`
- Linux or macOS

## Quick Start

```bash
# Run hello world
mansaf run examples/marhaba.nsh

# Run fibonacci (uses ranges + interpolation)
mansaf run examples/fibonacci.nsh

# Build a binary
mansaf build examples/marhaba.nsh
./marhaba

# Build with celebration
mansaf build --tarab examples/marhaba.nsh

# Format a file (prints to stdout)
mansaf fmt examples/fibonacci.nsh

# Format in-place
mansaf fmt --write examples/fibonacci.nsh

# Start the interactive REPL
mansaf repl

# Run the test suite
mansaf test

# Dump tokens (debug)
mansaf lex examples/marhaba.nsh

# Run all examples (from source)
make run-all
```

## Examples

### Hello World

```
yalla() {
    itba3("مرحبا يا عالم!\n")
    itba3("Welcome to NashmiC, ya nashmi!\n")
}
```

### Fibonacci with Ranges + Interpolation

```
dalle fibonacci(n: adad64) -> adad64 {
    iza n <= 1 {
        rajje3 n
    }
    rajje3 fibonacci(n - 1) + fibonacci(n - 2)
}

yalla() {
    lakol i fi 0..15 {
        itba3("fib({i}) = {fibonacci(i)}\n")
    }
}
```

### Structs

```
haikal Point {
    x: fasle64,
    y: fasle64,
}

yalla() {
    khalli p: Point = Point{ x: 3.0, y: 4.0 }
    itba3("Point: ({p.x}, {p.y})\n")
}
```

### String Interpolation

```
yalla() {
    khalli name: nass = "Ziad"
    khalli age: adad64 = 23

    itba3("marhaba ya {name}!\n")
    itba3("age: {age}, next year: {age + 1}\n")
}
```

### Easter Eggs

```
yalla() {
    nashmi()    // NashmiC manifesto
    drobi()     // Random Jordanian proverb
    mansaf()    // Mansaf recipe in ASCII art
    sahteen()   // "bon appetit"
}
```

## Keyword Reference

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
| `mante2` | `bool` | boolean (ah/la) |
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
| `tamam(x)` | Ok(x), success result |
| `ghalat(x)` | Err(x), error result |
| `fi(x)` | Some(x), optional has value |
| `mafi` | None, optional is empty |

### Error Handling

| NashmiC | Meaning |
|---------|---------|
| `natije<T>` | Result type |
| `wala?` | unwrap or propagate error |
| `yimkin<T>` | Optional type |
| `ba3dain` | defer (runs on scope exit) |
| `faz3a` | panic |

### I/O

| NashmiC | Meaning |
|---------|---------|
| `itba3(...)` | print to stdout |
| `i2ra()` | read line from stdin |
| `itla3(code)` | exit program |

### Standard Library

| Module | Functions |
|--------|-----------|
| `riyadiyat` (math) | `jadhr`, `qowa`, `mutlaq`, `ashwa2i`, `jeta`, `jeta_tamam`, `dal`, `ardiye`, `sa2fiye`, `da2reb`, `aqall`, `akthar`, `log_tabi3i`, `log10` |
| `nusoos` (strings) | `toul`, `qass`, `damj`, `badel`, `yihtawi`, `bdaya`, `nihaya`, `a3la`, `asfal`, `qass_haddi`, `harf_3ind`, `juz2`, `karrer` |
| `malafat` (files) | `iftah_malaf`, `iftah_malaf_ktaba`, `sakker_malaf`, `i2ra_kol`, `uktub_malaf`, `dahef_malaf`, `mawjood`, `imsah_malaf`, `hajm_malaf`, `i2ra_sutoor` |

### Easter Eggs

| NashmiC | Meaning |
|---------|---------|
| `drobi()` | random Jordanian proverb |
| `mansaf()` | mansaf recipe (ASCII art) |
| `sahteen()` | "bon appetit" |
| `nashmi()` | about NashmiC |

## Compiler Messages

NashmiC errors come with Jordanian proverbs:

```
خطأ[E0001]: expected expression
  --> src/main.nsh:12:18
     |
  12 |     khalli x = ???
     |                ^^^
     |
      = اصبر على الحصرم تاكله عنب
      = patience turns sour to sweet
```

Build success messages rotate randomly:

```
✓ تم! زي الفل
✓ تم! نشمي والله
✓ تم! والله ما قصرت
```

## Documentation

Full documentation is available in the `docs/` directory, built with [mdBook](https://rust-lang.github.io/mdBook/).

```bash
# Install mdBook (if you don't have it)
cargo install mdbook

# Build and serve docs locally
cd docs && mdbook serve --open
```

Covers: language basics, types, error handling, pattern matching, methods, and complete keyword reference.

## Project Structure

```
nashmic/
├── compiler/src/              # mansaf compiler (C)
│   ├── main.c                 # CLI entry point
│   ├── lexer.c/h              # UTF-8 tokenizer
│   ├── keywords.c/h           # Franco-Arab keyword table
│   ├── parser.c/h             # Recursive descent + Pratt parser
│   ├── ast.c/h                # AST node types
│   ├── codegen_c.c/h          # C transpiler backend
│   ├── sema.c/h               # Semantic analysis + type checking
│   ├── formatter.c/h          # Auto-formatter (mansaf fmt)
│   ├── repl.c/h               # Interactive REPL (mansaf repl)
│   └── diagnostics.c/h        # Errors with proverbs
├── runtime/                   # Runtime library (C)
│   └── nsh_runtime.c/h        # Core: print, read, exit, easter eggs
├── stdlib/                    # Standard library
│   ├── riyadiyat/             # Math (sqrt, pow, sin, cos, etc.)
│   ├── nusoos/                # Strings (split, join, replace, etc.)
│   └── malafat/               # File I/O (read, write, delete, etc.)
├── editor/                    # Editor extensions
│   ├── vscode-nashmic/        # VS Code extension (published)
│   └── tree-sitter-nashmic/   # Tree-sitter grammar (Neovim/Helix/Zed)
├── packaging/                 # Distribution packaging
│   ├── aur/                   # Arch Linux (AUR)
│   └── deb/                   # Debian/Ubuntu (.deb)
├── examples/                  # 15 example programs
├── tools/playground/          # Web playground source
├── docs/site/                 # GitHub Pages site + playground
├── tests/                     # Test suite (23 tests)
├── install.sh                 # One-liner installer
├── Makefile
├── LICENSE
└── README.md
```

## How It Works

NashmiC compiles through C:

```
.nsh source → Lexer → Parser → AST → Sema → C Codegen → .c file → cc → native binary
```

The semantic analysis pass catches type mismatches, undeclared variables, invalid field access, and more — with Jordanian proverb error messages. The generated C code links against `nsh_runtime.c` for print/read/exit and easter eggs.

## Developer Tools

### Formatter — `mansaf fmt`

Auto-format NashmiC source with consistent 4-space indentation, brace placement, and spacing.

```bash
# Print formatted output to stdout
mansaf fmt myfile.nsh

# Format in-place (overwrite the file)
mansaf fmt --write myfile.nsh
```

Works with any editor — set `mansaf fmt --write` as your external formatter.

### Interactive REPL — `mansaf repl`

Type NashmiC code and see results instantly. Multi-line input via brace tracking, accumulated declarations across evaluations.

```bash
mansaf repl
```

```
nashmi> itba3("marhaba!\n")
marhaba!
nashmi> dalle square(n: adad64) -> adad64 { rajje3 n * n }
✓ declaration saved
nashmi> itba3("5^2 = {square(5)}\n")
5^2 = 25
```

Commands: `:q` quit, `:clear` reset declarations, `:decls` show saved declarations, `:help`.

### Test Suite — `mansaf test`

Run the full test suite (pass tests + semantic error tests):

```bash
mansaf test          # Run all tests
mansaf test -v       # Verbose mode (show diffs on failure)
mansaf test arrays   # Run only tests matching "arrays"
```

## Editor Support

### VS Code (recommended)

Install from the [VS Code Marketplace](https://marketplace.visualstudio.com/items?itemName=ziadstr.nashmic):

```bash
code --install-extension ziadstr.nashmic
```

Or search **"NashmiC"** in the Extensions panel (`Ctrl+Shift+X`).

**Includes:**
- Syntax highlighting for 80+ keywords
- **Format on save** via `mansaf fmt` (enabled by default)
- **Run file** (`Ctrl+Shift+R`) and **Build file** (`Ctrl+Shift+B`)
- 22 snippets for `yalla`, `dalle`, `iza`, `hasab`, `natije`, `haikal`, `ta3dad`, `tabbe2`, and more
- Bracket matching, auto-closing, code folding
- Auto-indentation

### Neovim (Tree-sitter)

Full syntax highlighting via tree-sitter. Add to your `init.lua`:

```lua
local parser_config = require("nvim-treesitter.parsers").get_parser_configs()
parser_config.nashmic = {
  install_info = {
    url = "https://github.com/Ziadstr/nashmic",
    files = { "src/parser.c" },
    subdirectory = "editor/tree-sitter-nashmic",
    branch = "main",
  },
  filetype = "nashmic",
}
vim.filetype.add({ extension = { nsh = "nashmic" } })
```

Then `:TSInstall nashmic` and copy the queries:

```bash
mkdir -p ~/.config/nvim/queries/nashmic
cp editor/tree-sitter-nashmic/queries/*.scm ~/.config/nvim/queries/nashmic/
```

Formatting with `mansaf fmt`:

```lua
vim.bo.formatprg = "mansaf fmt"
```

### Any Editor

`mansaf fmt` reads from a file and writes formatted output to stdout. Set it as your editor's external format command for `.nsh` files. Use `mansaf fmt --write <file>` for in-place formatting.

## Status

NashmiC is in active development. The compiler handles:

- ✅ Functions, variables, constants
- ✅ Control flow (if/else, while, for-each, loop, break, continue)
- ✅ Range iteration (`lakol i fi 0..20`)
- ✅ String interpolation (`"hello {name}"`)
- ✅ String concatenation (`"hello " + name`)
- ✅ Structs (declaration, literals, member access)
- ✅ Recursive functions
- ✅ Operator precedence (Pratt parsing)
- ✅ Cultural compiler messages with proverbs
- ✅ Easter egg runtime functions
- ✅ Error handling (`natije<T>` + `wala?`)
- ✅ Optional types (`yimkin<T>` + `iza fi`)
- ✅ Enums (tagged unions) + pattern matching (`hasab`/`hale`)
- ✅ Impl blocks (methods with `tabbe2`)
- ✅ Defer (`ba3dain`)
- ✅ Arrays (`saff<T>` with `.zeed()`, `.toul()`, iteration)
- ✅ Semantic analysis (type checking, field/method validation)
- ✅ Auto-formatter (`mansaf fmt`)
- ✅ Interactive REPL (`mansaf repl`)
- ✅ Test harness (`mansaf test`)
- ✅ Standard library (math, strings, file I/O)
- ✅ VS Code extension with formatter, run/build, 22 snippets
- ✅ Tree-sitter grammar (Neovim, Helix, Zed)
- ✅ Web playground (try it in the browser)
- ✅ CI/CD (build + test + deploy on every push)
- ✅ Distribution (Homebrew, AUR, .deb)
- 📋 Multiple return values
- 📋 Semicolon-free syntax

## License

MIT. See [LICENSE](LICENSE).

## Author

Built by [Ziad Al Sharif](https://github.com/Ziadstr) (ziadstr) 🇯🇴
