# Pattern Matching

> **Status:** Parser complete, codegen in progress. This feature is coming soon.

## `hasab` / `hale` -- Match Expressions

`hasab` means "depending on" or "according to" in Jordanian dialect. `hale` means "case" or "situation." Together, they form NashmiC's pattern matching construct.

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

Each `hale` arm destructures the variant, binding its payload to local variables that are available in the arm body.

## The Default Case: `3adi`

`3adi` means "whatever" or "normal" -- the Jordanian catch-all response. It's the default arm that matches anything not covered by previous arms.

```
hasab value {
    hale 1 => itba3("one\n")
    hale 2 => itba3("two\n")
    3adi => itba3("something else\n")
}
```

## Exhaustiveness Checking

The compiler checks that all variants of an enum are covered. If you match on a `ta3dad` and miss a variant without providing a `3adi` default, the compiler reports an error.

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

// Fix: add remaining variants or use 3adi
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

Similarly, match on `yimkin<T>`:

```
hasab find_user(42) {
    hale fi(user) => itba3("found: {user.name}\n")
    hale mafi => itba3("not found\n")
}
```

## Why `hasab`?

In Jordan, "hasab" is how you start a conditional explanation: "hasab il-mawqef" means "depending on the situation." It maps directly to pattern matching -- your code does different things depending on the shape of the data. And `hale` (case/situation) is what follows naturally: "in the case that..."
