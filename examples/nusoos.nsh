// nusoos.nsh — Standard Library: Strings
// Demonstrates string manipulation functions
// EXPECT: === nusoos (Strings) ===
// EXPECT: toul("marhaba") = 7
// EXPECT: a3la: NASHMIC
// EXPECT: asfal: nashmic
// EXPECT: yihtawi: 1
// EXPECT: bdaya: 1
// EXPECT: nihaya: 1
// EXPECT: badel: marhaba ya NashmiC!
// EXPECT: karrer: hahaha
// EXPECT: qass_haddi: nash

yalla() {
    itba3("=== nusoos (Strings) ===\n")

    // Length
    itba3("toul(\"marhaba\") = %lld\n", toul("marhaba"))

    // Case conversion
    itba3("a3la: %s\n", a3la("nashmic"))
    itba3("asfal: %s\n", asfal("NASHMIC"))

    // Search
    itba3("yihtawi: %d\n", yihtawi("hello nashmic", "nashmic"))
    itba3("bdaya: %d\n", bdaya("nashmic rocks", "nashmic"))
    itba3("nihaya: %d\n", nihaya("hello.nsh", ".nsh"))

    // Replace
    itba3("badel: %s\n", badel("marhaba ya world!", "world", "NashmiC"))

    // Repeat
    itba3("karrer: %s\n", karrer("ha", 3))

    // Substring
    itba3("qass_haddi: %s\n", qass_haddi("nashmic", 0, 4))

    // Char at index
    itba3("harf_3ind: %c\n", harf_3ind("nashmic", 0))

    // Slice
    itba3("juz2: %s\n", juz2("nashmic", 4, 7))
}
