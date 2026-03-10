# Methods & Impl Blocks

> **Status:** Parser complete, codegen in progress. This feature is coming soon.

## `tabbe2` -- Impl Blocks

`tabbe2` means "apply" or "implement" in Jordanian dialect. It attaches methods to a struct type.

```
haikal Point {
    x: fasle64,
    y: fasle64,
}

tabbe2 Point {
    dalle distance(had, other: Point) -> fasle64 {
        khalli dx: fasle64 = had.x - other.x
        khalli dy: fasle64 = had.y - other.y
        rajje3 sqrt(dx * dx + dy * dy)
    }

    dalle origin() -> Point {
        rajje3 Point{ x: 0.0, y: 0.0 }
    }
}
```

## `had` -- The Self Reference

`had` means "this" or "this thing" in Jordanian dialect. It's the receiver -- the instance the method is called on. Equivalent to `self` in Rust or `this` in Java/C++.

```
tabbe2 Point {
    dalle magnitude(had) -> fasle64 {
        rajje3 sqrt(had.x * had.x + had.y * had.y)
    }
}
```

`had` must be the first parameter of a method. It receives the struct instance automatically when the method is called.

## Calling Methods

Use dot notation to call methods on struct instances:

```
khalli p: Point = Point{ x: 3.0, y: 4.0 }
khalli q: Point = Point{ x: 6.0, y: 8.0 }

khalli d: fasle64 = p.distance(q)
khalli m: fasle64 = p.magnitude()
```

The instance before the dot becomes the `had` parameter.

## Static Methods

Methods without `had` as the first parameter are static -- they belong to the type, not an instance:

```
tabbe2 Point {
    dalle origin() -> Point {
        rajje3 Point{ x: 0.0, y: 0.0 }
    }
}
```

Static methods would be called on the type directly (calling convention for static methods is still being finalized).

## Multiple Methods

A single `tabbe2` block can contain any number of methods:

```
tabbe2 Rectangle {
    dalle area(had) -> fasle64 {
        rajje3 had.width * had.height
    }

    dalle perimeter(had) -> fasle64 {
        rajje3 2.0 * (had.width + had.height)
    }

    dalle is_square(had) -> mante2 {
        rajje3 had.width == had.height
    }
}
```

## Under the Hood

In the generated C code, methods become regular functions with the struct passed as the first argument:

```
// NashmiC
tabbe2 Point {
    dalle distance(had, other: Point) -> fasle64 { ... }
}

// Generated C (conceptual)
double Point_distance(struct Point *had, struct Point other) { ... }
```

The method call `p.distance(q)` becomes `Point_distance(&p, q)` in C. The dot syntax is syntactic sugar that the compiler translates.
