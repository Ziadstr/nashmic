# Enums (Tagged Unions)

> **Status:** Parser complete, codegen in progress. The syntax below is final, but you cannot compile enum programs yet.

## Declaring Enums with `ta3dad`

`ta3dad` (تعداد) means "enumeration" or "counting" in Arabic. The `3` represents the letter ain (ع). In NashmiC, enums are tagged unions: each variant can optionally carry data.

```
ta3dad Shape {
    Da2ira(fasle64),
    Mustateel(fasle64, fasle64),
    Noqta,
}
```

This declares a `Shape` type with three variants:
- `Da2ira` (circle) - carries one `fasle64` (the radius)
- `Mustateel` (rectangle) - carries two `fasle64` values (width and height)
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

Variants can carry one or more typed values:

```
ta3dad Message {
    Text(nass),
    Number(adad64),
    Pair(nass, adad64),
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
    hale Da2ira(r) => itba3("Circle with radius {r}\n")
    hale Mustateel(w, h) => itba3("Rectangle {w}x{h}\n")
    hale Noqta => itba3("Just a point\n")
}
```

See [Pattern Matching](../advanced/pattern-matching.md) for full details.

## Exhaustiveness

The compiler checks that all variants are covered. If you match on a `ta3dad` and miss a variant without providing a `3adi` (default) arm, the compiler reports an error.

```
// Compiler error: missing variants
hasab color {
    hale Ahmar => itba3("red\n")
}

// Fix: cover all variants or add 3adi
hasab color {
    hale Ahmar => itba3("red\n")
    3adi => itba3("some other color\n")
}
```

## Under the Hood

In the generated C code, enums become tagged unions: a struct with an integer tag field and a union of payloads. This gives you the safety of sum types with the performance of C unions.
