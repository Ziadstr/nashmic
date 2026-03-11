# Variables & Constants

> **Status:** Working.

## Variables with `khalli`

`khalli` (خلّي) means "let there be" in Jordanian dialect. It declares a variable.

```
khalli name: nass = "Ziad"
khalli age: adad64 = 23
khalli pi: fasle64 = 3.14159
khalli alive: mante2 = ah
```

The pattern is always:

```
khalli <name>: <type> = <value>
```

Variables declared with `khalli` are mutable. You can reassign them:

```
khalli count: adad64 = 0
count = count + 1
```

A complete example:

```
yalla() {
    khalli name: nass = "Ziad"
    khalli age: adad64 = 23
    itba3("{name} is {age} years old\n")

    age = age + 1
    itba3("next year: {age}\n")
}
```

## Constants with `thabet`

`thabet` (ثابت) means "fixed" or "firm." This value is not going anywhere.

```
thabet MAX_USERS: adad64 = 1000
thabet PI: fasle64 = 3.14159265
thabet GREETING: nass = "marhaba"
```

Constants cannot be reassigned after declaration. Attempting to do so is a compiler error.

## Type Annotations

Every variable needs an explicit type annotation:

```
// Correct
khalli x: adad64 = 10

// Will not compile - no type annotation
// khalli x = 10
```

This is by design. Explicit types make code easier to read and reason about, especially in a language where the keywords are already unfamiliar to most of the world.

### Numeric Types

| Type | C Equivalent | Description |
|------|-------------|-------------|
| `adad` | `int32_t` | 32-bit integer. "adad" (عدد) means "number" |
| `adad64` | `int64_t` | 64-bit integer |
| `fasle` | `float` | 32-bit float. "fasle" (فاصلة) means "decimal point" |
| `fasle64` | `double` | 64-bit float |

```
khalli small: adad = 42
khalli big: adad64 = 9999999999
khalli precise: fasle64 = 3.14159265358979
```

### Boolean Type

| Type | C Equivalent | Description |
|------|-------------|-------------|
| `mante2` | `bool` | Boolean. "mante2" (منطقي) means "logical" |

Boolean values use Jordanian words:
- `ah` - true (literally "yeah")
- `la` - false (literally "no")

```
khalli isReady: mante2 = ah
khalli isDone: mante2 = la
```

### Text Types

| Type | C Equivalent | Description |
|------|-------------|-------------|
| `harf` | `char` | Single character. "harf" (حرف) means "letter" |
| `nass` | `const char*` | String. "nass" (نص) means "text" |

```
khalli letter: harf = 'A'
khalli message: nass = "marhaba ya 3alam"
```

Strings support interpolation with `{expr}`. See [Strings & Interpolation](strings.md) for details.

### Void Type

| Type | C Equivalent | Description |
|------|-------------|-------------|
| `fadi` | `void` | No value. "fadi" (فاضي) means "empty" |

`fadi` is used for functions that don't return a value. You don't use it for variables.

## Full Working Example

```
dalle describe_person(name: nass, age: adad64) {
    itba3("{name} is {age} years old\n")
}

yalla() {
    // Variables with type annotations
    khalli name: nass = "Ziad"
    khalli age: adad64 = 23
    khalli height: fasle64 = 1.78
    khalli isStudent: mante2 = la

    describe_person(name, age)

    // Constants
    thabet MAX_AGE: adad64 = 120
    itba3("max age: {MAX_AGE}\n")

    // Mutable reassignment
    age = age + 1
    itba3("happy birthday! now {age}\n")
}
```
