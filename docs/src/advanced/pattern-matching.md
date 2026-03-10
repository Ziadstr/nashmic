# Pattern Matching

> **Status:** Working. Pattern matching on enums compiles and runs.

## `hasab` / `hale`

`hasab` (حسب) means "depending on" or "according to." `hale` (حالة) means "case" or "situation." Together, they form NashmiC's pattern matching construct.

```
hasab shape {
    hale Da2ira(r) => itba3("Circle: radius {r}\n")
    hale Mustateel(w, h) => itba3("Rect: {w}x{h}\n")
    hale Noqta => itba3("Point\n")
}
```

## Matching on Enums

The primary use of `hasab` is matching on `ta3dad` (enum) values with destructuring:

```
ta3dad Animal {
    Cat(nass),
    Dog(nass),
    Fish,
}

dalle describe(animal: Animal) {
    hasab animal {
        hale Cat(name) => {
            itba3("{name} is a cat\n")
        }
        hale Dog(name) => {
            itba3("{name} is a dog\n")
        }
        hale Fish => {
            itba3("just a fish\n")
        }
    }
}
```

Each `hale` arm destructures the variant, binding its payload to local variables available in the arm body.

## The Default Case: `3adi`

`3adi` (عادي) means "whatever" or "normal." It's the Jordanian catch-all response and serves as the default arm that matches anything not covered by previous arms.

```
hasab value {
    hale 1 => itba3("one\n")
    hale 2 => itba3("two\n")
    3adi => itba3("something else\n")
}
```

## Matching on Results

Pattern matching works with `natije<T>` for explicit error handling:

```
hasab parse_number(input) {
    hale tamam(n) => itba3("got: {n}\n")
    hale ghalat(msg) => itba3("error: {msg}\n")
}
```

## Matching on Optionals

Match on `yimkin<T>`:

```
hasab find_user(42) {
    hale fi(user) => itba3("found: {user.name}\n")
    hale mafi => itba3("not found\n")
}
```

## Full Example

```
ta3dad Shape {
    Da2ira(fasle64),
    Mustateel(fasle64),
    Noqta,
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

yalla() {
    khalli circle: Shape = Da2ira(5.0)
    khalli rect: Shape = Mustateel(3.0)
    describe_shape(circle)
    describe_shape(rect)
}
```
