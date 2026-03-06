# Bidirectional Io-JS Bridge

The browser target includes a generic bidirectional bridge between the Io VM (running in WASM) and the JS runtime. Io code can call any JS object/method/property, and JS code can send messages to any Io object — similar to the old ObjcBridge for Objective-C.

## Files

| File | Role |
|------|------|
| `browser/io_js_bridge.h` | C header: proto factory, handle factory, GC hook |
| `browser/io_js_bridge.c` | C implementation: JSObject proto, serialization, ioHandles, WASM exports |
| `browser/io.js` | JS runtime: jsHandles, `js` import namespace, serialization, Io Proxy |
| `browser/io_browser.c` | Registers JSObject proto + JS singleton during `io_init()` |

## Io API

```io
// The JS singleton wraps globalThis
JS Math sqrt(144)                // → 12
JS document title                // → "Page Title"
JS console log("hello")         // → calls console.log, returns nil

// DOM access (no custom bindings — everything goes through the bridge)
el := JS document createElement("div")
el set("textContent", "Hello!")
el style setProperty("color", "red")
JS document body appendChild(el)

// Value marshaling
JS JSON parse("[1,2,3]") size   // → 3 (JS Array → Io List)
JS JSON stringify(list(1,2,3))  // → "[1,2,3]" (Io List → JS Array → JSON)
m := Map clone atPut("a", 1)
JS JSON stringify(m)            // → "{\"a\":1}" (Io Map → JS Object → JSON)
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
const result = lobby.list(1, 2, 3);   // → [1, 2, 3]
lobby.x = 42;                         // setSlot("x", 42)

io.send(handle, "message", arg1);     // lower-level API
```

JS→Io proxies use `FinalizationRegistry` to release ioHandles when the proxy is GC'd.

## Architecture

```
┌──────────────────────┐                    ┌──────────────────────┐
│  Io VM (C/WASM)      │   bridge_buf       │  JS Runtime          │
│                      │   (64KB shared)    │                      │
│  JSObject proto      │                    │  jsHandles Map       │
│    forward ──────────┼── js_call ───────→ │    (any JS value)    │
│    get/set/call/at   │                    │                      │
│    typeof/type       │                    │  serialize/          │
│                      │← result in buf ───┤  deserialize         │
│  JS singleton        │   (value types     │                      │
│    handle=globalThis │    deep-copied)    │  Proxy factory       │
│                      │                    │    get → io_send     │
│  ioHandles[] ←───────┼── io_send ────────┤    set → setSlot     │
│  (GC-marked roots)   │                    │  FinalizationRegistry│
│                      │── result in buf ──→│    → io_release      │
│  serialize/          │                    │                      │
│  deserialize         │                    │                      │
└──────────────────────┘                    └──────────────────────┘
```

## Value vs Reference Passing

| Type | Io side | JS side | Passing |
|------|---------|---------|---------|
| Number | IoNumber | number | **value** (copy) |
| String | Sequence/Symbol | string | **value** (copy) |
| Boolean | ioTrue/ioFalse | true/false | **value** (copy) |
| Nil | ioNil | null/undefined | **value** (copy) |
| List | IoList | Array | **value** (deep copy) |
| Map | IoMap | plain Object | **value** (Io→JS only) |
| JS object | JSObject | any object | **reference** (handle) |
| Io object | via ioHandle | Proxy | **reference** (handle) |

**Asymmetry note:** Io Maps serialize as `TYPE_OBJECT` going Io→JS (so `JSON.stringify(map)` works). But JS objects — including plain ones — always serialize as `TYPE_JSREF` going JS→Io. This avoids the problem where built-in objects like `Math` or `JSON` would be deep-copied as empty maps (their properties are non-enumerable).

## Binary Serialization Format

Both directions share a 64KB `bridge_buf` in WASM linear memory. The format is recursive and self-describing:

```
TYPE_NIL    = 0  → [0]
TYPE_TRUE   = 1  → [1]
TYPE_FALSE  = 2  → [2]
TYPE_NUMBER = 3  → [3][f64 LE]                        (9 bytes)
TYPE_STRING = 4  → [4][len:u32 LE][UTF-8 bytes]       (5+N bytes)
TYPE_ARRAY  = 5  → [5][count:u32 LE][elem0][elem1]... (recursive)
TYPE_OBJECT = 6  → [6][count:u32 LE][key0_len:u32][key0_bytes][val0]... (recursive)
TYPE_JSREF  = 7  → [7][handle:i32 LE]                 (5 bytes)
TYPE_IOREF  = 8  → [8][handle:i32 LE]                 (5 bytes)
TYPE_ERROR  = 9  → [9][len:u32 LE][message bytes]     (5+N bytes)
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

## Exception Propagation

### JS → Io (JS error during Io→JS call)

When Io code triggers a JS error (e.g., calling a non-existent method, TypeError):

1. JS `js_call` handler catches the exception with `try/catch`
2. JS serializes the error as `TYPE_ERROR` + message string into `bridge_buf`, returns -1
3. C-side `IoJSObject_forward` sees `TYPE_ERROR` in bridge_buf
4. Calls `IoState_error_(state, m, "JS error: %s", message)` — sets `errorRaised = 1`
5. Returns `IONIL(self)` — no longjmp, just a normal return
6. The eval loop checks `errorRaised` and unwinds frames
7. Io-level `try(...)` catches these like any other Io exception

### Io → JS (Io error during JS→Io call)

When JS sends a message that causes an Io exception:

1. C-side `io_send` calls `IoMessage_locals_performOn_` to dispatch the message
2. If `state->errorRaised` is set afterward, C serializes `TYPE_ERROR` + "Io error" into `bridge_buf`, returns status 1
3. JS-side `ioSend` deserializes the result — `TYPE_ERROR` produces an `Error` object
4. JS checks the non-zero status and `throw`s the Error
5. Standard JS `try/catch` can handle it

### Error type mapping

| Origin | Serialized as | Received as |
|--------|---------------|-------------|
| JS exception | `TYPE_ERROR` in bridge_buf | `IoState_error_()` → Io exception |
| Io exception | `TYPE_ERROR` in bridge_buf | `throw new Error()` → JS exception |

Both directions use `TYPE_ERROR` as the uniform transport. The error message string crosses the boundary; stack traces do not.

## Handle Tables

### jsHandles (JS side)

A `Map<int, any>` mapping integer handles to arbitrary JS values. Monotonically incrementing keys. Used for all JS objects referenced from Io (DOM elements, functions, global objects, etc.).

- `registerHandle(obj)` → new handle
- `getHandle(h)` → JS value
- `releaseHandle(h)` → delete entry
- JSObject's `freeFunc` calls `js_release(h)` when GC collects the Io wrapper

### ioHandles (C side)

A fixed array `IoObject *ioHandles[1024]` mapping integer handles to Io objects. Used for Io objects referenced from JS via Proxy.

- `ioHandles_register(obj)` → handle (linear scan for free slot)
- `ioHandles_get(h)` → IoObject pointer
- `ioHandles_release(h)` → NULL the slot
- `FinalizationRegistry` calls `io_release(h)` when JS Proxy is GC'd

## Garbage Collection

The ioHandles table contains GC roots — Io objects referenced from JS must not be collected. The marking hook uses a function pointer to avoid link-time coupling:

```c
// IoCoroutine.c
void (*IoJSBridge_markIoHandlesFunc)(void) = NULL;

// In IoCoroutine_mark, when marking main coroutine:
if (IoJSBridge_markIoHandlesFunc) IoJSBridge_markIoHandlesFunc();
```

`IoJSObject_proto()` sets the pointer to `IoJSBridge_markIoHandles`, which iterates all 1024 ioHandles slots and marks non-null entries. This keeps the bridge out of non-browser builds entirely.

## WASM Imports/Exports

### Imports (JS provides to WASM, `js` module)

| Import | Signature | Purpose |
|--------|-----------|---------|
| `js_call` | `(handle, name_ptr, name_len, argc) → int` | Property get or method call |
| `js_get_prop` | `(handle, name_ptr, name_len) → int` | Pure property get |
| `js_set_prop` | `(handle, name_ptr, name_len) → void` | Property set (value in bridge_buf) |
| `js_call_func` | `(handle, argc) → int` | Invoke handle as function |
| `js_typeof` | `(handle, buf, sz) → int` | Write typeof string |
| `js_get_global` | `() → int` | Handle for globalThis |
| `js_release` | `(h) → void` | Release JS handle |

### Exports (WASM provides to JS)

| Export | Purpose |
|--------|---------|
| `io_get_bridge_buf` | Pointer to shared 64KB buffer |
| `io_get_bridge_buf_size` | Buffer size |
| `io_send` | Send message to Io object (args in bridge_buf) |
| `io_release` | Release Io handle |
| `io_get_lobby_handle` | Get handle to Io lobby |

## Design Decisions

**No custom DOM bindings.** The generic bridge replaces the earlier hand-wrapped DOM CFunctions (`io_dom.c`). Performance difference is negligible for DOM operations — the DOM API itself dominates. This eliminates ~400 lines of C and ~145 lines of JS that would need updating for every new DOM method.

**Objects always pass by reference JS→Io.** Early versions deep-copied "plain objects" (prototype === Object.prototype) as `TYPE_OBJECT` → IoMap. This broke built-in objects like `Math` and `JSON` whose properties are non-enumerable (copied as empty maps). The fix: all JS objects pass as `TYPE_JSREF` handles. Only Arrays deep-copy (as `TYPE_ARRAY`), since array indexing semantics differ.

**Maps deep-copy Io→JS only.** When passing an Io Map to JS, it serializes as `TYPE_OBJECT` so that `JSON.stringify(map)` and property access work naturally on the JS side.

**Auto-call convention for 0-arg messages.** Property get auto-calls function results when the message has 0 args. This matches user expectations: `el click` calls `click()`, `el tagName` returns the string. The `get("name")` escape hatch handles the rare case of reading a function property.

**Function pointer for GC hook.** Using `void (*IoJSBridge_markIoHandlesFunc)(void) = NULL` in IoCoroutine.c avoids `#ifdef` guards and keeps the bridge completely out of non-browser WASM builds (which don't link `io_js_bridge.o`).
