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
9. **A tiering JIT via module generation and table patching.** WASM forbids writing executable memory, but it permits the two ingredients a method-granularity JIT actually needs. First, new code can be created at runtime: the VM emits wasm bytes for a hot Io method into linear memory, and JS glue compiles and instantiates them as a fresh module importing the VM's *own* memory and function table. Second, **table slots are mutable** — `Table.set()` is the legal form of code patching. Hot call sites dispatch through `call_indirect` on a table index; the JIT swaps in the compiled function, and the next send lands in machine code reading the same heap.

   The supporting cast: the VM runs in a Web Worker because the main thread refuses synchronous compilation of modules beyond a few KB (workers have no such limit). Inline caches are data-driven (loaded from memory and compared) since instruction-level patching is impossible — a few cycles slower than patched ICs, and what production wasm-hosted runtimes do anyway. Accumulated one-method modules pay cross-module call overhead and block cross-function inlining, so a background pass periodically recompiles everything hot into one consolidated module (`WebAssembly.compile` is async) and swaps it at a safe point.

   The stackless design is what makes the swap — and tiering in general — cheap: all execution state lives in heap frames, so between eval-loop steps the native stack is empty. Attaching a new instance to the same `WebAssembly.Memory` and continuing the loop *is* the state migration. The same property gives near-free on-stack replacement (tier up a hot loop by rewriting its heap frame's resume target) and a ready-made deoptimization target (fall back to the frame machine when a guard fails).

### Sequencing

Items 2 and 3 are the foundation and pay for themselves independently; 4–6 build on the bytecode representation; 7 is orthogonal and can proceed in parallel; 9 builds on all of it and only makes sense once the interpreter itself is no longer the bottleneck.

## Expected Performance, Honestly

How do the two paths compare, assuming both are pushed hard?

- **Hot numeric code.** The JS path inherits V8's speculative unboxing, escape analysis, and inlining for free — type feedback and deoptimization are the engine's job. The WASM JIT's output is compiled well by the engine, but the engine only compiles what we emit: it won't speculate on Io types or elide our tag checks. A first-generation template JIT emitting generic tagged-value code trails a mature JS path by 3–10×; closing that gap means building our own type-feedback and specialization machinery, at which point specialized wasm can match or slightly beat compiled-to-JS Io.
- **Send-heavy code** (the typical Io workload). Data-driven ICs and pre-consolidation cross-module calls leave the JS path ahead by 1.5–3× for a long time.
- **Allocation-heavy code.** The WASM path's structural edge: tagged immediates mean arithmetic allocates nothing, and objects are a fraction of the size of V8 objects. At maturity this is worth perhaps 1.2–2× — not the 5–10× one might hope.
- **Bridge- and DOM-heavy code.** The JS path wins by an order of magnitude, permanently — there is no boundary.

Rough scorecard against the current interpreter on the [micro set](/bench/): a mature JS path lands around 10–30×; a first-generation WASM JIT around 3–8×; a fully mature WASM JIT overlaps the JS path's range, ahead on allocation and numeric work, behind on bridge-heavy work. Per unit of engineering effort it is not close: closure compilation plus `new Function` codegen is perhaps 15% of the work of bytecode + profiler + type feedback + template JIT + specialization + generational GC + deopt paths. Part of the JS path's value is that it cheaply establishes how fast Io's semantics can go when a world-class JIT does the lifting — the honest bar any custom backend must clear.

## The Suspension Problem

Coroutines, `callcc`, and resumable exceptions are where both JIT stories get uncomfortable, because the optimization that makes calls fast — using the host's native stack — is the same one that makes suspension expensive. The current interpreter is near-optimal on this axis: a coroutine switch is a couple of pointer swaps and `callcc` capture is O(1), because frames already live on the heap. The [bench suite](/bench/) tracks this directly (`coroutineSwitches` in the micro set, `cheapconcurrency` in the program set) so neither path can regress it silently.

The options, per path:

- **JS.** Keeping the frame machine under closure-compiled code preserves O(1) switches but forces every send back through the trampoline, capping the JIT win on call-heavy code at perhaps 3–5×. JS generators are the wrong shape entirely: suspension costs O(stack depth) — every frame yields individually — with heap-heavy generator objects and inlining barriers at every yield point.
- **WASM.** Compiled code cannot read its own stack, so reifying frames at the moment of suspension requires cooperative unwinding — each function spills its locals and returns a suspending sentinel — the same O(depth) shape as generators with better constants. The **stack-switching proposal (WasmFX / typed continuations)** would change the game: first-class O(1) stack switching would let compiled code use the real stack *and* switch cheaply. It is not yet shipped; the shipped alternative (JSPI) routes suspensions through the JS event loop, far too slow for fine-grained switching.
- **Resumable exceptions are the cheap case in both.** `signal`/`withHandler` doesn't unwind: handler lookup walks a heap-resident handler stack and the handler runs as an ordinary call. Compilation doesn't disturb it. The expensive citizens are `callcc` and coroutine switches — and serializable, network-transmittable continuations *require* the heap representation regardless; no stack-based scheme can deliver them.

The design conclusion is the same for both paths: **the heap-frame machine stays the canonical representation**; compiled code is an optimization for regions that don't suspend, entered through guards and abandoned via deoptimization when suspension occurs. Actor-style code that switches constantly runs near today's speeds; straight-line and numeric code gets the full JIT win; and code that is both send-dense and suspension-prone keeps perhaps 2–5× rather than 10–30× — in either path — unless WasmFX ships and tilts the board toward the WASM side.

## Choosing

The paths aren't exclusive — the JS interpreter is also the best way to discover how fast Io's *semantics* can go when the world's best dynamic-language JITs do the heavy lifting, which sets an honest target for the WASM VM. If one must come first: the JS path has the higher ceiling in the browser and deletes the most code (collector, bridge, WASI shim); the WASM path keeps a single implementation and runs everywhere wasmtime does.
