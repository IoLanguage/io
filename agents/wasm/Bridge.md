# Io-JS Bridge

The Io VM runs as WASM. Io is the application layer — the brains of the program. JS provides low-level libraries, DOM access, and event dispatch. The bridge enforces a clean, predictable convention for data crossing the boundary.

## Files

| File | Role |
|------|------|
| `browser/io_js_bridge.h` | C header: proto factory, handle factory, GC hook |
| `browser/io_js_bridge.c` | C implementation: JSObject proto, serialization, ioHandles, WASM exports |
| `browser/io.js` | JS runtime: jsHandles, `js` import namespace, serialization, Io Proxy |
| `browser/io_browser.c` | Registers JSObject proto + JS singleton during `io_init()` |

## Architecture

```
+----------------------+                    +----------------------+
|  Io VM (C/WASM)      |   bridge_buf       |  JS Runtime          |
|                      |   (64KB shared)    |                      |
|  JSObject proto      |                    |  jsHandles Map       |
|    forward ----------+-- js_call -------> |    (any JS value)    |
|    get/set/call/at   |                    |                      |
|    typeof/type       |                    |  serialize/          |
|                      |<- result in buf ---|  deserialize         |
|  JS singleton        |   (value types     |                      |
|    handle=globalThis |    deep-copied)    |  Proxy factory       |
|                      |                    |    get -> io_send    |
|  ioHandles[] <-------+-- io_send --------|    set -> setSlot    |
|  (GC-marked roots)   |                    |  FinalizationRegistry|
|                      |-- result in buf -->|    -> io_release     |
|  serialize/          |                    |                      |
|  deserialize         |                    |                      |
+----------------------+                    +----------------------+
```

---

## Passing Convention

Every value crossing the bridge (in either direction, as argument or return value) follows one rule based on its type:

| Type | Convention |
|------|-----------|
| number, string, bool, null/nil | **Copy** |
| undefined (JS) | **Copy** — maps to `JsTypes undefined` singleton in Io |
| Array, Map, Set (JS) / List, Map (Io) | **Deep copy** — recursively applying these same rules to each element |
| TypedArray (JS) / Vector (Io) | **Copy** — typed numeric arrays, preserving element type |
| Everything else (DOM nodes, class instances, etc.) | **Proxy** (remote ref) |

This applies uniformly to arguments and return values in both directions. There is no directional asymmetry.

**Asymmetry note (objects):** Io Maps serialize as `TYPE_OBJECT` going Io->JS (so `JSON.stringify(map)` works). But JS objects — including plain ones — always serialize as `TYPE_JSREF` going JS->Io. This avoids the problem where built-in objects like `Math` or `JSON` would be deep-copied as empty maps (their properties are non-enumerable).

### Containers

Containers are deep-copied across the bridge. Each element follows the top-level rules: primitives copy, objects become proxies, nested containers are recursively copied.

A `querySelectorAll` result becomes a copied List of proxied DOM elements. A list of numbers becomes a copied list of numbers. Mixed content just works — each element follows its own rule.

The receiver owns the copy. Mutations to the copy do not affect the original.

### Cycles

The serializer tracks visited containers by identity. A cycle throws immediately:

    bridge error: cyclic structure cannot be serialized

No attempt to handle cycles. The programmer fixes their data structure.

### Proxies (Remote Refs)

Any non-primitive, non-container value crosses as an opaque proxy. The proxy lives on the receiving side and forwards operations back across the bridge to the original object.

Proxy lifecycle is simple: Io is the orchestrator, JS objects are mostly transient. A ref-counting or explicit `close`/`release` pattern handles cleanup, with `FinalizationRegistry` on the JS side as a safety net for leaked refs. No cross-heap GC coordination is needed.

---

## Io API

```io
// The JS singleton wraps globalThis
JS Math sqrt(144)                // -> 12
JS document title                // -> "Page Title"
JS console log("hello")         // -> calls console.log, returns nil

// DOM access (no custom bindings — everything goes through the bridge)
el := JS document createElement("div")
el set("textContent", "Hello!")
el style setProperty("color", "red")
JS document body appendChild(el)

// Value marshaling
JS JSON parse("[1,2,3]") size   // -> 3 (JS Array -> Io List)
JS JSON stringify(list(1,2,3))  // -> "[1,2,3]" (Io List -> JS Array -> JSON)
m := Map clone atPut("a", 1)
JS JSON stringify(m)            // -> "{\"a\":1}" (Io Map -> JS Object -> JSON)
```

### JSObject methods

