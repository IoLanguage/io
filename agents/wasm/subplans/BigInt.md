# BigInt Support for Io-JS Bridge

## Goal

Add a BigInt type to Io that can interop with JS BigInts across the browser bridge, replacing the current BigInt rejection with proper round-trip support.

## Library Choice: libtommath

- **License**: Public domain (Unlicense) — no licensing friction
- **Portability**: Pure C, no platform asm, no system calls, no floating point — builds cleanly with wasi-sdk
- **Size**: ~15 source files, ~100KB total
- **Dependencies**: Only `malloc`/`realloc`/`free` and `string.h`/`stdio.h`
- **Static linking**: Designed for it, no shared library requirements
- **Track record**: Used by LibreSSL, Python, Ruby for their bigint needs
- **WASM**: Has been compiled to WASM by others; no platform-specific code paths
- **Alternative considered**: mini-gmp (single .c/.h from GMP) but it's LGPL

## Implementation Plan

### 1. Vendor libtommath

Add to `libs/libtommath/` (~15 .c files), add to Makefile with wasi-sdk.

### 2. New `IoBigInt.c`

IoObject with `mp_int` as data, tag, basic method table.

### 3. Bridge wire format

New `TYPE_BIGINT = 12`. Serialize as sign byte + hex/decimal string (simplest) or sign + LE byte array (faster). JS side uses `BigInt("0x...")` / `"0x" + n.toString(16)`.

### 4. Methods on IoBigInt

`+`, `-`, `*`, `/`, `%`, `==`, `<`, `>`, `asString`, `asNumber` (lossy), `from(aNumber)`.

## Design Decisions (punt for now)

- No implicit Number↔BigInt coercion (match JS semantics — explicit conversion)
- BigInt ops return BigInt, Number ops return Number, mixing is an error
- `asNumber` truncates/loses precision with a warning, like JS `Number(bigint)`

## Effort

Library integration and basic type: ~1-2 sessions. Bridge serialization is straightforward since both sides already handle a type-tagged binary protocol. The bulk of future work is deciding how BigInt interacts with the rest of Io (operator dispatch, comparison with Numbers, etc.), but that can be deferred.
