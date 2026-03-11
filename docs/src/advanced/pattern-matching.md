# Pattern Matching

> **Status:** Working. Pattern matching on enums, results, and optionals all compile and run.

## `hasab` / `hale`

`hasab` (حسب) means "depending on" or "according to." `hale` (حالة) means "case" or "situation." Together, they form NashmiC's pattern matching construct.

```
hasab shape {
    hale Da2ira(r) => {
        itba3("Circle: radius %g\n", r)
    }
    hale Noqta => {
        itba3("Point\n")
    }
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
dalle divide(a: fasle64, b: fasle64) -> natije<fasle64> {
    iza b == 0.0 {
        rajje3 ghalat("ya zalameh, division by zero!")
    }
    rajje3 tamam(a / b)
}

yalla() {
    hasab divide(10.0, 3.0) {
        hale tamam(val) => {
            itba3("%g\n", val)
        }
        hale ghalat(msg) => {
            itba3("Error: %s\n", msg)
        }
    }
}
```

## Matching on Optionals

Match on `yimkin<T>`:

```
dalle find_even(n: adad64) -> yimkin<adad64> {
    iza n % 2 == 0 {
        rajje3 fi(n)
    }
    rajje3 mafi
}

yalla() {
    hasab find_even(7) {
        hale fi(val) => {
            itba3("%lld\n", val)
        }
        hale mafi => {
            itba3("nothing\n")
        }
    }
}
```

## Full Working Example

From `examples/enums.nsh`:

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
    khalli point: Shape = Noqta

    describe_shape(circle)
    describe_shape(rect)
    describe_shape(point)
}
```

## Under the Hood

Pattern matching compiles differently depending on what you're matching:

**Enums:** `hasab` becomes a `switch` on the tag field, with each `hale` arm extracting the payload:
```c
switch (s._tag) {
    case Shape_Da2ira: {
        double radius = s._v.Da2ira._0;
        // arm body
        break;
    }
    case Shape_Noqta: {
        // arm body (no payload)
        break;
    }
}
```

**Results (`natije<T>`):** Matches on `_is_ok` — `hale tamam(val)` checks `_is_ok == 1` and extracts `_v._ok`, while `hale ghalat(msg)` checks `_is_ok == 0` and extracts `_v._err`.

**Optionals (`yimkin<T>`):** Matches on `_has_value` — `hale fi(val)` checks `_has_value == 1` and extracts `_value`, while `hale mafi` checks `_has_value == 0`.

All pattern variables (`radius`, `val`, `msg`) are scoped to their arm body — they don't leak into surrounding code.
