# Stackless Evaluator Report

## Overview

The `stackless` branch replaces Io's recursive C-stack-based evaluator with a heap-allocated frame-based iterative evaluator. Every piece of execution state — message pointer, target, locals, arguments, control flow — lives in GC-managed frame objects on the heap rather than in C stack frames.

### Goals

1. **First-class continuations** — `callcc` captures the entire execution state as a serializable, network-transmittable object. Impossible with C stack recursion.
2. **Portable coroutines** — No platform-specific assembly, no `setjmp`/`longjmp`, no `ucontext`, no fibers. Coroutine switching is just swapping a frame pointer.
3. **No stack overflow** — Tail call optimization reuses frames. Deep recursion is bounded by heap, not C stack depth.
4. **Performance** — The iterative evaluator with object pooling and inline caching is 1.1x–4x faster than the recursive evaluator across all benchmarks.

### Non-Goals

- Changing the Io language semantics. All existing Io code runs unmodified.
- JIT compilation or bytecode. The evaluator still interprets the message tree directly.

---

## Architecture

### The Eval Loop

A single C function, `IoState_evalLoop_()`, runs a `while(1)` loop that processes frames from a linked stack. Each frame has a `state` field (a state machine enum) that tells the loop what to do next. There is no C recursion for message evaluation — argument evaluation, block activation, and control flow are all driven by pushing/popping frames and transitioning states.

```
┌─────────────────────────────────────────────────┐
│                 IoState_evalLoop_                │
│                                                 │
│  while(1) {                                     │
│      frame = state->currentFrame                │
│      switch(frame->state) {                     │
│          START → LOOKUP_SLOT → ACTIVATE         │
│              ├── CFunction: call directly        │
│              └── Block: push child frame         │
│          CONTINUE_CHAIN → next message or RETURN │
│          RETURN → pop frame, store result        │
│          IF_*, WHILE_*, FOR_*, ... (control flow)│
│      }                                          │
│  }                                              │
└─────────────────────────────────────────────────┘
```

### Frame Structure

Frames are full IoObjects managed by the garbage collector (`typedef IoObject IoEvalFrame`). Their data payload is `IoEvalFrameData`:

| Field | Purpose |
|-------|---------|
| `message` | Current message being evaluated (instruction pointer) |
| `target` | Receiver object (`self`) |
| `locals` | Enclosing scope for slot lookup |
| `result` | Accumulated result of this frame |
| `state` | Current state machine state |
| `parent` | Parent frame (for returning results) |
| `argValues` / `inlineArgs[4]` | Pre-evaluated argument values |
| `blockLocals` | Block's local scope (if block activation) |
| `call` / `savedCall` | Call introspection objects |
| `controlFlow` | Union of per-primitive state (for/while/if/foreach/etc.) |

### Frame State Machine

The evaluator has 24 states organized by function:

**Core evaluation:** `START` → `LOOKUP_SLOT` → `EVAL_ARGS` → `ACTIVATE` → `CONTINUE_CHAIN` → `RETURN`

**Control flow (each primitive has its own states):**
- **if:** `IF_EVAL_CONDITION` → `IF_CONVERT_BOOLEAN` → `IF_EVAL_BRANCH`
- **while:** `WHILE_EVAL_CONDITION` → `WHILE_CHECK_CONDITION` → `WHILE_DECIDE` → `WHILE_EVAL_BODY`
- **for:** `FOR_EVAL_SETUP` → `FOR_EVAL_BODY` → `FOR_AFTER_BODY`
- **loop:** `LOOP_EVAL_BODY` → `LOOP_AFTER_BODY`
- **foreach:** `FOREACH_EVAL_BODY` → `FOREACH_AFTER_BODY`
- **callcc:** `CALLCC_EVAL_BLOCK`
- **coroutines:** `CORO_WAIT_CHILD`, `CORO_YIELDED`
- **do/doString/doFile:** `DO_EVAL`, `DO_WAIT`

### No C Stack Manipulation

The entire design avoids platform-specific stack tricks:

- No `setjmp`/`longjmp` — errors set `state->errorRaised = 1` and return normally. The eval loop unwinds frames.
- No `ucontext` or fibers — coroutines save/restore a frame pointer.
- No assembly — continuations copy frame chains on the heap.

