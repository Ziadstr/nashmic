# Structs

## Declaring Structs with `haikal`

`haikal` means "structure" or "skeleton" in Arabic. It declares a struct type with named fields.

```
haikal Point {
    x: fasle64,
    y: fasle64,
}
```

Fields are declared with `name: type` syntax, separated by commas.

```
haikal Person {
    name: nass,
    age: adad64,
    height: fasle64,
}
```

## Creating Struct Instances

Use the struct name followed by `{` field initializers `}`:

```
khalli p: Point = Point{ x: 3.0, y: 4.0 }
khalli person: Person = Person{ name: "Ziad", age: 23, height: 1.78 }
```

All fields must be initialized at creation. There are no default values.

## Accessing Fields

Use dot notation to access struct fields:

```
khalli p: Point = Point{ x: 3.0, y: 4.0 }
itba3("x = {p.x}, y = {p.y}\n")
```

## Structs as Function Parameters

Structs can be passed to functions. They are passed by value (copied):

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

## Structs and String Interpolation

You can interpolate struct fields directly:

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

See [Methods & Impl Blocks](../advanced/methods.md) for how to add methods to structs using `tabbe2`.
