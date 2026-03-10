# NashmiC — Makefile
# Builds the mansaf compiler

CC = cc
CFLAGS = -std=gnu11 -Wall -Wextra -g -O2
CFLAGS += -I compiler/src -I runtime

# Source files
COMPILER_SRC = \
	compiler/src/main.c \
	compiler/src/lexer.c \
	compiler/src/keywords.c \
	compiler/src/parser.c \
	compiler/src/ast.c \
	compiler/src/codegen_c.c \
	compiler/src/diagnostics.c \
	compiler/src/utf8.c

# Output
BUILD_DIR = build
MANSAF = $(BUILD_DIR)/mansaf

.PHONY: all clean test run-hello run-fib run-enums run-natije run-yimkin run-all test

all: $(MANSAF)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(MANSAF): $(COMPILER_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $(COMPILER_SRC) -lm

clean:
	rm -rf $(BUILD_DIR)

# Run test suite — compares example outputs against expected
test: $(MANSAF)
	@bash tests/run_tests.sh

# Quick test: lex the hello world example
test-lex: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) lex examples/marhaba.nsh

# Run examples
run-hello: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/marhaba.nsh

run-fib: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/fibonacci.nsh

run-ranges: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/ranges.nsh

run-interp: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/interpolation.nsh

run-structs: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/structs.nsh

run-easter: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/easter_eggs.nsh

run-enums: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/enums.nsh

run-natije: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/natije.nsh

run-yimkin: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/yimkin.nsh

run-methods: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/methods.nsh

# Run all examples
run-all: $(MANSAF)
	@echo "=== marhaba ==="
	@NASHMIC_ROOT=. $(MANSAF) run examples/marhaba.nsh
	@echo "\n=== fibonacci ==="
	@NASHMIC_ROOT=. $(MANSAF) run examples/fibonacci.nsh
	@echo "\n=== ranges ==="
	@NASHMIC_ROOT=. $(MANSAF) run examples/ranges.nsh
	@echo "\n=== interpolation ==="
	@NASHMIC_ROOT=. $(MANSAF) run examples/interpolation.nsh
	@echo "\n=== structs ==="
	@NASHMIC_ROOT=. $(MANSAF) run examples/structs.nsh
	@echo "\n=== easter eggs ==="
	@NASHMIC_ROOT=. $(MANSAF) run examples/easter_eggs.nsh
	@echo "\n=== enums ==="
	@NASHMIC_ROOT=. $(MANSAF) run examples/enums.nsh
	@echo "\n=== natije ==="
	@NASHMIC_ROOT=. $(MANSAF) run examples/natije.nsh
	@echo "\n=== yimkin ==="
	@NASHMIC_ROOT=. $(MANSAF) run examples/yimkin.nsh
	@echo "\n=== methods ==="
	@NASHMIC_ROOT=. $(MANSAF) run examples/methods.nsh

install: $(MANSAF)
	mkdir -p /usr/local/share/nashmic/runtime
	cp $(MANSAF) /usr/local/bin/mansaf
	cp runtime/nsh_runtime.c runtime/nsh_runtime.h /usr/local/share/nashmic/runtime/
	@echo "✓ mansaf installed to /usr/local/bin/"
	@echo "✓ runtime installed to /usr/local/share/nashmic/runtime/"

uninstall:
	rm -f /usr/local/bin/mansaf
	rm -rf /usr/local/share/nashmic
	@echo "✓ mansaf uninstalled"
