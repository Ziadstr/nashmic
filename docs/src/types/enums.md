# Enums (Tagged Unions)

> **Status:** Working. See `examples/enums.nsh`.

## Declaring Enums with `ta3dad`

`ta3dad` (تعداد) means "enumeration" or "counting" in Arabic. The `3` represents the letter ain (ع). In NashmiC, enums are tagged unions: each variant can optionally carry data.

```
ta3dad Shape {
    Da2ira(fasle64),
    Mustateel(fasle64),
    Noqta,
}
```

This declares a `Shape` type with three variants:
- `Da2ira` (circle) - carries one `fasle64` (the radius)
- `Mustateel` (rectangle) - carries one `fasle64` (the width)
- `Noqta` (point) - carries no data

## Variants Without Data

Simple marker variants with no payload:

```
ta3dad Color {
    Ahmar,
    Azra2,
    Akhdar,
}
```

## Variants With Data

Variants can carry a typed value:

```
ta3dad Message {
    Text(nass),
    Number(adad64),
}
```

## Creating Enum Values

Use the variant name to construct a value:

```
khalli shape: Shape = Da2ira(5.0)
khalli color: Color = Ahmar
khalli msg: Message = Text("marhaba")
```

## Using Enums with Pattern Matching

Enums are designed to be used with `hasab`/`hale` (pattern matching):

```
hasab shape {
    hale Da2ira(radius) => {
        itba3("Circle with radius %g\n", radius)
    }
    hale Mustateel(width) => {
        itba3("Rectangle with width %g\n", width)
    }
    hale Noqta => {
        itba3("Just a point\n")
    }
}
```

See [Pattern Matching](../advanced/pattern-matching.md) for full details.

## Full Working Example

From `examples/enums.nsh`:

```
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

dalle color_name(c: Color) {
    hasab c {
        hale Ahmar => {
            itba3("ahmar (red)\n")
        }
        hale Azra2 => {
            itba3("azra2 (blue)\n")
        }
        hale Akhdar => {
            itba3("akhdar (green)\n")
        }
    }
}

yalla() {
    // Create enum values
    khalli circle: Shape = Da2ira(5.0)
    khalli rect: Shape = Mustateel(3.0)
    khalli point: Shape = Noqta

    // Pattern match on shapes
    describe_shape(circle)
    describe_shape(rect)
    describe_shape(point)

    // Simple enums (no payload)
    khalli fav: Color = Ahmar
    color_name(fav)
    color_name(Azra2)
    color_name(Akhdar)
}
```

## Under the Hood

In the generated C code, enums become tagged unions:

```c
// ta3dad Shape { Da2ira(fasle64), Mustateel(fasle64), Noqta }
enum Shape_Tag { Shape_Da2ira = 0, Shape_Mustateel = 1, Shape_Noqta = 2 };

struct Shape {
    int _tag;
    union {
        struct { double _0; } Da2ira;
        struct { double _0; } Mustateel;
    } _v;
};
```

Each variant gets a unique integer tag. Variants with payloads store their data in a union. `hasab`/`hale` compiles to a `switch` on `_tag` with payload extraction in each case.

Constructing a variant:
```c
// NashmiC: Da2ira(5.0)
// Generated C:
(struct Shape){._tag = Shape_Da2ira, ._v.Da2ira._0 = 5.0}
```

This gives you algebraic data types with zero overhead — the same memory layout a C programmer would write by hand.

## Design Rationale

NashmiC enums are inspired by Rust's `enum` (sum types with data) rather than C's `enum` (just named integers). The Franco-Arab naming makes the concept accessible: `ta3dad` is the Arabic word for enumeration, and variant names use natural Arabic words (`Da2ira` = circle, `Noqta` = point). The semantic analysis pass verifies that variant constructors use correct types and that pattern matching covers all variants.
