# Io Language WASM Migration Plan

## Strategic Direction

The goal is to **target WASM as Io's only platform**, eliminating all OS-specific dependencies and multi-platform C code. This dramatically simplifies maintenance and leverages WASM as a universal runtime. The primary target is **WASI** (using Wasmtime with Cranelift JIT) with browser as a secondary target.

## Key Architectural Decisions

**Toolchain**: Compile Io's existing C implementation to WASM using wasi-sdk. Io's C codebase is already fairly portable and the stackless work has eliminated the biggest porting hazard (setjmp/longjmp and stack manipulation).

**Memory**: Use WASM's `memory.grow` for dynamic heap expansion in 64KB pages rather than a static allocation. Start with ~16-32MB, grow on demand, no artificial ceiling.

**Garbage Collector**: Keep Io's existing `libgarbagecollector` — an incremental tricolor Baker Treadmill collector — as the primary collector. Do **not** switch to WASM GC, whose nominal type system is a poor fit for Io's dynamic prototype-based object model.

**External references** (browser target): Use `externref` for holding JS/DOM object references from within WASM. Use `FinalizationRegistry` on the JS side as a safety net, with Io's existing object `free` hook as the primary eager cleanup path. Use `WeakRef` for non-owning JS->WASM references to avoid cycles.

**API bindings**: Replace platform-specific C bindings with WASI imports for the server target. For the browser target, expose DOM/fetch/WebSockets as WASM imports through a thin JS glue module. Long term, target the WASI component model for typed cross-language interop.

---

## 1. Audit Summary

### What's already WASM-compatible (~80% of codebase)

The entire VM core compiles as pure C with no platform dependencies:
- IoObject, IoMessage, IoList, IoMap, IoNumber, IoSeq — core data types
- IoBlock, IoCFunction, IoTag — method dispatch
- IoCoroutine, IoContinuation, IoEvalFrame — stackless execution (no C stack switching)
- IoState_iterative.c — eval loop (frame state machine)
- IoCompiler, IoLexer, IoToken — parser
- IoProfiler, IoDebugger — introspection
- libgarbagecollector — Baker treadmill collector (pure C, no platform deps)
- libbasekit — collections, UArray, hashing (mostly pure C)
- parson — embedded JSON library

### What needs changes (~20%)

| Category | Files | Difficulty |
|----------|-------|------------|
| Dynamic loading (dlopen) | DynLib.c, IoDynLib.c, AddonLoader.io | **Disable** — not applicable in WASM |
| Process execution | IoSystem.c (system, daemon, getpid) | **Stub** — return errors |
| Pipe I/O | IoFile.c (popen/pclose) | **Stub** — not available in WASI |
| CPU detection | IoSystem.c (sysctl, sysconf) | **Stub** — return 1 |
| Signal handling | IoState_debug.c (SIGINT) | **Stub** — no signals in WASI |
| Platform detection | IoConfig.h, IoSystem.c (uname) | **Add WASM case** |
| Time functions | PortableGettimeofday.c, PortableUsleep.c | **Replace** with WASI clock_gettime/nanosleep |
| NAN detection | libs/iovm/CMakeLists.txt (check_c_source_runs) | **Hardcode** USE_BUILTIN_NAN |
| Build system | All CMakeLists.txt | **Restructure** for cross-compilation |

### What gets dropped

- `libs/coroutine/` — already removed on stackless
- All addons in `addons/` — rely on dlopen, not needed for core
- Windows-specific code — `#ifdef WIN32` blocks, NSIS packaging
- Symbian stubs — dead code

---

## 2. Phased Plan

### Phase 1: Minimal WASI Build (get it compiling)

**Goal**: `io.wasm` that runs `"hello" println` under wasmtime.

#### 1a. Create wasi-sdk toolchain file

Create `cmake/wasi-sdk.cmake`:
```cmake
set(CMAKE_SYSTEM_NAME WASI)
set(CMAKE_SYSTEM_PROCESSOR wasm32)
set(WASI_SDK_PREFIX "/opt/wasi-sdk" CACHE PATH "wasi-sdk install path")
set(CMAKE_C_COMPILER "${WASI_SDK_PREFIX}/bin/clang")
set(CMAKE_SYSROOT "${WASI_SDK_PREFIX}/share/wasi-sysroot")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__wasi__ -DUSE_BUILTIN_NAN")
```

