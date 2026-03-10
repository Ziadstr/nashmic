# Contributing to NashmiC

Ahlan wa sahlan! Thanks for wanting to contribute to NashmiC.

## Getting Started

### Prerequisites

- C11 compiler (`gcc` or `clang`)
- `make`
- `git`
- Linux or macOS

### Build from source

```bash
git clone https://github.com/Ziadstr/nashmic.git
cd nashmic
make
```

### Run examples

```bash
make run-all                              # All examples
NASHMIC_ROOT=. ./build/mansaf run examples/marhaba.nsh  # Single example
```

### Run tests

```bash
make test
```

## Project Structure

```
compiler/src/     # mansaf compiler (C11)
  main.c          # CLI entry point
  lexer.c/h       # UTF-8 tokenizer
  keywords.c/h    # Franco-Arab keyword table
  parser.c/h      # Recursive descent + Pratt parser
  ast.c/h         # AST node types
  codegen_c.c/h   # C transpiler backend
  diagnostics.c/h # Errors with Jordanian proverbs
runtime/          # Runtime library
  nsh_runtime.c/h # I/O, easter eggs
examples/         # Working example programs (.nsh)
docs/             # Documentation site
tests/            # Test suite
```

## Compilation Pipeline

```
.nsh source -> Lexer -> Parser -> AST -> C Codegen -> .c file -> cc -> binary
```

## How to Contribute

### Adding a keyword

1. Add the token type in `compiler/src/lexer.h`
2. Register it in `compiler/src/keywords.c`
3. Handle parsing in `compiler/src/parser.c`
4. Add AST node (if needed) in `compiler/src/ast.h` and `compiler/src/ast.c`
5. Generate C code in `compiler/src/codegen_c.c`
6. Add an example in `examples/`
7. Add a Makefile target to run it

### Fixing a bug

1. Write a minimal `.nsh` file that reproduces the issue
2. Fix the bug with minimal changes
3. Verify `make run-all` still passes
4. Add a test case if applicable

### Documentation

Docs live in `docs/site/index.html` (the live site) and `docs/src/` (mdBook source). Update both if changing user-facing features.

## Code Style

- C11 standard
- 4-space indentation
- `snake_case` for functions and variables
- `PascalCase` for types and enum values
- `UPPER_SNAKE_CASE` for constants and macros
- Keep functions under 40 lines where possible
- Comments for non-obvious logic only

## Commit Style

Conventional commits, one line, concise:

```
feat: add saff<T> array type
fix: correct range codegen for negative steps
docs: update pattern matching examples
chore: clean up unused AST nodes
```

## Submitting a PR

1. Fork the repo
2. Create a branch from `main`: `git checkout -b feat/my-feature main`
3. Make your changes
4. Verify: `make clean && make && make run-all`
5. Push and open a PR against `main`

## Franco-Arab Naming Guidelines

NashmiC keywords use Franco-Arab (Arabizi) — the way Arabs text. When naming new keywords:

- Use Jordanian/Levantine dialect, not Modern Standard Arabic
- Spell it how you'd text it to a friend
- Numbers replace Arabic letters: `3` = ع, `2` = ء/ق, `7` = ح, `5` = خ
- Keep it short and natural
- Test it: would a Jordanian CS student actually say this word?

Examples: `khalli` (let), `dalle` (function), `rajje3` (return), `ba3dain` (later/defer)

## Questions?

Open an issue or start a discussion. We don't bite — unless you use `npm`.
