// tabbe2 — Impl blocks (methods on structs)
// NashmiC's approach to object-oriented behavior
// EXPECT: === tabbe2 — Methods ===
// EXPECT: Distance from origin: 5
// EXPECT: Counter is at 42

haikal Point {
    x: fasle64,
    y: fasle64
}

tabbe2 Point {
    dalle distance(had) -> fasle64 {
        rajje3 sqrt(had.x * had.x + had.y * had.y)
    }

    dalle translate(had, dx: fasle64, dy: fasle64) -> Point {
        rajje3 Point{ x: had.x + dx, y: had.y + dy }
    }

    dalle display(had) {
        itba3("(%g, %g)", had.x, had.y)
    }
}

haikal Counter {
    count: adad64
}

tabbe2 Counter {
    dalle value(had) -> adad64 {
        rajje3 had.count
    }

    dalle describe(had) {
        itba3("Counter is at %lld", had.count)
    }
}

yalla() {
    itba3("=== tabbe2 — Methods ===\n\n")

    khalli p: Point = Point{ x: 3.0, y: 4.0 }
    itba3("Point: ")
    p.display()
    itba3("\n")

    khalli dist: fasle64 = p.distance()
    itba3("Distance from origin: %g\n", dist)

    khalli moved: Point = p.translate(1.0, -1.0)
    itba3("After translate(1, -1): ")
    moved.display()
    itba3("\n")

    khalli c: Counter = Counter{ count: 42 }
    c.describe()
    itba3("\n")
    itba3("Value: %lld\n", c.value())
}