**Difficulty**: Easy

#### 1b. Solve the io2c bootstrap problem

io2c is a host tool that runs during build to convert `.io` files to C. In cross-compilation, the WASM-compiled io2c can't run on the host.

**Solution**: Two-stage build.
1. Build native io2c first (host compiler)
2. Cross-compile everything else for WASM, using native io2c to generate IoVMInit.c

Implementation: CMake `ExternalProject_Add` or a simple wrapper script:
```bash
# Stage 1: build io2c natively
mkdir -p build-native && cd build-native
cmake .. && make io2c
# Stage 2: cross-compile for WASM using native io2c
mkdir -p build-wasm && cd build-wasm
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/wasi-sdk.cmake \
         -DIO2C_EXECUTABLE=../build-native/_build/binaries/io2c
make
```

The libs/iovm/CMakeLists.txt custom command must use `IO2C_EXECUTABLE` instead of `${PROJECT_BINARY_DIR}/_build/binaries/io2c`.

**Difficulty**: Medium

#### 1c. Switch all libraries to static

WASM doesn't support dynamic linking. Change `add_library(xxx SHARED ...)` to `add_library(xxx STATIC ...)` when building for WASI. Gate with:
```cmake
if(CMAKE_SYSTEM_NAME STREQUAL "WASI")
    add_library(basekit STATIC ${SRCS})
else()
    add_library(basekit SHARED ${SRCS})
endif()
```

Apply to: basekit, garbagecollector, iovmall.

**Difficulty**: Easy

#### 1d. Stub out unsupported APIs

Create `libs/iovm/source/IoWASI_stubs.c` (compiled only for WASI target) with stubs for:

```c
#ifdef __wasi__
// DynLib — no dynamic loading in WASM
void *DynLib_open(DynLib *self) { return NULL; }
void *DynLib_symbol(DynLib *self, const char *name) { return NULL; }

// popen — no process pipes
// system() — no shell execution
// daemon() — no daemonization
// getpid() — return 1
// signal() — no-op
#endif
```

Also add `#ifdef __wasi__` guards in:
- `IoSystem.c` — stub system(), daemon(), getpid(), cpuCount(), platform()
- `IoFile.c` — stub popen/pclose
- `IoState_debug.c` — skip signal(SIGINT, ...)
- `IoConfig.h` — add WASM platform detection

**Difficulty**: Medium (many small changes, but straightforward)

#### 1e. Fix platform-specific build flags

- Root CMakeLists.txt: skip `-msse2` for WASM
- libs/basekit/CMakeLists.txt: skip `-ldl` for WASM
- tools/CMakeLists.txt: skip `-ldl`, `-lm`, `-lcurses`, `SANE_POPEN` for WASM
- libs/iovm/CMakeLists.txt: skip `check_c_source_runs` for WASM, hardcode `USE_BUILTIN_NAN`

**Difficulty**: Easy

#### 1f. Verify basic execution

```bash
wasmtime build-wasm/_build/binaries/io.wasm -- -e '"hello" println'
```

**Milestone**: Io prints "hello" running as WASM under wasmtime.

---

### Phase 2: File I/O via WASI

**Goal**: Io scripts can read/write files using WASI's preopened directory model.

WASI provides POSIX-like file APIs (`fopen`, `fread`, `opendir`, etc.) through wasi-libc. Most of IoFile.c and IoDirectory.c should work with minimal changes since they already use POSIX calls.

**Work items**:
- Test file operations under wasmtime with `--dir` preopened directories
- Fix any `getcwd`/`chdir` issues (WASI sandboxes these)
- Verify stat/lstat work (WASI provides these)
- Stub `IoFile_popen_` to raise Io exception ("not supported on WASM")
- Test IoDirectory operations (opendir/readdir/closedir)

**Difficulty**: Easy-Medium

**Milestone**: Test suite runs under wasmtime (at least the subset not requiring process spawning or dynamic loading).

