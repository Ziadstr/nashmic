# Built-in Functions

NashmiC provides built-in functions for I/O and a few cultural extras. These are implemented in `nsh_runtime.c` and linked at compile time.

## Core I/O

### `itba3(...)` - Print

Prints to stdout. The name means "spit it out" (اطبع) in Jordanian dialect.

```
itba3("marhaba ya 3alam!\n")
itba3("name: {name}, age: {age}\n")
```

`itba3` supports string interpolation: expressions in `{braces}` are evaluated and formatted based on their type. See [Strings & Interpolation](../basics/strings.md) for details.

Printf-style format specifiers also work for raw control:

```
itba3("%lld ", value)
itba3("Point: (%g, %g)\n", p.x, p.y)
```

### `i2ra()` - Read

Reads a line from stdin. The name is Franco-Arab for "read" (اقرأ). The `2` represents hamza (ء).

```
khalli input: nass = i2ra()
itba3("you said: {input}\n")
```

Returns a `nass` (string) with the line content.

### `itla3(code)` - Exit

Exits the program with the given exit code. The name means "get out" (اطلع).

```
itla3(0)    // success
itla3(1)    // error
```

## Easter Eggs

NashmiC wouldn't be Jordanian without personality baked in.

### `nashmi()` - About NashmiC

Prints the NashmiC manifesto: what the language is, why it exists, the philosophy behind it.

```
yalla() {
    nashmi()
}
```

"nashmi" (نشمي) means brave, noble, valiant. The ultimate Jordanian compliment.

### `drobi()` - Random Proverb

Prints a random Jordanian proverb. The name means "hit me" (as in "hit me with a proverb").

```
yalla() {
    drobi()
}
```

Each call gives you a different proverb from a curated collection. Call it more than 5 times in a row and the runtime starts roasting you for being addicted to proverbs.

### `mansaf()` - Mansaf Recipe

Prints the mansaf recipe in ASCII art. Mansaf is Jordan's national dish: lamb cooked in dried yogurt (jameed), served over rice. Not just food; a cultural institution.

```
yalla() {
    mansaf()
}
```

### `sahteen()` - Bon Appetit

Prints "صحتين" (sahteen), the Jordanian "bon appetit." Literally means "two healths." Usually follows mansaf.

```
yalla() {
    mansaf()
    sahteen()
}
```

## Complete Easter Eggs Example

```
yalla() {
    itba3("=== NashmiC Easter Eggs ===\n\n")
    nashmi()

    itba3("--- Random proverb ---\n")
    drobi()

    mansaf()
    sahteen()
}
```

## Runtime Mapping

All built-in functions map to C functions in `runtime/nsh_runtime.c`:

| NashmiC | C Function | Description |
|---------|-----------|-------------|
| `itba3()` | `nsh_itba3()` | Print to stdout |
| `i2ra()` | `nsh_i2ra()` | Read line from stdin |
| `itla3()` | `nsh_itla3()` | Exit program |
| `drobi()` | `nsh_drobi()` | Random proverb (rate-limited) |
| `mansaf()` | `nsh_mansaf()` | Mansaf recipe ASCII art |
| `sahteen()` | `nsh_sahteen()` | "Bon appetit" |
| `nashmi()` | `nsh_nashmi()` | NashmiC manifesto |
