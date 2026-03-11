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
	compiler/src/sema.c \
	compiler/src/diagnostics.c \
	compiler/src/utf8.c \
	compiler/src/repl.c \
	compiler/src/formatter.c

# Output
BUILD_DIR = build
MANSAF = $(BUILD_DIR)/mansaf

# WASM source files (all compiler sources except main.c, plus wasm_entry.c)
WASM_SRC = \
	compiler/src/wasm_entry.c \
	compiler/src/lexer.c \
	compiler/src/keywords.c \
	compiler/src/parser.c \
	compiler/src/ast.c \
	compiler/src/codegen_c.c \
	compiler/src/sema.c \
	compiler/src/diagnostics.c \
	compiler/src/utf8.c

WASM_OUT_DIR = tools/playground
WASM_JS = $(WASM_OUT_DIR)/nashmic.js
WASM_FILE = $(WASM_OUT_DIR)/nashmic.wasm

.PHONY: all clean test wasm run-hello run-fib run-enums run-natije run-yimkin run-arrays run-riyadiyat run-nusoos run-malafat run-all test

all: $(MANSAF)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Detect readline availability
HAVE_READLINE := $(shell echo 'int main(){return 0;}' | $(CC) -x c - -lreadline -o /dev/null 2>/dev/null && echo 1)
ifeq ($(HAVE_READLINE),1)
  LDFLAGS += -lreadline
else
  CFLAGS += -UHAVE_READLINE
endif

$(MANSAF): $(COMPILER_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $(COMPILER_SRC) -lm $(LDFLAGS)

# WASM build — compile mansaf transpiler to WebAssembly for in-browser use
wasm: $(WASM_SRC)
	emcc -std=gnu11 -O2 \
		-I compiler/src -I runtime \
		-s WASM=1 \
		-s MODULARIZE=1 \
		-s EXPORT_NAME='NashmiCModule' \
		-s EXPORTED_FUNCTIONS='["_nsh_compile","_nsh_get_errors","_malloc","_free"]' \
		-s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","UTF8ToString","stringToUTF8","lengthBytesUTF8"]' \
		-s TOTAL_MEMORY=16MB \
		-s ALLOW_MEMORY_GROWTH=0 \
		-o $(WASM_JS) \
		$(WASM_SRC)
	@echo "✓ WASM build complete: $(WASM_JS) + $(WASM_FILE)"

clean:
	rm -rf $(BUILD_DIR)

clean-wasm:
	rm -f $(WASM_JS) $(WASM_FILE)

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

run-defer: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/defer.nsh

run-arrays: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/arrays.nsh

run-riyadiyat: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/riyadiyat.nsh

run-nusoos: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/nusoos.nsh

run-malafat: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) run examples/malafat.nsh

run-repl: $(MANSAF)
	NASHMIC_ROOT=. $(MANSAF) repl

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
	@echo "\n=== defer ==="
	@NASHMIC_ROOT=. $(MANSAF) run examples/defer.nsh
	@echo "\n=== arrays ==="
	@NASHMIC_ROOT=. $(MANSAF) run examples/arrays.nsh
	@echo "\n=== riyadiyat ==="
	@NASHMIC_ROOT=. $(MANSAF) run examples/riyadiyat.nsh
	@echo "\n=== nusoos ==="
	@NASHMIC_ROOT=. $(MANSAF) run examples/nusoos.nsh
	@echo "\n=== malafat ==="
	@NASHMIC_ROOT=. $(MANSAF) run examples/malafat.nsh

PREFIX ?= /usr/local
DESTDIR ?=

install: $(MANSAF)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -d $(DESTDIR)$(PREFIX)/share/nashmic/runtime
	install -d $(DESTDIR)$(PREFIX)/share/nashmic/stdlib
	install -m 755 $(MANSAF) $(DESTDIR)$(PREFIX)/bin/mansaf
	install -m 644 runtime/nsh_runtime.c runtime/nsh_runtime.h $(DESTDIR)$(PREFIX)/share/nashmic/runtime/
	cp -r stdlib/* $(DESTDIR)$(PREFIX)/share/nashmic/stdlib/
	@echo "mansaf installed to $(DESTDIR)$(PREFIX)/bin/"
	@echo "runtime installed to $(DESTDIR)$(PREFIX)/share/nashmic/runtime/"
	@echo "stdlib installed to $(DESTDIR)$(PREFIX)/share/nashmic/stdlib/"

uninstall:
	rm -f /usr/local/bin/mansaf
	rm -rf /usr/local/share/nashmic
	@echo "✓ mansaf uninstalled"
