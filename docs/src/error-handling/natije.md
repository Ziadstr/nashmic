# Result Type (`natije`)

> **Status:** Parser complete, codegen in progress. The syntax below is final, but you cannot compile natije programs yet.

## Errors Are Values

NashmiC has no exceptions. No try/catch. No hidden control flow. Errors are values, and you handle them explicitly.

`natije<T>` (نتيجة, meaning "result" or "outcome") is how NashmiC does error handling. Every function that can fail returns one.

## `tamam` and `ghalat`

A `natije<T>` is one of two things:

- `tamam(value)` - success. "tamam" (تمام) means "all good" or "perfect." Equivalent to Rust's `Ok`.
- `ghalat(message)` - error. "ghalat" (غلط) means "wrong" or "mistake." Equivalent to Rust's `Err`.

```
dalle parse_number(input: nass) -> natije<adad64> {
    // ... parsing logic ...
    iza success {
        rajje3 tamam(parsed_value)
    }
    rajje3 ghalat("مو رقم يا زلمة")  // "that's not a number, man"
}
```

## `wala?` - Error Propagation

`wala?` is the error propagation operator. It means "or?" as in "it worked, or?" If the result is `ghalat`, it returns the error up the call chain automatically.

```
dalle read_config(path: nass) -> natije<nass> {
    khalli f = iftah(path) wala? "ما قدرت افتح الملف"
    khalli content = i2ra_kol(f) wala?
    rajje3 tamam(content)
}
```

How `wala?` works:
- If the expression returns `tamam(value)`, unwrap it and continue with `value`
- If the expression returns `ghalat(err)`, immediately return `ghalat(err)` from the current function

### With context

Add a message to wrap propagated errors:

```
khalli data = read_file(path) wala? "failed to read config"
```

If `read_file` returns `ghalat`, the error gets wrapped with the additional context before propagating.

### Without context

The original error propagates unchanged:

```
khalli data = read_file(path) wala?
```

## Mandatory Handling

Ignoring a `natije` is a compiler error, not a warning. If a function returns `natije<T>`, you must either:

1. Use `wala?` to propagate the error
2. Handle it with `hasab` (pattern matching)
3. Explicitly unwrap it

There is no "just ignore the error" path. This is deliberate.

## Pattern Matching on Results

Match on a `natije<T>` to handle both cases explicitly:

```
hasab parse_number(input) {
    hale tamam(n) => itba3("parsed: {n}\n")
    hale ghalat(msg) => itba3("error: {msg}\n")
}
```

See [Pattern Matching](../advanced/pattern-matching.md) for more details.

## `faz3a` - Panic

`faz3a` (فزعة) means "panic" or "alarm" (a sudden fright). It terminates the program immediately with an error message.

```
faz3a("something went very wrong")
```

Use `faz3a` only for truly unrecoverable situations: programmer errors, violated invariants, things that should never happen. For expected failures, use `natije<T>`.

## Design Rationale

NashmiC's error model draws from Go and Rust:
- Like Go: errors are values, not exceptions. No hidden control flow.
- Like Rust: errors are typed and mandatory to handle. The compiler enforces it.
- Unlike both: the syntax is Jordanian. `tamam`/`ghalat` feels natural. "Was it tamam?" "La, ghalat."
