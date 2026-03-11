// bad_struct_lit.nsh — should trigger sema warnings for bad struct literal fields
// Expected: warning for non-existent field 'z' and wrong type for 'x'
// ERROR: has no field 'z'

haikal Point {
    x: fasle64,
    y: fasle64,
}

yalla() {
    // Non-existent field in literal — should warn
    khalli p: Point = Point{ x: 1.0, z: 3.0 }

    // Wrong type for field — should warn
    khalli q: Point = Point{ x: "not a number", y: 2.0 }
}
