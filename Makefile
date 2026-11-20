# Io Language — WASM build via wasi-sdk
#
# Usage:
#   make                Build io_static (WASM binary)
#   make test           Build test_iterative_eval
#   make check          Run tests with wasmtime
#   make clean          Remove build artifacts
#   make regenerate     Regenerate IoVMInit.c from .io files
#   make browser        Build browser/io_browser.wasm (reactor module)
#   make serve          Serve browser REPL on localhost:8000
#   make check-browser-node  Test the browser JS module in Node (no browser needed)
#   make npm-package    Stage the @iolanguage/io npm package in build/npm/
#   make check-npm-package  Pack, install and smoke-test the npm package
#   make component      Build io_component.wasm (WASI 0.2 component, wasm32-wasip2)
#   make check-component  Run the Io test suite against the component

# wasi-sdk: deps/wasi-sdk (untracked) if it is there, otherwise ~/wasi-sdk.
WASI_SDK    ?= $(if $(wildcard deps/wasi-sdk/bin/clang),deps/wasi-sdk,$(HOME)/wasi-sdk)
BUILD       := build
BINDIR      := $(BUILD)/bin
OBJDIR      := $(BUILD)/obj

# Host compiler (for io2c code generator)
HOST_CC     := cc

# WASM cross-compiler
CC          := $(WASI_SDK)/bin/clang
AR          := $(WASI_SDK)/bin/llvm-ar

CFLAGS      := --sysroot=$(WASI_SDK)/share/wasi-sysroot \
               -D__wasi__ -DUSE_BUILTIN_NAN -D__STDC_IEC_559__ \
               -D_WASI_EMULATED_PROCESS_CLOCKS -D_WASI_EMULATED_SIGNAL \
               -fno-exceptions -O2 \
               -msimd128 \
               -Ilibs/basekit/source \
               -Ilibs/basekit/source/simd_cph/include \
               -Ilibs/garbagecollector/source \
               -Ilibs/iovm/source \
               -Ideps/parson \
               -Ideps/libtommath

LDFLAGS     := -lwasi-emulated-process-clocks -lwasi-emulated-signal -static

# Each compile also writes a .d file listing the headers it read, so editing
# a header rebuilds the objects that include it (see "Header dependencies").
DEPFLAGS    = -MMD -MP

# --- Sources ---

