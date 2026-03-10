# Enums (Tagged Unions)

> **Status:** Parser complete, codegen in progress. This feature is coming soon.

## Declaring Enums with `ta3dad`

`ta3dad` means "enumeration" or "counting" in Arabic. In NashmiC, enums are tagged unions -- each variant can optionally carry data.

```
ta3dad Shape {
    Da2ira(fasle64),
    Mustateel(fasle64, fasle64),
    Noqta,
}
```

This declares a `Shape` type with three variants:
- `Da2ira` (circle) -- carries one `fasle64` value (the radius)
- `Mustateel` (rectangle) -- carries two `fasle64` values (width and height)
- `Noqta` (point) -- carries no data

## Variants

### Variants without data

Simple marker variants with no payload:

```
ta3dad Color {
    Ahmar,
    Azra2,
    Akhdar,
}
```

### Variants with data

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

## Using Enums

Enums are designed to be used with [pattern matching](../advanced/pattern-matching.md) via `hasab`/`hale`:

```
hasab shape {
    hale Da2ira(r) => itba3("Circle with radius {r}\n")
    hale Mustateel(w, h) => itba3("Rectangle {w}x{h}\n")
    hale Noqta => itba3("Just a point\n")
}
```

## Under the Hood

In the generated C code, enums become tagged unions -- a struct with an integer tag field and a union of payloads. This gives you the safety of sum types with the performance of C unions.

## Why `ta3dad`?

The word `ta3dad` literally means "counting/enumeration." In Franco-Arab, the `3` represents the Arabic letter ain (ع), which has no English equivalent. So `ta3dad` is pronounced "ta-adad" with a guttural ain sound -- if you're not from the region, just say it fast and confident and nobody will notice.
