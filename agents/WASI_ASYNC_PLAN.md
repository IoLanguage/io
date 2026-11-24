# WASI 0.3 Async Integration Plan

Status: **prep work landed, 0.3 integration blocked on toolchain** (June 2026)

Reference: [WASI 0.3 announcement](https://bytecodealliance.org/articles/WASI-0.3)

## What WASI 0.3 changes

WASI 0.3 makes async native to the component model. The canonical ABI gains
three first-class constructs:

- `future<T>` — replaces the `pollable` resource
- `stream<T>` — replaces `input-stream` / `output-stream`
- async functions — replaces the 0.2 `start-foo`/`finish-foo`/`subscribe` pattern

Scheduling is completion-based (io_uring/IOCP style): the host runtime owns a
single shared event loop and drives task scheduling across components. The
design explicitly accommodates stackless coroutine runtimes — which is what
this VM is.

## What has landed (works today, WASI 0.1/0.2)

1. **WASI 0.2 component build** (`make component`, `make check-component`).
   `build/bin/io_component.wasm` is a real component (layer-1 binary) built
   via wasi-sdk's `wasm32-wasip2` target and `wasm-component-ld`. The full
   correctness suite passes against it under wasmtime 42.

2. **Scheduler timer queue** (`Scheduler addTimerAt/addTimer/removeTimerFor/
   wakeExpiredTimers/idleUntilNextTimer` in `libs/iovm/io/Exception.io`).
   `Object wait` parks the current coroutine on the timer queue instead of
   busy-yielding. Expired timers re-enter the run queue on every
   `yield`/`pause`. When nothing is runnable but timers are pending, the VM
   blocks in **one** host wait until the nearest deadline
   (`Scheduler idleUntilNextTimer` → `System sleep`).

   That single idle point is the WASI 0.3 seam: under 0.3 it awaits a host
   future instead of sleeping.

3. **Dead-ancestor walk in the eval loop** (`IoState_iterative.c`, empty-frame
   handler). When a coroutine finishes, the eval loop resumes its
   `parentCoroutine`. Timer wakeups let a coroutine outlive the coroutine
   that started it, so the walk now skips dead ancestors (no saved frames)
   to find the nearest resumable one. Without this, a finishing coroutine
   whose starter already finished hit the `nestedEvalDepth > 0` early-return
   and stranded every parked coroutine.

## The browser already works this way (November 2026)

The browser build has a host-driven scheduler (`browser/io_browser.c`,
`agents/wasm/subplans/Async.md` "Layer 3 as built"): the VM never sleeps;
a tick runs coroutines until nothing is runnable and returns the next timer
deadline to the host, which calls back in. JS Promises park coroutines the
way host futures would. It is a working model for the 0.3 integration below,
and it added to the list below: Io `forward` methods and
`aBlock call(...)` are now ordinary frames, so code under them can park, and
a locals object that something still refers to is no longer recycled.

## VM contracts to preserve (learned the hard way)

- **Spurious wakeups are normal.** A finished child coroutine resumes its
  parent directly, bypassing the scheduler. Anything that parks a coroutine
  must use condition-variable semantics: loop, re-check the condition
  (deadline, future resolved, …), re-park if unsatisfied. `Object wait` does
  this; a future `awaitHostFuture` must too.
- **Timer entries can go stale.** A coroutine woken by something other than
  its timer must remove its entry (`Scheduler removeTimerFor`), or
  `wakeExpiredTimers` will later re-enqueue a coroutine that is already
  running or dead — and `resume` on a dead coroutine restarts its body.
- **Coroutine bodies resolve chain heads against `runTarget`.** `coroDoLater`
  sets `runTarget := self`, so method locals are NOT visible to chain-head
  messages (argument evaluation does see them, which makes failures look
  intermittent). Use `coroDo`/`coroFor` (runTarget = sender) for bodies that
  capture locals. Misuse shows up as a swallowed Importer "does not respond
  to" exception that silently kills the coroutine.
- **Locals that escape are not recycled.** A block's scope, a coroutine's
  `runLocals` (`coroFor`, `coroDo`), a stored `thisLocalContext`: storing a
  reference to a locals object marks it (`IoObject_addingRef_`), and the
  RETURN handler then leaves it out of the pool. Before November 2026 it was
  pooled regardless and the next method call wiped it, which is the likeliest
  cause of the "intermittent" coroutine-body failures described above. Code
  that stores a locals pointer without `IOREF` bypasses the mark.
- **Don't park under a nested C eval loop.** A coroutine switch inside a C
  function that is evaluating Io code itself (`sortBy` blocks, anything
  calling `IoMessage_locals_performOn_` mid-eval) leaves that C call
  half-finished once control has to return to the host. Natively this
  usually works by luck, because control comes back before the C call
  returns. A host-driven scheduler must refuse the switch (the browser's
  `Scheduler hostCanSwitch`) or the primitive must be made frame-based.

## The 0.3 integration, when unblocked

Target shape: a `HostFuture` primitive owned by C, with the scheduler extended
from "timers only" to "timers + host futures".

1. `Scheduler idleUntilNextTimer` generalizes to `Scheduler idle`: collect the
   nearest timer deadline and all pending host futures, and make one blocking
   host call (`waitable-set.wait` in the 0.3 canonical ABI) instead of
   `System sleep`.
2. `File read`/`write` and any socket primitive lower onto `stream<T>`; the
   calling coroutine parks on the paired completion future and re-enters the
   run queue when the host completes it. No Asyncify, no transform: parking
   is the same heap-frame operation as a coroutine switch.
3. `@`/`@@` actor futures (`Actor.io`) optionally back `Future setResult` with
   host-future completion so an actor awaiting host I/O consumes no VM
   scheduling at all.
4. Export the VM's eval entry as an async component function so embedders can
   call Io code without blocking their event loop (service chaining).

## Upgrade triggers (re-check before starting)

- **wasmtime ≥ 46 installed** (0.3.0 interfaces, async on by default).
  Local machine has 42.0.1 — `wasmtime --version` (re-checked 2026-11-23).
- **wasi-sdk / wit-bindgen C support for 0.3 async** (`future<T>`/`stream<T>`
  lowering and `waitable-set` intrinsics from C). At the time of writing,
  guest toolchain support was rolling out for Rust/Go/JS/Python first;
  wasi-sdk 25 has no 0.3 target.
- **jco 0.3 support** if the browser target should share the same model.

When both land: regenerate nothing — start from `Scheduler idle` (step 1) and
keep `System sleep` as the fallback for hosts without 0.3.
