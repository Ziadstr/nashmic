# Defer (`ba3dain`)

> **Status:** Working. See `examples/defer.nsh`.

## What is Defer?

`ba3dain` (بعدين) means "later" in Jordanian dialect. It's the word you say when someone asks you to do something and you'll get to it... eventually. In NashmiC, `ba3dain` schedules a block to run when the current scope exits. Procrastination as a language feature.

```
dalle greet() {
    itba3("start\n")
    ba3dain {
        itba3("cleanup 1\n")
    }
    itba3("middle\n")
    rajje3
}
```

When `greet()` returns, the deferred block runs after "middle" prints.

## Execution Order

Multiple defers execute in LIFO (last-in, first-out) order:

```
dalle greet() {
    itba3("start\n")
    ba3dain {
        itba3("cleanup 1\n")
    }
    ba3dain {
        itba3("cleanup 2\n")
    }
    itba3("middle\n")
    rajje3
}
```

Output:

```
start
middle
cleanup 2
cleanup 1
```

The last `ba3dain` registered runs first. Stack order, just like Go's `defer`.

## With Return Values

`ba3dain` works in functions that return values too. The deferred code runs after the return value is computed:

```
dalle compute(x: adad64) -> adad64 {
    ba3dain {
        itba3("  compute done\n")
    }
    rajje3 x * 2
}

yalla() {
    khalli result: adad64 = compute(21)
    itba3("  result = %lld\n", result)
}
```

Output:

```
  compute done
  result = 42
```

## Full Working Example

From `examples/defer.nsh`:

```
dalle greet() {
    itba3("start\n")
    ba3dain {
        itba3("cleanup 1\n")
    }
    ba3dain {
        itba3("cleanup 2\n")
    }
    itba3("middle\n")
    rajje3
}

dalle compute(x: adad64) -> adad64 {
    ba3dain {
        itba3("  compute done\n")
    }
    rajje3 x * 2
}

yalla() {
    itba3("--- void function ---\n")
    greet()

    itba3("\n--- function with return value ---\n")
    khalli result: adad64 = compute(21)
    itba3("  result = %lld\n", result)
}
```

## Use Cases

### Resource cleanup

The primary use case. Open a resource, immediately defer its cleanup, then use it freely:

```
dalle process_file(path: nass) -> natije<nass> {
    khalli f = iftah(path) wala?
    ba3dain sakker(f)

    khalli data = i2ra_kol(f) wala?
    rajje3 tamam(result)
    // sakker(f) runs here, whether tamam or ghalat
}
```

You declare cleanup right next to acquisition. The compiler guarantees it runs regardless of how the scope exits.

### Logging and tracing

```
dalle handle_request(req: Request) {
    itba3("request started\n")
    ba3dain {
        itba3("request finished\n")
    }
    // ... handle the request ...
}
```

## Design Rationale

`ba3dain` is directly inspired by Go's `defer`. The concept maps perfectly to Jordanian culture: "ba3dain" is probably the most-used word in Jordan. "I'll clean my room ba3dain." "I'll fix that bug ba3dain." Now it's a language feature. Unlike real life, `ba3dain` in NashmiC actually keeps its promise.
