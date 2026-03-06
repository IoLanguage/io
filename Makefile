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

WASI_SDK    ?= $(HOME)/wasi-sdk
BUILD       := build
BINDIR      := $(BUILD)/bin
OBJDIR      := $(BUILD)/obj

# Host compiler (for io2c code generator)
HOST_CC     := cc

# WASM cross-compiler
CC          := $(WASI_SDK)/bin/clang
AR          := $(WASI_SDK)/bin/llvm-ar

CFLAGS      := --sysroot=$(WASI_SDK)/share/wasi-sysroot \
               -D__wasi__ -DUSE_BUILTIN_NAN \
               -D_WASI_EMULATED_PROCESS_CLOCKS -D_WASI_EMULATED_SIGNAL \
               -fno-exceptions -O2 \
               -Ilibs/basekit/source \
               -Ilibs/basekit/source/simd_cph/include \
               -Ilibs/garbagecollector/source \
               -Ilibs/iovm/source \
               -Ideps/parson

LDFLAGS     := -lwasi-emulated-process-clocks -lwasi-emulated-signal -static

# --- Sources ---

BASEKIT_SRCS := $(wildcard libs/basekit/source/*.c)
GC_SRCS      := $(wildcard libs/garbagecollector/source/*.c)
IOVM_SRCS    := $(filter-out %/IoState_iterative_fast.c, $(wildcard libs/iovm/source/*.c))
PARSON_SRCS  := deps/parson/parson.c

# Ensure IoVMInit.c is included even before first generation
IOVM_SRCS    := $(sort $(IOVM_SRCS) libs/iovm/source/IoVMInit.c)

ALL_SRCS     := $(BASEKIT_SRCS) $(GC_SRCS) $(IOVM_SRCS) $(PARSON_SRCS)
ALL_OBJS     := $(patsubst %.c,$(OBJDIR)/%.o,$(ALL_SRCS))

# .io standard library files (load order defined by _imports.json)
IO_IMPORTS  := libs/iovm/io/_imports.json
IO_SOURCES  := $(wildcard libs/iovm/io/*.io)

# --- Targets ---

.PHONY: all test check clean regenerate browser serve check-browser

all: $(BINDIR)/io_static

test: $(BINDIR)/test_iterative_eval

check: $(BINDIR)/io_static $(BINDIR)/test_iterative_eval
	wasmtime $(BINDIR)/test_iterative_eval
	wasmtime --dir=. --dir=/tmp $(BINDIR)/io_static libs/iovm/tests/correctness/run.io

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
	$(CC) $(CFLAGS) -c -o $@ $<

# --- Browser (reactor module) ---

BROWSER_DIR  := browser
BROWSER_WASM := $(BROWSER_DIR)/io_browser.wasm
BROWSER_OBJS := $(OBJDIR)/browser/io_browser.o $(OBJDIR)/browser/io_js_bridge.o

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
	-Wl,--export=io_get_lobby_handle

browser: $(BROWSER_WASM)

$(BROWSER_WASM): $(BROWSER_OBJS) $(ALL_OBJS)
	$(CC) $(BROWSER_CFLAGS) -o $@ $^ $(BROWSER_LDFLAGS)

$(OBJDIR)/browser/io_browser.o: browser/io_browser.c browser/io_js_bridge.h
	@mkdir -p $(dir $@)
	$(CC) $(BROWSER_CFLAGS) -c -o $@ $<

$(OBJDIR)/browser/io_js_bridge.o: browser/io_js_bridge.c browser/io_js_bridge.h
	@mkdir -p $(dir $@)
	$(CC) $(BROWSER_CFLAGS) -c -o $@ $<

serve: $(BROWSER_WASM)
	@echo "Serving at http://localhost:8000"
	python3 -m http.server 8000 -d $(BROWSER_DIR)

check-browser: $(BROWSER_WASM)
	@test -d node_modules/playwright || npm install --no-save playwright
	node browser/run_tests.mjs

# --- Directories ---

$(BINDIR):
	mkdir -p $@
