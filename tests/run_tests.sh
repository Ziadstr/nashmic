#!/usr/bin/env bash
# NashmiC Test Runner
# Compiles and runs each example, comparing output against expected
#
# Usage:
#   ./run_tests.sh              Run all tests
#   ./run_tests.sh -v           Verbose mode (show diffs on failure)
#   ./run_tests.sh arrays       Run only tests matching "arrays"
#   ./run_tests.sh -v structs   Verbose + filter

set -uo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
MANSAF="$ROOT_DIR/build/mansaf"
EXPECTED_DIR="$SCRIPT_DIR/expected"
FAIL_DIR="$SCRIPT_DIR/fail"

# Examples that have non-deterministic output (random proverbs, etc.)
# These are tested for successful compilation+execution only, not output matching
SKIP_OUTPUT_CHECK="easter_eggs"

# ── Color output ──────────────────────────────────────────────
if [ -t 1 ]; then
    GREEN="\033[1;32m"
    RED="\033[1;31m"
    YELLOW="\033[1;33m"
    CYAN="\033[0;36m"
    BOLD="\033[1m"
    DIM="\033[2m"
    RESET="\033[0m"
else
    GREEN=""
    RED=""
    YELLOW=""
    CYAN=""
    BOLD=""
    DIM=""
    RESET=""
fi

# ── Parse arguments ──────────────────────────────────────────
VERBOSE=0
FILTER=""

while [ $# -gt 0 ]; do
    case "$1" in
        -v|--verbose)
            VERBOSE=1
            ;;
        -h|--help)
            echo "Usage: $0 [-v|--verbose] [filter]"
            echo ""
            echo "Options:"
            echo "  -v, --verbose    Show actual vs expected output on failure"
            echo "  filter           Run only tests whose name contains this string"
            exit 0
            ;;
        *)
            FILTER="$1"
            ;;
    esac
    shift
done

# ── Counters ─────────────────────────────────────────────────
passed=0
failed=0
skipped=0
start_time=$(date +%s)

# ── Helper: check EXPECT comments ───────────────────────────
# Extracts "// EXPECT: <text>" lines from a .nsh file and verifies
# each string appears in the actual output.
check_expect_comments() {
    local nsh_file="$1"
    local actual_output="$2"
    local test_name="$3"
    local expect_failures=0

    while IFS= read -r line; do
        # Strip the "// EXPECT: " prefix
        expected_str="${line#*// EXPECT: }"
        if ! echo "$actual_output" | grep -qF "$expected_str"; then
            if [ $VERBOSE -eq 1 ]; then
                printf "    ${RED}EXPECT not found${RESET}: %s\n" "$expected_str"
            fi
            expect_failures=$((expect_failures + 1))
        fi
    done < <(grep '^// EXPECT: ' "$nsh_file" 2>/dev/null)

    return $expect_failures
}

# ── Helper: check ERROR comments ────────────────────────────
# Extracts "// ERROR: <text>" lines from a .nsh file and verifies
# each string appears in stderr output.
check_error_comments() {
    local nsh_file="$1"
    local stderr_output="$2"
    local test_name="$3"
    local error_failures=0

    while IFS= read -r line; do
        expected_str="${line#*// ERROR: }"
        if ! echo "$stderr_output" | grep -qF "$expected_str"; then
            if [ $VERBOSE -eq 1 ]; then
                printf "    ${RED}ERROR not found${RESET}: %s\n" "$expected_str"
            fi
            error_failures=$((error_failures + 1))
        fi
    done < <(grep '^// ERROR: ' "$nsh_file" 2>/dev/null)

    return $error_failures
}

# ── Pass tests (examples/) ───────────────────────────────────
printf "${BOLD}Running pass tests...${RESET}\n"

