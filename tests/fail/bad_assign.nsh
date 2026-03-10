// bad_assign.nsh — should trigger sema warnings for assignment type mismatches
// Expected: warning for assigning string to adad64 variable

yalla() {
    khalli x: adad64 = 42

    // Wrong type assignment — should warn
    x = "hello"

    // Numeric assignment (int to int) — no warning
    x = 99
}
