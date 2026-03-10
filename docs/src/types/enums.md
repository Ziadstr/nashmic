# Enums (Tagged Unions)

> **Status:** Working. Enums compile and run.

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

## Under the Hood

In the generated C code, enums become tagged unions: a struct with an integer tag field and a union of payloads. This gives you the safety of sum types with the performance of C unions.
