# Io/JS Bridge Convention

The Io VM runs as WASM. Io is the application layer — the brains of the program. JS provides low-level libraries, DOM access, and event dispatch. The bridge enforces a clean, predictable convention for data crossing the boundary.

## Passing Convention

Every value crossing the bridge (in either direction, as argument or return value) follows one rule based on its type:

| Type | Convention |
|------|-----------|
| number, string, bool, null/nil | **Copy** |
| undefined (JS) | **Copy** — maps to `JsTypes undefined` singleton in Io (see below) |
| Array, Map, Set (JS) / List, Map (Io) | **Deep copy** — recursively applying these same rules to each element |
| TypedArray (JS) / Vector (Io) | **Copy** — typed numeric arrays, preserving element type |
| Everything else (DOM nodes, class instances, etc.) | **Proxy** (remote ref) |

This applies uniformly to arguments and return values in both directions. There is no directional asymmetry.

### Containers

Containers are deep-copied across the bridge. Each element follows the top-level rules: primitives copy, objects become proxies, nested containers are recursively copied.

A `querySelectorAll` result becomes a copied List of proxied DOM elements. A list of numbers becomes a copied list of numbers. Mixed content just works — each element follows its own rule.

The receiver owns the copy. Mutations to the copy do not affect the original. This matches how JS APIs universally behave — returned containers are snapshots owned by the caller.

### Cycles

The serializer tracks visited containers by identity. A cycle throws immediately:

    bridge error: cyclic structure cannot be serialized

No attempt to handle cycles. The programmer fixes their data structure.

### Proxies (Remote Refs)

Any non-primitive, non-container value crosses as an opaque proxy. The proxy lives on the receiving side and forwards operations back across the bridge to the original object.

Proxy lifecycle is simple: Io is the orchestrator, JS objects are mostly transient. A ref-counting or explicit `close`/`release` pattern handles cleanup, with `FinalizationRegistry` on the JS side as a safety net for leaked refs. No cross-heap GC coordination is needed.

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

The existing serialization format already preserves encoding and itemType metadata for Sequences, so this maps cleanly.

## Iterators

JS iterators are **not** eagerly enumerated. They cross as proxies like any other object. This avoids surprise infinite enumeration.

To consume an iterator's contents from Io, the JS side provides a helper method (e.g., `toArray`) that enumerates on the JS side and returns the result as a copied array following the normal container rules. One bridge call, no surprise, and the programmer opted in.

## Functions

Io blocks cross as proxies. If you need a real JS-side function (e.g., for an event handler callback), use `jsfunction`:

```io
handler := jsfunction("return event.target.value;")
```

The string contains actual JS code. No Io-to-JS translation, no subset semantics. The programmer writes JS and knows exactly what they're getting.

## Async / Promises

### Io calling JS

If a JS function returns a Promise (whether declared `async` or not), the bridge wraps it as an Io `Future`:

1. Io calls a JS function
2. Bridge detects the return value is a Promise (has `.then`)
3. Bridge immediately returns an Io `Future`
4. JS promise resolves or rejects
5. Bridge sets the Future's result or raises an exception on it
6. Io code accessing the Future's value yields the coroutine until ready

Synchronous JS functions return their result directly. No Future wrapping.

### JS calling Io

The bridge calls the Io handler synchronously and returns whatever it returns. Io is expected to return promptly. If the Io programmer needs to do long-running work, they schedule it in a separate coroutine and post results back as events.

## Exceptions

Direct mapping both ways:

- JS `Error` becomes an Io `Exception`
- Io `Exception` becomes a JS `Error`

The message string carries across. The original error object is available as a proxy for further inspection.

## Events

Event handlers follow the synchronous bridge convention. The Io programmer decides the pattern based on the use case:

- **Network events**: stash the data, schedule a coroutine resume, return immediately
- **UI events**: call `preventDefault`, update state, return synchronously

The bridge does not distinguish between these cases. It calls the Io handler synchronously and returns. Nested bridge calls during the handler (e.g., calling `event preventDefault` from Io back into JS) work naturally since the WASM bridge is synchronous.

`preventDefault` and similar synchronous event APIs must be called during the handler — not after async work. This is the same constraint as in pure JS.

## Unsupported JS Types

These types throw immediately at the bridge boundary if encountered inside a container or as a direct value:

- **BigInt**: `bridge error: BigInt cannot cross the bridge` — avoids silent precision loss. To be revisited if a compelling use case appears.
- **Symbol**: `bridge error: JS Symbol cannot cross the bridge` — Symbols are identity-based and have no meaningful Io equivalent.

## Strings

Strings cross the bridge as UTF-8. The bridge converts JS UTF-16 strings to UTF-8 on the way in and UTF-8 back to JS strings on the way out.

Io's default string encoding is UTF-8. Method names used in bridge calls are cached after conversion to avoid repeated transcoding overhead.

## Serialization Format

The existing custom binary format is retained. Handles have no clean JSON representation, and the format is already compact and well-structured. The type tags are:

| Tag | Payload |
|-----|---------|
| NIL | (none) |
| NUMBER | float32 |
| SYMBOL | encoding + itemType + size + bytes |
| REFERENCE | uint32 proxy id |

Containers are serialized recursively — each element encoded per its type. The format extends naturally to support the convention described above.

## Design Philosophy

- Pure data crosses as copies. Platform objects stay as proxies.
- Containers are transparent (deep-copied). Everything else is opaque (proxied).
- Violations are loud and immediate with clear error messages.
- The programmer understands the conventions. The conventions are simple enough to internalize quickly.
- Io is the brains. JS is the platform. The bridge is thin and predictable.
