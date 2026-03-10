# Getting Started

## Requirements

- A C11 compiler (`gcc` or `clang`)
- `make`
- Linux or macOS

## Installation

### Build from Source

```bash
git clone https://github.com/ziadstr/nashmic.git
cd nashmic
make
```

This builds the `mansaf` compiler binary in `build/`.

### Install Globally

```bash
sudo make install
```

After installation, set the `NASHMIC_ROOT` environment variable so the compiler can find the runtime:

```bash
export NASHMIC_ROOT=~/work/nashmic
```

Add that line to your `.bashrc` or `.zshrc` to make it permanent.

## Your First Program

Create a file called `marhaba.nsh`:

```
yalla() {
    itba3("marhaba ya 3alam!\n")
}
```

Every NashmiC program needs a `yalla()` function -- it's the entry point. The name literally means "let's go!" in Jordanian dialect.

`itba3` is the print function. It means "spit it out" -- direct, no nonsense, very Jordanian.

## Running Your Code

### Run directly

```bash
NASHMIC_ROOT=. build/mansaf run marhaba.nsh
```

This compiles and runs in one step. You should see:

```
marhaba ya 3alam!
```

### Build a binary

```bash
NASHMIC_ROOT=. build/mansaf build marhaba.nsh
./marhaba
```

This produces a native binary you can distribute.

### Build with celebration

```bash
NASHMIC_ROOT=. build/mansaf build --tarab marhaba.nsh
```

The `--tarab` flag attempts to play celebratory audio on a successful build. Because shipping code deserves a party.

### Debug: dump tokens

```bash
NASHMIC_ROOT=. build/mansaf lex marhaba.nsh
```

This prints the token stream for debugging the lexer.

## A More Interesting Program

Let's write Fibonacci with string interpolation:

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
- `dalle` -- function declaration ("a function")
- `iza` -- if statement
- `rajje3` -- return ("bring back")
- `lakol i fi 0..15` -- for-each over a range ("for each i in 0 to 15")
- `{expr}` -- string interpolation with arbitrary expressions

## Running the Examples

NashmiC ships with six example programs:

```bash
# Run all examples at once
make run-all

# Or individually:
NASHMIC_ROOT=. build/mansaf run examples/marhaba.nsh
NASHMIC_ROOT=. build/mansaf run examples/fibonacci.nsh
NASHMIC_ROOT=. build/mansaf run examples/ranges.nsh
NASHMIC_ROOT=. build/mansaf run examples/interpolation.nsh
NASHMIC_ROOT=. build/mansaf run examples/structs.nsh
NASHMIC_ROOT=. build/mansaf run examples/easter_eggs.nsh
```

## Next Steps

- [Variables & Constants](basics/variables.md) -- declaring data with `khalli` and `thabet`
- [Functions](basics/functions.md) -- defining functions with `dalle`
- [Control Flow](basics/control-flow.md) -- `iza`, `wala`, loops, and more
