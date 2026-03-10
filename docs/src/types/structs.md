# Structs

## Declaring Structs with `haikal`

`haikal` (هيكل) means "structure" or "skeleton" in Arabic. It declares a struct type with named fields.

```
haikal Point {
    x: fasle64,
    y: fasle64,
}
```

Fields use `name: type` syntax, separated by commas.

```
haikal Person {
    name: nass,
    age: adad64,
    height: fasle64,
}
```

## Creating Struct Instances

Use the struct name followed by `{ field: value }`:

```
khalli p: Point = Point{ x: 3.0, y: 4.0 }
khalli person: Person = Person{ name: "Ziad", age: 23, height: 1.78 }
```

All fields must be initialized at creation. There are no default values.

## Accessing Fields

Dot notation:

```
khalli p: Point = Point{ x: 3.0, y: 4.0 }
itba3("x = {p.x}, y = {p.y}\n")
```

## Structs as Function Parameters

Structs can be passed to and returned from functions. They are passed by value (copied).

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
    khalli q: Point = Point{ x: 1.0, y: 2.0 }
    khalli sum: Point = add_points(p, q)
    itba3("Sum: ({sum.x}, {sum.y})\n")
}
```

> **Note:** Field-level arithmetic on struct fields in return expressions is not yet supported in codegen. The example above returns a zeroed Point. Full struct field arithmetic in expressions is planned.

## Structs and String Interpolation

Struct fields work directly in interpolation:

```
khalli p: Point = Point{ x: 3.0, y: 4.0 }
itba3("Point: ({p.x}, {p.y})\n")
```

## Nested Structs

Structs can contain other structs as fields:

```
haikal Line {
    start: Point,
    end: Point,
}

khalli line: Line = Line{
    start: Point{ x: 0.0, y: 0.0 },
    end: Point{ x: 10.0, y: 10.0 },
}
```

## Methods on Structs

You can attach methods to structs using `tabbe2` (impl blocks). See [Methods & Impl Blocks](../advanced/methods.md) for details.
