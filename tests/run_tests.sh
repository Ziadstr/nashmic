#!/usr/bin/env bash
# NashmiC Test Runner
# Compiles and runs each example, comparing output against expected

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
MANSAF="$ROOT_DIR/build/mansaf"
EXPECTED_DIR="$SCRIPT_DIR/expected"

# Examples that have non-deterministic output (random proverbs, etc.)
# These are tested for successful compilation+execution only, not output matching
SKIP_OUTPUT_CHECK="easter_eggs"

passed=0
failed=0
errors=""

for expected_file in "$EXPECTED_DIR"/*.txt; do
    name=$(basename "$expected_file" .txt)
    example="$ROOT_DIR/examples/${name}.nsh"

    if [ ! -f "$example" ]; then
        echo "SKIP  $name (no example file)"
        continue
    fi

    # Run the example
    actual=$(NASHMIC_ROOT="$ROOT_DIR" "$MANSAF" run "$example" 2>&1) || {
        echo "FAIL  $name (runtime error)"
        errors="$errors\n  $name: runtime error"
        failed=$((failed + 1))
        continue
    }

    # Check if this example should skip output comparison
    if echo "$SKIP_OUTPUT_CHECK" | grep -qw "$name"; then
        echo "PASS  $name (compile+run only)"
        passed=$((passed + 1))
        continue
    fi

    # Compare output
    expected=$(cat "$expected_file")
    if [ "$actual" = "$expected" ]; then
        echo "PASS  $name"
        passed=$((passed + 1))
    else
        echo "FAIL  $name (output mismatch)"
        errors="$errors\n  $name: output mismatch"
        echo "    --- expected ---"
        echo "$expected" | head -5
        echo "    --- actual ---"
        echo "$actual" | head -5
        echo "    ---"
        failed=$((failed + 1))
    fi
done

# Also check that all examples in examples/ have expected output files
for example in "$ROOT_DIR"/examples/*.nsh; do
    name=$(basename "$example" .nsh)
    if [ ! -f "$EXPECTED_DIR/${name}.txt" ]; then
        echo "WARN  $name has no expected output (add tests/expected/${name}.txt)"
    fi
done

echo ""
echo "Results: $passed passed, $failed failed"

if [ $failed -gt 0 ]; then
    echo -e "Failures:$errors"
    exit 1
fi
