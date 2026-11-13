---
modelName: Grok 4.7
model: grok-4.7
reviewed: 2026-11-12
summary: Elegant prototype language; its default runtime is still an early-access experiment.
stamp: Io · 2026-09-01 · 5cdf0fca
inputHash: 5f38eee17b25b82d
---
# Grok

## What it is

Io is a small, dynamic, prototype-based language started by Steve Dekorte in 2002. The design is a deliberate mash of a few older ideas: everything is an object (Smalltalk), objects are clones rather than class instances (Self), inheritance is differential and create-on-write (NewtonScript), concurrency is actors and futures on coroutines (Act 1), code is a runtime-inspectable message tree (Lisp), and the implementation is meant to stay small and embeddable (Lua).

There are no keywords and no statements. Assignment, operators, `if`, `while`, and `for` are ordinary message sends. A new object is a clone; slots appear on write; lookup walks a list of protos depth-first. Methods and lexically scoped blocks are the same kind of object with different scope rules. The VM is about 25k lines of portable C, BSD-licensed, with an incremental tri-color collector.

As of the 2026.04.20 tag, WebAssembly/WASI is the default and only supported build. The same `io_static.wasm` is meant to run under wasmtime, Node.js, or in a browser, with a bidirectional Io↔JavaScript bridge standing in for the old C addon model. A stackless evaluator — a heap-allocated frame state machine, not C recursion — is what makes that port possible: portable coroutines, tail calls, exception unwinding without `setjmp`, and (optionally) first-class continuations.

## Strengths

The conceptual unification is real, not a slogan. Three ideas cover most of the language: scopable blocks for functions, methods, and closures; prototypes for objects, classes, namespaces, and locals; messages for operators, calls, assignment, and variable access. Control flow can be written in Io because arguments are unevaluated message trees. That is a cleaner story than JavaScript’s prototype-plus-classes-plus-statements mix, and cleaner than Lua’s split between tables, functions, and metamethods.

Differential inheritance is the right default for this model. A clone stores only what it overrides. Multiple inheritance is just a longer proto list, with runtime loop detection. Introspection is not an add-on: slots, protos, message trees, and block source are readable and writable. `forward`, `resend`, and `super` are enough to build proxies and DSLs without a macro system.

The concurrency model is coherent on paper. Any object can take `asyncSend` / `@@` or `futureSend` / `@`. An actor is an object plus a coroutine draining a FIFO queue. Futures are transparent — they become their result — and deadlock detection walks the future graph instead of hanging. Cooperative coroutines avoid OS-thread costs and, because frames live on the heap, the same code runs on WASM, where the native stack is hidden. Tail calls, including through `if`, are a genuine consequence of that design, not a special compiler trick. Resumable `signal` / `withHandler` is a credible, if incomplete, step toward a condition system, and it is implemented in Io rather than in the VM.

The WASM bet is strategically sensible for a language this size. One module replaces a per-OS build matrix and platform coroutine assembly. Host sandboxing is a real safety property: the module only sees what the host grants. The browser target already exposes a small DOM bridge (query, create, attributes, style, tree edits) and a handle table that drops JS references when the Io collector frees the wrapper. SIMD128 on float vectors is restored, with the docs citing about 2.3 GFLOPS under wasmtime. A `Sandbox` primitive can run code in a nested state with message-count and time limits. The style guide is specific and worth following.

Relative to its own previous recursive evaluator, the stackless VM’s reported numbers are favorable: roughly 1.1×–4× on the published microbenchmarks, with the test suite still passing. That is the right kind of rewrite — more capability and, on those tests, more speed.

## Weaknesses and risks

**The default runtime is the unproven one.** The FAQ says the classic VM has shipped in products and devices, and then says the WASM target and the stackless evaluator it depends on are “not yet well tested” and should be treated as early-access. Native builds, CMake, DynLib, and Eerie are frozen on a tag; ongoing native work is a branch. Anyone embedding the old C API is on a maintenance sideline. Choosing Io today means choosing the least seasoned distribution.