BASEKIT_SRCS := $(wildcard libs/basekit/source/*.c)
GC_SRCS      := $(wildcard libs/garbagecollector/source/*.c)
IOVM_SRCS    := $(filter-out %/IoState_iterative_fast.c, $(wildcard libs/iovm/source/*.c))
PARSON_SRCS  := deps/parson/parson.c
TOMMATH_SRCS := $(wildcard deps/libtommath/*.c)

# Ensure IoVMInit.c is included even before first generation
IOVM_SRCS    := $(sort $(IOVM_SRCS) libs/iovm/source/IoVMInit.c)

ALL_SRCS     := $(BASEKIT_SRCS) $(GC_SRCS) $(IOVM_SRCS) $(PARSON_SRCS) $(TOMMATH_SRCS)
ALL_OBJS     := $(patsubst %.c,$(OBJDIR)/%.o,$(ALL_SRCS))

# .io standard library files (load order defined by _imports.json)
IO_IMPORTS  := libs/iovm/io/_imports.json
IO_SOURCES  := $(wildcard libs/iovm/io/*.io)

# --- Targets ---

.PHONY: all test check clean regenerate browser serve check-browser check-browser-node npm-package check-npm-package component check-component

all: $(BINDIR)/io_static

test: $(BINDIR)/test_iterative_eval

check: $(BINDIR)/io_static $(BINDIR)/test_iterative_eval
	wasmtime $(BINDIR)/test_iterative_eval
	wasmtime --dir=. --dir=/tmp $(BINDIR)/io_static libs/iovm/tests/correctness/run.io
	@# An uncaught exception must make the process exit non-zero
	@wasmtime $(BINDIR)/io_static -e 'Exception raise("expected")' >/dev/null 2>&1; \
		status=$$?; if [ $$status -ne 1 ]; then \
		echo "FAIL: an uncaught exception exited with $$status, not 1"; exit 1; fi
	@wasmtime $(BINDIR)/io_static -e 'try(Exception raise("caught")); System exit(3)' >/dev/null 2>&1; \
		status=$$?; if [ $$status -ne 3 ]; then \
		echo "FAIL: System exit(3) exited with $$status"; exit 1; fi
	@echo "exit status checks OK"

clean:
	rm -rf $(BUILD) $(BROWSER_WASM)

regenerate: $(BINDIR)/io2c
	$(BINDIR)/io2c VMCode IoState_doString_ $(IO_IMPORTS) > libs/iovm/source/IoVMInit.c

# --- Binaries ---

$(BINDIR)/io_static: $(OBJDIR)/tools/source/main.o $(ALL_OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BINDIR)/test_iterative_eval: $(OBJDIR)/libs/iovm/tests/test_iterative_eval.o $(ALL_OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Host-native io2c (not WASM)
$(BINDIR)/io2c: libs/iovm/tools/io2c.c deps/parson/parson.c | $(BINDIR)
	$(HOST_CC) -Ideps/parson -o $@ libs/iovm/tools/io2c.c deps/parson/parson.c

# --- Code generation ---

libs/iovm/source/IoVMInit.c: $(BINDIR)/io2c $(IO_IMPORTS) $(IO_SOURCES)
	$(BINDIR)/io2c VMCode IoState_doString_ $(IO_IMPORTS) > $@

# --- Compilation ---

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(DEPFLAGS) -c -o $@ $<

# --- Component (WASI 0.2, wasm32-wasip2) ---
#
# Same sources compiled for the wasm32-wasip2 triple, linked through
# wasm-component-ld into a WebAssembly component speaking the WASI 0.2
# interfaces. Objects live in a separate tree because the triple differs.

P2_OBJDIR := $(BUILD)/obj-p2
P2_CFLAGS := --target=wasm32-wasip2 $(CFLAGS)
P2_OBJS   := $(patsubst %.c,$(P2_OBJDIR)/%.o,$(ALL_SRCS))

component: $(BINDIR)/io_component.wasm

$(BINDIR)/io_component.wasm: $(P2_OBJDIR)/tools/source/main.o $(P2_OBJS) | $(BINDIR)
	$(CC) $(P2_CFLAGS) -o $@ $^ $(LDFLAGS)

$(P2_OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(P2_CFLAGS) $(DEPFLAGS) -c -o $@ $<

check-component: $(BINDIR)/io_component.wasm
	wasmtime --dir=. --dir=/tmp $(BINDIR)/io_component.wasm libs/iovm/tests/correctness/run.io

# --- Browser (reactor module) ---

BROWSER_DIR  := browser
BROWSER_WASM := $(BROWSER_DIR)/io_browser.wasm
BROWSER_OBJS := $(OBJDIR)/browser/io_browser.o $(OBJDIR)/browser/io_js_bridge.o $(OBJDIR)/browser/io_future.o

BROWSER_CFLAGS := $(CFLAGS) -Ibrowser
BROWSER_LDFLAGS := -lwasi-emulated-process-clocks -lwasi-emulated-signal \
	-mexec-model=reactor \
	-Wl,--allow-undefined \
	-Wl,--export=io_init \
	-Wl,--export=io_eval \
	-Wl,--export=io_get_output \
	-Wl,--export=io_get_output_len \
	-Wl,--export=io_get_input_buf \
	-Wl,--export=io_get_input_buf_size \
	-Wl,--export=io_eval_input \
	-Wl,--export=io_get_bridge_buf \
	-Wl,--export=io_get_bridge_buf_size \
	-Wl,--export=io_send \
	-Wl,--export=io_release \
	-Wl,--export=io_get_lobby_handle \
	-Wl,--export=io_resolve_future \
	-Wl,--export=io_reject_future \
	-Wl,--export=io_tick \
	-Wl,--export=io_next_wake_ms \
	-Wl,--export=io_get_eval_output_len

browser: $(BROWSER_WASM)

$(BROWSER_WASM): $(BROWSER_OBJS) $(ALL_OBJS)
	$(CC) $(BROWSER_CFLAGS) -o $@ $^ $(BROWSER_LDFLAGS)

$(OBJDIR)/browser/%.o: browser/%.c
	@mkdir -p $(dir $@)
	$(CC) $(BROWSER_CFLAGS) $(DEPFLAGS) -c -o $@ $<

serve: $(BROWSER_WASM)
	@echo "Serving at http://localhost:8000"
	python3 -m http.server 8000 -d $(BROWSER_DIR)

check-browser-node: $(BROWSER_WASM)
	node --test browser/test/vm.test.mjs

check-browser: check-browser-node
	@test -d node_modules/playwright || npm install --no-save playwright
	node browser/run_tests.mjs

# NPM_TAG is an Io release tag (e.g. 2026.04.20-native-final); it sets the
# package version. Empty gives 0.0.0-dev.
npm-package: $(BROWSER_WASM)
	node browser/npm/pack.mjs $(NPM_TAG)

check-npm-package: npm-package
	node --test browser/npm/package.test.mjs

# --- Header dependencies ---
#
# The .d files written next to the objects. Missing ones (a first build) are
# skipped; -MP keeps a deleted header from breaking the build.

-include $(shell find $(BUILD) -name '*.d' 2>/dev/null)

# --- Directories ---

$(BINDIR):
	mkdir -p $@
