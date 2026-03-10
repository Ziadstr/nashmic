# Examples

NashmiC ships with six working examples in the `examples/` directory. Each one demonstrates different language features. Run them all with `make run-all` or individually with `mansaf run examples/<name>.nsh`.

## 1. Hello World (`marhaba.nsh`)

The classic first program. Demonstrates `yalla()` entry point and `itba3()` printing with Arabic script.

```
// marhaba.nsh — مرحبا يا عالم
// The first NashmiC program — Hello World!

yalla() {
    itba3("مرحبا يا عالم! 🇯🇴\n")
    itba3("Welcome to NashmiC — ya nashmi!\n")
}
```

**Run it:**
```bash
NASHMIC_ROOT=. build/mansaf run examples/marhaba.nsh
```

**Output:**
```
مرحبا يا عالم! 🇯🇴
Welcome to NashmiC — ya nashmi!
```

**What it shows:** NashmiC handles UTF-8 natively. Arabic text and emoji work in strings. `yalla()` is the entry point. `itba3` prints to stdout.

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

**Run it:**
```bash
NASHMIC_ROOT=. build/mansaf run examples/fibonacci.nsh
```

**Output:**
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

**What it shows:** Function declarations with `dalle`, recursion, return with `rajje3`, range iteration with `lakol..fi`, and string interpolation with function calls inside `{braces}`.

---

## 3. Ranges (`ranges.nsh`)

Demonstrates range iteration with different start and end values.

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

**Run it:**
```bash
NASHMIC_ROOT=. build/mansaf run examples/ranges.nsh
```

**What it shows:** Ranges with different bounds (`0..10`, `5..15`), mutable variables with reassignment, and iterative Fibonacci as an alternative to the recursive version. Also shows that `itba3` supports printf-style format specifiers alongside interpolation.

---

## 4. String Interpolation (`interpolation.nsh`)

Focused demonstration of NashmiC's string interpolation features.

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

**Run it:**
```bash
NASHMIC_ROOT=. build/mansaf run examples/interpolation.nsh
```

**Output:**
```
marhaba ya Ziad!
age: 23, next year: 24
no interpolation here
this is a literal {brace}
```

**What it shows:** Variable interpolation, expression interpolation (arithmetic inside braces), plain strings without interpolation, and escaped braces with `{{`.

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

**Run it:**
```bash
NASHMIC_ROOT=. build/mansaf run examples/structs.nsh
```

**What it shows:** Defining structs with `haikal`, creating instances with `TypeName{ field: value }` syntax, accessing fields with dot notation, and passing structs as function parameters.

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

**Run it:**
```bash
NASHMIC_ROOT=. build/mansaf run examples/easter_eggs.nsh
```

**What it shows:** The four cultural built-in functions:
- `nashmi()` -- prints the NashmiC manifesto
- `drobi()` -- prints a random Jordanian proverb (call it more than 5 times and it starts roasting you)
- `mansaf()` -- prints the mansaf recipe in ASCII art
- `sahteen()` -- prints "bon appetit" in Arabic

These aren't just novelties. They're a statement: programming languages carry culture, and NashmiC carries Jordan's.
