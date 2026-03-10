# Optionals

> **Status:** Working. See `examples/yimkin.nsh`.

## No Null in NashmiC

NashmiC has no null. If a value might be absent, you must use `yimkin<T>`.

## `yimkin<T>` - The Optional Type

`yimkin` (يمكن) means "maybe" or "possibly" in Jordanian dialect. `yimkin<T>` either contains a value of type `T` or it doesn't.

```
dalle find_user(id: adad) -> yimkin<User> {
    iza id == 0 { rajje3 mafi }
    rajje3 fi(users[id])
}
```

## `fi` and `mafi`

Two values represent presence or absence:

- `fi(value)` - "there is" - wraps a value (like `Some` in Rust/Swift)
- `mafi` - "there isn't" - no value (like `None`)

These words are core Jordanian. Ask "fi chai?" (is there tea?) and the answer is either "fi" (there is) or "mafi" (there isn't). NashmiC uses the same logic for optionals.

```
khalli found: yimkin<nass> = fi("Ziad")
khalli missing: yimkin<nass> = mafi
```

## `iza fi` - Optional Binding

You cannot access the inner value of a `yimkin<T>` without checking first. Using `.field` on an optional without unwrapping is a compiler error.

Use `iza fi` to safely unwrap:

```
iza fi user = find_user(42) {
    itba3("found: {user.name}\n")
} wala {
    itba3("user not found\n")
}
```

The pattern `iza fi x = expr` checks if the optional contains a value. If so, it binds the inner value to `x` for use inside the block. If the optional is `mafi`, the `wala` branch runs.

## Pattern Matching on Optionals

You can also use `hasab`/`hale`:

```
hasab find_user(42) {
    hale fi(user) => itba3("found: {user.name}\n")
    hale mafi => itba3("not found\n")
}
```

See [Pattern Matching](../advanced/pattern-matching.md) for more.

## Design Rationale

If a value can be absent, the type system must know about it. No surprises, no null pointer exceptions, no "undefined is not a function."

This is the Go/Rust approach, adapted with Jordanian vocabulary that makes the concept intuitive: "fi?" (is there?) "mafi." (there isn't.) "iza fi..." (if there is...).