---

### Phase 3: Clean up platform code

**Goal**: Remove dead platform code, simplify for WASM-only target.

- Remove all `#ifdef WIN32` / `#ifdef _MSC_VER` blocks
- Remove `#ifdef __SYMBIAN32__` blocks
- Remove Windows-specific implementations in DynLib.c, IoSystem.c, IoFile.c, IoDirectory.c, PortableUsleep.c, PortableGettimeofday.c
- Remove NSIS/DEB/RPM packaging from CMakeLists.txt
- Remove `extras/win32vc10/` directory
- Remove `libs/coroutine/` if any remnants exist
- Simplify IoConfig.h to only detect WASM
- Remove shared library build paths (static only)

**Difficulty**: Easy (deletions)

**Milestone**: Codebase compiles only for WASM. No platform conditionals remain.

---

### Phase 4: Browser target

**Goal**: Io REPL running in a browser with full JS interop.

#### Phase 4a: Browser REPL MVP - COMPLETE
- Custom C entry point (`browser/io_browser.c`) with `io_init()`, `io_eval()`, `io_get_output()` exports
- Compiled with `-mexec-model=reactor` (exports `_initialize` + custom functions)
- JS glue (`browser/io.js`) — WASM loader + minimal WASI shim (fd_write capture, clock, proc_exit)
- HTML REPL (`browser/index.html`) — input, output, persistent eval
- `make browser` / `make serve` targets

#### Phase 4b: Bidirectional Io-JS bridge - COMPLETE
- Generic bridge replacing hand-wrapped DOM CFunctions (see Bridge.md)
- JSObject proto with forward dispatch, get/set/at/call/typeof methods
- Binary serialization protocol (12 type tags, 64KB shared buffer)
- Handle tables: jsHandles (JS side, Map), ioHandles (C side, fixed array)
- Deep copy for containers (List/Array, Map/Object, Set), proxy for opaque JS objects
- TypedArray/Vector support, JS undefined singleton, cycle detection
- Error propagation both directions via TYPE_ERROR
- GC marking hook via function pointer (no link-time coupling)
- FinalizationRegistry cleanup for JS->Io proxies

#### Phase 4c: jsfunction (JS callbacks from Io) - COMPLETE
- CFunction `jsfunction(codeString)` on Lobby — wraps `new Function(...)` on JS side, returns JSObject handle
- WASM import `js_new_function(code_ptr, code_len) -> handle`
- Syntax errors in code string raise Io exceptions
- Enables event handlers, callbacks, requestAnimationFrame, setTimeout

#### Phase 4d: Promise/Future (async JS interop)

**Goal**: Io code can call async JS APIs and await results via coroutines.

```io
response := JS fetch("/api/data")    // returns Future
body := response text                // yields coroutine until resolved
body println
```

- Promise detection: JS side checks if return value has `.then` method
- New Io `Future` type wrapping a pending result
- JS `.then()` / `.catch()` wiring calls back into bridge on resolve/reject
- Accessing Future value yields the current coroutine until ready
- Rejection maps to Io exception

**Difficulty**: Hard (touches coroutine system)

**Milestone**: `JS fetch(url)` returns a Future; accessing its value yields and resumes correctly.

#### Phase 4e: BigInt (future)

- Vendor libtommath, create IoBigInt type, add TYPE_BIGINT to wire format
- See BigInt.md for full design

**Difficulty**: Medium

---

### Phase 5: Compacting collector (optimization)

The current Baker treadmill collector works but fragments memory over time. A compacting collector would reduce memory usage for long-running browser sessions.

The codebase is **moderately ready** (7/10).

#### Design

**New GC hook needed**: A `remap` callback alongside the existing `mark` and `free` callbacks. Where `mark` visits references, `remap` must be able to **write back** updated addresses to pointer fields. The cleanest implementation is a single `walkRefs` function per primitive type that accepts a visitor, with both `mark` and `remap` implemented as thin wrappers over it:

```c
void IoObject_walkRefs(IoObject *self, IoRefVisitor *visitor, void *ctx);
void IoObject_mark(IoObject *self);   // wraps walkRefs with mark visitor
void IoObject_remap(IoObject *self);  // wraps walkRefs with remap visitor
```

