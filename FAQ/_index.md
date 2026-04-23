# FAQ

Common questions about Io — its origins, philosophy, and practical use.

## Who created Io, and when?

Io was created by Steve Dekorte, starting in 2002, as an experiment in seeing how small a language could be while still expressing ideas borrowed from Smalltalk, Self, NewtonScript, Act1, and Lisp.

## What makes Io different from other languages?

Io has no keywords and no statements — only expressions, and every expression is a message send. Assignment, conditionals, and loops are ordinary methods that receive their arguments as unevaluated message trees, which keeps the core tiny and makes control flow and DSLs first-class citizens of the language.

Every object is a prototype. There are no classes, no distinction between instances and templates, and no hidden machinery for inheritance — any object can be cloned and any clone can be used as a proto for further objects.

Concurrency is built around actors and futures rather than threads. Any object can receive `asyncSend` or `futureSend` messages and run its own coroutine; futures are transparent (they become their result when touched) and the VM detects deadlocks by walking the future graph.

The evaluator is stackless — execution runs on a heap-allocated frame state machine instead of the C stack — enabling first-class continuations (`callcc`), portable coroutines without platform-specific assembly, and robust exception unwinding without `setjmp`/`longjmp`.

## Is Io production-ready?

The classic Io VM is small, stable, and has been used in shipping products, embedded devices, and research. It is not trying to compete with JavaScript or Python on ecosystem size — it is a language for people who value expressiveness, simplicity, and runtime flexibility over a large standard library or raw throughput.

The new WASM target (and the stackless evaluator it is built on) is not yet well tested — treat it as early-access until it has more mileage.

## How is Io different from JavaScript, which is also prototype-based?

JavaScript has classes, `new`, statements, keywords, and a sharp distinction between primitives and objects. Io has none of these. Inheritance is differential — clones share their proto's slots until written — and multiple inheritance falls out naturally because every object carries a list of protos.

Control flow is not built in. In Io, `if`, `while`, and `for` are regular methods written in Io itself, made possible because messages receive their arguments as unevaluated message trees.

Concurrency is coroutine-based rather than callback- or `async`/`await`-based. There is no function coloring — any method can yield, and any object can be sent asynchronously with `@` (futures) or `@@` (fire-and-forget). Code that waits on a future reads like straight-line code; there is no separate async function type or await keyword to propagate through call sites.

The evaluator is stackless — execution runs on heap-allocated frames rather than the C stack — so Io also has first-class continuations (`callcc`), something JavaScript does not expose at all.

## Why a WASM/WASI target?

WebAssembly gives Io a single portable binary that runs under wasmtime, Node.js, or a browser — without platform-specific build matrices, coroutine assembly, or native addons. Hosting Io in a browser unlocks direct DOM access, and the bidirectional Io↔JavaScript bridge replaces the old native-addon model with something that works the same everywhere the host does.

## Can Io still embed in a C or native application?

Yes — a WebAssembly module is an embeddable binary by design. You can run the Io VM inside a browser, inside Node.js, or inside a native application via a WASM runtime like wasmtime or wasmer. The VM is BSD-licensed.

## How does concurrency work?

Every object can receive async messages (`@@`) or future messages (`@`) that run in their own coroutine. Coroutines are cooperative user-level tasks — not OS threads — so a single Io process can run thousands of them at once. Futures are transparent: the caller keeps running until it tries to use the result, then blocks only if the result isn't ready yet.

## Where does the name "Io" come from?

A combination of searching for short two-letter names, childhood memories of the Voyager photos of Jupiter's moon Io, and the I/O towers from the movie *TRON*.

## How do I contribute?

The source lives at [github.com/IoLanguage/io](https://github.com/IoLanguage/io). Open issues, send pull requests, or just clone and experiment.
