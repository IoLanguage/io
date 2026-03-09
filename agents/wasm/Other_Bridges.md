# WASM-to-JS Bridge Survey

A comparison of how languages targeting WebAssembly handle their JavaScript interop boundary.

## Design Choice Matrix


|                           | Rust (wasm-bindgen)                        | Go (syscall/js)               | C# / Blazor                 | Python (Pyodide)                        | Ruby (ruby.wasm)                 | Lua (wasmoon)             | AssemblyScript             | Kotlin/Wasm                    | Swift (JavaScriptKit)       | Zig                         | **Io**                                  |
| ------------------------- | ------------------------------------------ | ----------------------------- | --------------------------- | --------------------------------------- | -------------------------------- | ------------------------- | -------------------------- | ------------------------------ | --------------------------- | --------------------------- | --------------------------------------- |
| **JS obj repr in WASM**   | u32 index into heap/slab                   | NaN-boxed u64 ref             | GCHandle proxy              | JsProxy wrapper                         | JS::Object (externref or handle) | userdata                  | pointer into linear mem    | JsAny (WasmGC native)          | u32 handle table            | NaN-boxed u64 or handle     | u32 handle table (jsHandles)            |
| **WASM obj repr in JS**   | class with `__wbg_ptr`                     | via `_pendingEvent` callback  | GCHandle proxy              | PyProxy wrapper                         | via `vm.eval` / RbValue          | auto-converted table      | opaque ref-counted pointer | JsReferenceT (frozen, opaque)  | JSClosure / BridgeJS export | manual                      | Proxy with io_send                      |
| **String encoding**       | UTF-8, copied (ptr+len)                    | UTF-8, copied (TextEncoder)   | copied                      | copied                                  | copied                           | copied                    | UTF-16LE, copied           | copied                         | copied                      | UTF-8, copied               | UTF-8, copied (binary protocol)         |
| **Number passing**        | direct WASM numeric types                  | NaN-boxed float64             | direct                      | auto-convert                            | wrapped as JS::Object            | auto-convert              | direct WASM types          | direct (Long→BigInt)           | f64 in JSValue enum         | direct WASM types           | f64 in binary buffer                    |
| **Bool passing**          | u32 (0/1)                                  | NaN-boxed constant            | direct                      | auto-convert                            | wrapped (JS::True/JS::False)     | auto-convert              | direct                     | direct                         | JSValue.boolean(Bool)       | manual                      | byte tag (TYPE_TRUE/FALSE)              |
| **null vs undefined**     | OptionT for null                           | separate Value constants      | n/a (C# has null only)      | both → None                             | JS::Null, JS::Undefined          | null→userdata (truthy!)   | null only                  | Unit→undefined                 | .null, .undefined in enum   | n/a                         | nil + JsTypes undefined singleton       |
| **Array/List**            | copied (Vec↔Array)                         | copied ([]interface{}→Array)  | copied                      | **proxied** (mutable)                   | proxied                          | copied (table↔Array)      | copied                     | copied                         | handle-based                | manual                      | deep-copied (List↔Array)                |
| **Map/Dict/Object**       | n/a (use JsValue)                          | copied (map[string]→Object)   | proxied                     | **proxied** (mutable)                   | proxied                          | copied (table↔Object)     | n/a                        | n/a                            | handle-based                | manual                      | deep-copied (Map↔Map)                   |
| **BigInt**                | supported (u64/i64)                        | not supported                 | not supported               | auto-convert (large int)                | not documented                   | not documented            | supported (i64/u64)        | Long/ULong→BigInt              | JSBigInt type               | not documented              | **rejected with error**                 |
| **TypedArray**            | view into WASM memory (zero-copy possible) | not special                   | optimized byte[]            | buffer↔TypedArray                       | not documented                   | shared memory view        | ArrayBuffer-backed         | not special                    | not special                 | shared memory view possible | TYPE_TYPEDARRAY (copied)                |
| **Dispatch style**        | static (compile-time codegen)              | runtime (string-based)        | static (Roslyn source gen)  | runtime (proxy forwarding)              | runtime (.call(:method))         | runtime (metatable)       | static (bindings)          | static (compiler)              | both (BridgeJS + dynamic)   | manual                      | runtime (forward + binary protocol)     |
| **Glue code**             | generated by CLI tool                      | wasm_exec.js (600 lines)      | Roslyn source-generated     | Emscripten + custom proxy               | WASI shim                        | compiled Lua C + shim     | compiler --bindings flag   | compiler-generated             | generated (BridgeJS)        | manual                      | hand-written io.js                      |
| **GC coordination**       | FinalizationRegistry + Drop                | Go finalizer (SetFinalizer)   | dual GC (CLR + JS)          | FinalizationRegistry + manual destroy() | CRuby GC + externref             | Lua __gc metamethod       | custom GC in linear mem    | **browser VM GC (WasmGC)**     | Swift ARC + manual          | manual release()            | FinalizationRegistry + handle tables    |
| **Cross-boundary cycles** | leak                                       | leak                          | leak                        | **leak** (documented)                   | leak                             | leak                      | n/a (no cross-refs)        | **collected** (WasmGC)         | leak                        | n/a                         | leak (cycles detected at serialization) |
| **Error propagation**     | ResultT,E↔throw                            | JS throw→Go panic (no catch)  | JS throw→.NET exception     | JS→JsException, Py→PythonError          | limited                          | propagated with type info | manual                     | JsException (with stack trace) | JSException catch           | error unions                | TYPE_ERROR with message + handle        |
| **Promise/async**         | wasm-bindgen-futures                       | manual (channels + callbacks) | Task↔Promise automatic      | Future↔Promise automatic                | Asyncify (.await)                | coroutine :await()        | none                       | coroutines + PromiseT          | async/await↔Promise         | none                        | not yet implemented                     |
| **Binary wire protocol**  | no (import/export + glue)                  | no (import/export + glue)     | no (source-generated stubs) | no (Emscripten FFI)                     | no                               | no                        | no (bindings)              | no (WasmGC direct)             | no (handle table)           | no                          | **yes** (12 type tags, 64KB buffer)     |


## Detailed Notes

### Reference Strategies

Languages fall into three camps for how JS objects are represented in WASM:

**Handle tables** (most common): A JS-side array maps integer IDs to real objects. WASM holds only the integer. Used by Rust, Swift, Zig, and Io. Simple and portable.

**NaN-boxing**: Encode type + ID into 64-bit IEEE 754 NaN payloads. Used by Go and zig-js. Clever encoding that avoids separate handle lookups for numbers (they pass as-is), but limited to 32-bit IDs.

**WasmGC native references**: The browser VM manages both WASM and JS objects in a single GC heap. Used by Kotlin/Wasm. The only approach that can collect cross-boundary cycles. Requires compiler support for the WasmGC proposal.

### Deep Copy vs Proxy

The most consequential design split. When a JS Array is passed to WASM, is it copied or proxied?

**Deep copy** (Rust, Go, Lua, AssemblyScript, Io): The WASM side gets its own independent copy. Mutations don't propagate. Simpler, avoids cross-boundary cycles, but loses identity and mutation semantics.

**Proxy** (Pyodide, Ruby): The WASM side gets a proxy that forwards operations back to JS. Mutations propagate. Preserves semantics but creates cross-boundary reference cycles that neither GC can collect. Pyodide documents this as a known leak and recommends manual `destroy()` calls.

**Io's approach**: Deep-copy containers (List↔Array, Map↔Map) but proxy opaque JS objects (JSObject with handle). This is a pragmatic middle ground — containers that Io can natively represent are copied, while JS objects with methods (Math, DOM elements, etc.) are referenced.

### String Passing

Every surveyed language copies strings across the boundary. The UTF-8 (WASM) vs UTF-16 (JS engine internal) mismatch makes zero-copy impractical. TextEncoder/TextDecoder overhead is a universal bottleneck. Rust's Sledgehammer Bindgen batches string decoding operations for measurable speedups.

### GC Cleanup Mechanisms


| Mechanism                                          | Used by                                  | Deterministic?                                                 |
| -------------------------------------------------- | ---------------------------------------- | -------------------------------------------------------------- |
| FinalizationRegistry                               | Rust, Pyodide, Io                        | No (non-deterministic timing, won't run during sync execution) |
| Language finalizer (Drop, SetFinalizer, ARC, __gc) | Rust, Go, Swift, Lua                     | Depends on language GC                                         |
| Manual destroy()/release()/free()                  | Pyodide, Zig, AssemblyScript (pin/unpin) | Yes                                                            |
| WasmGC (browser VM manages both)                   | Kotlin/Wasm                              | Same as browser GC                                             |


FinalizationRegistry is standard but has documented unreliability — Cloudflare's experience shows callbacks can be delayed indefinitely under memory pressure. Pyodide's recommendation of explicit `destroy()` reflects production experience with this.

### Error Handling Spectrum

From most to least capable:

1. **Kotlin/Wasm**: Full exception objects cross the boundary with stack traces (requires WebAssembly.JSTag browser support)
2. **Rust**: `Result<T, JsValue>` catches JS exceptions as first-class Rust errors
3. **C# / Pyodide**: Exceptions wrapped in language-specific error types
4. **Io**: TYPE_ERROR carries message string + handle to original error object
5. **Go**: JS exceptions become Go panics — no structured catch mechanism

### Promise/Async Integration

Languages with first-class async handle this well (Kotlin coroutines, C# Task, Pyodide Future). Languages without must improvise:

- **Ruby**: Uses Asyncify (WASM stack unwind/rewind transform) to simulate blocking `.await` calls
- **Go**: Manual channel + callback wiring
- **Io**: Not yet implemented (planned as Future integration)

Asyncify is the heaviest approach — it transforms the entire WASM binary to support cooperative stack switching.

### The Binary Protocol Question

Io is unique among these languages in using a binary serialization protocol. Most languages use direct WASM import/export with per-value marshaling across the boundary.

Advantages of Io's binary buffer approach:

- Single WASM boundary crossing per complex operation (batch multiple values)
- Self-contained — no generated glue code dependency
- Protocol can evolve without recompiling (just add type tags)

Disadvantages:

- Extra serialization/deserialization step even for simple values
- 64KB buffer size limit
- No zero-copy path for large data

Most other languages cross the WASM boundary once per primitive value but generate glue code to minimize per-crossing overhead. Rust's wasm-bindgen and Kotlin's compiler optimize this to near-zero overhead for numeric types.

### Unique Design Choices Worth Noting

**Rust's two-tier heap**: Borrowed `JsValue` references use a stack (LIFO with auto-cleanup in `finally`), owned references use a slab with free-list recycling. Elegant separation of temporary vs persistent references.

**Go's NaN-boxing**: Packing type tags and IDs into NaN payloads avoids a separate handle table for the common case (numbers). But limits IDs to 32 bits.

**Pyodide's proxy-by-default for mutables**: The only language that preserves cross-boundary mutation semantics. Prioritizes correctness over simplicity, with documented leak risks.

**Pyodide's semantic conflict detection**: When converting a Map whose keys would have different equality semantics in Python vs JS (e.g., `True` and `1` are equal in JS but not Python), throws `ConversionError` rather than silently producing wrong behavior.

**Kotlin/Wasm's WasmGC**: The only approach where cross-boundary cycles can be collected. Represents the future for GC'd languages on WASM, but requires compiler-level support for the WasmGC proposal.

**AssemblyScript's three runtime variants**: incremental GC (default), minimal (manual `__collect()`), stub (bump allocator, never frees). Lets developers choose the GC trade-off per module.

**Ruby's truthiness footgun**: All JS values including `false` and `null` are truthy in Ruby because they're non-nil `JS::Object` instances. Developers must compare with `== JS::True`.

**Lua's null-as-userdata**: JS `null` becomes Lua userdata (truthy), distinct from Lua `nil`. Similar footgun to Ruby.

### Where Io's Bridge Sits

Io's bridge design is closest to **Swift's JavaScriptKit** in overall approach: handle table, runtime dispatch via `forward`, and hand-written glue. The binary protocol is unique.

Compared to the field:

- **More dynamic** than Rust, C#, Kotlin, AssemblyScript (compile-time codegen)
- **More structured** than Go, Ruby (raw string dispatch with no wire format)
- **Simpler GC story** than Pyodide (no explicit destroy(), no proxy lifecycle)
- **Appropriate for a dynamic language** — static binding doesn't suit prototype-based dispatch

The main area where Io could learn from others:

- **Promise integration** (Pyodide, Kotlin, C# all have good models)
- **Zero-copy TypedArray views** (Rust, Zig share WASM memory directly for numeric arrays)
- **WasmGC** as a long-term aspiration (eliminates GC coordination overhead entirely)

