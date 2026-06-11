# Performance

A plan for making Io substantially faster, along two implementation paths: a pure JavaScript interpreter, and a more aggressively optimized WASM VM.

## Where Time Goes Today

The [Performance page](/bench/) tracks the current numbers: roughly 8–13 million slot accesses per second and ~2.3 million instantiations per second on Apple Silicon under wasmtime. Profiles of the current VM show the cost is spread across a few structural sources rather than one hot spot:

- **Message-tree walking.** Evaluation chases pointers through heap-allocated message nodes. Every send touches the message, its data, the target's tag, and the slot table — poor cache locality by construction.
- **Boxed numbers.** Every `Number` is a full heap object. `1 + 2` allocates, and arithmetic-heavy loops spend much of their time in the allocator and collector rather than adding.
- **Monomorphic send caches.** Each message node caches one `(tag, slotValue, slotVersion)` triple. Polymorphic call sites miss every time the receiver type alternates, falling back to a cuckoo-hash slot walk up the proto chain.
- **Frame traffic.** The stackless evaluator allocates (pooled) frames and pre-evaluates arguments through a state machine. This bought continuations, portable coroutines, and clean unwinding — but each step pays dispatch overhead a native stack would not.

Both paths below attack these same costs with different tools. Progress on either is measured the same way: the correctness suite is the contract, and every change lands as a point on the [benchmark history](/bench/).

## Path 1: A Pure JavaScript Interpreter

### Why

The browser is now Io's primary target, and a JS implementation changes the economics:

- **The JIT and GC come for free.** Modern JS engines are among the most heavily optimized dynamic-language runtimes ever built. An interpreter that stays on their happy path inherits inline caching, hidden classes, and a generational collector without writing any of it.
- **The bridge disappears.** Io↔JS calls become plain function calls. No handle table, no copying strings across a linear-memory boundary, no WASI shim. A JS value is an Io value.
- **Smaller and more debuggable.** No 1.4 MB wasm download; DevTools profiles the interpreter directly.

The risk is the ceiling: performance depends on staying monomorphic from the engine's point of view. That constraint shapes the whole design.

### Design

1. **Shape-stable object model.** Every Io object is a JS object with a fixed field layout (`slots`, `protos`, `data`) created by one constructor, so the engine assigns a single hidden class to all of them. Slot tables are plain objects or `Map`s depending on what profiles show; never mix the two.
2. **Unboxed primitives.** Io `Number` is a raw JS number; `Sequence` (immutable) is a JS string; `true`/`false`/`nil` are singletons. A `typeof` check replaces tag dispatch on the hot path, and the engine inlines it.
3. **Closure compilation instead of tree-walking.** Each message chain is compiled once into nested JS closures — the dispatch structure is decided at compile time and the engine sees straight-line calls it can inline. This is the classic tree-walker-to-closure transformation and typically yields 5–20× by itself. The compiled form is cached on the message node and invalidated the same way the current inline cache is.
4. **Port the frame machine, not the recursion.** Coroutines, `callcc`, and resumable exceptions port directly: the heap-allocated frame state machine from the stackless work is plain data and works identically in JS. No generators, no async/await coloring — the eval loop is a loop.
5. **Slot-version inline caches.** Keep the per-send-site cache keyed by proto shape with version invalidation on slot writes — the design already exists in the C VM; in JS it composes with the engine's own caches rather than fighting them.
6. **Hot-block codegen (optional tier).** Count block activations; above a threshold, emit JS source for straight-line Io and compile it with `new Function`, guarded by receiver-shape checks. Falls back to the closure interpreter under CSP restrictions that forbid runtime codegen.
7. **GC and weak references for free.** Drop the tri-color collector entirely; `WeakLink` maps to `WeakRef` + `FinalizationRegistry`.

### Milestones

1. Lexer/parser + core object model; smoke tests pass under Node.
2. Full correctness suite (238 tests) passes — the same suite the C VM runs, unmodified.
3. Benchmark parity with the WASM VM on the [micro set](/bench/).
4. Closure compilation lands; target 3–5× the WASM VM's current send rates.
5. Hot-block codegen; revisit targets with profile data.

The bench harness already supports this: `bench/run.mjs` can drive a Node entry point instead of wasmtime, and history entries carry a label per series, so the JS interpreter shows up as its own line on the chart from day one.

### Risks

Two VMs can drift semantically — the correctness suite is the only defense, so it grows with every divergence found (the lazy-argument and special-form bugs found in the stackless evaluator are exactly the kind of thing that bites here). Deopt cliffs in the engine need profiling discipline: one megamorphic site or one accidental shape change can silently halve throughput.

## Path 2: A Highly Optimized WASM VM

The C VM keeps full control of memory layout and pays no JS-engine unpredictability — the optimizations are classical interpreter engineering, applied under WASM's constraints (no runtime codegen in-module, no computed goto).

### Plan

1. **Measure first.** Wire up `wasmtime --profile` and browser CPU profiles; every item below is gated on showing up in a profile, and every change lands as a point on the [benchmark history](/bench/).
2. **Unbox numbers with tagged immediates.** Use low-bit pointer tagging so small numbers live in the pointer itself: arithmetic stops allocating, and the collector stops tracing number objects. This is the largest single win available and touches a contained set of macros (`IONUMBER`, `CNUMBER`, tag checks) plus the collector's mark path.
3. **Compile message trees to linear bytecode.** Walk the (already operator-shuffled) message tree once, emit a compact bytecode array, and cache it on the block. The eval loop reads sequential memory instead of chasing node pointers — better locality, cheaper dispatch, and a natural place to do the next item.
4. **Superinstructions.** Fuse the pairs profiles say are hot: slot-access+activate, compare+branch in `while`, assignment forms. The special-form/lazy-args machinery already identifies most of these sites.
5. **Threaded dispatch via WASM tail calls.** The tail-call proposal is supported in all major engines and wasmtime; replacing the central `switch` with mutually tail-calling opcode handlers removes the dispatch-loop branch mispredictions that dominate interpreter profiles.
6. **Polymorphic inline caches.** The per-message cache is monomorphic; extend it to a small PIC (2–4 entries) keyed by tag, and replace the global `slotVersion` with per-object shape versions so unrelated slot writes stop invalidating every cache.
7. **Generational GC.** A bump-allocated nursery in front of the existing tri-color collector. Most Io objects die young (locals, intermediate messages, argument frames); generational collection turns most of the current mark/sweep work into pointer-bump allocation.
8. **Toolchain.** `-O3` + LTO, a `wasm-opt` pass over the binary (typically another 5–15%), and relaxed-SIMD for `Vector` where it helps.
9. **Runtime codegen, eventually.** WASM modules can't generate code into themselves, but the host can: compile hot Io methods to small fresh wasm modules at runtime (`WebAssembly.instantiate` of generated bytes) and link them in. A real JIT without leaving the sandbox — speculative, large, and explicitly last.

### Sequencing

Items 2 and 3 are the foundation and pay for themselves independently; 4–6 build on the bytecode representation; 7 is orthogonal and can proceed in parallel; 9 only makes sense once the interpreter itself is no longer the bottleneck.

## Choosing

The paths aren't exclusive — the JS interpreter is also the best way to discover how fast Io's *semantics* can go when the world's best dynamic-language JITs do the heavy lifting, which sets an honest target for the WASM VM. If one must come first: the JS path has the higher ceiling in the browser and deletes the most code (collector, bridge, WASI shim); the WASM path keeps a single implementation and runs everywhere wasmtime does.