| Method | Behavior |
|--------|----------|
| `forward` | Dispatch to JS (see message convention below) |
| `get("name")` | Pure property get, never auto-calls functions |
| `set("name", val)` | Property set, returns self |
| `at(index)` | Array index: `obj[index]` |
| `call(args...)` | Invoke self as function: `obj(args...)` |
| `typeof` | JS typeof string: `"object"`, `"function"`, etc. |
| `type` | `"JSObject"` (Io convention) |

### Message dispatch convention

- **0 args** (`jsObj foo`): property get. If result is a function, auto-call with 0 args.
- **1+ args** (`jsObj foo(x, y)`): method call `obj.foo(x, y)`.
- **`get("name")`**: pure property get, never auto-calls (escape hatch).

Rationale: `el click` should call `el.click()`, not return the function. `el tagName` returns the string (not a function, no auto-call). Use `get("onclick")` when you need to read a function-valued property without calling it.

## JS API

```js
const lobby = io.lobby;               // Proxy wrapping the Io lobby
const result = lobby.list(1, 2, 3);   // -> [1, 2, 3]
lobby.x = 42;                         // setSlot("x", 42)

io.send(handle, "message", arg1);     // lower-level API
```

JS->Io proxies use `FinalizationRegistry` to release ioHandles when the proxy is GC'd.

---

## Binary Serialization Format

Both directions share a 64KB `bridge_buf` in WASM linear memory. The format is recursive and self-describing:

```
TYPE_NIL    = 0  -> [0]
TYPE_TRUE   = 1  -> [1]
TYPE_FALSE  = 2  -> [2]
TYPE_NUMBER = 3  -> [3][f64 LE]                        (9 bytes)
TYPE_STRING = 4  -> [4][len:u32 LE][UTF-8 bytes]       (5+N bytes)
TYPE_ARRAY  = 5  -> [5][count:u32 LE][elem0][elem1]... (recursive)
TYPE_OBJECT = 6  -> [6][count:u32 LE][key0_len:u32][key0_bytes][val0]... (recursive)
TYPE_JSREF  = 7  -> [7][handle:i32 LE]                 (5 bytes)
TYPE_IOREF  = 8  -> [8][handle:i32 LE]                 (5 bytes)
TYPE_ERROR  = 9  -> [9][len:u32 LE][message bytes]     (5+N bytes)
```

### Calling convention

1. Caller serializes args consecutively into `bridge_buf`
2. Caller invokes the WASM import/export (passes argc)
3. Callee deserializes args from `bridge_buf`
4. Callee performs the operation
5. Callee serializes result into `bridge_buf` (overwrites — args already consumed)
6. Caller deserializes result from `bridge_buf`

### Buffer re-entrancy

`forward` evaluates ALL Io args into a local C array before serializing any of them. This ensures nested message sends (which may themselves use bridge_buf) complete before serialization begins. Serialization + the WASM call are atomic from the buffer's perspective.

---

## Strings

Strings cross the bridge as UTF-8. The bridge converts JS UTF-16 strings to UTF-8 on the way in and UTF-8 back to JS strings on the way out.

Io's default string encoding is UTF-8. Method names used in bridge calls are cached after conversion to avoid repeated transcoding overhead.

## JS Undefined

JS has both `undefined` and `null`. Io represents these as distinct values:

- JS `null` maps to Io `nil`
- JS `undefined` maps to `JsTypes undefined` — a distinct singleton

```io
JsTypes undefined := Object clone do(type = "undefined")
```

Io code that doesn't care about the distinction can ignore it. Code that does can check identity (`value == JsTypes undefined`). When serializing back to JS, the bridge checks for this singleton and produces JS `undefined`.

## TypedArrays / Vectors

JS TypedArrays map to Io Vectors (Sequences with numeric item types). They copy across as data, like primitives — they're typed byte buffers, not containers.

| JS TypedArray | Io Vector itemType |
|---|---|
| `Uint8Array` | `uint8_t` |
| `Int8Array` | `int8_t` |
| `Uint16Array` | `uint16_t` |
| `Int16Array` | `int16_t` |
| `Uint32Array` | `uint32_t` |
| `Int32Array` | `int32_t` |
| `Float32Array` | `float32_t` |
| `Float64Array` | `float64_t` |

## Iterators

JS iterators are **not** eagerly enumerated. They cross as proxies like any other object. This avoids surprise infinite enumeration.

To consume an iterator's contents from Io, the JS side provides a helper method (e.g., `toArray`) that enumerates on the JS side and returns the result as a copied array following the normal container rules.

## Functions

### jsfunction (create JS functions from code strings)

