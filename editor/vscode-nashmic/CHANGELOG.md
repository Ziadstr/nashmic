# Changelog

## 0.2.0 — Formatter, Run/Build, and More Snippets

- **Formatter integration** — auto-format via `mansaf fmt` with format-on-save enabled by default
- **Run command** — `Ctrl+Shift+R` runs the active `.nsh` file via `mansaf run`
- **Build command** — `Ctrl+Shift+B` builds the active `.nsh` file via `mansaf build`
- **7 new snippets** — `saff` (array), `lakol-arr` (array iteration), `lakol-range` (range iteration), `wala?` (error propagation), `iza-fi` (optional binding), `ba3dain` (defer), `marhaba` (hello world template)
- **Extension entry point** — proper activation on `.nsh` files with command registration
- Graceful degradation when `mansaf` is not installed

## 0.1.0 — Initial Release

- Syntax highlighting for 80+ NashmiC keywords
- String interpolation highlighting (`"hello {name}"`)
- 15 code snippets for common patterns
- Bracket matching, auto-closing, and code folding
- Comment toggling support
- `.nsh` file association
