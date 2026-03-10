// Enums (ta3dad) + Pattern Matching (hasab/hale)
// Tagged unions with destructuring — NashmiC style

ta3dad Shape {
    Da2ira(fasle64),
    Mustateel(fasle64),
    Noqta,
}

ta3dad Color {
    Ahmar,
    Azra2,
    Akhdar,
}

dalle describe_shape(s: Shape) {
    hasab s {
        hale Da2ira(radius) => {
            itba3("Da2ira (circle) with radius %g\n", radius)
        }
        hale Mustateel(width) => {
            itba3("Mustateel (rectangle) with width %g\n", width)
        }
        hale Noqta => {
            itba3("Noqta (just a point)\n")
        }
    }
}

dalle color_name(c: Color) {
    hasab c {
        hale Ahmar => {
            itba3("ahmar (red)\n")
        }
        hale Azra2 => {
            itba3("azra2 (blue)\n")
        }
        hale Akhdar => {
            itba3("akhdar (green)\n")
        }
    }
}

yalla() {
    itba3("=== Enums (ta3dad) ===\n\n")

    // Create enum values
    khalli circle: Shape = Da2ira(5.0)
    khalli rect: Shape = Mustateel(3.0)
    khalli point: Shape = Noqta

    // Pattern match on shapes
    describe_shape(circle)
    describe_shape(rect)
    describe_shape(point)

    itba3("\n=== Colors ===\n\n")

    // Simple enums (no payload)
    khalli fav: Color = Ahmar
    color_name(fav)
    color_name(Azra2)
    color_name(Akhdar)
}
