// bad_method.nsh — should trigger sema warnings for invalid method calls
// Expected: warning for calling non-existent method 'fly' on Point
// ERROR: has no method 'fly'

haikal Point {
    x: fasle64,
    y: fasle64,
}

tabbe2 Point {
    dalle display(had) {
        itba3("(%g, %g)", had.x, had.y)
    }
}

yalla() {
    khalli p: Point = Point{ x: 1.0, y: 2.0 }

    // Valid method — no warning
    p.display()

    // Invalid method — should warn
    p.fly()
}
