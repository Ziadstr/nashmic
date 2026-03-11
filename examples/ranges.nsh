// ranges.nsh — Fixed range iteration with lakol..fi
// EXPECT: Counting 0 to 9:
// EXPECT: Fibonacci with ranges:
// EXPECT: fib(14) = 377

yalla() {
    itba3("Counting 0 to 9:\n")
    lakol i fi 0..10 {
        itba3("%lld ", i)
    }
    itba3("\n")

    itba3("Counting 5 to 14:\n")
    lakol j fi 5..15 {
        itba3("%lld ", j)
    }
    itba3("\n")

    // Fibonacci with ranges
    itba3("\nFibonacci with ranges:\n")
    khalli a: adad64 = 0
    khalli b: adad64 = 1
    lakol i fi 0..15 {
        itba3("fib(%lld) = %lld\n", i, a)
        khalli temp: adad64 = a + b
        a = b
        b = temp
    }
}
