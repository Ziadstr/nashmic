# Methods & Impl Blocks

> **Status:** Parser complete, codegen written but untested. This feature is coming soon.

## `tabbe2` - Impl Blocks

`tabbe2` (طبّق) means "apply" or "implement" in Jordanian dialect. It attaches methods to a struct type.

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

## `had` - The Self Reference

`had` (هاد) means "this" or "this thing" in Jordanian dialect. It's the receiver: the instance the method is called on. Equivalent to `self` in Rust or `this` in Java/C++.

```
tabbe2 Point {
    dalle magnitude(had) -> fasle64 {
        rajje3 sqrt(had.x * had.x + had.y * had.y)
    }
}
```

`had` must be the first parameter of a method. It receives the struct instance automatically when the method is called.

## Calling Methods

Dot notation:

```
khalli p: Point = Point{ x: 3.0, y: 4.0 }
khalli q: Point = Point{ x: 6.0, y: 8.0 }

khalli d: fasle64 = p.distance(q)
khalli m: fasle64 = p.magnitude()
```

The instance before the dot becomes the `had` parameter.

## Static Methods

Methods without `had` as the first parameter are static. They belong to the type, not an instance:

```
tabbe2 Point {
    dalle origin() -> Point {
        rajje3 Point{ x: 0.0, y: 0.0 }
    }
}
```

The calling convention for static methods is still being finalized.

## Multiple Methods

A single `tabbe2` block can contain any number of methods:

```
haikal Rectangle {
    width: fasle64,
    height: fasle64,
}

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

## Full Example

```
haikal Point {
    x: fasle64,
    y: fasle64,
}

tabbe2 Point {
    dalle magnitude(had) -> fasle64 {
        rajje3 sqrt(had.x * had.x + had.y * had.y)
    }

    dalle distance(had, other: Point) -> fasle64 {
        khalli dx: fasle64 = had.x - other.x
        khalli dy: fasle64 = had.y - other.y
        rajje3 sqrt(dx * dx + dy * dy)
    }
}

yalla() {
    khalli p: Point = Point{ x: 3.0, y: 4.0 }
    khalli q: Point = Point{ x: 6.0, y: 8.0 }
    itba3("p magnitude: {p.magnitude()}\n")
    itba3("distance p to q: {p.distance(q)}\n")
}
```

## Under the Hood

In the generated C code, methods become regular functions with the struct passed as the first argument:

```c
// NashmiC: p.distance(q)
// Generated C: Point_distance(&p, q)
double Point_distance(struct Point *had, struct Point other) { ... }
```

The dot syntax is syntactic sugar that the compiler translates.
