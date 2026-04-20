# Io Language VM

## What is this?

The Io programming language — a dynamic prototype-based language built on message passing, targeting WASM/WASI.

## Build

Requires [wasi-sdk](https://github.com/WebAssembly/wasi-sdk) and [wasmtime](https://wasmtime.dev/).

```bash
make            # build build/bin/io_static (WASM binary)
make test       # build build/bin/test_iterative_eval
make check      # run both test suites with wasmtime
make clean      # remove build artifacts
make regenerate # regenerate IoVMInit.c from .io files
```

Set `WASI_SDK` if wasi-sdk is not at `~/wasi-sdk`:
```bash
make WASI_SDK=/opt/wasi-sdk
```

## Test

```bash
# Run a one-liner
wasmtime build/bin/io_static -e '"hello" println'

# Run a file
wasmtime --dir=. build/bin/io_static path/to/script.io

# C test suite (iterative evaluator)
wasmtime build/bin/test_iterative_eval

# Io test suite
wasmtime --dir=. --dir=/tmp build/bin/io_static libs/iovm/tests/correctness/run.io
IO_TEST_VERBOSE=1 wasmtime --dir=. --dir=/tmp build/bin/io_static libs/iovm/tests/correctness/run.io
```

## Project Structure

```
libs/iovm/source/     — Core VM implementation (C)
libs/iovm/io/         — Io standard library (.io files, compiled to C via io2c)
libs/iovm/tests/      — C test files
tools/source/main.c   — REPL / CLI entry point
agents/               — Design docs for the continuations/stackless work
```

### Key Source Files

| File | Purpose |
|------|---------|
| `IoState_iterative.c` | Iterative eval loop with frame state machine |
| `IoState_eval.c` | Entry points (`IoState_doCString_`, `IoState_on_doCString_withLabel_`) |
| `IoMessage.c` | `IoMessage_locals_performOn_` — recursive evaluator |
| `IoBlock.c` | Block activation (currently uses recursive evaluator) |
| `IoObject_flow.c` | Control flow primitives: `if`, `while`, `for`, `loop`, `break`, `continue`, `return` |
| `IoCoroutine.c` | Coroutine implementation (frame-based, no C stack switching) |
| `IoContinuation.c` | First-class continuations (`callcc`, capture, invoke) |
| `IoEvalFrame.h/c` | Frame structure and state machine enums |
| `IoState_inline.h` | Inline helpers, arg pre-evaluation |
| `IoState.h` | VM state structure |

## Branch: `stackless` (off `continuations`)

Replacing C stack recursion with heap-allocated frames to enable:
- First-class continuations (serializable, network-transmittable)
- Portable coroutines (no platform-specific assembly)
- No setjmp/longjmp, no ucontext, no fibers

See `agents/` for detailed design docs:
- `CONTINUATIONS_TODO.md` — Phase tracker and implementation notes
- `C_STACK_ELIMINATION_PLAN.md` — Overall architecture plan
- `VM_EVAL_LOOP.md` — Eval loop design reference

## Important Conventions

### IoVMInit.c regeneration
When `.io` files in `libs/iovm/io/` are modified, the generated `libs/iovm/source/IoVMInit.c` must be regenerated:
```bash
make regenerate
make
```

### Evaluator
- **Iterative** (`IoState_iterative.c`): Frame state machine. Used for all evaluation including control flow, continuations, coroutine switching.
- `IoMessage_locals_performOn_` redirects to the iterative eval loop when `currentFrame` is set. A bootstrap-only recursive fallback exists for VM initialization (before the first eval loop starts).

### Special forms
Messages whose arguments must NOT be pre-evaluated: `if`, `while`, `loop`, `for`, `callcc`, `method`, `block`, `foreach`, `reverseForeach`, `foreachLine`. Checked in two places in `IoState_iterative.c`.

### Error handling
- C-level: `IoState_error_` sets `state->errorRaised = 1`. Eval loop unwinds frames.
- Io-level: `Exception raise` calls `rawSignalException` which also sets `errorRaised`.
- Helper functions return early on error (no longjmp). Eval loop handles all unwinding.

### Debug compile flags
- `DEBUG_EVAL_LOOP` — verbose iterative eval loop tracing
- `DEBUG_CORO_EVAL` — coroutine operation tracing

## Architecture (General)

Every Io object is a `CollectorMarker` (`typedef struct CollectorMarker IoObject`). The marker's `object` field points to `IoObjectData` containing: a `tag` (vtable), `slots` (PHash cuckoo hash table), `protos` array, and a data union for primitive values.

All operations are message sends. The parser produces message chains, then `IoMessage_opShuffle.c` rewrites them by operator precedence. Assignment operators: `:=` → `setSlot`, `=` → `updateSlot`, `::=` → `newSlot`.

Standard library files in `libs/iovm/io/` are loaded in the explicit order listed in the `Makefile` `IO_FILES` variable: bootstrap files (`List_bootstrap.io`, `Object_bootstrap.io`, `OperatorTable.io`, `Object.io`, `List.io`, `Exception.io`) first, then alphabetical core, then `CLI.io`, `Importer.io` last.

## Code Style

- **Indentation**: Tabs in both C and Io code
- **C naming**: `IoObjectName_methodName` (e.g., `IoList_append_`)
- **C method macro**: `IO_METHOD(CLASS, NAME)` expands to `IoObject *CLASS_NAME(CLASS *self, IoObject *locals, IoMessage *m)`
- **Io naming**: camelCase for methods, PascalCase for objects
