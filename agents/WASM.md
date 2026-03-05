# Claude Code Prompt: Io Language WASM Migration

You are working on the **Io programming language** (github.com/IoLanguage/io), a dynamic prototype-based language inspired by Smalltalk, Self, and Lisp. The codebase has recently been updated on the **stackless branch** to be independent of the C stack, which is a critical prerequisite for the work described below.

## Strategic Direction

The goal is to **target WASM as Io's only platform**, eliminating all OS-specific dependencies and multi-platform C code. This dramatically simplifies maintenance and leverages WASM as a universal runtime. The primary target is **WASI** (using Wasmtime with Cranelift JIT) with browser as a secondary target.

## Key Architectural Decisions Already Made

**Toolchain**: Compile Io's existing C implementation to WASM using wasi-sdk. Io's C codebase is already fairly portable and the stackless work has eliminated the biggest porting hazard (setjmp/longjmp and stack manipulation).

**Memory**: Use WASM's `memory.grow` for dynamic heap expansion in 64KB pages rather than a static allocation. Start with ~16-32MB, grow on demand, no artificial ceiling.

**Garbage Collector**: Keep Io's existing `libgarbagecollector` — an incremental tricolor Baker Treadmill collector — as the primary collector. Do **not** switch to WASM GC, whose nominal type system is a poor fit for Io's dynamic prototype-based object model.

**External references** (browser target): Use `externref` for holding JS/DOM object references from within WASM. Use `FinalizationRegistry` on the JS side as a safety net, with Io's existing object `free` hook as the primary eager cleanup path. Use `WeakRef` for non-owning JS→WASM references to avoid cycles.

**API bindings**: Replace platform-specific C bindings with WASI imports for the server target. For the browser target, expose DOM/fetch/WebSockets as WASM imports through a thin JS glue module. Long term, target the WASI component model for typed cross-language interop.

## Phase 2: Compacting Collector (Future Work, Design Agreed)

This is a **phase 2 feature** — not part of the initial WASM migration. The design below has been agreed upon and should inform how you structure phase 1 code — avoid making architectural decisions that would make this harder to add later, but do not implement any of it during the initial WASM port:

**New GC hook needed**: A `remap` callback alongside the existing `mark` and `free` callbacks. Where `mark` visits references, `remap` must be able to **write back** updated addresses to pointer fields. The cleanest implementation is a single `walkRefs` function per primitive type that accepts a visitor, with both `mark` and `remap` implemented as thin wrappers over it:

```c
void IoObject_walkRefs(IoObject *self, IoRefVisitor *visitor, void *ctx);
void IoObject_mark(IoObject *self);   // wraps walkRefs with mark visitor
void IoObject_remap(IoObject *self);  // wraps walkRefs with remap visitor
```

Only a small number of Io primitives contain object references and need `walkRefs` — `IoObject`, `IoList`, `IoMessage`, `IoCoroutine`. Leaf primitives (`IoNumber`, `IoSeq`, `IoDate`, `IoFile`) contain no object references and need only a `memmove` during compaction.

**50% boundary policy**: Compaction only moves objects **above the 50% memory mark** to lower addresses. Objects below 50% are never moved. This preserves JIT optimization assumptions for hot long-lived objects in the lower half, while allowing the upper half to act as a nursery zone. The midpoint should be dynamic:

```c
size_t midpoint = max(wasmMemorySize() / 2, liveObjectBytes() * 1.2);
```

**Recycling interaction**: `libgarbagecollector` has a `COLLECTOR_RECYCLE_FREED` mechanism (currently disabled via a commented-out `#define`). When compaction is added, recycling should only operate on freed slots **above the 50% boundary**, keeping the lower half stable and dense.

**Compaction trigger**: Only fire under memory pressure, before calling `memory.grow`. Sequence: compact upper half → if still above 80% full → grow. This makes compaction a pressure relief valve rather than a routine operation.

**JIT consideration**: Compaction invalidates JIT-learned access patterns. The 50% policy mitigates this by never moving JIT-hot lower-half objects. Compaction should be infrequent and ideally triggered between activity bursts.

**Treadmill color as hotness proxy**: The Baker Treadmill's off-white segment (tenured/long-lived objects) naturally identifies hot objects without any separate heat tracking. During compaction, off-white objects in the lower half should be left in place.

## Your Task

Please explore the Io codebase — particularly `libs/iovm/source/`, `libs/libgarbagecollector/`, and the build system — and produce a **detailed implementation plan** for the WASM migration with the following structure:

1. **Audit**: What platform-specific code exists and where? What are the biggest obstacles to compiling with wasi-sdk? What does the current build system need to change?

2. **Dependency map**: Which addons and libraries have native dependencies that need replacing with WASI equivalents or pure-WASM implementations? Which can simply be dropped?

3. **Phased plan**: A concrete sequence of steps from current state to a working WASI build, with clear milestones and an estimate of difficulty for each phase.

4. **GC preparation**: Identify where `walkRefs` needs to be implemented and flag any existing code that would complicate adding the `remap` callback later. Do not implement compaction yet — just make sure the path is clear.

5. **Risk assessment**: What are the hardest parts? Where are the most likely surprises?

Do not start implementing yet. Produce the plan first so it can be reviewed before any code changes are made.