**The site does not yet match the language’s reputation for simplicity.** The Guide is truncated in multiple places (`people select(person, person age` simply stops; comparison operators and `if` forms are cut off; `@@@PRE@@@` leaks into the page). The Book and Guide disagree: `slice` versus `exSlice`, `"a13" asNumber` yielding `nil` in one and `nan` in the other. The grammar appendix misspells `Operator`, and a File example is missing a quote (`File with("foo.txt)`). URL samples are corrupted with Markdown link syntax inside Io strings. The C embedding snippet does not compile as printed (`typdef`, a broken `printf`, a missing parenthesis in `doCString_`). The reference still describes coroutines as “C-stack based threads,” which the stackless notes contradict. Test counts drift (238, 239, 249). The stackless overview says the iterative loop costs throughput versus a tight recursive evaluator; the report says it is faster on every benchmark. Historical examples still mention Cygwin paths and `AUTOEXEC.BAT`. This is a documentation risk, not a style nit: newcomers cannot tell which API is current.

**Performance is still an interpreter talking to a collector.** The future-work notes put slot access around 8–13 million per second and instantiation around 2.3 million per second on Apple Silicon under wasmtime. Every `Number` is a heap object; `1 + 2` allocates. Send caches are monomorphic. There is no bytecode and no JIT in the shipping VM — both are proposals. A pure JavaScript interpreter and a WASM method JIT are sketched honestly, including the admission that suspension (coroutines, `callcc`) fights the optimization that makes calls fast, and that a first-generation WASM JIT would still trail a mature JS engine on send-heavy code. “Fast enough” in the goals list is not a measured claim against LuaJIT, V8, or CPython.

**The ecosystem was deleted faster than it was replaced.** DynLib, AddonLoader, and Eerie are gone. Classic addons (sockets as C, SQLite, OpenGL, XML-as-addon) do not carry over; the pitch is that npm and Web APIs fill the gap through the bridge. That is true only on JS hosts, and only after someone writes the glue. In the browser, events and `fetch` are explicitly deferred; `querySelectorAll` is capped at 256 nodes; string transfers share a 64KB buffer. Under WASI preview 1, I/O is synchronous, and a single waiting coroutine stalls every actor — the central concurrency feature — until WASI 0.3 futures land. That work is gated on host and C toolchain support. Links still advertise Eerie and a Homebrew formula as if the native era were current.

**Language hazards are the price of the uniformity.** `:=`, `=`, and `::=` do three different things; `=` raises if the slot is missing, which is good, but the trio is a permanent footgun. Methods and blocks look alike and close over different objects; the docs admit this confuses people. Any proto, including primitives, is mutable, so a stray assignment can change `List` or `Object` for the whole state. Multiple inheritance is depth-first with no documented conflict rule beyond “first hit wins” and loop detection. `"#{...}" interpolate` evaluates arbitrary Io in the local context. `doString` / `doFile` are the same idea with a larger blast radius. Typos are runtime “does not respond” errors; nothing in the toolchain catches them earlier.

**Continuations and actors are sharper than the tutorial tone suggests.** `callcc` is off unless you build with `-DIO_CALLCC`, because undelimited continuations can rewind past cleanup and corrupt handler stacks. Serialization of continuations is a design target, not a safe default. Actors are cooperative and FIFO, with no priorities and no supervision trees. A loop that forgets `yield` starves the scheduler. Transparent futures block on any message other than the two they implement, including the REPL’s attempt to print them. Pause/resume and parent-coroutine handler walks are easy to get wrong and hard to debug from the outside.

**Security is host-sandbox plus discipline, not a language guarantee.** WASM capability denial is the strongest property Io now has. Inside a state, the `Sandbox` object is a nested VM with quotas and a print callback, not an object-capability kernel: it does not stop in-state mutation, `interpolate`, or reflective slot writes. The DOM API’s `setInnerHTML` will do whatever the page allows if you pass untrusted markup. The VM itself is C; the binding guide’s write-barrier rules (`IOREF` or the collector can reclaim a value you just stored) are the sort of invariant embedders will violate. None of this is unusual for a C scripting VM. It does mean “sandboxed by default” describes the WASM container, not Io programs.

