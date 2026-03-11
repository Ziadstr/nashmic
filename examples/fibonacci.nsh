// fibonacci.nsh — Fibonacci sequence in NashmiC
// Uses ranges and string interpolation
// EXPECT: fib(0) = 0
// EXPECT: fib(10) = 55
// EXPECT: fib(14) = 377

dalle fibonacci(n: adad64) -> adad64 {
    iza n <= 1 {
        rajje3 n
    }
    rajje3 fibonacci(n - 1) + fibonacci(n - 2)
}

yalla() {
    lakol i fi 0..15 {
        itba3("fib({i}) = {fibonacci(i)}\n")
    }
}
