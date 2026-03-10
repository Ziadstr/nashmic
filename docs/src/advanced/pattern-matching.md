# Pattern Matching

> **Status:** Parser complete, codegen in progress. The syntax below is final, but you cannot compile pattern matching programs yet.

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
    Dog(nass, adad64),
    Fish,
}

dalle describe(animal: Animal) {
    hasab animal {
        hale Cat(name) => itba3("{name} is a cat\n")
        hale Dog(name, age) => itba3("{name} is a {age}-year-old dog\n")
        hale Fish => itba3("just a fish\n")
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

## Exhaustiveness Checking

The compiler checks that all variants are covered. Missing a variant without a `3adi` default is a compiler error.

```
ta3dad Direction {
    North,
    South,
    East,
    West,
}

// Compiler error: missing variants East, West
hasab dir {
    hale North => itba3("up\n")
    hale South => itba3("down\n")
}

// Fix: cover all variants or add 3adi
hasab dir {
    hale North => itba3("up\n")
    hale South => itba3("down\n")
    3adi => itba3("sideways\n")
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
    Mustateel(fasle64, fasle64),
    Noqta,
}

dalle area(shape: Shape) -> fasle64 {
    hasab shape {
        hale Da2ira(r) => rajje3 3.14159 * r * r
        hale Mustateel(w, h) => rajje3 w * h
        hale Noqta => rajje3 0.0
    }
}

yalla() {
    khalli circle: Shape = Da2ira(5.0)
    khalli rect: Shape = Mustateel(3.0, 4.0)
    itba3("circle area: {area(circle)}\n")
    itba3("rect area: {area(rect)}\n")
}
```
