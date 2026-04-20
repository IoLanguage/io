# BigInt Support for Io-JS Bridge — COMPLETE

## Goal

Add a BigInt type to Io that interops with JS BigInts across the browser bridge. BigInt is a separate type from Number (which stays as 64-bit double, matching JS). No implicit coercion between Number and BigInt (matches JS semantics).

## Design Decisions

- **Separate type**: BigInt and Number are distinct. Mixing in arithmetic is an error (like JS).
- **Number stays as double**: No changes to IoNumber. 1:1 mapping with JS `number`.
- **Explicit conversion**: `bigint asNumber` (lossy), `number asBigInt` (exact for integers).
- **String construction**: `BigInt from("123456789012345678901234567890")` for literals.
- **Bridge round-trip**: JS BigInt crosses as TYPE_BIGINT, Io BigInt crosses back. No data loss.
- **Name shadowing**: `BigInt` is registered on `state->core`, so `JS BigInt(42)` finds the Io proto, not the JS global. Use `JS get("BigInt") call(42)` to call JS's BigInt constructor.

## Library: libtommath

- **License**: Public domain (Unlicense)
- **Portability**: Pure C, no platform asm, no system calls — builds cleanly with wasi-sdk
- **Size**: ~100KB source, but WASM -O2 strips unused code
- **Dependencies**: Only malloc/realloc/free, string.h
- **Track record**: Used by LibreSSL, Python, Ruby
- **WASM note**: Requires `-D__STDC_IEC_559__` for `mp_set_double` (WASM uses IEEE 754 but clang doesn't define the macro by default)

## Wire Format

```
TYPE_BIGINT = 13  ->  [13][len: u32 LE][decimal_string: len bytes]
```

Decimal string representation (e.g., "-123456789"). Simple, debuggable, any size.
- JS→C: `bigint.toString()` → wire → `mp_read_radix(str, 10)`
- C→JS: `mp_to_radix(mp, 10)` → wire → `BigInt(str)`

## IoBigInt Methods

| Method | Behavior |
|--------|----------|
| `+`, `-`, `*`, `/`, `%` | BigInt arithmetic (both operands must be BigInt) |
| `**` | Power (exponent must fit in unsigned long) |
| `==`, `<`, `>`, `<=`, `>=`, `!=` | Comparison (BigInt with BigInt) |
| `compare` | Returns -1, 0, 1 |
| `negate` | Return negated copy |
| `abs` | Return absolute value |
| `asString` | Decimal string representation |
| `asNumber` | Lossy conversion to double |
| `asBigInt` | Identity (for polymorphism) |
| `type` | Returns "BigInt" |

## Construction

```io
BigInt from(42)                           // from Number (exact for integers)
BigInt from("123456789012345678901234")    // from string (any size)
```

## Implementation (all steps complete)

1. Vendored libtommath v1.3.0 to `deps/libtommath/`
2. Updated Makefile: compile libtommath, added `-D__STDC_IEC_559__` for IEEE 754 support
3. Created `libs/iovm/source/IoBigInt.c` / `IoBigInt.h`
4. Registered BigInt proto on `state->core` in browser `io_init()`
5. Added TYPE_BIGINT (13) to C bridge serialization/deserialization (`io_js_bridge.c`)
6. Added TYPE_BIGINT to JS bridge (`io.js`) — replaced BigInt rejection with serialization
7. Updated tests: 11 new BigInt tests (round-trip, arithmetic, comparison, construction)
8. All 83 browser tests pass
