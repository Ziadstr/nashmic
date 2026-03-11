// riyadiyat.nsh — Standard Library: Math
// Demonstrates all math functions from the riyadiyat module
// EXPECT: === riyadiyat (Math) ===
// EXPECT: jadhr(16) = 4
// EXPECT: qowa(2, 10) = 1024
// EXPECT: mutlaq(-42.5) = 42.5
// EXPECT: jeta(0) = 0
// EXPECT: ardiye(3.7) = 3
// EXPECT: sa2fiye(3.2) = 4
// EXPECT: da2reb(3.5) = 4
// EXPECT: aqall(10, 20) = 10
// EXPECT: akthar(10, 20) = 20

yalla() {
    itba3("=== riyadiyat (Math) ===\n")

    // Square root
    itba3("jadhr(16) = %g\n", jadhr(16.0))

    // Power
    itba3("qowa(2, 10) = %g\n", qowa(2.0, 10.0))

    // Absolute value
    itba3("mutlaq(-42.5) = %g\n", mutlaq(-42.5))

    // Random number
    khalli r = ashwa2i(1, 100)
    itba3("ashwa2i(1, 100) = %lld\n", r)

    // Trig
    itba3("jeta(0) = %g\n", jeta(0.0))
    itba3("jeta_tamam(0) = %g\n", jeta_tamam(0.0))
    itba3("dal(0) = %g\n", dal(0.0))

    // Rounding
    itba3("ardiye(3.7) = %g\n", ardiye(3.7))
    itba3("sa2fiye(3.2) = %g\n", sa2fiye(3.2))
    itba3("da2reb(3.5) = %g\n", da2reb(3.5))

    // Min/Max
    itba3("aqall(10, 20) = %g\n", aqall(10.0, 20.0))
    itba3("akthar(10, 20) = %g\n", akthar(10.0, 20.0))

    // Logarithms
    itba3("log_tabi3i(1) = %g\n", log_tabi3i(1.0))
    itba3("log10(100) = %g\n", log10(100.0))
}