for expected_file in "$EXPECTED_DIR"/*.txt; do
    name=$(basename "$expected_file" .txt)
    example="$ROOT_DIR/examples/${name}.nsh"

    # Apply filter
    if [ -n "$FILTER" ] && ! echo "$name" | grep -q "$FILTER"; then
        continue
    fi

    if [ ! -f "$example" ]; then
        printf "  ${YELLOW}SKIP${RESET}  %s (no example file)\n" "$name"
        skipped=$((skipped + 1))
        continue
    fi

    # Run the example
    stderr_file=$(mktemp)
    actual=$(NASHMIC_ROOT="$ROOT_DIR" "$MANSAF" run "$example" 2>"$stderr_file") || {
        printf "  ${RED}FAIL${RESET}  %s (runtime error)\n" "$name"
        if [ $VERBOSE -eq 1 ]; then
            printf "    ${DIM}stderr:${RESET}\n"
            sed 's/^/    /' "$stderr_file"
        fi
        rm -f "$stderr_file"
        failed=$((failed + 1))
        continue
    }
    rm -f "$stderr_file"

    # Check if this example should skip output comparison
    if echo "$SKIP_OUTPUT_CHECK" | grep -qw "$name"; then
        # Still check EXPECT comments if present
        if grep -q '^// EXPECT: ' "$example" 2>/dev/null; then
            check_expect_comments "$example" "$actual" "$name"
            if [ $? -ne 0 ]; then
                printf "  ${RED}FAIL${RESET}  %s (EXPECT comment mismatch)\n" "$name"
                failed=$((failed + 1))
                continue
            fi
        fi
        printf "  ${GREEN}PASS${RESET}  %s (compile+run only)\n" "$name"
        passed=$((passed + 1))
        continue
    fi

    # Compare output against expected file
    expected=$(cat "$expected_file")
    test_passed=1

    if [ "$actual" != "$expected" ]; then
        printf "  ${RED}FAIL${RESET}  %s (output mismatch)\n" "$name"
        if [ $VERBOSE -eq 1 ]; then
            printf "    ${DIM}--- expected ---${RESET}\n"
            echo "$expected" | head -5 | sed 's/^/    /'
            printf "    ${DIM}--- actual ---${RESET}\n"
            echo "$actual" | head -5 | sed 's/^/    /'
            printf "    ${DIM}---${RESET}\n"
        fi
        test_passed=0
    fi

    # Check EXPECT comments if present
    if [ $test_passed -eq 1 ] && grep -q '^// EXPECT: ' "$example" 2>/dev/null; then
        check_expect_comments "$example" "$actual" "$name"
        if [ $? -ne 0 ]; then
            printf "  ${RED}FAIL${RESET}  %s (EXPECT comment mismatch)\n" "$name"
            test_passed=0
        fi
    fi

    if [ $test_passed -eq 1 ]; then
        printf "  ${GREEN}PASS${RESET}  %s\n" "$name"
        passed=$((passed + 1))
    else
        failed=$((failed + 1))
    fi
done

# ── Warn about examples without expected output ──────────────
for example in "$ROOT_DIR"/examples/*.nsh; do
    name=$(basename "$example" .nsh)
    if [ ! -f "$EXPECTED_DIR/${name}.txt" ]; then
        if [ -n "$FILTER" ] && ! echo "$name" | grep -q "$FILTER"; then
            continue
        fi
        printf "  ${YELLOW}WARN${RESET}  %s has no expected output (add tests/expected/${name}.txt)\n" "$name"
    fi
done

# ── Fail tests (tests/fail/) ────────────────────────────────
if [ -d "$FAIL_DIR" ] && ls "$FAIL_DIR"/*.nsh >/dev/null 2>&1; then
    printf "\n${BOLD}Running fail tests...${RESET}\n"

    for fail_file in "$FAIL_DIR"/*.nsh; do
        name=$(basename "$fail_file" .nsh)

        # Apply filter
        if [ -n "$FILTER" ] && ! echo "$name" | grep -q "$FILTER"; then
            continue
        fi

        # Run the compiler — capture stderr for error checking
        stderr_file=$(mktemp)
        NASHMIC_ROOT="$ROOT_DIR" "$MANSAF" run "$fail_file" >/dev/null 2>"$stderr_file"
        exit_code=$?
        stderr_output=$(cat "$stderr_file")
        rm -f "$stderr_file"

        test_passed=1

        # Check that stderr contains some diagnostic output
        if [ -z "$stderr_output" ]; then
            printf "  ${RED}FAIL${RESET}  %s (no errors produced — expected diagnostics)\n" "$name"
            test_passed=0
        fi

        # Check ERROR comments if present
        if [ $test_passed -eq 1 ] && grep -q '^// ERROR: ' "$fail_file" 2>/dev/null; then
            check_error_comments "$fail_file" "$stderr_output" "$name"
            if [ $? -ne 0 ]; then
                printf "  ${RED}FAIL${RESET}  %s (ERROR comment mismatch)\n" "$name"
                test_passed=0
            fi
        fi

        if [ $test_passed -eq 1 ]; then
            printf "  ${GREEN}PASS${RESET}  %s (errors detected)\n" "$name"
            passed=$((passed + 1))
        else
            if [ $VERBOSE -eq 1 ] && [ -n "$stderr_output" ]; then
                printf "    ${DIM}stderr:${RESET}\n"
                echo "$stderr_output" | head -10 | sed 's/^/    /'
            fi
            failed=$((failed + 1))
        fi
    done
fi

# ── Summary ──────────────────────────────────────────────────
end_time=$(date +%s)
elapsed=$((end_time - start_time))

printf "\n${BOLD}Results:${RESET} "
if [ $passed -gt 0 ]; then
    printf "${GREEN}%d passed${RESET}" "$passed"
fi
if [ $failed -gt 0 ]; then
    if [ $passed -gt 0 ]; then printf ", "; fi
    printf "${RED}%d failed${RESET}" "$failed"
fi
if [ $skipped -gt 0 ]; then
    if [ $passed -gt 0 ] || [ $failed -gt 0 ]; then printf ", "; fi
    printf "${YELLOW}%d skipped${RESET}" "$skipped"
fi
printf " in %d.%ds\n" "$elapsed" "0"

if [ $failed -gt 0 ]; then
    exit 1
fi
