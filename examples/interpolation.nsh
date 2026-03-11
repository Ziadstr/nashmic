// interpolation.nsh — String interpolation demo
// EXPECT: marhaba ya Ziad!
// EXPECT: age: 23, next year: 24
// EXPECT: literal {{brace}}

yalla() {
    khalli name: nass = "Ziad"
    khalli age: adad64 = 23

    // Simple interpolation
    itba3("marhaba ya {name}!\n")

    // Expression interpolation
    itba3("age: {age}, next year: {age + 1}\n")

    // Plain strings still work
    itba3("no interpolation here\n")

    // Escaped braces
    itba3("this is a literal {{brace}}\n")
}
