// fibonacci.nsh — Fibonacci sequence in NashmiC

dalle fibonacci(n: adad64) -> adad64 {
    iza n <= 1 {
        rajje3 n
    }
    rajje3 fibonacci(n - 1) + fibonacci(n - 2)
}

yalla() {
    khalli i: adad64 = 0
    tool_ma i < 15 {
        itba3("fib(%lld) = %lld\n", i, fibonacci(i))
        i = i + 1
    }
}