This makes the VM fully portable to any platform with a C99 compiler.

---

## New Language Features

### First-Class Continuations

`callcc(block)` captures the current execution state into a Continuation object:

```io
result := callcc(block(escape,
    list(1, 2, 3) foreach(v,
        if(v == 3, escape invoke("found it"))
    )
    "not found"
))
result println  // => "found it"
```

**Continuation API:**

| Method | Description |
|--------|-------------|
| `invoke(value)` | Restore captured frames, return value at callcc site |
| `copy` | Deep-copy the frame chain (enables multi-shot use) |
| `isInvoked` | Returns true if this continuation has been invoked |
| `frameCount` | Number of captured frames |
| `frameStates` | List of state names per frame |
| `frameMessages` | List of current messages per frame |
| `asMap` / `fromMap` | Serialize/deserialize continuation state |

Continuations are one-shot by default. Use `copy` to create a fresh continuation for multi-shot patterns (generators, backtracking). See `docs/IoContinuationsExamples.md` for detailed examples.

### Frame Introspection

Live execution frames are exposed to Io code for debugging and metaprogramming:

```io
f := Coroutine currentCoroutine currentFrame
while(f != nil,
    f description println
    f = f parent
)
```

**EvalFrame methods:** `message`, `target`, `locals`, `state`, `parent`, `result`, `depth`, `call`, `blockLocals`, `description`

### Portable Coroutines

Coroutines work by saving and restoring the frame pointer — no C stack switching:

```io
o := Object clone
o work := method(
    for(i, 1, 5, i println; yield)
)
o @@work
for(i, 1, 5, yield)
```

A suspended coroutine's entire state is a single pointer to its saved frame chain. Switching coroutines is O(1).

---

## Performance Optimizations

### Object Pooling

Four pools eliminate allocation overhead in hot paths:

| Pool | Size | What's Reused |
|------|------|---------------|
| Frame pool | 256 | GC-managed EvalFrame objects |
| Block locals pool | 8 | PHash-allocated locals for block activation |
| Call pool | 8 | IoCallData-allocated Call objects |
| Number data freelist | 512 | IoObjectData allocations for Numbers |

All pooled objects are GC-marked through `IoCoroutine_mark()` to prevent premature collection.

### Inline Argument Buffer

95% of method calls have 4 or fewer arguments. These use a stack-allocated `inlineArgs[4]` buffer instead of heap-allocating an argument array.

### Monomorphic Inline Cache

Each IoMessage has a one-entry cache for slot lookups:

```c
if (tag matches && slotVersion matches && no local shadow)
    → return cached value (skip proto chain walk)
```

The cache stores `(tag, slotValue, context, version)` and only caches proto-chain hits. A local-slot shadow guard prevents stale results when different objects of the same type have overriding local slots (e.g., `false.isTrue` shadowing `Object.isTrue`).

### Special Form Detection

Each CFunction that needs lazy argument evaluation carries an `isLazyArgs` flag, set at VM init time. This includes control flow primitives (`if`, `while`, `for`, `loop`, `callcc`), block constructors (`method`, `block`), iteration (`foreach`, `foreachSlot`), and others. Since Io's `getSlot` returns the same CFunction object, aliases automatically inherit the flag (e.g., `false.elseif := Object getSlot("if")`). The result is cached per-message-site for fast subsequent lookups.

### Cached Literal Fast Paths

When a control flow body is a single cached literal (nil, number, or string), the evaluator skips frame allocation entirely and uses the cached value directly. For-loops with literal bodies (`for(i, 1, 1000000, nil)`) run as tight C loops.

### Tail Call Optimization

Two complementary mechanisms keep frame stacks flat:

1. **Direct TCO:** When a Block call is the last message in a block body, `activateBlockTCO_()` reuses the current frame instead of pushing a new one.

2. **TCO through if:** When `if()` is the last message in a chain, the selected branch evaluates in-place without a child frame. This enables idiomatic Io recursion:

