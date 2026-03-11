# Methods & Impl Blocks

> **Status:** Working. See `examples/methods.nsh`.

## `tabbe2` - Impl Blocks

`tabbe2` (طبّق) means "apply" or "implement" in Jordanian dialect. It attaches methods to a struct type.

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
```

## `had` - The Self Reference

`had` (هاد) means "this" or "this thing" in Jordanian dialect. It's the receiver: the instance the method is called on. Equivalent to `self` in Rust or `this` in Java/C++.

`had` must be the first parameter of a method. It receives the struct instance automatically when the method is called.

## Calling Methods

Dot notation:

```
khalli p: Point = Point{ x: 3.0, y: 4.0 }

khalli dist: fasle64 = p.distance()
itba3("Distance from origin: %g\n", dist)

khalli moved: Point = p.translate(1.0, -1.0)
moved.display()
```

The instance before the dot becomes the `had` parameter.

## Multiple Methods

A single `tabbe2` block can contain any number of methods:

```
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
```

## Full Working Example

From `examples/methods.nsh`:

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
    khalli p: Point = Point{ x: 3.0, y: 4.0 }
    itba3("Point: ")
    p.display()
    itba3("\n")

    khalli dist: fasle64 = p.distance()
    itba3("Distance from origin: %g\n", dist)

    khalli moved: Point = p.translate(1.0, -1.0)
    itba3("After translate(1, -1): ")
    moved.display()
    itba3("\n")

    khalli c: Counter = Counter{ count: 42 }
    c.describe()
    itba3("\n")
    itba3("Value: %lld\n", c.value())
}
```

## Under the Hood

In the generated C code, methods become regular functions with the struct passed as the first argument:

```c
// NashmiC: p.distance()
// Generated C: Point_distance(&p)
double Point_distance(struct Point *had) { ... }
```

The dot syntax is syntactic sugar that the compiler translates.
