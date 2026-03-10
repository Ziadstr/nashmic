// ba3dain — Defer (run code at end of scope)
// Like Go's defer, but Jordanian

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
    itba3("=== ba3dain — Defer ===\n\n")

    itba3("--- void function ---\n")
    greet()
    itba3("\n")

    itba3("--- function with return value ---\n")
    khalli result: adad64 = compute(21)
    itba3("  result = %lld\n", result)
}
