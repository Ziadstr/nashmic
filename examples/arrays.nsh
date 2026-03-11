// arrays.nsh — saff<T> array type demo
// Demonstrates: array literals, indexing, iteration, push, length, string concat
// EXPECT: === saff (Arrays) ===
// EXPECT: toul: 5
// EXPECT: marhaba ya NashmiC!

dalle print_array(nums: saff<adad64>) {
    itba3("[ ")
    lakol n fi nums {
        itba3("%lld ", n)
    }
    itba3("]\n")
}

yalla() {
    // Array literal
    khalli a3dad: saff<adad64> = [10, 20, 30, 40, 50]

    itba3("=== saff (Arrays) ===\n")

    // Length
    itba3("toul: %lld\n", a3dad.toul())

    // Index access
    itba3("a3dad[0] = %lld\n", a3dad[0])
    itba3("a3dad[2] = %lld\n", a3dad[2])

    // Iteration with lakol
    itba3("kol al-a3dad: ")
    print_array(a3dad)

    // Push element
    a3dad.zeed(60)
    a3dad.zeed(70)
    itba3("ba3d al-ziyade: ")
    print_array(a3dad)
    itba3("toul jadeed: %lld\n", a3dad.toul())

    // Float array
    khalli drajat: saff<fasle64> = [95.5, 87.3, 92.1]
    itba3("\ndrajat: [ ")
    lakol d fi drajat {
        itba3("%g ", d)
    }
    itba3("]\n")

    // String concatenation
    khalli ism = "NashmiC"
    khalli tahiyye = "marhaba ya " + ism + "!"
    itba3("\n%s\n", tahiyye)

    // String length
    khalli toul_ism = toul(ism)
    itba3("toul '%s' = %lld\n", ism, toul_ism)

    // Empty array with push
    khalli farigh: saff<adad64> = []
    farigh.zeed(1)
    farigh.zeed(2)
    farigh.zeed(3)
    itba3("\nfarigh ba3d al-ziyade: ")
    print_array(farigh)
}