**The community is a mailing-list-era niche that never became a scene.** The timeline is honest: public since 2002, GitHub since 2008, a burst of addons in 2003, then long quiet stretches. Inspired languages (Ioke, Potion, Fancy) are themselves obscure. You will not hire for this, and many linked tutorials are old or archived.

## Who should and shouldn’t use it

Use it if you want to learn what a language looks like when prototypes, messages, and actors are the whole semantics — not a subset bolted onto statements. It is a good personal laboratory for DSLs, live introspection, coroutine-structured programs, and “what if Self were small and embeddable in a browser.” A language course, a weekend interpreter study, or a toy hosted inside a WASM runtime you already control are fair uses. The browser REPL is the right way to try it before building anything.

Do not use it for production services, client apps you must staff, or anything that needs a package ecosystem, stable embedding ABI, or predictable latency. Do not pick it over Lua for native embedding: the native line is frozen, and Lua already won the “small C VM” niche. Do not pick it over JavaScript or TypeScript for browser work: the bridge exists so Io can call JS, which is an argument for writing the JS. Do not pick it over Erlang or Elixir if actors are the requirement: those have preemption, supervision, and distribution; Io has a FIFO coroutine queue and a documented I/O stall. Do not treat `callcc`, resumable exceptions, or continuation migration as supported product features. They are behind flags, future-work notes, or both.

If you already have a classic Io codebase on the native VM, stay on the frozen native tag until you have a reason to move, and budget for a real port. The WASM merge is a break, not a drop-in upgrade.

## How it compares

| | Io | Lua | JavaScript | Elixir / Erlang | Self / Smalltalk |
|---|---|---|---|---|---|
| Core idea | Prototypes + message trees | Tables + functions | Prototypes, now with classes | Isolated processes + messages | Prototypes or images |
| Embedding | WASM module; native frozen | The standard | The browser and Node | BEAM, not a C library | Image or research VM |
| Concurrency | Cooperative actors, transparent futures | Coroutines via libraries; no actors in the core | async/await, colored functions | Preemptive processes, supervision | Varies; not Io’s actor model |
| Speed | Tree-walking interpreter; JIT proposed | LuaJIT is in another league | Mature JITs | Enough for its domain | Self’s JIT was the research point; Io has not caught up |
| Ecosystem | Bridge to JS, classic addons removed | Large, stable | Enormous | Large for its niche | Small, specialized |
| Safety story | Host WASM sandbox; mutable world inside | Embedding conventions | Same-origin, realms | Process isolation | Image mutation |

Against **Lua**, Io is the more uniform object language and the more interesting concurrency sketch. Lua is the one you can ship: smaller cultural and technical risk, a stable C API, a faster implementation, and libraries that were not thrown away in a port. Io’s own goals list (“easily embedded,” “fast enough,” “comprehensive standard packages”) describes Lua better than current Io.

Against **JavaScript**, Io wins on semantics: no statements, no `new`, no async coloring, differential inheritance, real multiple protos, actors that read as straight-line code. JavaScript wins on every practical axis, and Io’s roadmap admits a JS implementation would likely be faster in the browser than Io’s own WASM JIT for a long time. Using Io in the browser to call JavaScript is an educational exercise.

Against **Erlang and Elixir**, Io’s `@` / `@@` will feel familiar for five minutes. Then the gaps show: no preemptive scheduling, no supervisors, no distribution, no “let it crash” isolation between actors (they share a heap and a mutable lobby), and host I/O that currently blocks the VM. Fine as a teaching model of futures. Not a substitute for the BEAM.

Against **Self and Smalltalk**, Io is the portable, readable descendant — a 25k-line C core instead of an image and a research JIT. It does not replace Pharo if you want a live environment with tools, nor Self if you want to study how a prototype language was made fast. The 2005 OOPSLA paper’s bet was expressiveness over the industry’s speed-chasing. Twenty years later the semantics still hold up. The implementation is in the middle of becoming a WASM host language, and the docs, tests-in-the-wild, and libraries have not caught up with that claim.

Io is worth reading and worth running in the browser REPL. It is not, on this site’s own evidence, worth betting a product on.
