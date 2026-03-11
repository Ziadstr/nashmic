// malafat.nsh — Standard Library: File I/O
// Demonstrates file operations
// EXPECT: === malafat (Files) ===
// EXPECT: uktub: 1
// EXPECT: mawjood: 1
// EXPECT: i2ra_kol: marhaba ya NashmiC!
// EXPECT: dahef: 1
// EXPECT: imsah: 1
// EXPECT: mawjood ba3d imsah: 0

yalla() {
    itba3("=== malafat (Files) ===\n")

    // Write a file
    khalli result = uktub_malaf("/tmp/nsh_test.txt", "marhaba ya NashmiC!")
    itba3("uktub: %d\n", result)

    // Check existence
    itba3("mawjood: %d\n", mawjood("/tmp/nsh_test.txt"))

    // Read it back
    khalli content: nass = i2ra_kol("/tmp/nsh_test.txt")
    itba3("i2ra_kol: %s\n", content)

    // Get file size
    itba3("hajm: %lld\n", hajm_malaf("/tmp/nsh_test.txt"))

    // Append to file
    khalli append_result = dahef_malaf("/tmp/nsh_test.txt", "\nmore content")
    itba3("dahef: %d\n", append_result)

    // Read after append
    khalli updated: nass = i2ra_kol("/tmp/nsh_test.txt")
    itba3("ba3d dahef: %s\n", updated)

    // Delete file
    khalli deleted = imsah_malaf("/tmp/nsh_test.txt")
    itba3("imsah: %d\n", deleted)

    // Verify deletion
    itba3("mawjood ba3d imsah: %d\n", mawjood("/tmp/nsh_test.txt"))
}
