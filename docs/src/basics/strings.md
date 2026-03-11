# Strings & Interpolation

> **Status:** Working. See `examples/interpolation.nsh` and `examples/arrays.nsh`.

## String Basics

Strings in NashmiC use the `nass` type ("nass" (نص) means "text") and are declared with double quotes:

```
khalli message: nass = "marhaba ya 3alam"
```

NashmiC strings are UTF-8 and fully support Arabic script:

```
khalli greeting: nass = "مرحبا يا عالم!"
```

## String Interpolation

Put expressions in `{braces}` inside strings. No format specifiers, no string concatenation needed.

### Variable interpolation

```
khalli name: nass = "Ziad"
itba3("marhaba ya {name}!\n")
```

Output: `marhaba ya Ziad!`

### Expression interpolation

Any expression works inside the braces, not just variable names:

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

### Struct fields in interpolation

```
khalli p: Point = Point{ x: 3.0, y: 4.0 }
itba3("Point: ({p.x}, {p.y})\n")
```

Output: `Point: (3, 4)`

## String Concatenation

Strings can be concatenated with the `+` operator:

```
khalli ism = "NashmiC"
khalli tahiyye = "marhaba ya " + ism + "!"
itba3("%s\n", tahiyye)
```

Output: `marhaba ya NashmiC!`

## String Length

Use `toul(str)` to get the length of a string:

```
khalli ism = "NashmiC"
khalli toul_ism = toul(ism)
itba3("toul '%s' = %lld\n", ism, toul_ism)
```

Output: `toul 'NashmiC' = 7`

## Type-Aware Formatting

Interpolation is type-aware. The compiler uses C11 `_Generic` to pick the correct format for each type:

| Type | Format |
|------|--------|
| `adad` / `adad64` | integer |
| `fasle` / `fasle64` | floating-point |
| `nass` | string |
| `mante2` | boolean |

You never specify format codes. The compiler handles it.

## Printf-Style Fallback

`itba3` also supports printf-style format specifiers when you need direct control:

```
itba3("%lld ", value)
itba3("Point: (%g, %g)\n", p.x, p.y)
```

The older example files (`ranges.nsh`, `structs.nsh`) use this style. Both approaches work.

## Escape Sequences

| Sequence | Meaning |
|----------|---------|
| `\n` | Newline |
| `\t` | Tab |
| `\\` | Literal backslash |
| `\"` | Literal double quote |

## Literal Braces

To print a literal `{` in a string, double it:

```
itba3("this is a literal {{brace}}\n")
```

Output: `this is a literal {brace}`

## Complete Example

```
yalla() {
    khalli name: nass = "Ziad"
    khalli age: adad64 = 23

    itba3("marhaba ya {name}!\n")
    itba3("age: {age}, next year: {age + 1}\n")
    itba3("no interpolation here\n")
    itba3("this is a literal {{brace}}\n")
}
```

Output:

```
marhaba ya Ziad!
age: 23, next year: 24
no interpolation here
this is a literal {brace}
```

## Under the Hood

String interpolation is a compile-time transformation, not a runtime feature. The lexer recognizes `{` inside a string and triggers expression sub-lexing. The parser builds an interpolation AST node. The C codegen emits a `_Generic`-based format dispatch that resolves types at C compile time.

For a string like `"hello {name}, age {age}"`, the compiler generates something equivalent to:

```c
printf("hello %s, age %lld", name, age);
```

The format specifier is selected by `_Generic` to match the actual type of each expression.
