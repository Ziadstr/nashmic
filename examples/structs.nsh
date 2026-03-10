// structs.nsh — Struct declaration, literals, and member access

haikal Point {
    x: fasle64,
    y: fasle64,
}

dalle add_points(a: Point, b: Point) -> Point {
    khalli result: Point = Point{ x: 0.0, y: 0.0 }
    rajje3 result
}

yalla() {
    khalli p: Point = Point{ x: 3.0, y: 4.0 }
    itba3("Point: (%g, %g)\n", p.x, p.y)

    khalli q: Point = Point{ x: 1.0, y: 2.0 }
    itba3("Other: (%g, %g)\n", q.x, q.y)

    khalli sum: Point = add_points(p, q)
    itba3("Sum: (%g, %g)\n", sum.x, sum.y)
}
