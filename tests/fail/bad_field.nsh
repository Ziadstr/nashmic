// bad_field.nsh — should trigger sema warnings for invalid struct fields
// Expected: warning for accessing non-existent field 'z' on Point
// ERROR: has no field 'z'

haikal Point {
    x: fasle64,
    y: fasle64,
}

yalla() {
    khalli p: Point = Point{ x: 1.0, y: 2.0 }

    // Valid field access — no warning
    itba3("%g\n", p.x)

    // Invalid field access — should warn
    itba3("%g\n", p.z)
}
