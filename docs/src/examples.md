# Examples

NashmiC ships with six working examples in the `examples/` directory. Run them all with `make run-all` or individually with `mansaf run examples/<name>.nsh`.

## 1. Hello World (`marhaba.nsh`)

The classic first program. Arabic script and emoji in strings, `yalla()` entry point, `itba3()` printing.

```
// marhaba.nsh — مرحبا يا عالم
// The first NashmiC program — Hello World!

yalla() {
    itba3("مرحبا يا عالم! 🇯🇴\n")
    itba3("Welcome to NashmiC — ya nashmi!\n")
}
```

```bash
mansaf run examples/marhaba.nsh
```

Output:

```
مرحبا يا عالم! 🇯🇴
Welcome to NashmiC — ya nashmi!
```

**Features:** UTF-8 strings, Arabic text, emoji, `yalla()` entry point, `itba3()`.

---

## 2. Fibonacci (`fibonacci.nsh`)

Recursive Fibonacci with range iteration and string interpolation.

```
// fibonacci.nsh — Fibonacci sequence in NashmiC
// Uses ranges and string interpolation

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

```bash
mansaf run examples/fibonacci.nsh
```

Output:

```
fib(0) = 0
fib(1) = 1
fib(2) = 1
fib(3) = 2
fib(4) = 3
fib(5) = 5
fib(6) = 8
fib(7) = 13
fib(8) = 21
fib(9) = 34
fib(10) = 55
fib(11) = 89
fib(12) = 144
fib(13) = 233
fib(14) = 377
```

**Features:** `dalle` function declaration, recursion, `rajje3` return, `lakol..fi` range iteration, `{expr}` interpolation with function calls.

---

## 3. Ranges (`ranges.nsh`)

Range iteration with different start/end values and mutable variable reassignment.

```
// ranges.nsh — Fixed range iteration with lakol..fi

yalla() {
    itba3("Counting 0 to 9:\n")
    lakol i fi 0..10 {
        itba3("%lld ", i)
    }
    itba3("\n")

    itba3("Counting 5 to 14:\n")
    lakol j fi 5..15 {
        itba3("%lld ", j)
    }
    itba3("\n")

    // Fibonacci with ranges
    itba3("\nFibonacci with ranges:\n")
    khalli a: adad64 = 0
    khalli b: adad64 = 1
    lakol i fi 0..15 {
        itba3("fib(%lld) = %lld\n", i, a)
        khalli temp: adad64 = a + b
        a = b
        b = temp
    }
}
```

```bash
mansaf run examples/ranges.nsh
```

**Features:** Ranges with different bounds (`0..10`, `5..15`), mutable variables with reassignment, iterative Fibonacci, printf-style format specifiers.

---

## 4. String Interpolation (`interpolation.nsh`)

All the ways string interpolation works.

```
// interpolation.nsh — String interpolation demo

yalla() {
    khalli name: nass = "Ziad"
    khalli age: adad64 = 23

    // Simple interpolation
    itba3("marhaba ya {name}!\n")

    // Expression interpolation
    itba3("age: {age}, next year: {age + 1}\n")

    // Plain strings still work
    itba3("no interpolation here\n")

    // Escaped braces
    itba3("this is a literal {{brace}}\n")
}
```

```bash
mansaf run examples/interpolation.nsh
```

Output:

```
marhaba ya Ziad!
age: 23, next year: 24
no interpolation here
this is a literal {brace}
```

**Features:** Variable interpolation, expression interpolation (arithmetic inside braces), plain strings, escaped braces with `{{`.

---

## 5. Structs (`structs.nsh`)

Struct declaration, literal creation, member access, and passing structs to functions.

```
// structs.nsh — Struct declaration, literals, and member access

haikal Point {
    x: fasle64,
    y: fasle64,
}

dalle add_points(a: Point, b: Point) -> Point {
    khalli result: Point = Point{ x: 0.0, y: 0.0 }
    rajje3 result
}

yalla() {
    khalli p: Point = Point{ x: 3.0, y: 4.0 }
    itba3("Point: (%g, %g)\n", p.x, p.y)

    khalli q: Point = Point{ x: 1.0, y: 2.0 }
    itba3("Other: (%g, %g)\n", q.x, q.y)

    khalli sum: Point = add_points(p, q)
    itba3("Sum: (%g, %g)\n", sum.x, sum.y)
}
```

```bash
mansaf run examples/structs.nsh
```

**Features:** `haikal` struct declaration, `TypeName{ field: value }` creation syntax, dot-notation field access, structs as function parameters and return values.

---

## 6. Easter Eggs (`easter_eggs.nsh`)

NashmiC's cultural personality on display.

```
// easter_eggs.nsh — NashmiC cultural functions

yalla() {
    itba3("=== NashmiC Easter Eggs ===\n\n")

    // About NashmiC
    nashmi()

    // Random proverb
    itba3("--- Random proverb ---\n")
    drobi()

    // Mansaf recipe
    mansaf()

    // Bon appetit
    sahteen()
}
```

```bash
mansaf run examples/easter_eggs.nsh
```

**Features:** The four cultural built-in functions:
- `nashmi()` - prints the NashmiC manifesto
- `drobi()` - prints a random Jordanian proverb (call it more than 5 times and it starts roasting you)
- `mansaf()` - prints the mansaf recipe in ASCII art
- `sahteen()` - prints "bon appetit" in Arabic

These aren't novelties. They're a statement: programming languages carry culture, and NashmiC carries Jordan's.
