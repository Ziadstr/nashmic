# Getting Started

## Requirements

- A C11 compiler (`gcc` or `clang`)
- `make`
- `git`
- Linux or macOS

## Installation

### One-liner (recommended)

```bash
curl -fsSL https://raw.githubusercontent.com/Ziadstr/nashmic/main/install.sh | bash
```

This checks for dependencies, clones and builds the compiler, installs to `~/.nashmic/`, and adds `mansaf` to your PATH. No sudo needed.

### Build from source

```bash
git clone https://github.com/Ziadstr/nashmic.git
cd nashmic
make
```

This builds the `mansaf` compiler binary in `build/`.

To install globally:

```bash
sudo make install
```

Or set the environment manually:

```bash
export NASHMIC_ROOT=~/path/to/nashmic
export PATH="$NASHMIC_ROOT/build:$PATH"
```

Add those lines to your `.bashrc` or `.zshrc` to make them permanent.

### Uninstall

```bash
~/.nashmic/bin/install.sh --uninstall
# or from the source directory:
./uninstall.sh
```

## Editor Support

NashmiC has a VS Code extension with syntax highlighting, snippets, bracket matching, and code folding.

### Install from VS Code Marketplace

Search for **"NashmiC"** in the Extensions panel (`Ctrl+Shift+X`), or install from the command line:

```bash
code --install-extension ziadstr.nashmic
```

The extension provides:
- Full syntax highlighting for 80+ keywords
- Snippets for common patterns (`yalla`, `dalle`, `iza`, `hasab`, etc.)
- Bracket matching and auto-closing
- Code folding and auto-indentation

## Write Your First Program

Create a file called `marhaba.nsh`:

```
yalla() {
    itba3("marhaba ya 3alam!\n")
}
```

Every NashmiC program needs a `yalla()` function. It's the entry point, like `main` in C. The word means "let's go!" in Jordanian dialect.

`itba3` is the print function. It means "spit it out" (اطبع) - direct, no nonsense, very Jordanian.

## Run It

```bash
mansaf run marhaba.nsh
```

Output:

```
marhaba ya 3alam!
```

`mansaf run` compiles and executes in one step. If you built from source and haven't installed globally, use:

```bash
NASHMIC_ROOT=. build/mansaf run marhaba.nsh
```

## Build a Binary

```bash
mansaf build marhaba.nsh
./marhaba
```

This produces a native binary you can distribute. Add `--tarab` for celebratory audio on success:

```bash
mansaf build --tarab marhaba.nsh
```

"Tarab" (طرب) means musical ecstasy. Because shipping code deserves a party.

## Try String Interpolation

Create `greet.nsh`:

```
yalla() {
    khalli name: nass = "Ziad"
    khalli age: adad64 = 23

    itba3("marhaba ya {name}!\n")
    itba3("age: {age}, next year: {age + 1}\n")
}
```

Run it:

```bash
mansaf run greet.nsh
```

Output:

```
marhaba ya Ziad!
age: 23, next year: 24
```

`khalli` means "let there be" (خلّي). It declares a variable. The `{expr}` syntax inside strings evaluates any expression inline, no format specifiers needed.

## Try a Loop

Create `counting.nsh`:

```
yalla() {
    lakol i fi 0..10 {
        itba3("{i}\n")
    }
}
```

Run it:

```bash
mansaf run counting.nsh
```

This prints 0 through 9. `lakol` means "for each" (لكل), `fi` means "in" (في). The range `0..10` goes from 0 up to (but not including) 10.

## A Bigger Example: Fibonacci

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

This demonstrates:
- `dalle` (دالّة) - function declaration, means "a function"
- `iza` (اذا) - if statement
- `rajje3` (رجّع) - return, means "bring back"
- `lakol i fi 0..15` - for-each over a range
- `{fibonacci(i)}` - function calls inside string interpolation

## Running the Shipped Examples

NashmiC comes with working examples in the `examples/` directory:

```bash
# Run all examples at once
make run-all

# Or individually:
mansaf run examples/marhaba.nsh        # Hello world
mansaf run examples/fibonacci.nsh      # Recursive fibonacci
mansaf run examples/ranges.nsh         # Range iteration
mansaf run examples/interpolation.nsh  # String interpolation
mansaf run examples/structs.nsh        # Struct basics
mansaf run examples/easter_eggs.nsh    # Cultural easter eggs
mansaf run examples/enums.nsh          # Enums + pattern matching
mansaf run examples/natije.nsh         # Result type + error propagation
mansaf run examples/yimkin.nsh         # Optional type
mansaf run examples/methods.nsh        # Methods + impl blocks
mansaf run examples/defer.nsh          # Defer (ba3dain)
mansaf run examples/arrays.nsh         # Arrays, push, iteration
```

## Debug: Dump Tokens

```bash
mansaf lex marhaba.nsh
```

This prints the token stream, useful for understanding how the lexer processes your code.

## Where to Go Next

- [Variables & Constants](basics/variables.md) - declaring data with `khalli` and `thabet`
- [Functions](basics/functions.md) - defining functions with `dalle`
- [Control Flow](basics/control-flow.md) - `iza`, `wala`, loops, and more
- [Strings & Interpolation](basics/strings.md) - string interpolation and type-aware formatting
- [Arrays](basics/arrays.md) - `saff<T>` arrays with push and iteration
- [Enums](types/enums.md) - tagged unions with `ta3dad`
- [Error Handling](error-handling/natije.md) - `natije<T>` result type
- [Pattern Matching](advanced/pattern-matching.md) - `hasab`/`hale` destructuring
