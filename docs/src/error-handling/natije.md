# Result Type (`natije`)

> **Status:** Working. See `examples/natije.nsh`.

## Errors Are Values

NashmiC has no exceptions. No try/catch. No hidden control flow. Errors are values, and you handle them explicitly.

`natije<T>` (نتيجة, meaning "result" or "outcome") is how NashmiC does error handling. Every function that can fail returns one.

## `tamam` and `ghalat`

A `natije<T>` is one of two things:

- `tamam(value)` - success. "tamam" (تمام) means "all good" or "perfect." Equivalent to Rust's `Ok`.
- `ghalat(message)` - error. "ghalat" (غلط) means "wrong" or "mistake." Equivalent to Rust's `Err`.

```
dalle divide(a: fasle64, b: fasle64) -> natije<fasle64> {
    iza b == 0.0 {
        rajje3 ghalat("ya zalameh, division by zero!")
    }
    rajje3 tamam(a / b)
}
```

## `wala?` - Error Propagation

`wala?` is the error propagation operator. It means "or?" as in "it worked, or?" If the result is `ghalat`, it returns the error up the call chain automatically.

```
dalle safe_math(x: fasle64) -> natije<fasle64> {
    // wala? unwraps tamam or propagates ghalat automatically
    khalli result: fasle64 = divide(x, 2.0) wala?
    rajje3 tamam(result + 1.0)
}
```

How `wala?` works:
- If the expression returns `tamam(value)`, unwrap it and continue with `value`
- If the expression returns `ghalat(err)`, immediately return `ghalat(err)` from the current function

## Pattern Matching on Results

Match on a `natije<T>` to handle both cases explicitly:

```
hasab divide(10.0, 3.0) {
    hale tamam(val) => {
        itba3("%g\n", val)
    }
    hale ghalat(msg) => {
        itba3("Error: %s\n", msg)
    }
}
```

## `faz3a` - Panic

`faz3a` (فزعة) means "panic" or "alarm" (a sudden fright). It terminates the program immediately with an error message.

```
faz3a("something went very wrong")
```

Use `faz3a` only for truly unrecoverable situations: programmer errors, violated invariants, things that should never happen. For expected failures, use `natije<T>`.

## Full Working Example

From `examples/natije.nsh`:

```
dalle divide(a: fasle64, b: fasle64) -> natije<fasle64> {
    iza b == 0.0 {
        rajje3 ghalat("ya zalameh, division by zero!")
    }
    rajje3 tamam(a / b)
}

dalle safe_math(x: fasle64) -> natije<fasle64> {
    // wala? unwraps tamam or propagates ghalat automatically
    khalli result: fasle64 = divide(x, 2.0) wala?
    rajje3 tamam(result + 1.0)
}

yalla() {
    // Direct pattern matching on results
    itba3("10 / 3 = ")
    hasab divide(10.0, 3.0) {
        hale tamam(val) => {
            itba3("%g\n", val)
        }
        hale ghalat(msg) => {
            itba3("Error: %s\n", msg)
        }
    }

    itba3("10 / 0 = ")
    hasab divide(10.0, 0.0) {
        hale tamam(val) => {
            itba3("%g\n", val)
        }
        hale ghalat(msg) => {
            itba3("Error: %s\n", msg)
        }
    }

    // wala? propagation
    itba3("\nsafe_math(8) = ")
    hasab safe_math(8.0) {
        hale tamam(val) => {
            itba3("%g\n", val)
        }
        hale ghalat(msg) => {
            itba3("Error: %s\n", msg)
        }
    }
}
```

## Design Rationale

NashmiC's error model draws from Go and Rust:
- Like Go: errors are values, not exceptions. No hidden control flow.
- Like Rust: errors are typed and mandatory to handle. The compiler enforces it.
- Unlike both: the syntax is Jordanian. `tamam`/`ghalat` feels natural. "Was it tamam?" "La, ghalat."
