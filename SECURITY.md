# Security Policy

## Reporting a Vulnerability

If you discover a security vulnerability in the NashmiC compiler or runtime, please report it responsibly.

**Email:** ziad1sharif@gmail.com

**Please include:**
- Description of the vulnerability
- Steps to reproduce
- Impact assessment
- Suggested fix (if you have one)

**Do NOT:**
- Open a public issue for security vulnerabilities
- Exploit the vulnerability beyond what's needed to demonstrate it

## Response

- Acknowledgment within 48 hours
- Fix timeline communicated within 1 week
- Credit given in release notes (unless you prefer anonymity)

## Scope

This policy covers:
- The `mansaf` compiler (`compiler/src/`)
- The NashmiC runtime (`runtime/`)
- The install script (`install.sh`)
- The documentation site (`docs/site/`)

## Known Considerations

NashmiC transpiles to C and invokes a system C compiler. The generated C code is not sandboxed. NashmiC programs have the same capabilities as any native C program. This is by design, not a vulnerability.
