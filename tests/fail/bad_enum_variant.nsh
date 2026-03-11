// bad_enum_variant.nsh — should trigger sema warnings for enum variant misuse
// Expected: warning for wrong payload type on Da2ira
// ERROR: takes no payload

ta3dad Shape {
    Da2ira(fasle64),
    Noqta,
}

yalla() {
    // Correct usage — no warning
    khalli c: Shape = Da2ira(5.0)

    // Wrong payload type — should warn
    khalli bad: Shape = Da2ira("not a number")

    // Payload on payloadless variant — should warn
    khalli worse: Shape = Noqta(42.0)
}
