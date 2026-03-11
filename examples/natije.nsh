// natije<T> — Result type with tamam/ghalat + wala? propagation
// NashmiC's signature error handling feature
// EXPECT: === natije<T> — Result Type ===
// EXPECT: 10 / 3 = 3.33333
// EXPECT: division by zero!

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
    itba3("=== natije<T> — Result Type ===\n\n")

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
