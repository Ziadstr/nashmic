# Strings & Interpolation

## String Basics

Strings in NashmiC use the `nass` type (meaning "text") and are declared with double quotes:

```
khalli message: nass = "marhaba ya 3alam"
```

NashmiC strings are UTF-8 and fully support Arabic script:

```
khalli greeting: nass = "مرحبا يا عالم!"
```

## String Interpolation

NashmiC uses `{expr}` inside strings to embed expressions. No format specifiers, no string concatenation -- just put the expression in braces.

### Simple variable interpolation

```
khalli name: nass = "Ziad"
itba3("marhaba ya {name}!\n")
```

Output: `marhaba ya Ziad!`

### Expression interpolation

You can put any expression inside the braces, not just variable names:

```
khalli age: adad64 = 23
itba3("age: {age}, next year: {age + 1}\n")
```

Output: `age: 23, next year: 24`

### Function calls in interpolation

```
dalle double(x: adad64) -> adad64 {
    rajje3 x * 2
}

yalla() {
    khalli n: adad64 = 21
    itba3("double of {n} is {double(n)}\n")
}
```

Output: `double of 21 is 42`

## Type-Aware Formatting

Interpolation is type-aware. The compiler uses C11 `_Generic` to dispatch the correct format for each type:

- `adad` / `adad64` -- formatted as integers
- `fasle` / `fasle64` -- formatted as floating-point numbers
- `nass` -- formatted as strings
- `mante2` -- formatted as boolean

You don't need to specify format codes. The compiler handles it.

## Escape Sequences

Standard escape sequences work in NashmiC strings:

| Sequence | Meaning |
|----------|---------|
| `\n` | Newline |
| `\t` | Tab |
| `\\` | Literal backslash |
| `\"` | Literal double quote |

## Literal Braces

To print a literal `{` or `}` in a string, double them:

```
itba3("this is a literal {{brace}}\n")
```

Output: `this is a literal {brace}`

## How It Works Under the Hood

String interpolation is not a runtime feature -- it's a compile-time transformation. The lexer recognizes `{` inside a string and triggers expression sub-lexing. The parser builds an interpolation AST node. The C codegen emits a `_Generic`-based format string that resolves types at C compile time.

For a string like `"hello {name}, age {age}"`, the compiler generates something equivalent to:

```c
printf("hello %s, age %lld", name, age);
```

But with `_Generic` dispatch so the format specifier matches the actual type.
