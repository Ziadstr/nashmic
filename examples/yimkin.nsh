// yimkin<T> — Optional type with fi/mafi + iza fi binding
// NashmiC's null-safe value handling
// EXPECT: === yimkin<T> — Optional Type ===
// EXPECT: find_even(4) = 4
// EXPECT: first_positive(-5, 3) = 3

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
    itba3("=== yimkin<T> — Optional Type ===\n\n")

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
    itba3("\niza fi binding:\n")
    iza fi result = find_even(10) {
        itba3("  found even: %lld\n", result)
    } wala {
        itba3("  no even number\n")
    }

    iza fi result = find_even(3) {
        itba3("  found even: %lld\n", result)
    } wala {
        itba3("  no even number\n")
    }

    // first_positive
    itba3("\nfirst_positive(-5, 3) = ")
    hasab first_positive(-5, 3) {
        hale fi(val) => {
            itba3("%lld\n", val)
        }
        hale mafi => {
            itba3("none\n")
        }
    }

    itba3("first_positive(-1, -2) = ")
    hasab first_positive(-1, -2) {
        hale fi(val) => {
            itba3("%lld\n", val)
        }
        hale mafi => {
            itba3("none\n")
        }
    }
}
