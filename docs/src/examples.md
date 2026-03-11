# Examples

NashmiC ships with twelve working examples in the `examples/` directory. Run them all with `make run-all` or individually with `mansaf run examples/<name>.nsh`.

## 1. Hello World (`marhaba.nsh`)

The classic first program. Arabic script and emoji in strings, `yalla()` entry point, `itba3()` printing.

```
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

**Features:** `dalle` function declaration, recursion, `rajje3` return, `lakol..fi` range iteration, `{expr}` interpolation with function calls.

---

## 3. Ranges (`ranges.nsh`)

Range iteration with different start/end values and mutable variable reassignment.

```
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
yalla() {
    khalli name: nass = "Ziad"
    khalli age: adad64 = 23

    itba3("marhaba ya {name}!\n")
    itba3("age: {age}, next year: {age + 1}\n")
    itba3("no interpolation here\n")
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
yalla() {
    itba3("=== NashmiC Easter Eggs ===\n\n")
    nashmi()

    itba3("--- Random proverb ---\n")
    drobi()

    mansaf()
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

---

## 7. Enums + Pattern Matching (`enums.nsh`)

Tagged unions with `ta3dad` and destructuring with `hasab`/`hale`.

```
ta3dad Shape {
    Da2ira(fasle64),
    Mustateel(fasle64),
    Noqta,
}

dalle describe_shape(s: Shape) {
    hasab s {
        hale Da2ira(radius) => {
            itba3("Da2ira (circle) with radius %g\n", radius)
        }
        hale Mustateel(width) => {
            itba3("Mustateel (rectangle) with width %g\n", width)
        }
        hale Noqta => {
            itba3("Noqta (just a point)\n")
        }
    }
}

yalla() {
    khalli circle: Shape = Da2ira(5.0)
    khalli rect: Shape = Mustateel(3.0)
    khalli point: Shape = Noqta

    describe_shape(circle)
    describe_shape(rect)
    describe_shape(point)
}
```

```bash
mansaf run examples/enums.nsh
```

**Features:** `ta3dad` enum declaration, variants with and without payload, `hasab`/`hale` pattern matching, variant destructuring.

---

## 8. Result Type (`natije.nsh`)

Error handling with `natije<T>`, `tamam`/`ghalat`, and `wala?` propagation.

```
dalle divide(a: fasle64, b: fasle64) -> natije<fasle64> {
    iza b == 0.0 {
        rajje3 ghalat("ya zalameh, division by zero!")
    }
    rajje3 tamam(a / b)
}

dalle safe_math(x: fasle64) -> natije<fasle64> {
    khalli result: fasle64 = divide(x, 2.0) wala?
    rajje3 tamam(result + 1.0)
}

yalla() {
    hasab divide(10.0, 3.0) {
        hale tamam(val) => {
            itba3("10 / 3 = %g\n", val)
        }
        hale ghalat(msg) => {
            itba3("Error: %s\n", msg)
        }
    }

    hasab divide(10.0, 0.0) {
        hale tamam(val) => {
            itba3("10 / 0 = %g\n", val)
        }
        hale ghalat(msg) => {
            itba3("Error: %s\n", msg)
        }
    }
}
```

```bash
mansaf run examples/natije.nsh
```

**Features:** `natije<T>` result type, `tamam(value)` success, `ghalat(message)` error, `wala?` error propagation, pattern matching on results.

---

## 9. Optionals (`yimkin.nsh`)

Optional values with `yimkin<T>`, `fi`/`mafi`, and `iza fi` binding.

```
dalle find_even(n: adad64) -> yimkin<adad64> {
    iza n % 2 == 0 {
        rajje3 fi(n)
    }
    rajje3 mafi
}

yalla() {
    hasab find_even(4) {
        hale fi(val) => {
            itba3("find_even(4) = %lld\n", val)
        }
        hale mafi => {
            itba3("nothing\n")
        }
    }

    // iza fi binding
    iza fi result = find_even(10) {
        itba3("found even: %lld\n", result)
    } wala {
        itba3("no even number\n")
    }
}
```

```bash
mansaf run examples/yimkin.nsh
```

**Features:** `yimkin<T>` optional type, `fi(value)` and `mafi` constructors, `iza fi` binding for safe unwrap, pattern matching on optionals.

---

## 10. Methods (`methods.nsh`)

Impl blocks with `tabbe2` and `had` (self) pointer.

```
haikal Point {
    x: fasle64,
    y: fasle64
}

tabbe2 Point {
    dalle distance(had) -> fasle64 {
        rajje3 sqrt(had.x * had.x + had.y * had.y)
    }

    dalle translate(had, dx: fasle64, dy: fasle64) -> Point {
        rajje3 Point{ x: had.x + dx, y: had.y + dy }
    }

    dalle display(had) {
        itba3("(%g, %g)", had.x, had.y)
    }
}

yalla() {
    khalli p: Point = Point{ x: 3.0, y: 4.0 }
    p.display()
    itba3("\n")

    khalli dist: fasle64 = p.distance()
    itba3("Distance: %g\n", dist)

    khalli moved: Point = p.translate(1.0, -1.0)
    moved.display()
    itba3("\n")
}
```

```bash
mansaf run examples/methods.nsh
```

**Features:** `tabbe2` impl blocks, `had` (self) receiver, dot-syntax method calls, methods with parameters, methods returning structs.

---

## 11. Defer (`defer.nsh`)

Scope cleanup with `ba3dain` in LIFO order.

```
dalle greet() {
    itba3("start\n")
    ba3dain {
        itba3("cleanup 1\n")
    }
    ba3dain {
        itba3("cleanup 2\n")
    }
    itba3("middle\n")
    rajje3
}

dalle compute(x: adad64) -> adad64 {
    ba3dain {
        itba3("  compute done\n")
    }
    rajje3 x * 2
}

yalla() {
    greet()

    khalli result: adad64 = compute(21)
    itba3("  result = %lld\n", result)
}
```

```bash
mansaf run examples/defer.nsh
```

**Features:** `ba3dain` defer blocks, LIFO execution order, defer with return values.

---

## 12. Arrays (`arrays.nsh`)

Dynamic arrays with `saff<T>`, push, length, indexing, and iteration.

```
dalle print_array(nums: saff<adad64>) {
    itba3("[ ")
    lakol n fi nums {
        itba3("%lld ", n)
    }
    itba3("]\n")
}

yalla() {
    khalli a3dad: saff<adad64> = [10, 20, 30, 40, 50]

    itba3("toul: %lld\n", a3dad.toul())
    itba3("a3dad[0] = %lld\n", a3dad[0])

    print_array(a3dad)

    a3dad.zeed(60)
    a3dad.zeed(70)
    print_array(a3dad)

    khalli drajat: saff<fasle64> = [95.5, 87.3, 92.1]
    lakol d fi drajat {
        itba3("%g ", d)
    }
    itba3("\n")

    // String concatenation
    khalli ism = "NashmiC"
    khalli tahiyye = "marhaba ya " + ism + "!"
    itba3("%s\n", tahiyye)
}
```

```bash
mansaf run examples/arrays.nsh
```

**Features:** `saff<T>` array type, array literals `[1, 2, 3]`, indexing `arr[i]`, iteration `lakol x fi arr`, `.zeed()` push, `.toul()` length, string concatenation with `+`.