```io
factorial := method(n, acc,
    if(n <= 1, acc, factorial(n - 1, acc * n))
)
factorial(100000, 1)  // no stack overflow
```

### Boolean Singleton Fast Path

`if` and `while` conditions that are `true`, `false`, or `nil` skip the `asBoolean` frame push — the singleton is used directly.

### Number Cache

Pre-allocated Number objects for the range [-10, 1024] eliminate allocation for most loop counters and arithmetic.

---

## Hybrid Reference Counting (Optional)

An optional RC layer (disabled by default, enable with `#define COLLECTOR_USE_REFCOUNT 1` in `CollectorMarker.h`) promptly reclaims short-lived objects. The existing mark/sweep GC remains as backup for cycles.

When enabled, for-loop counter Numbers are reclaimed immediately via RC drain, keeping the freed list populated and avoiding `calloc`. This gives a 1.5x speedup on `for(i, 1, 1000000, i + 1)` at the cost of ~7% regression on method-heavy workloads from the refcount increment on every `IOREF`.

---

## Benchmarks

All benchmarks on macOS, Release build. Times are best-of-3 wall clock.

### Stackless vs Master (Recursive Evaluator)

| Benchmark | Master | Stackless | Speedup |
|-----------|--------|-----------|---------|
| `for(i, 1, 1M, nil)` | 0.32s | 0.08s | **4.0x** |
| `for(i, 1, 1M, i+1)` | 0.69s | 0.42s | **1.6x** |
| `x = x + 1` (500K) | 0.41s | 0.29s | **1.4x** |
| 500K `method(y, y+1)` | 0.74s | 0.28s | **2.6x** |
| `while(i < 1M)` | 0.70s | 0.64s | **1.1x** |
| `fib(30)` recursive | 3.10s | 1.77s | **1.8x** |
| List ops (100K) | 1.74s | 1.14s | **1.5x** |
| Test suite (239 tests) | 0.93s | 0.83s | **1.1x** |

Stackless is faster on every benchmark. Method calls and tight for-loops benefit most from the iterative eval loop, frame pooling, and inline caches. Recursive workloads like `fib(30)` benefit from reduced frame allocation overhead.

### With Optional RC Enabled

| Benchmark | Without RC | With RC | Change |
|-----------|-----------|---------|--------|
| `for(i, 1, 1M, i+1)` | 0.42s | 0.35s | 1.2x faster |
| 500K `method(y, y+1)` | 0.28s | 0.32s | 12% slower |
| `fib(30)` | 1.77s | 1.96s | 11% slower |

RC is a targeted optimization for allocation-heavy for-loops. It trades ~10% overhead on general workloads for prompt reclamation of loop temporaries.

---

## Test Results

- **30/30** C tests (TCO, continuations, exceptions, coroutines, `?` operator, `asMap`)
- **239/239** Io tests via `run.io` (230 original + 9 EvalFrame introspection tests)
- SwitchTest: 6 pre-existing failures (same on master, not in run.io suite)

---

## Key Files

| File | Purpose |
|------|---------|
| `libs/iovm/source/IoState_iterative.c` | Iterative eval loop, state machine, control flow |
| `libs/iovm/source/IoEvalFrame.h` / `.c` | Frame structure, state enum, introspection methods |
| `libs/iovm/source/IoObject_flow.c` | Control flow primitives (if, while, for, loop, etc.) |
| `libs/iovm/source/IoContinuation.h` / `.c` | Continuation capture, invoke, copy, serialization |
| `libs/iovm/source/IoCoroutine.c` | Frame-based coroutine switching |
| `libs/iovm/source/IoState_eval.c` | Entry points (doCString, runCLI) |
| `libs/iovm/source/IoState_inline.h` | Inline helpers, pre-eval arg access |
| `libs/iovm/source/IoState.h` | VM state, pools, cached symbols |
| `libs/iovm/tests/correctness/EvalFrameTest.io` | Frame introspection tests |
| `libs/garbagecollector/source/Collector_inline.h` | RC increment/decrement (optional) |
| `agents/C_STACK_ELIMINATION_PLAN.md` | Architecture design document |
| `agents/CONTINUATIONS_TODO.md` | Phase tracker and implementation notes |
