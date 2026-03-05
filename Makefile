# Io Language — WASM build via wasi-sdk
#
# Usage:
#   make                Build io_static (WASM binary)
#   make test           Build test_iterative_eval
#   make check          Run tests with wasmtime
#   make clean          Remove build artifacts
#   make regenerate     Regenerate IoVMInit.c from .io files

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

# .io standard library files (order matters: bootstrap first, Importer last)
IO_FILES := \
    libs/iovm/io/List_bootstrap.io \
    libs/iovm/io/Object_bootstrap.io \
    libs/iovm/io/OperatorTable.io \
    libs/iovm/io/Object.io \
    libs/iovm/io/List.io \
    libs/iovm/io/Exception.io \
    libs/iovm/io/Actor.io \
    libs/iovm/io/Sequence.io \
    libs/iovm/io/Block.io \
    libs/iovm/io/CFunction.io \
    libs/iovm/io/Date.io \
    libs/iovm/io/Debugger.io \
    libs/iovm/io/Directory.io \
    libs/iovm/io/Error.io \
    libs/iovm/io/File.io \
    libs/iovm/io/List_schwartzian.io \
    libs/iovm/io/Map.io \
    libs/iovm/io/Message.io \
    libs/iovm/io/Number.io \
    libs/iovm/io/Profiler.io \
    libs/iovm/io/Sandbox.io \
    libs/iovm/io/Serialize.io \
    libs/iovm/io/System.io \
    libs/iovm/io/UnitTest.io \
    libs/iovm/io/Vector.io \
    libs/iovm/io/Path.io \
    libs/iovm/io/CLI.io \
    libs/iovm/io/Importer.io

# --- Targets ---

.PHONY: all test check clean regenerate

all: $(BINDIR)/io_static

test: $(BINDIR)/test_iterative_eval

check: $(BINDIR)/io_static $(BINDIR)/test_iterative_eval
	wasmtime $(BINDIR)/test_iterative_eval
	wasmtime --dir=. --dir=/tmp $(BINDIR)/io_static libs/iovm/tests/correctness/run.io

clean:
	rm -rf $(BUILD)

regenerate: $(BINDIR)/io2c
	$(BINDIR)/io2c VMCode IoState_doString_ $(IO_FILES) > libs/iovm/source/IoVMInit.c

# --- Binaries ---

$(BINDIR)/io_static: $(OBJDIR)/tools/source/main.o $(ALL_OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BINDIR)/test_iterative_eval: $(OBJDIR)/libs/iovm/tests/test_iterative_eval.o $(ALL_OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Host-native io2c (not WASM)
$(BINDIR)/io2c: libs/iovm/tools/io2c.c | $(BINDIR)
	$(HOST_CC) -o $@ $<

# --- Code generation ---

libs/iovm/source/IoVMInit.c: $(BINDIR)/io2c $(IO_FILES)
	$(BINDIR)/io2c VMCode IoState_doString_ $(IO_FILES) > $@

# --- Compilation ---

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR):
	mkdir -p $@
