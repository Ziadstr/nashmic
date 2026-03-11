const EXAMPLES = [
  {
    id: "marhaba",
    name: "Marhaba (Hello World)",
    code: `// marhaba.nsh — \u0645\u0631\u062d\u0628\u0627 \u064a\u0627 \u0639\u0627\u0644\u0645
// The first NashmiC program — Hello World!

yalla() {
    itba3("\u0645\u0631\u062d\u0628\u0627 \u064a\u0627 \u0639\u0627\u0644\u0645! \ud83c\uddef\ud83c\uddf4\\n")
    itba3("Welcome to NashmiC — ya nashmi!\\n")
}`,
    output: `\u0645\u0631\u062d\u0628\u0627 \u064a\u0627 \u0639\u0627\u0644\u0645! \ud83c\uddef\ud83c\uddf4
Welcome to NashmiC \u2014 ya nashmi!`,
    generatedC: `#include <stdio.h>
#include "nashmic_runtime.h"

int main(void) {
    printf("\u0645\u0631\u062d\u0628\u0627 \u064a\u0627 \u0639\u0627\u0644\u0645! \ud83c\uddef\ud83c\uddf4\\n");
    printf("Welcome to NashmiC \u2014 ya nashmi!\\n");
    return 0;
}`
  },
  {
    id: "fibonacci",
    name: "Fibonacci",
    code: `// fibonacci.nsh — Fibonacci sequence in NashmiC
// Uses ranges and string interpolation

dalle fibonacci(n: adad64) -> adad64 {
    iza n <= 1 {
        rajje3 n
    }
    rajje3 fibonacci(n - 1) + fibonacci(n - 2)
}

yalla() {
    lakol i fi 0..15 {
        itba3("fib({i}) = {fibonacci(i)}\\n")
    }
}`,
    output: `fib(0) = 0
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
fib(14) = 377`,
    generatedC: `#include <stdio.h>
#include "nashmic_runtime.h"

int64_t fibonacci(int64_t n) {
    if (n <= 1) {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main(void) {
    for (int64_t i = 0; i < 15; i++) {
        printf("fib(%lld) = %lld\\n", i, fibonacci(i));
    }
    return 0;
}`
  },
  {
    id: "arrays",
    name: "Arrays (saff)",
    code: `// arrays.nsh — saff<T> array type demo
// Demonstrates: array literals, indexing, iteration, push, length, string concat

dalle print_array(nums: saff<adad64>) {
    itba3("[ ")
    lakol n fi nums {
        itba3("%lld ", n)
    }
    itba3("]\\n")
}

yalla() {
    // Array literal
    khalli a3dad: saff<adad64> = [10, 20, 30, 40, 50]

    itba3("=== saff (Arrays) ===\\n")

    // Length
    itba3("toul: %lld\\n", a3dad.toul())

    // Index access
    itba3("a3dad[0] = %lld\\n", a3dad[0])
    itba3("a3dad[2] = %lld\\n", a3dad[2])

    // Iteration with lakol
    itba3("kol al-a3dad: ")
    print_array(a3dad)

    // Push element
    a3dad.zeed(60)
    a3dad.zeed(70)
    itba3("ba3d al-ziyade: ")
    print_array(a3dad)
    itba3("toul jadeed: %lld\\n", a3dad.toul())

    // Float array
    khalli drajat: saff<fasle64> = [95.5, 87.3, 92.1]
    itba3("\\ndrajat: [ ")
    lakol d fi drajat {
        itba3("%g ", d)
    }
    itba3("]\\n")

    // String concatenation
    khalli ism = "NashmiC"
    khalli tahiyye = "marhaba ya " + ism + "!"
    itba3("\\n%s\\n", tahiyye)

    // String length
    khalli toul_ism = toul(ism)
    itba3("toul '%s' = %lld\\n", ism, toul_ism)

    // Empty array with push
    khalli farigh: saff<adad64> = []
    farigh.zeed(1)
    farigh.zeed(2)
    farigh.zeed(3)
    itba3("\\nfarigh ba3d al-ziyade: ")
    print_array(farigh)
}`,
    output: `=== saff (Arrays) ===
toul: 5
a3dad[0] = 10
a3dad[2] = 30
kol al-a3dad: [ 10 20 30 40 50 ]
ba3d al-ziyade: [ 10 20 30 40 50 60 70 ]
toul jadeed: 7

drajat: [ 95.5 87.3 92.1 ]

marhaba ya NashmiC!
toul 'NashmiC' = 7

farigh ba3d al-ziyade: [ 1 2 3 ]`,
    generatedC: `#include <stdio.h>
#include <string.h>
#include "nashmic_runtime.h"

void print_array(NshArray_i64 nums) {
    printf("[ ");
    for (int64_t _i = 0; _i < nums.len; _i++) {
        int64_t n = nums.data[_i];
        printf("%lld ", n);
    }
    printf("]\\n");
}

int main(void) {
    NshArray_i64 a3dad = nsh_array_i64_from((int64_t[]){10,20,30,40,50}, 5);
    printf("=== saff (Arrays) ===\\n");
    printf("toul: %lld\\n", (int64_t)a3dad.len);
    printf("a3dad[0] = %lld\\n", a3dad.data[0]);
    printf("a3dad[2] = %lld\\n", a3dad.data[2]);
    printf("kol al-a3dad: ");
    print_array(a3dad);
    nsh_array_i64_push(&a3dad, 60);
    nsh_array_i64_push(&a3dad, 70);
    printf("ba3d al-ziyade: ");
    print_array(a3dad);
    printf("toul jadeed: %lld\\n", (int64_t)a3dad.len);
    // ... (float array, strings, etc.)
    return 0;
}`
  },
  {
    id: "natije",
    name: "Result Type (natije)",
    code: `// natije<T> — Result type with tamam/ghalat + wala? propagation
// NashmiC's signature error handling feature

dalle divide(a: fasle64, b: fasle64) -> natije<fasle64> {
    iza b == 0.0 {
        rajje3 ghalat("ya zalameh, division by zero!")
    }
    rajje3 tamam(a / b)
}

dalle safe_math(x: fasle64) -> natije<fasle64> {
    // wala? unwraps tamam or propagates ghalat automatically
    khalli result: fasle64 = divide(x, 2.0) wala?
    rajje3 tamam(result + 1.0)
}

yalla() {
    itba3("=== natije<T> — Result Type ===\\n\\n")

    // Direct pattern matching on results
    itba3("10 / 3 = ")
    hasab divide(10.0, 3.0) {
        hale tamam(val) => {
            itba3("%g\\n", val)
        }
        hale ghalat(msg) => {
            itba3("Error: %s\\n", msg)
        }
    }

    itba3("10 / 0 = ")
    hasab divide(10.0, 0.0) {
        hale tamam(val) => {
            itba3("%g\\n", val)
        }
        hale ghalat(msg) => {
            itba3("Error: %s\\n", msg)
        }
    }

    // wala? propagation
    itba3("\\nsafe_math(8) = ")
    hasab safe_math(8.0) {
        hale tamam(val) => {
            itba3("%g\\n", val)
        }
        hale ghalat(msg) => {
            itba3("Error: %s\\n", msg)
        }
    }
}`,
    output: `=== natije<T> \u2014 Result Type ===

10 / 3 = 3.33333
10 / 0 = Error: ya zalameh, division by zero!

safe_math(8) = 5`,
    generatedC: `#include <stdio.h>
#include "nashmic_runtime.h"

NshResult_f64 divide(double a, double b) {
    if (b == 0.0) {
        return nsh_result_f64_err("ya zalameh, division by zero!");
    }
    return nsh_result_f64_ok(a / b);
}

NshResult_f64 safe_math(double x) {
    NshResult_f64 _tmp = divide(x, 2.0);
    if (_tmp.is_err) return _tmp;  // wala? propagation
    double result = _tmp.value;
    return nsh_result_f64_ok(result + 1.0);
}

int main(void) {
    printf("=== natije<T> \u2014 Result Type ===\\n\\n");
    printf("10 / 3 = ");
    NshResult_f64 r1 = divide(10.0, 3.0);
    if (!r1.is_err) printf("%g\\n", r1.value);
    else printf("Error: %s\\n", r1.error);
    // ... pattern matching continues
    return 0;
}`
  },
  {
    id: "yimkin",
    name: "Optional Type (yimkin)",
    code: `// yimkin<T> — Optional type with fi/mafi + iza fi binding
// NashmiC's null-safe value handling

dalle find_even(n: adad64) -> yimkin<adad64> {
    iza n % 2 == 0 {
        rajje3 fi(n)
    }
    rajje3 mafi
}

dalle first_positive(a: adad64, b: adad64) -> yimkin<adad64> {
    iza a > 0 {
        rajje3 fi(a)
    }
    iza b > 0 {
        rajje3 fi(b)
    }
    rajje3 mafi
}

yalla() {
    itba3("=== yimkin<T> — Optional Type ===\\n\\n")

    // Pattern matching on optionals
    itba3("find_even(4) = ")
    hasab find_even(4) {
        hale fi(val) => {
            itba3("%lld\\n", val)
        }
        hale mafi => {
            itba3("nothing\\n")
        }
    }

    itba3("find_even(7) = ")
    hasab find_even(7) {
        hale fi(val) => {
            itba3("%lld\\n", val)
        }
        hale mafi => {
            itba3("nothing\\n")
        }
    }

    // iza fi binding — unwrap or else
    itba3("\\niza fi binding:\\n")
    iza fi result = find_even(10) {
        itba3("  found even: %lld\\n", result)
    } wala {
        itba3("  no even number\\n")
    }

    iza fi result = find_even(3) {
        itba3("  found even: %lld\\n", result)
    } wala {
        itba3("  no even number\\n")
    }

    // first_positive
    itba3("\\nfirst_positive(-5, 3) = ")
    hasab first_positive(-5, 3) {
        hale fi(val) => {
            itba3("%lld\\n", val)
        }
        hale mafi => {
            itba3("none\\n")
        }
    }

    itba3("first_positive(-1, -2) = ")
    hasab first_positive(-1, -2) {
        hale fi(val) => {
            itba3("%lld\\n", val)
        }
        hale mafi => {
            itba3("none\\n")
        }
    }
}`,
    output: `=== yimkin<T> \u2014 Optional Type ===

find_even(4) = 4
find_even(7) = nothing

iza fi binding:
  found even: 10
  no even number

first_positive(-5, 3) = 3
first_positive(-1, -2) = none`,
    generatedC: `#include <stdio.h>
#include "nashmic_runtime.h"

NshOption_i64 find_even(int64_t n) {
    if (n % 2 == 0) {
        return nsh_option_i64_some(n);
    }
    return nsh_option_i64_none();
}

NshOption_i64 first_positive(int64_t a, int64_t b) {
    if (a > 0) return nsh_option_i64_some(a);
    if (b > 0) return nsh_option_i64_some(b);
    return nsh_option_i64_none();
}

int main(void) {
    printf("=== yimkin<T> \u2014 Optional Type ===\\n\\n");
    NshOption_i64 r1 = find_even(4);
    printf("find_even(4) = ");
    if (r1.has_value) printf("%lld\\n", r1.value);
    else printf("nothing\\n");
    // ... pattern matching continues
    return 0;
}`
  },
  {
    id: "enums",
    name: "Enums (ta3dad)",
    code: `// Enums (ta3dad) + Pattern Matching (hasab/hale)
// Tagged unions with destructuring — NashmiC style

ta3dad Shape {
    Da2ira(fasle64),
    Mustateel(fasle64),
    Noqta,
}

ta3dad Color {
    Ahmar,
    Azra2,
    Akhdar,
}

dalle describe_shape(s: Shape) {
    hasab s {
        hale Da2ira(radius) => {
            itba3("Da2ira (circle) with radius %g\\n", radius)
        }
        hale Mustateel(width) => {
            itba3("Mustateel (rectangle) with width %g\\n", width)
        }
        hale Noqta => {
            itba3("Noqta (just a point)\\n")
        }
    }
}

dalle color_name(c: Color) {
    hasab c {
        hale Ahmar => {
            itba3("ahmar (red)\\n")
        }
        hale Azra2 => {
            itba3("azra2 (blue)\\n")
        }
        hale Akhdar => {
            itba3("akhdar (green)\\n")
        }
    }
}

yalla() {
    itba3("=== Enums (ta3dad) ===\\n\\n")

    // Create enum values
    khalli circle: Shape = Da2ira(5.0)
    khalli rect: Shape = Mustateel(3.0)
    khalli point: Shape = Noqta

    // Pattern match on shapes
    describe_shape(circle)
    describe_shape(rect)
    describe_shape(point)

    itba3("\\n=== Colors ===\\n\\n")

    // Simple enums (no payload)
    khalli fav: Color = Ahmar
    color_name(fav)
    color_name(Azra2)
    color_name(Akhdar)
}`,
    output: `=== Enums (ta3dad) ===

Da2ira (circle) with radius 5
Mustateel (rectangle) with width 3
Noqta (just a point)

=== Colors ===

ahmar (red)
azra2 (blue)
akhdar (green)`,
    generatedC: `#include <stdio.h>
#include "nashmic_runtime.h"

typedef enum { SHAPE_DA2IRA, SHAPE_MUSTATEEL, SHAPE_NOQTA } ShapeTag;
typedef struct {
    ShapeTag tag;
    union { double da2ira_0; double mustateel_0; };
} Shape;

typedef enum { COLOR_AHMAR, COLOR_AZRA2, COLOR_AKHDAR } ColorTag;
typedef struct { ColorTag tag; } Color;

void describe_shape(Shape s) {
    switch (s.tag) {
        case SHAPE_DA2IRA:
            printf("Da2ira (circle) with radius %g\\n", s.da2ira_0);
            break;
        case SHAPE_MUSTATEEL:
            printf("Mustateel (rectangle) with width %g\\n", s.mustateel_0);
            break;
        case SHAPE_NOQTA:
            printf("Noqta (just a point)\\n");
            break;
    }
}

// ... color_name, main follow same pattern`
  },
  {
    id: "structs",
    name: "Structs (haikal)",
    code: `// structs.nsh — Struct declaration, literals, and member access

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
    itba3("Point: (%g, %g)\\n", p.x, p.y)

    khalli q: Point = Point{ x: 1.0, y: 2.0 }
    itba3("Other: (%g, %g)\\n", q.x, q.y)

    khalli sum: Point = add_points(p, q)
    itba3("Sum: (%g, %g)\\n", sum.x, sum.y)
}`,
    output: `Point: (3, 4)
Other: (1, 2)
Sum: (0, 0)`,
    generatedC: `#include <stdio.h>
#include "nashmic_runtime.h"

typedef struct {
    double x;
    double y;
} Point;

Point add_points(Point a, Point b) {
    Point result = { .x = 0.0, .y = 0.0 };
    return result;
}

int main(void) {
    Point p = { .x = 3.0, .y = 4.0 };
    printf("Point: (%g, %g)\\n", p.x, p.y);
    Point q = { .x = 1.0, .y = 2.0 };
    printf("Other: (%g, %g)\\n", q.x, q.y);
    Point sum = add_points(p, q);
    printf("Sum: (%g, %g)\\n", sum.x, sum.y);
    return 0;
}`
  },
  {
    id: "methods",
    name: "Methods (tabbe2)",
    code: `// tabbe2 — Impl blocks (methods on structs)
// NashmiC's approach to object-oriented behavior

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

haikal Counter {
    count: adad64
}

tabbe2 Counter {
    dalle value(had) -> adad64 {
        rajje3 had.count
    }

    dalle describe(had) {
        itba3("Counter is at %lld", had.count)
    }
}

yalla() {
    itba3("=== tabbe2 — Methods ===\\n\\n")

    khalli p: Point = Point{ x: 3.0, y: 4.0 }
    itba3("Point: ")
    p.display()
    itba3("\\n")

    khalli dist: fasle64 = p.distance()
    itba3("Distance from origin: %g\\n", dist)

    khalli moved: Point = p.translate(1.0, -1.0)
    itba3("After translate(1, -1): ")
    moved.display()
    itba3("\\n")

    khalli c: Counter = Counter{ count: 42 }
    c.describe()
    itba3("\\n")
    itba3("Value: %lld\\n", c.value())
}`,
    output: `=== tabbe2 \u2014 Methods ===

Point: (3, 4)
Distance from origin: 5
After translate(1, -1): (4, 3)
Counter is at 42
Value: 42`,
    generatedC: `#include <stdio.h>
#include <math.h>
#include "nashmic_runtime.h"

typedef struct { double x; double y; } Point;
typedef struct { int64_t count; } Counter;

double Point_distance(Point had) {
    return sqrt(had.x * had.x + had.y * had.y);
}

Point Point_translate(Point had, double dx, double dy) {
    return (Point){ .x = had.x + dx, .y = had.y + dy };
}

void Point_display(Point had) {
    printf("(%g, %g)", had.x, had.y);
}

int64_t Counter_value(Counter had) {
    return had.count;
}

void Counter_describe(Counter had) {
    printf("Counter is at %lld", had.count);
}

int main(void) {
    printf("=== tabbe2 \u2014 Methods ===\\n\\n");
    Point p = { .x = 3.0, .y = 4.0 };
    printf("Point: ");
    Point_display(p);
    printf("\\n");
    double dist = Point_distance(p);
    printf("Distance from origin: %g\\n", dist);
    Point moved = Point_translate(p, 1.0, -1.0);
    printf("After translate(1, -1): ");
    Point_display(moved);
    printf("\\n");
    Counter c = { .count = 42 };
    Counter_describe(c);
    printf("\\n");
    printf("Value: %lld\\n", Counter_value(c));
    return 0;
}`
  },
  {
    id: "defer",
    name: "Defer (ba3dain)",
    code: `// ba3dain — Defer (run code at end of scope)
// Like Go's defer, but Jordanian

dalle greet() {
    itba3("start\\n")
    ba3dain {
        itba3("cleanup 1\\n")
    }
    ba3dain {
        itba3("cleanup 2\\n")
    }
    itba3("middle\\n")
    rajje3
}

dalle compute(x: adad64) -> adad64 {
    ba3dain {
        itba3("  compute done\\n")
    }
    rajje3 x * 2
}

yalla() {
    itba3("=== ba3dain — Defer ===\\n\\n")

    itba3("--- void function ---\\n")
    greet()
    itba3("\\n")

    itba3("--- function with return value ---\\n")
    khalli result: adad64 = compute(21)
    itba3("  result = %lld\\n", result)
}`,
    output: `=== ba3dain \u2014 Defer ===

--- void function ---
start
middle
cleanup 2
cleanup 1

--- function with return value ---
  compute done
  result = 42`,
    generatedC: `#include <stdio.h>
#include "nashmic_runtime.h"

void greet(void) {
    printf("start\\n");
    // ba3dain blocks are collected and run in LIFO order at return
    printf("middle\\n");
    // --- deferred cleanup (LIFO) ---
    printf("cleanup 2\\n");
    printf("cleanup 1\\n");
}

int64_t compute(int64_t x) {
    int64_t _retval = x * 2;
    // --- deferred ---
    printf("  compute done\\n");
    return _retval;
}

int main(void) {
    printf("=== ba3dain \u2014 Defer ===\\n\\n");
    printf("--- void function ---\\n");
    greet();
    printf("\\n");
    printf("--- function with return value ---\\n");
    int64_t result = compute(21);
    printf("  result = %lld\\n", result);
    return 0;
}`
  },
  {
    id: "interpolation",
    name: "String Interpolation",
    code: `// interpolation.nsh — String interpolation demo

yalla() {
    khalli name: nass = "Ziad"
    khalli age: adad64 = 23

    // Simple interpolation
    itba3("marhaba ya {name}!\\n")

    // Expression interpolation
    itba3("age: {age}, next year: {age + 1}\\n")

    // Plain strings still work
    itba3("no interpolation here\\n")

    // Escaped braces
    itba3("this is a literal {{brace}}\\n")
}`,
    output: `marhaba ya Ziad!
age: 23, next year: 24
no interpolation here
this is a literal {{brace}}`,
    generatedC: `#include <stdio.h>
#include "nashmic_runtime.h"

int main(void) {
    const char* name = "Ziad";
    int64_t age = 23;

    // {name} \u2192 %s interpolation
    printf("marhaba ya %s!\\n", name);

    // {age} and {age + 1} \u2192 expression evaluation
    printf("age: %lld, next year: %lld\\n", age, age + 1);

    printf("no interpolation here\\n");

    // {{ and }} \u2192 literal braces
    printf("this is a literal {{brace}}\\n");
    return 0;
}`
  },
  {
    id: "ranges",
    name: "Ranges (lakol..fi)",
    code: `// ranges.nsh — Fixed range iteration with lakol..fi

yalla() {
    itba3("Counting 0 to 9:\\n")
    lakol i fi 0..10 {
        itba3("%lld ", i)
    }
    itba3("\\n")

    itba3("Counting 5 to 14:\\n")
    lakol j fi 5..15 {
        itba3("%lld ", j)
    }
    itba3("\\n")

    // Fibonacci with ranges
    itba3("\\nFibonacci with ranges:\\n")
    khalli a: adad64 = 0
    khalli b: adad64 = 1
    lakol i fi 0..15 {
        itba3("fib(%lld) = %lld\\n", i, a)
        khalli temp: adad64 = a + b
        a = b
        b = temp
    }
}`,
    output: `Counting 0 to 9:
0 1 2 3 4 5 6 7 8 9
Counting 5 to 14:
5 6 7 8 9 10 11 12 13 14

Fibonacci with ranges:
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
fib(14) = 377`,
    generatedC: `#include <stdio.h>
#include "nashmic_runtime.h"

int main(void) {
    printf("Counting 0 to 9:\\n");
    for (int64_t i = 0; i < 10; i++) {
        printf("%lld ", i);
    }
    printf("\\n");

    printf("Counting 5 to 14:\\n");
    for (int64_t j = 5; j < 15; j++) {
        printf("%lld ", j);
    }
    printf("\\n");

    printf("\\nFibonacci with ranges:\\n");
    int64_t a = 0;
    int64_t b = 1;
    for (int64_t i = 0; i < 15; i++) {
        printf("fib(%lld) = %lld\\n", i, a);
        int64_t temp = a + b;
        a = b;
        b = temp;
    }
    return 0;
}`
  },
  {
    id: "riyadiyat",
    name: "Math (riyadiyat)",
    code: `// riyadiyat -- NashmiC standard library math functions
// Square roots, powers, trig, rounding, and more

yalla() {
    itba3("=== riyadiyat (Math) ===\\n\\n")

    // Square root and power
    itba3("jadhr(16) = {jadhr(16.0)}\\n")
    itba3("qowa(2, 10) = {qowa(2.0, 10.0)}\\n")

    // Absolute value
    itba3("mutlaq(-42) = {mutlaq(-42.0)}\\n")

    // Trig
    itba3("jeta(0) = {jeta(0.0)}\\n")
    itba3("jeta_tamam(0) = {jeta_tamam(0.0)}\\n")

    // Rounding
    itba3("ardiye(3.7) = {ardiye(3.7)}\\n")
    itba3("sa2fiye(3.2) = {sa2fiye(3.2)}\\n")
    itba3("da2reb(3.5) = {da2reb(3.5)}\\n")

    // Min / Max
    itba3("aqall(5, 3) = {aqall(5.0, 3.0)}\\n")
    itba3("akthar(5, 3) = {akthar(5.0, 3.0)}\\n")

    // Logarithms
    itba3("log10(1000) = {log10(1000.0)}\\n")
}`,
    output: `=== riyadiyat (Math) ===

jadhr(16) = 4
qowa(2, 10) = 1024
mutlaq(-42) = 42
jeta(0) = 0
jeta_tamam(0) = 1
ardiye(3.7) = 3
sa2fiye(3.2) = 4
da2reb(3.5) = 4
aqall(5, 3) = 3
akthar(5, 3) = 5
log10(1000) = 3`,
    generatedC: `#include <stdio.h>
#include <math.h>
#include "nsh_runtime.h"

int main(void) {
    nsh_runtime_init();
    printf("=== riyadiyat (Math) ===\\n\\n");
    printf("jadhr(16) = %g\\n", sqrt(16.0));
    printf("qowa(2, 10) = %g\\n", pow(2.0, 10.0));
    printf("mutlaq(-42) = %g\\n", fabs(-42.0));
    // ... trig, rounding, min/max, log
    nsh_runtime_shutdown();
    return 0;
}`
  },
  {
    id: "nusoos",
    name: "Strings (nusoos)",
    code: `// nusoos -- NashmiC standard library string functions
// Split, join, replace, contains, case conversion, and more

yalla() {
    itba3("=== nusoos (Strings) ===\\n\\n")

    khalli text: nass = "marhaba ya nashmi"

    // Length
    itba3("toul: {toul(text)}\\n")

    // Contains
    iza (yihtawi(text, "nashmi")) {
        itba3("yihtawi 'nashmi': ah\\n")
    }

    // Starts/ends with
    iza (bdaya(text, "marhaba")) {
        itba3("bdaya 'marhaba': ah\\n")
    }

    // Case conversion
    itba3("a3la: {a3la(text)}\\n")
    itba3("asfal: {asfal(text)}\\n")

    // Replace
    itba3("badel: {badel(text, "nashmi", "za3eem")}\\n")

    // Repeat
    itba3("karrer: {karrer("na", 5)}\\n")

    // Substring
    itba3("juz2: {juz2(text, 0, 7)}\\n")
}`,
    output: `=== nusoos (Strings) ===

toul: 17
yihtawi 'nashmi': ah
bdaya 'marhaba': ah
a3la: MARHABA YA NASHMI
asfal: marhaba ya nashmi
badel: marhaba ya za3eem
karrer: nanananana
juz2: marhaba`,
    generatedC: `#include <stdio.h>
#include <string.h>
#include "nsh_runtime.h"

int main(void) {
    nsh_runtime_init();
    const char* text = "marhaba ya nashmi";
    printf("toul: %lld\\n", (int64_t)strlen(text));
    printf("yihtawi: %s\\n", strstr(text, "nashmi") ? "ah" : "la");
    printf("a3la: %s\\n", nsh_a3la(text));
    // ... replace, repeat, substring
    nsh_runtime_shutdown();
    return 0;
}`
  }
];
