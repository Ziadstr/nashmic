# NashmiC for Visual Studio Code

Language support for [NashmiC](https://github.com/nashmic/nashmic), the Jordanian programming language using Franco-Arab (Arabizi) keywords that transpiles to C.

## Features

### Syntax Highlighting

Full TextMate grammar covering all NashmiC keywords and constructs:

- **Control flow:** `iza`, `wala`, `wala_iza`, `lakol`, `tool_ma`, `liff`, `hasab`, `hale`, `3adi`, `khalas`, `kammel`, `rajje3`
- **Declarations:** `dalle`, `khalli`, `thabet`, `yalla`, `tabbe2`, `sife`, `sammi`, `jeeb`, `sadder`, `qesm`
- **Types:** `adad`, `adad64`, `fasle`, `fasle64`, `mante2`, `harf`, `nass`, `saff`, `haikal`, `ta3dad`, `natije`, `yimkin`
- **Error handling:** `jarreb`, `imsek`, `irmi`, `tamam`, `ghalat`, `faz3a`, `ba3dain`, `binhayto`, `wala?`
- **Concurrency:** `shaghghel`, `istanna`, `qanah`, `ib3ath`, `istalam`, `3al_tayer`, `khait`
- **Memory:** `ihjez`, `hares`, `3al_3amyani`, `jdeed`, `fukk`
- **Values:** `ah`, `la`, `wala_ishi`
- **String interpolation:** `"marhaba ya {name}!"` with embedded expressions
- **Comments:** `// single-line`

### Snippets

Quick-start templates for common patterns:

| Prefix     | Description                        |
|------------|------------------------------------|
| `yalla`    | Main function entry point          |
| `dalle`    | Function declaration               |
| `iza`      | If/else block                      |
| `lakol`    | For-each loop                      |
| `tool_ma`  | While loop                         |
| `hasab`    | Pattern matching (match/case)      |
| `natije`   | Result-returning function          |
| `haikal`   | Struct definition                  |
| `ta3dad`   | Enum definition                    |
| `tabbe2`   | Impl block                        |
| `khalli`   | Variable declaration               |
| `thabet`   | Constant declaration               |
| `jarreb`   | Try/catch block                    |
| `itba3`    | Print statement                    |
| `sife`     | Trait definition                   |

### Bracket Matching & Folding

Auto-closing and matching for `{}`, `()`, `[]`, and `""`. Code folding based on braces.

### Comment Toggling

`Ctrl+/` (`Cmd+/` on macOS) toggles `//` line comments.

## Installation

### From Source

1. Clone or copy the `editor/vscode-nashmic/` directory
2. Copy it to your VS Code extensions directory:
   - **Linux:** `~/.vscode/extensions/nashmic/`
   - **macOS:** `~/.vscode/extensions/nashmic/`
   - **Windows:** `%USERPROFILE%\.vscode\extensions\nashmic\`
3. Restart VS Code
4. Open any `.nsh` file to activate

### From VSIX (packaged)

```bash
# Install vsce if you don't have it
pnpm install -g @vscode/vsce

# Package the extension
cd editor/vscode-nashmic
vsce package

# Install the .vsix
code --install-extension nashmic-0.1.0.vsix
```

## File Association

The extension automatically activates for files with the `.nsh` extension.

## Example

```
// fibonacci.nsh
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
