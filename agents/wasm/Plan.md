# Io Language WASM Migration Plan

## Strategic Direction

The goal is to **target WASM as Io's only platform**, eliminating all OS-specific dependencies and multi-platform C code. This dramatically simplifies maintenance and leverages WASM as a universal runtime. The primary target is **WASI** (using Wasmtime with Cranelift JIT) with browser as a secondary target.

## Status (November 2026)

| Phase | State | Where |
|-------|-------|-------|
| 1. Minimal WASI build | **Done**, with a Makefile and a native `io2c` instead of the CMake toolchain described below | `Makefile`, `build/bin/io_static` |
| 2. File I/O via WASI | **Done**: the correctness suite runs under wasmtime (`make check`) | CI |
| 3. Platform cleanup | **Done**: CMake, `addons/`, `extras/win32vc10/` and the `WIN32`/Symbian code paths are removed (a few comments and `PortableStdint.h` remain) | |
| 4a–4d. Browser REPL, bridge, callbacks | **Done** | [Bridge.md](Bridge.md) |
| 4e. Async: Futures, await, implicit forward | **Done** (steps 1–5) | [subplans/Async.md](subplans/Async.md) |
| 4e. Async: Layer 3, Io scheduler in the JS event loop | **Done** as Phase 4i | [subplans/Async.md](subplans/Async.md) |
| 4f. BigInt | **Done** | [subplans/BigInt.md](subplans/BigInt.md) |
| 4g. Rich JS objects | Deferred until a use case needs it | [subplans/Async.md](subplans/Async.md) Step 6 |
| 4h. JS module and npm package | Module **done** (PR #507), shim removed. npm packaging **done** (PR #508). First version `2026.11.16-alpha` published by hand 2026-11-16; later releases publish from the Release workflow by npm trusted publishing (no token) | [JS_Module_Refactor.md](JS_Module_Refactor.md) |
| 4i. Browser scheduler | **Done** (November 2026): coroutines, `@` actors, `wait` and concurrent awaits run inside the JS event loop | [subplans/Async.md](subplans/Async.md) "Layer 3 as built" |
| 4j. Time slices and interrupt | **Done** (2026-11-22, PR #548; background stop 2026-11-23, PR #553): `Io.load({ timeSlice })`, `io.interrupt()`, `io.interruptBackground()`, Stop in the REPL | [subplans/Async.md](subplans/Async.md) "Time slices and interrupt" |
| 5. Compacting collector | Designed, not started; an optimization | [subplans/CompactingGC.md](subplans/CompactingGC.md) |
| 6. Distribution | GitHub Release artifacts **done** (PRs #501, #502); npm in 4h | `.github/workflows/release.yml` |
| 7. WASI 0.2 component / 0.3 async | 0.2 component build **done**; 0.3 **blocked on toolchain** (wasmtime ≥ 46, wasi-sdk 0.3 async for C) | [../WASI_ASYNC_PLAN.md](../WASI_ASYNC_PLAN.md) |

### Recommended next

Updated 2026-11-25. Everything on master is released as `2026.11.25`. No
PRs are open. Open issues: #441 (terminal half only: WASI has no signals)
and #353 (no memory limit for a Sandbox).

Nothing is queued. The five items listed here on 2026-11-23 are done (see
"Recently done").

Parked, worth doing only if a real use turns up:

- **`sortBy`, `sortInPlaceBy` can't wait.** The comparison block runs
  inside C `qsort`. A merge sort in Io was measured at about 3x slower on
  20,000 numbers, so this needs a sort written as a frame state machine.
  Also unable to wait: `Date cpuSecondsToRun`, and Io callbacks JS calls
  synchronously during an Io call (not fixable from the Io side).
- **Ctrl-C in the terminal REPL** (the rest of #441). WASI delivers no
  signals, so under wasmtime Ctrl-C ends the process. Nothing to do until
  the host offers a way.
- Phase 4g (rich JS objects) and Phase 5 (compacting collector):
  deferred. Phase 7 (WASI 0.3): blocked on the toolchain; the local
  wasmtime is still 42.0.1 and the plan needs 46 or later.

Housekeeping:

- **`native` is unmaintained** (decided 2026-11-22; README notices on both
  branches). Fixes are no longer ported to it.

### Recently done

- **Released `2026.11.25`**: stdout is flushed before standard input is
  read, so the REPL prompt appears before the user types (#557, issue
  #556). Workflow actions moved off Node 20 (#555).
- **Released `2026.11.23`**:
  - **Output while an eval runs** (#550): `io.eval(code, { onOutput })`;
    the REPL shows a long eval's output live.
  - **Sandbox limits enforced** (#551, issue #353): `setTimeLimit` and
    `setMessageCount` are checked by a host poll installed in the
    sandbox's VM; the count is in evaluator steps.
  - **Stopping background code** (#553): `io.backgroundBusy`,
    `io.onBackgroundBusy`, `io.interruptBackground()`. Nothing is stopped
    automatically, and coroutines that wait or yield are never reported
    or stopped. Also fixed `loop(yield)` stalling with time slices on
    (`yield` and `pause` now finish before a slice ends).
  - **`CLAUDE.md` refreshed** (#552).
  - **Time slices and interrupt** (#548, issue #441). The eval loop calls
    `IoState.hostPollCallback` between steps every 1024 steps. The browser
    host uses it to return to JS when a time slice is used up
    (`Io.load({ timeSlice })`, off by default) and to raise an
    uncatchable "interrupted" error in the eval in progress
    (`io.interrupt()`). The REPL pages load with a 50 ms slice and a Stop
    button. Cost with no slice set: within noise.
  - **More code can wait in the browser** (#546): `lexicalDo`,
    `interpolate` and `sortInPlace(expression)`, which also became about
    3x faster because the expression runs once per item.
  - **Source text in wide Sequences** (#547): `doString`, `asMessage`,
    `Message fromString` and `interpolate` failed or truncated when the
    string held a non-ASCII character. Source files were never affected.
  - **Backtraces hide the VM's `opShuffle` line** on a compile error
    (#545).
- **Released `2026.11.22`**: parser and exception fixes from the issue
  backlog. Negative literals (`-1 acos`, #489); an operator directly
  before a minus (`a:=-1`, `x*-1`); operators added mid-file take effect,
  because run source is shuffled one statement at a time (#494, #438); a
  colon is no longer an identifier letter (`a:=2`, #253); `return`,
  `break` and `continue` pass through `try` and `catch` (#37); future
  sends leave code arguments unevaluated (#55); assignment with arguments
  on the left; the colvmn submodule over https (#504).
- **Issue backlog cleared** (2026-11-21 and 22): pre-WASM issues closed at
  the maintainer's request; #404 and #434 closed with replies.
- **VM bugs found while building the scheduler: all fixed.** Syntax errors
  crashing the VM (#511); the retain-stack growth in loops and coroutines
  ([../stackless/Debug_Loop_Leak.md](../stackless/Debug_Loop_Leak.md), #513);
  `make` not tracking header dependencies (#515); and backtraces saying
  "nothing on stack" (#516). An exception now records its trace from the
  frame chain when it is raised (`Exception stackTrace`, `stackTraceLines`;
  `IoState_recordStackTraceOn_`). Known limit: a tail call reuses its
  caller's frame, so the caller of a method's last call has no entry.
- **Released `2026.11.20`**: everything below dated 2026-11-19.
- **Released `2026.11.19`**: backtraces, CLI exit status 1 on an uncaught
  exception (#517), header dependencies.
- **Outside PRs #491 and #492** (2026-11-19, released in `2026.11.20`), both by Weixie
  Cui and both closed in favour of our PRs that carry their fixes: #521 on
  master and #522 on `native`.
  - `List fromEncodedList` cleared the reference id before sending
    `objectForReferenceId(id)`, so the hook always received nil. Also new:
    an error raised by the hook stops the decode.
  - `Date setDay` accepted day 31 for April, June and September.
- **`Date setMonth` and `setYear` keep the month** (2026-11-19,
  released in `2026.11.20`). A day the new month doesn't have used to carry into the
  month after (Jan 31 `setMonth(2)` gave March 3); it now becomes the
  month's last day. Same for `setYear` on Feb 29. Found while fixing
  `setDay`.
- **`FRAME_STATE_AWAIT_JS` removed** from the core VM (2026-11-19), with
  `awaitingJsPromise`, `suspendedCoro` and the frame's `awaitInfo`. Nothing
  had used them since Phase 4i.
- **The Io-side handle table grows** (2026-11-19, released in `2026.11.20`). It was
  fixed at 1024 entries (Io objects JS holds, plus unsettled Promises) and
  rejected new Promises when full; it now doubles when it fills
  (`ioHandles_grow`, `browser/io_js_bridge.c`).
- **Pages use `<script type="module">`** (2026-11-19, released in `2026.11.20`):
  `browser/index.html`, `browser/test.html` and `repl/index.html` import
  the module directly, and the `io.js` shim is deleted. The release's
  browser kit ships `index.html` instead of `io.js`.
- **#479 closed** (2026-11-19), the last open outside PR. Nothing in it
  applied to master: its build scripts were for the removed CMake build,
  DynLib is gone, the parser cases are now lexer errors, and `File atPut`
  no longer evaluates its position twice because arguments are evaluated
  before a primitive runs.
- **CI builds with wasi-sdk 25**, the same version as the local SDK in
  `deps/wasi-sdk` (was 24). Benchmark history may show a step at this
  commit.
- **More code can wait in the browser** (2026-11-19, released in `2026.11.20`):
  `IoState_evalInFrame_` lets a primitive hand code to the eval loop as a
  frame. Used by `call evalArgAt`, `perform`, `performWithArgList`, `do`
  and the `init` run by `clone`, which also makes List `map`, Map `select`
  and `foreachSlot` waitable. Side effect: `clone` with an `init` is about
  2x faster and `perform` about 1.6x, since no nested eval loop starts.
  A second batch the same day: `Number repeat` runs on the `for` frame
  state and Sequence `foreach` on the `foreach` frame state, which also
  makes List `select` and `selectInPlace` waitable (they use `repeat`).
  `n repeat(literal)` no longer evaluates anything per iteration.
- **Nested lists print correctly** (2026-11-19, released in `2026.11.20`):
  `list(list(2))` printed as `list(list("2"))` because `List
  asSimpleString` applied `asSimpleString` to its elements twice. Present
  since April (#36's fix), so `native` has it too.
- **Build**: the Makefile finds wasi-sdk in `deps/wasi-sdk` (untracked)
  before `~/wasi-sdk` (#519). colvmn pinned to `1a57f19` (#520).

---

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

> Phases 1–3 below are the original plan, kept for history. They were
> implemented with a Makefile and a native `io2c` instead of CMake; see the
> status table above.

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
- JS glue (originally `browser/io.js`, now the `browser/src/` ES module) — WASM loader + minimal WASI shim (fd_write capture, clock, proc_exit)
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

#### Phase 4d: Callable IoProxy (Io blocks as JS callbacks) - COMPLETE
- IoProxy uses function target with `apply` trap — Io objects are callable from JS
- `block(x, x * 2)` crosses as TYPE_IOREF, JS gets a callable Proxy
- When JS calls the Proxy, `apply` trap fires → `io_send(handle, "call", args...)`
- Enables: `setTimeout(block(...), ms)`, `addEventListener("click", block(...))`, `Array.map(block(...))`, etc.
- No C changes needed — JS-only change to `makeIoProxy`

#### Phase 4e: Async — Run Loop, Futures, Scheduler

JS owns the run loop. Io is a guest. Every entry into Io from JS is a short, synchronous call that returns promptly. Three layers: eval loop yield-to-host, Future type with auto Promise detection, JS-side scheduler driving Io coroutines via `requestIdleCallback`/`requestAnimationFrame`.

**See [subplans/Async.md](subplans/Async.md) for full design.**

**Step 1 COMPLETE**: IoFuture proto + TYPE_FUTURE wire format
- `io_future.c`/`.h` — Future type with await, isReady, state, label, setLabel, forward, type methods
- TYPE_FUTURE (12) wire type on both sides, auto-detection of JS thenables in serializeToWasm
- `js_watch_promise` import wires `.then()`/`.catch()` to `io_resolve_future`/`io_reject_future` exports
- `forward` delegates unrecognized messages (then, catch, etc.) to underlying JS Promise

**Step 2 COMPLETE**: FRAME_STATE_AWAIT_JS + eval loop yield/resume
- AWAIT_JS frame state in eval loop; `await` on pending Future suspends coro and yields to JS host
- `io_resume_eval` WASM export resumes suspended coro when Promise settles
- Saves *active* coroutine (handles `try()` child coro frame swap correctly)
- `do_resume_eval` supports chained awaits (re-yields if another await is hit)
- JS side: `resumeIfAwaiting()` + `pendingAsyncResolve` callback mechanism
- Steps 3+4 (resolve/reject, error handling) subsumed — all tested end-to-end
- 68/68 browser tests passing (2 new asyncAwait tests)

**Step 5 COMPLETE**: Implicit forward — auto-await on unrecognized messages
- Future's `forward` method: resolved → frame redirect to resolved value; pending → AWAIT_JS yield; rejected → error
- AWAIT_JS resume goes to LOOKUP_SLOT (not ACTIVATE) — correct for both explicit await and implicit forward
- `then`/`catch`/`finally` explicit methods delegate to underlying JS Promise
- 73/73 browser tests passing (5 new implicit forward tests)

**Difficulty**: Hard — **Milestone**: `JS fetch(url)` yields to JS, resumes on resolve, no callbacks in Io code. **Reached.**

**Layer 3 (JS-side scheduler) COMPLETE** as Phase 4i, which also replaced the Step 2 mechanism: `await` now parks the coroutine with the scheduler instead of leaving the eval loop through `FRAME_STATE_AWAIT_JS`, so several coroutines can await at once.

#### Phase 4f: BigInt — COMPLETE

- Vendored libtommath v1.3.0, created IoBigInt type, added TYPE_BIGINT (13) to wire format
- Full arithmetic, comparison, conversion methods; JS BigInt round-trips losslessly
- See [subplans/BigInt.md](subplans/BigInt.md) for full design

#### Phase 4g: Rich JS objects (future, driven by real use cases)

- Formerly Async Step 6 — deferred until needed
- JsXhrRequest, JsEvent, JsPersistence wrapping platform-specific JS objects
- See [subplans/Async.md](subplans/Async.md) Step 6 for design

**Difficulty**: Medium

#### Phase 4h: JS module and npm package — module COMPLETE, npm READY

- `browser/io.js` split into the `browser/src/` ES module: `class Io` with `Io.load()`, `await io.eval()`, `io.lobby`, `onOutput`, `dispose`. It works in browsers and Node, and each VM is independent.
- Node tests (`make check-browser-node`) in CI.
- `@iolanguage/io` package staged by `make npm-package` and checked by `make check-npm-package`. The Release workflow publishes it on a release tag by npm trusted publishing, with no token. The `iolanguage` npm org exists and `2026.11.16-alpha` was published by hand on 2026-11-16 (a package must exist before a trusted publisher can be added).
- The pages (`browser/index.html`, `test.html`, `/repl/`) import the module with `<script type="module">`; the `io.js` shim that first kept them working was removed on 2026-11-19.
- See [JS_Module_Refactor.md](JS_Module_Refactor.md).

#### Phase 4i: Browser scheduler — COMPLETE

Io coroutines, `@` actors and `wait` run inside the JS event loop. Io never blocks the JS thread.

- Every entry from JS runs one scheduler tick: a short-lived coroutine (`Scheduler hostRun`, Io source in `browser/io_browser.c`) resumes runnable coroutines until none are left or a 10 ms budget runs out, then tells JS when it next has work. `class Io` sets a timer for that, and ticks again when a Promise settles.
- An eval is a coroutine. If it parks, `io_eval_input` returns 2 and a later `io_tick` finishes it. An eval that nothing can wake fails with "nothing left to resume", as native Io does.
- `await` on a JS Promise parks the coroutine on the Future, so any number of coroutines can await at once.
- JS calls into Io from the event loop (proxy sends, event handlers, timers) run as coroutines too. One that parks returns `null` to JS and finishes later.
- Core VM changes, needed so parked code isn't inside a nested C eval loop: Io `forward` methods and `aBlock call(...)` are activated as ordinary frames (`IoState_iterative.c`). Side effects: recursion through either no longer uses the C stack, and a `yield` inside one no longer runs part of the body twice.
- Where switching still can't work (nested C eval loops), it raises an Io error instead of hanging.
- VM bugs fixed on the way, all present natively:
  - **Closures over method locals were broken.** A method's locals object went back to the reuse pool when the method returned, even if a block created there still had it as its scope; the next method call wiped it (`counter := method(n, block(n = n + 1; n))` failed). `IoObject_addingRef_` now marks a locals object when a reference to it is stored, and marked locals are not pooled. Cost: slot assignment about 2–3% slower in `bench/micro.io`; everything else within noise.
  - **`List_removeLast_` never removed anything** (unsigned index compared with -1), so `IoState_stopRetaining_` did nothing and every retained object lived forever.
  - **A message chain sent to an actor future was applied twice** (`Actor.io`).
- Bridge bugs fixed on the way:
  - Each JS Promise held an Io-side handle forever, so after about 1000 Promises every later `await` hung. Handles are released when the Promise settles.
  - A Future never released its Promise's JS handle; a temporary wrapper could release it early.
  - `aFuture then(...)`, `catch` and `finally` evaluated the rest of the statement a second time.
  - Calls from the JS event loop and Promise settlements left their objects on the main coroutine's retain stack.
- Tests: `SchedulerTimerTest.io` (3 new), `ClosureTest.io` (7 new), `browser/test/vm.test.mjs` (16 new).

See [subplans/Async.md](subplans/Async.md) "Layer 3 as built".

**Milestone reached**: `A @work` returns a value in the browser; `wait(1)` doesn't freeze the page; a coroutine parked on `wait` wakes on time with no further evals.

---

### Phase 5: Compacting collector (optimization)

The current Baker treadmill collector fragments memory over time. A compacting collector would reduce memory usage for long-running browser sessions. Codebase is moderately ready (7/10). Not needed during WASM migration, but avoid making it harder (no new interior pointers, no new global caches).

**See [subplans/CompactingGC.md](subplans/CompactingGC.md) for full design.**

---

### Phase 6: Distribution

- GitHub Releases carry `io_static.wasm`, `io_component.wasm`, `io_browser.wasm` and a browser kit — **done** (`.github/workflows/release.yml`).
- npm — see Phase 4h.

### Phase 7: WASI component model

- WASI 0.2 component (`make component`) — **done**; the full suite passes against it.
- WASI 0.3 native async — **blocked on toolchain**. See [../WASI_ASYNC_PLAN.md](../WASI_ASYNC_PLAN.md) for the plan and the upgrade triggers to re-check.

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

> Historical: this is the CMake flow the plan started with. The build that
> exists is the Makefile; see the README or `CLAUDE.md`.

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
