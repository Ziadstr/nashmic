# Optionals

> **Status:** Working. See `examples/yimkin.nsh`.

## No Null in NashmiC

NashmiC has no null. If a value might be absent, you must use `yimkin<T>`.

## `yimkin<T>` - The Optional Type

`yimkin` (يمكن) means "maybe" or "possibly" in Jordanian dialect. `yimkin<T>` either contains a value of type `T` or it doesn't.

```
dalle find_even(n: adad64) -> yimkin<adad64> {
    iza n % 2 == 0 {
        rajje3 fi(n)
    }
    rajje3 mafi
}
```

## `fi` and `mafi`

Two values represent presence or absence:

- `fi(value)` - "there is" - wraps a value (like `Some` in Rust/Swift)
- `mafi` - "there isn't" - no value (like `None`)

These words are core Jordanian. Ask "fi chai?" (is there tea?) and the answer is either "fi" (there is) or "mafi" (there isn't). NashmiC uses the same logic for optionals.

```
dalle first_positive(a: adad64, b: adad64) -> yimkin<adad64> {
    iza a > 0 {
        rajje3 fi(a)
    }
    iza b > 0 {
        rajje3 fi(b)
    }
    rajje3 mafi
}
```

## `iza fi` - Optional Binding

You cannot access the inner value of a `yimkin<T>` without checking first. Use `iza fi` to safely unwrap:

```
iza fi result = find_even(10) {
    itba3("found even: %lld\n", result)
} wala {
    itba3("no even number\n")
}
```

The pattern `iza fi x = expr` checks if the optional contains a value. If so, it binds the inner value to `x` for use inside the block. If the optional is `mafi`, the `wala` branch runs.

## Pattern Matching on Optionals

You can also use `hasab`/`hale`:

```
hasab find_even(7) {
    hale fi(val) => {
        itba3("%lld\n", val)
    }
    hale mafi => {
        itba3("nothing\n")
    }
}
```

See [Pattern Matching](../advanced/pattern-matching.md) for more.

## Full Working Example

From `examples/yimkin.nsh`:

```
dalle find_even(n: adad64) -> yimkin<adad64> {
    iza n % 2 == 0 {
        rajje3 fi(n)
    }
    rajje3 mafi
}

dalle first_positive(a: adad64, b: adad64) -> yimkin<adad64> {
    iza a > 0 {
        rajje3 fi(a)
    }
    iza b > 0 {
        rajje3 fi(b)
    }
    rajje3 mafi
}

yalla() {
    // Pattern matching on optionals
    itba3("find_even(4) = ")
    hasab find_even(4) {
        hale fi(val) => {
            itba3("%lld\n", val)
        }
        hale mafi => {
            itba3("nothing\n")
        }
    }

    itba3("find_even(7) = ")
    hasab find_even(7) {
        hale fi(val) => {
            itba3("%lld\n", val)
        }
        hale mafi => {
            itba3("nothing\n")
        }
    }

    // iza fi binding — unwrap or else
    iza fi result = find_even(10) {
        itba3("found even: %lld\n", result)
    } wala {
        itba3("no even number\n")
    }

    iza fi result = find_even(3) {
        itba3("found even: %lld\n", result)
    } wala {
        itba3("no even number\n")
    }

    // first_positive
    hasab first_positive(-5, 3) {
        hale fi(val) => {
            itba3("first_positive(-5, 3) = %lld\n", val)
        }
        hale mafi => {
            itba3("none\n")
        }
    }
}
```

## Design Rationale

If a value can be absent, the type system must know about it. No surprises, no null pointer exceptions, no "undefined is not a function."

This is the Go/Rust approach, adapted with Jordanian vocabulary that makes the concept intuitive: "fi?" (is there?) "mafi." (there isn't.) "iza fi..." (if there is...).
