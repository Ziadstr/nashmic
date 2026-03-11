// type_mismatch.nsh — should trigger sema warnings for type mismatches
// Expected: warnings for string + int, bool in arithmetic
// ERROR: type mismatch in binary operation

yalla() {
    // Binary op: string + int — should warn
    khalli name: nass = "ziad"
    khalli bad: adad64 = name + 5

    // Bool in arithmetic — should warn
    khalli flag: mante2 = ah
    khalli wrong: adad64 = flag + 1
}
