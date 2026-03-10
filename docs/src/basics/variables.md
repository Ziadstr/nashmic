# Variables & Constants

## Variables with `khalli`

`khalli` declares a variable. The word means "let there be" in Jordanian dialect -- you're telling the world this thing exists now.

```
khalli name: nass = "Ziad"
khalli age: adad64 = 23
khalli pi: fasle64 = 3.14159
khalli alive: mante2 = ah
```

Every variable declaration follows this pattern:

```
khalli <name>: <type> = <value>
```

Variables declared with `khalli` are mutable -- you can reassign them:

```
khalli count: adad64 = 0
count = count + 1
```

## Constants with `thabet`

`thabet` declares a constant. The word means "fixed" or "firm" -- this value is not going anywhere.

```
thabet MAX_USERS: adad64 = 1000
thabet PI: fasle64 = 3.14159265
thabet GREETING: nass = "marhaba"
```

Constants cannot be reassigned after declaration. Attempting to do so is a compiler error.

## Types

NashmiC has a straightforward type system. Every variable must have an explicit type annotation.

### Numeric Types

| Type | C Equivalent | Description |
|------|-------------|-------------|
| `adad` | `int32_t` | 32-bit integer. "adad" means "number" |
| `adad64` | `int64_t` | 64-bit integer |
| `fasle` | `float` | 32-bit floating point. "fasle" means "decimal" |
| `fasle64` | `double` | 64-bit floating point |

```
khalli small: adad = 42
khalli big: adad64 = 9999999999
khalli precise: fasle64 = 3.14159265358979
```

### Boolean Type

| Type | C Equivalent | Description |
|------|-------------|-------------|
| `mante2` | `bool` | Boolean. "mante2" means "logical" |

Boolean values use Jordanian words:
- `ah` -- true (literally "yeah")
- `la` -- false (literally "no")

```
khalli isReady: mante2 = ah
khalli isDone: mante2 = la
```

### Text Types

| Type | C Equivalent | Description |
|------|-------------|-------------|
| `harf` | `char` | Single character. "harf" means "letter" |
| `nass` | `const char*` | String. "nass" means "text" |

```
khalli letter: harf = 'A'
khalli message: nass = "marhaba ya 3alam"
```

Strings support interpolation with `{expr}` -- see [Strings & Interpolation](strings.md) for details.

### Void Type

| Type | C Equivalent | Description |
|------|-------------|-------------|
| `fadi` | `void` | No value. "fadi" means "empty" |

`fadi` is used for functions that don't return a value. You don't use it for variables.

## Type Annotations are Required

NashmiC does not have type inference (yet). Every variable needs an explicit type:

```
// Correct
khalli x: adad64 = 10

// Will not compile -- no type annotation
// khalli x = 10
```

This is by design. Explicit types make code easier to read and reason about, especially in a language where the keywords are already unfamiliar to most of the world.
