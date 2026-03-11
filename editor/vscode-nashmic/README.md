# NashmiC for Visual Studio Code

> The first Franco-Arab programming language — now in your editor.

[![VS Code Marketplace](https://img.shields.io/visual-studio-marketplace/v/ziadstr.nashmic?label=VS%20Code%20Marketplace&color=007ACC)](https://marketplace.visualstudio.com/items?itemName=ziadstr.nashmic)
[![GitHub](https://img.shields.io/github/stars/Ziadstr/nashmic?style=social)](https://github.com/Ziadstr/nashmic)

Language support for [NashmiC](https://github.com/Ziadstr/nashmic), the Jordanian programming language that uses Franco-Arab (Arabizi) keywords and transpiles to C.

**Easy like Python. Powerful like Go. Jordanian to the bone.**

## Features

### Syntax Highlighting

Full TextMate grammar with 80+ keywords across all NashmiC constructs:

| Category | Keywords |
|----------|----------|
| Control flow | `iza`, `wala`, `wala_iza`, `lakol`, `tool_ma`, `liff`, `hasab`, `hale`, `3adi`, `khalas`, `kammel`, `rajje3` |
| Declarations | `dalle`, `khalli`, `thabet`, `yalla`, `tabbe2`, `sife`, `sammi`, `jeeb`, `sadder`, `qesm` |
| Types | `adad`, `adad64`, `fasle`, `fasle64`, `mante2`, `harf`, `nass`, `saff`, `haikal`, `ta3dad`, `natije`, `yimkin` |
| Error handling | `jarreb`, `imsek`, `irmi`, `tamam`, `ghalat`, `faz3a`, `ba3dain`, `binhayto`, `wala?` |
| Values | `ah`, `la`, `wala_ishi`, `fi`, `mafi` |
| I/O | `itba3`, `i2ra`, `itla3` |
| Operators | `w` (and), `aw` (or), `mish` (not), `..` (range), `=>`, `->` |

Plus string interpolation (`"marhaba ya {name}!"`), comments, numbers, and operators.

### Formatter

Integrated formatting via `mansaf fmt`. Requires the NashmiC compiler (`mansaf`) to be installed.

- **Format on save** enabled by default for `.nsh` files
- Use `Shift+Alt+F` (or `Shift+Option+F` on macOS) to format manually
- If `mansaf` is not installed, a helpful message with install instructions is shown

### Run & Build Commands

Run and build NashmiC files directly from VS Code:

| Command | Keybinding | Description |
|---------|------------|-------------|
| **NashmiC: Run Current File** | `Ctrl+Shift+R` (`Cmd+Shift+R` on macOS) | Runs `mansaf run` on the active file |
| **NashmiC: Build Current File** | `Ctrl+Shift+B` (`Cmd+Shift+B` on macOS) | Runs `mansaf build` on the active file |

Output appears in the integrated terminal. The file is auto-saved before running.

### Snippets

Type a prefix and hit Tab to expand:

| Prefix | Expands to |
|--------|-----------|
| `yalla` | Main function `yalla() { ... }` |
| `dalle` | Function declaration |
| `iza` | If/else block |
| `lakol` | For-each loop with range |
| `lakol-arr` | Array iteration |
| `lakol-range` | Range iteration with explicit bounds |
| `tool_ma` | While loop |
| `hasab` | Pattern match block |
| `natije` | Result-returning function with `tamam`/`ghalat` |
| `haikal` | Struct definition |
| `ta3dad` | Enum definition |
| `tabbe2` | Impl block with method |
| `khalli` | Variable declaration |
| `thabet` | Constant declaration |
| `jarreb` | Try/catch block |
| `itba3` | Print statement |
| `saff` | Array declaration |
| `wala?` | Error propagation |
| `iza-fi` | Optional binding with `iza fi` |
| `ba3dain` | Defer block |
| `marhaba` | Full hello world program |
| `sife` | Trait definition |

### Other Features

- **Bracket matching & auto-closing** for `{}`, `()`, `[]`, `""`
- **Code folding** based on braces
- **Comment toggling** with `Ctrl+/` (`Cmd+/` on macOS)
- **Auto-indentation** after `{` and before `}`

## Installation

### From VS Code Marketplace

Search for **"NashmiC"** in the Extensions panel (`Ctrl+Shift+X`), or:

```bash
code --install-extension ziadstr.nashmic
```

### From Source (development)

```bash
cd editor/vscode-nashmic
pnpm install -g @vscode/vsce
vsce package
code --install-extension nashmic-0.2.0.vsix
```

### Debug / Test

1. Open `editor/vscode-nashmic/` in VS Code
2. Press `F5` to launch an Extension Development Host
3. Open any `.nsh` file to see highlighting in action

## Requirements

- **VS Code** 1.75.0 or later
- **mansaf** (NashmiC compiler) — optional, needed for formatting and run/build commands. Install from [github.com/Ziadstr/nashmic](https://github.com/Ziadstr/nashmic)

The extension works without `mansaf` installed — syntax highlighting, snippets, and bracket support work standalone.

## Example

```nashmic
// NashmiC — Fibonacci
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

## What is NashmiC?

NashmiC is the first programming language to use **Franco-Arab (Arabizi)** — the way 400M+ Arabs actually text. No RTL issues, no special keyboard needed. Just code the way you talk.

- [GitHub](https://github.com/Ziadstr/nashmic)
- [Documentation](https://ziadstr.github.io/nashmic/)

## Contributing

Found a missing keyword or highlighting bug? [Open an issue](https://github.com/Ziadstr/nashmic/issues) or submit a PR.

## License

MIT
