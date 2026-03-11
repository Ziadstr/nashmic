// undeclared.nsh — should trigger sema error for undeclared variable
// Expected: sema reports "undeclared variable 'ghost'"
// ERROR: undeclared variable 'ghost'

yalla() {
    itba3("testing undeclared variable\n")
    khalli x: adad64 = ghost + 1
}
