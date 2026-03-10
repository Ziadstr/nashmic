# Optionals

> **Status:** Parser complete, codegen in progress. This feature is coming soon.

## No Null in NashmiC

NashmiC has no null. The billion-dollar mistake does not exist here. If a value might be absent, you must use `yimkin<T>`.

## `yimkin<T>` -- The Optional Type

`yimkin` means "maybe" or "possibly" in Jordanian dialect. `yimkin<T>` is NashmiC's optional type -- it either contains a value of type `T` or it doesn't.

```
dalle find_user(id: adad) -> yimkin<User> {
    iza id == 0 { rajje3 mafi }
    rajje3 fi(users[id])
}
```

## `fi` and `mafi`

Two values represent the presence or absence of data:

- `fi(value)` -- "there is" -- wraps a value (equivalent to `Some` in Rust/Swift)
- `mafi` -- "there isn't" -- no value (equivalent to `None`)

These words are deeply Jordanian. Ask "fi chai?" (is there tea?) and the answer is either "fi" (there is) or "mafi" (there isn't). NashmiC uses the same logic for optionals.

```
khalli found: yimkin<nass> = fi("Ziad")
khalli missing: yimkin<nass> = mafi
```

## `iza fi` -- Optional Binding

You cannot access the inner value of a `yimkin<T>` without checking first. Attempting to use `.field` on an optional without unwrapping is a compiler error.

Use `iza fi` to safely unwrap:

```
iza fi user = find_user(42) {
    itba3("found: {user.name}\n")
} wala {
    itba3("user not found\n")
}
```

The pattern `iza fi x = expr` checks if the optional contains a value and, if so, binds it to `x` for use inside the block. If the optional is `mafi`, the `wala` branch runs.

## Design Rationale

NashmiC takes a strong stance: if a value can be absent, the type system must know about it. No surprises, no null pointer exceptions, no "undefined is not a function."

This is the Go/Rust approach, adapted with Jordanian words that make the concept intuitive: "fi?" (is there?) "mafi." (there isn't.) "iza fi..." (if there is...).
