# NashmiC — Makefile
# Builds the mansaf compiler

CC = cc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g -O2
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

.PHONY: all clean test run-hello

all: $(MANSAF)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(MANSAF): $(COMPILER_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $(COMPILER_SRC) -lm

clean:
	rm -rf $(BUILD_DIR)

# Quick test: lex the hello world example
test-lex: $(MANSAF)
	$(MANSAF) lex examples/marhaba.nsh

# Quick test: build and run hello world
run-hello: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/marhaba.nsh

# Build hello world
build-hello: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) build examples/marhaba.nsh

# Run fibonacci
run-fib: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/fibonacci.nsh

install: $(MANSAF)
	cp $(MANSAF) /usr/local/bin/mansaf
	@echo "✓ mansaf installed to /usr/local/bin/"