Only a small number of Io primitives contain object references and need `walkRefs` — `IoObject`, `IoList`, `IoMessage`, `IoCoroutine`. Leaf primitives (`IoNumber`, `IoSeq`, `IoDate`, `IoFile`) contain no object references and need only a `memmove` during compaction.

**50% boundary policy**: Compaction only moves objects **above the 50% memory mark** to lower addresses. Objects below 50% are never moved. This preserves JIT optimization assumptions for hot long-lived objects in the lower half, while allowing the upper half to act as a nursery zone. The midpoint should be dynamic:

```c
size_t midpoint = max(wasmMemorySize() / 2, liveObjectBytes() * 1.2);
```

**Recycling interaction**: `libgarbagecollector` has a `COLLECTOR_RECYCLE_FREED` mechanism (currently disabled via a commented-out `#define`). When compaction is added, recycling should only operate on freed slots **above the 50% boundary**, keeping the lower half stable and dense.

**Compaction trigger**: Only fire under memory pressure, before calling `memory.grow`. Sequence: compact upper half -> if still above 80% full -> grow. This makes compaction a pressure relief valve rather than a routine operation.

**JIT consideration**: Compaction invalidates JIT-learned access patterns. The 50% policy mitigates this by never moving JIT-hot lower-half objects. Compaction should be infrequent and ideally triggered between activity bursts.

**Treadmill color as hotness proxy**: The Baker Treadmill's off-white segment (tenured/long-lived objects) naturally identifies hot objects without any separate heat tracking. During compaction, off-white objects in the lower half should be left in place.

#### Types needing walkRefs

| Type | Complexity | Key references |
|------|-----------|----------------|
| IoObject | Medium | slots (PHash k,v), protos[] (**interior pointer**), tag's walkRefs |
| IoMessage | Medium | name, args, next, label, cachedResult, **inline cache value/context** |
| IoEvalFrame | High | 15+ fields + control flow union (60-line switch) |
| IoCoroutine | High | ioStack, frameStack chain, pooled frames/blockLocals/calls |
| IoList | Easy | iterate items |
| IoMap | Easy | iterate PHash k,v |
| IoBlock | Easy | message, scope, argNames |
| IoCall | Easy | 6 pointer fields |
| IoContinuation | Medium | capturedLocals, capturedFrame chain |
| IoCFunction | Easy | uniqueName |
| IoFile | Easy | path, mode |
| IoDirectory | Easy | path |
| IoWeakLink | Easy | link (needs remap despite no mark func) |

#### Known complications

1. **Interior pointer in protos**: `IoObject_justAlloc` allocates `IoObjectData + protos[]` as one block. Protos pointer is `data + 1`. Compaction must either:
   - Allocate protos separately (simpler, more fragments), or
   - Remap interior pointer via offset calculation

2. **Inline cache**: IoMessage caches slot lookups. Compaction must invalidate all caches or remap cached pointers.

3. **argValues interior pointer**: IoEvalFrame's `argValues` may point to embedded `inlineArgs[4]` buffer.

4. **COLLECTOR_RECYCLE_FREED**: Currently disabled. Keep disabled during compaction work.

#### Recommendation

Don't implement compaction during the WASM migration. But avoid making it harder:
- Don't add new interior pointer patterns
- Don't add new global caches of IoObject pointers
- Keep mark functions as the single source of truth for reference traversal

---

## 3. Dependency Map

### Core (must compile for WASM)

| Library | External deps | WASM status |
|---------|--------------|-------------|
| basekit | `-lm` (math) | OK — wasi-libc provides math |
| garbagecollector | basekit only | OK — pure C |
| iovm | basekit, garbagecollector, parson | OK with stubs |
| parson | none (embedded) | OK |
| io2c | none (standalone) | Host-only tool |

### Dropped

| Component | Reason |
|-----------|--------|
| `addons/*` | All require dlopen or platform-specific libs |
| `libs/coroutine/` | Replaced by stackless frames |
| `DynLib` functionality | No dynamic loading in WASM |
| `AddonLoader.io` | Depends on DynLib |