```io
handler := jsfunction("return arguments[0] + arguments[1];")
handler call(3, 4)   // -> 7

getter := jsfunction("return document.title;")
getter call          // -> page title string
```

The string contains actual JS code passed to `new Function(...)`. No Io-to-JS translation, no subset semantics. The created function is a JSObject and can be passed to JS APIs that expect callbacks. Access arguments via the `arguments` object. Syntax errors in the code string raise an Io exception.

### Io blocks as JS callbacks (callable IoProxy)

Io blocks (and any Io objects) cross to JS as callable proxies. When JS calls an IoProxy as a function, it sends a `call` message to the Io object with the arguments:

```io
// Pass an Io block directly to a JS API expecting a callback
caller := jsfunction("return arguments[0](42);")
caller call(block(x, x * 2))   // -> 84

// Works with Promise.then
JS get("Promise") resolve(99) then(block(v, v println))

// Works with setTimeout, addEventListener, etc.
JS setTimeout(block("fired!" println), 100)
```

On the JS side, IoProxy uses a function target with an `apply` trap. When called, it re-enters WASM via `io_send` to dispatch the `call` message synchronously. Return values cross back through the bridge normally.

This means `.then()` chains work — the Io block's return value becomes the next Promise's resolved value:

```io
JS get("Promise") resolve(10) then(block(v, v * 2)) then(block(v, v println))
// prints: 20
```

## Unsupported JS Types

These types throw immediately at the bridge boundary:

- **BigInt**: `bridge error: BigInt cannot cross the bridge` — to be revisited (see BigInt.md).
- **Symbol**: `bridge error: JS Symbol cannot cross the bridge` — Symbols are identity-based and have no meaningful Io equivalent.

---

## Async / Promises

JS owns the run loop. Io is a guest. Every entry into Io from JS is a short, synchronous call that returns promptly. Io never blocks, never spins.

### Io calling async JS

If a JS function returns a Promise, the bridge automatically wraps it as an Io `Future`:

1. Io calls a JS function (e.g., `JS fetch(url)`)
2. JS side detects the return value is a Promise (has `.then`)
3. Bridge returns an Io `Future` wrapping the Promise handle
4. When Io code sends a message to the Future, the eval loop yields to JS (`FRAME_STATE_AWAIT_JS`)
5. JS attaches `.then()` / `.catch()` on the Promise
6. Promise resolves → JS calls `io_resume(value)` → eval loop resumes
7. Future forwards the original message to the resolved value

```io
response := JS fetch("/api/data")    // returns Future, yields on use
body := response text                // yields again
body println
```

Synchronous JS functions return their result directly. No Future wrapping.

**Not yet implemented.** Currently Promises cross as plain JSObject proxies. Io blocks can be passed as callbacks via `.then(block(...))` using the callable IoProxy mechanism, but there is no automatic yield/resume. See Plan.md Phase 4e for the full design.

### JS calling Io

The bridge calls the Io handler synchronously and returns whatever it returns. Io is expected to return promptly. If the Io programmer needs to do long-running work, they schedule it in a separate coroutine and yield in small chunks. JS drives the Io Scheduler via `requestIdleCallback` / `requestAnimationFrame` to give coroutines time slices.

---

## Exception Propagation

### JS -> Io (JS error during Io->JS call)

1. JS `js_call` handler catches the exception with `try/catch`
2. JS serializes the error as `TYPE_ERROR` + message string into `bridge_buf`, returns -1
3. C-side `IoJSObject_forward` sees `TYPE_ERROR` in bridge_buf
4. Calls `IoState_error_(state, m, "JS error: %s", message)` — sets `errorRaised = 1`
5. Returns `IONIL(self)` — no longjmp, just a normal return
6. The eval loop checks `errorRaised` and unwinds frames
7. Io-level `try(...)` catches these like any other Io exception

### Io -> JS (Io error during JS->Io call)

1. C-side `io_send` calls `IoMessage_locals_performOn_` to dispatch the message
2. If `state->errorRaised` is set afterward, C serializes `TYPE_ERROR` + "Io error" into `bridge_buf`, returns status 1
3. JS-side `ioSend` deserializes the result — `TYPE_ERROR` produces an `Error` object
4. JS checks the non-zero status and `throw`s the Error
5. Standard JS `try/catch` can handle it

Both directions use `TYPE_ERROR` as the uniform transport. The error message string crosses the boundary; stack traces do not.

---

## Events

Event handlers follow the synchronous bridge convention. The Io programmer decides the pattern based on the use case:

- **Network events**: stash the data, schedule a coroutine resume, return immediately
- **UI events**: call `preventDefault`, update state, return synchronously

The bridge does not distinguish between these cases. It calls the Io handler synchronously and returns. Nested bridge calls during the handler (e.g., calling `event preventDefault` from Io back into JS) work naturally since the WASM bridge is synchronous.

`preventDefault` and similar synchronous event APIs must be called during the handler — not after async work. This is the same constraint as in pure JS.

---

## Handle Tables

### jsHandles (JS side)

A `Map<int, any>` mapping integer handles to arbitrary JS values. Monotonically incrementing keys. Used for all JS objects referenced from Io (DOM elements, functions, global objects, etc.).

- `registerHandle(obj)` -> new handle
- `getHandle(h)` -> JS value
- `releaseHandle(h)` -> delete entry
- JSObject's `freeFunc` calls `js_release(h)` when GC collects the Io wrapper

### ioHandles (C side)

A fixed array `IoObject *ioHandles[1024]` mapping integer handles to Io objects. Used for Io objects referenced from JS via Proxy.

- `ioHandles_register(obj)` -> handle (linear scan for free slot)
- `ioHandles_get(h)` -> IoObject pointer
- `ioHandles_release(h)` -> NULL the slot
- `FinalizationRegistry` calls `io_release(h)` when JS Proxy is GC'd

---

## Garbage Collection

The ioHandles table contains GC roots — Io objects referenced from JS must not be collected. The marking hook uses a function pointer to avoid link-time coupling:

```c
// IoCoroutine.c
void (*IoJSBridge_markIoHandlesFunc)(void) = NULL;

// In IoCoroutine_mark, when marking main coroutine:
if (IoJSBridge_markIoHandlesFunc) IoJSBridge_markIoHandlesFunc();
```

`IoJSObject_proto()` sets the pointer to `IoJSBridge_markIoHandles`, which iterates all 1024 ioHandles slots and marks non-null entries. This keeps the bridge out of non-browser builds entirely.

---

## WASM Imports/Exports

### Imports (JS provides to WASM, `js` module)

| Import | Signature | Purpose |
|--------|-----------|---------|
| `js_call` | `(handle, name_ptr, name_len, argc) -> int` | Property get or method call |
| `js_get_prop` | `(handle, name_ptr, name_len) -> int` | Pure property get |
| `js_set_prop` | `(handle, name_ptr, name_len) -> void` | Property set (value in bridge_buf) |
| `js_call_func` | `(handle, argc) -> int` | Invoke handle as function |
| `js_typeof` | `(handle, buf, sz) -> int` | Write typeof string |
| `js_get_global` | `() -> int` | Handle for globalThis |
| `js_release` | `(h) -> void` | Release JS handle |

### Exports (WASM provides to JS)

| Export | Purpose |
|--------|---------|
| `io_get_bridge_buf` | Pointer to shared 64KB buffer |
| `io_get_bridge_buf_size` | Buffer size |
| `io_send` | Send message to Io object (args in bridge_buf) |
| `io_release` | Release Io handle |
| `io_get_lobby_handle` | Get handle to Io lobby |

---

## Design Decisions

**No custom DOM bindings.** The generic bridge replaces the earlier hand-wrapped DOM CFunctions (`io_dom.c`). Performance difference is negligible for DOM operations — the DOM API itself dominates. This eliminates ~400 lines of C and ~145 lines of JS that would need updating for every new DOM method.

**Objects always pass by reference JS->Io.** Early versions deep-copied "plain objects" (prototype === Object.prototype) as `TYPE_OBJECT` -> IoMap. This broke built-in objects like `Math` and `JSON` whose properties are non-enumerable (copied as empty maps). The fix: all JS objects pass as `TYPE_JSREF` handles. Only Arrays deep-copy (as `TYPE_ARRAY`), since array indexing semantics differ.

**Maps deep-copy Io->JS only.** When passing an Io Map to JS, it serializes as `TYPE_OBJECT` so that `JSON.stringify(map)` and property access work naturally on the JS side.

**Auto-call convention for 0-arg messages.** Property get auto-calls function results when the message has 0 args. This matches user expectations: `el click` calls `click()`, `el tagName` returns the string. The `get("name")` escape hatch handles the rare case of reading a function property.

**Function pointer for GC hook.** Using `void (*IoJSBridge_markIoHandlesFunc)(void) = NULL` in IoCoroutine.c avoids `#ifdef` guards and keeps the bridge completely out of non-browser WASM builds (which don't link `io_js_bridge.o`).

**Io is the brains. JS is the platform. The bridge is thin and predictable.**
