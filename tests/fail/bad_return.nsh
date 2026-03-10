// bad_return.nsh — should trigger sema warnings for return type mismatches
// Expected: warning for returning string from adad64 function

dalle get_number() -> adad64 {
    rajje3 "not a number"
}

dalle get_nothing() {
    // returning a value from void function — should warn
    rajje3 42
}

yalla() {
    khalli n: adad64 = get_number()
    itba3("%lld\n", n)
}