### Replaced/Stubbed

| Component | Current | WASM approach |
|-----------|---------|---------------|
| `system()` | Spawns shell | Raise exception |
| `popen()` | Opens pipe to process | Raise exception |
| `getpid()` | Returns PID | Return 1 |
| `daemon()` | Daemonizes | Raise exception |
| `signal()` | Installs handler | No-op |
| `gettimeofday()` | Platform-specific | clock_gettime (WASI) |
| `usleep()` | Platform-specific | nanosleep (WASI) |
| `dlopen/dlsym` | Load .so/.dll | Return NULL |
| `uname()` | Platform detection | Return "wasm-wasi" |
| `sysctl/sysconf` | CPU detection | Return 1 |

---

## 4. Risk Assessment

### Highest risk: io2c bootstrap

The two-stage build (native io2c + cross-compiled WASM) is the most architecturally complex change. If CMake's cross-compilation support fights us, we may need a wrapper script or Makefile layer.

**Mitigation**: io2c is 94 lines of trivial C with zero dependencies. We can pre-generate IoVMInit.c and check it in as a fallback.

### Medium risk: WASI filesystem sandboxing

WASI's preopened directory model may surprise code that assumes free filesystem access. `getcwd`, `chdir`, absolute paths, and symlink resolution may behave differently.

**Mitigation**: Test early with wasmtime's `--dir` flag. Most Io code uses relative paths.

### Medium risk: Test suite coverage under WASM

Some tests may rely on features we stub out (system calls, process spawning). Need to identify and skip those.

**Mitigation**: Run test suite incrementally, track which tests need WASM-specific skips.

### Low risk: Math/NaN handling

WASM has IEEE 754 float semantics. The `USE_BUILTIN_NAN` flag already handles NaN portably.

### Low risk: Memory/performance

WASM's linear memory model with `memory.grow` is a good fit for Io's malloc-based allocation. No architectural changes needed for phase 1. Compaction (phase 2) can reclaim fragmented memory later.

---

## 5. Build Instructions (target state)

```bash
# Install wasi-sdk
# https://github.com/WebAssembly/wasi-sdk/releases

# Stage 1: build native io2c
mkdir -p build-native && cd build-native
cmake .. && cmake --build . --target io2c
cd ..

# Stage 2: cross-compile for WASM
mkdir -p build-wasm && cd build-wasm
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/wasi-sdk.cmake \
         -DIO2C_EXECUTABLE=../build-native/_build/binaries/io2c
cmake --build .

# Run
wasmtime --dir=. _build/binaries/io.wasm -- -e '"hello" println'
wasmtime --dir=.. _build/binaries/io.wasm -- ../libs/iovm/tests/correctness/run.io
```

---

## 6. File Change Summary

### New files
- `cmake/wasi-sdk.cmake` — toolchain file
- `libs/iovm/source/IoWASI_stubs.c` — WASI-specific stubs (compiled only for WASM)

### Modified files
- `CMakeLists.txt` — skip SSE2, skip packaging for WASM
- `libs/CMakeLists.txt` — io2c bootstrap, static-only libs
- `libs/iovm/CMakeLists.txt` — io2c path, skip NAN check, static-only
- `libs/basekit/CMakeLists.txt` — skip `-ldl`, static-only
- `libs/garbagecollector/CMakeLists.txt` — static-only
- `libs/iovm/tools/CMakeLists.txt` — support external io2c path
- `tools/CMakeLists.txt` — skip `-ldl`, `-lcurses`, SANE_POPEN; WASM binary
- `libs/iovm/source/IoConfig.h` — add WASM platform detection
- `libs/iovm/source/IoSystem.c` — stub system/daemon/getpid/cpuCount
- `libs/iovm/source/IoFile.c` — stub popen
- `libs/iovm/source/IoState_debug.c` — skip signal handler
- `libs/basekit/source/DynLib.c` — WASM stubs

### Deleted (phase 3)
- `extras/win32vc10/` — Windows project files
- All `#ifdef WIN32` code paths
- All `#ifdef __SYMBIAN32__` code paths
- NSIS/DEB/RPM packaging logic
