# Defer (`ba3dain`)

> **Status:** Parser complete, codegen partially implemented. This feature is coming soon.

## What is Defer?

`ba3dain` (بعدين) means "later" in Jordanian dialect. It's the word you say when someone asks you to do something and you'll get to it... eventually. In NashmiC, `ba3dain` schedules a statement to run when the current scope exits. Procrastination as a language feature.

```
khalli f = iftah("data.txt")
ba3dain sakker(f)
// ... use f ...
// sakker(f) runs automatically when scope exits
```

## Execution Order

Multiple defers execute in LIFO (last-in, first-out) order:

```
dalle example() {
    itba3("1\n")
    ba3dain itba3("deferred 1\n")
    itba3("2\n")
    ba3dain itba3("deferred 2\n")
    itba3("3\n")
}
```

Output:

```
1
2
3
deferred 2
deferred 1
```

The last `ba3dain` registered runs first. Stack order, just like Go's `defer`.

## Use Cases

### Resource cleanup

The primary use case. Open a resource, immediately defer its cleanup, then use it freely:

```
dalle process_file(path: nass) -> natije<nass> {
    khalli f = iftah(path) wala?
    ba3dain sakker(f)

    khalli data = i2ra_kol(f) wala?
    // ... process data ...
    rajje3 tamam(result)
    // sakker(f) runs here, whether tamam or ghalat
}
```

You declare cleanup right next to acquisition. The compiler guarantees it runs regardless of how the scope exits.

### Logging and tracing

```
dalle handle_request(req: Request) {
    itba3("request started\n")
    ba3dain itba3("request finished\n")
    // ... handle the request ...
}
```

## Design Rationale

`ba3dain` is directly inspired by Go's `defer`. The concept maps perfectly to Jordanian culture: "ba3dain" is probably the most-used word in Jordan. "I'll clean my room ba3dain." "I'll fix that bug ba3dain." Now it's a language feature. Unlike real life, `ba3dain` in NashmiC actually keeps its promise.
