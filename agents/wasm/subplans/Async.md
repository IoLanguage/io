# Phase 4e: Async — Run Loop, Futures, Scheduler

**Status (November 2026):** complete. Layer 3 (the scheduler) was built as Phase 4i, and it changed how Layers 1–2 work: see "Layer 3 as built" below. The sections on `FRAME_STATE_AWAIT_JS`, `io_resume_eval` and `waitingCoro` describe the first implementation, which has since been removed from the VM, and are kept for history.

**Goal**: Io code can call async JS APIs, handle browser events, and run background coroutines — all as a guest inside the browser's event loop.

## Mental model

JS owns the run loop. Io is a guest. Every entry into Io from JS is a **short, synchronous call** that returns promptly. Io never blocks, never spins. Long-running Io work lives in coroutines that yield in small chunks.

```
Browser event loop
  │
  ├─ Promise resolves  ──→  JS calls io_resolve(future_h, value)  ──→  Future resolved, coro marked ready
  ├─ DOM event fires    ──→  JS calls io_send(handler)             ──→  Io handler runs, returns
  ├─ Idle time          ──→  JS calls io_scheduler_tick             ──→  scheduler runs ready coros
  ├─ Animation frame    ──→  JS calls io_scheduler_tick             ──→  scheduler runs ready coros
  │
  └─ (JS resumes its own work)
```

## Io programmer's view

```io
// Async JS calls — explicit await
response := JS fetch("/api/data") await   // yields coro, resumes when resolved
body := response text await                // yields again (text returns Promise)
body println

// Later: implicit await via forward (transparent proxy)
response := JS fetch("/api/data")          // returns Future
body := response text                      // auto-awaits, then forwards "text"

// Event handlers return promptly
handler := block(event,
    event preventDefault
    Scheduler schedule(block(doExpensiveStuff))
)
JS document body addEventListener("click", handler)

// Background coroutines yield in small chunks
doExpensiveStuff := method(
    1000 repeat(i,
        processItem(i)
        if(i % 100 == 0, yield)
    )
)
```

---

## IoFuture object

The Future is an Io object that wraps a JS Promise. It tracks everything needed for the scheduler to resume the right coroutine with the right value.

```c
typedef struct {
    int promiseHandle;      // JS Promise handle (in jsHandles)
    IoObject *waitingCoro;  // coroutine that called await (NULL if nobody waiting)
    IoObject *value;        // resolved value (NULL while pending)
    int state;              // 0=pending, 1=resolved, 2=rejected
    IoObject *label;        // optional description (e.g., "fetch /api/data")
} IoFutureData;
```

### Methods

| Method | Behavior |
|--------|----------|
| `await` | If resolved, return value. If pending, suspend current coro and yield to JS. |
| `isReady` | Return true/false without blocking. |
| `state` | Return "pending", "resolved", "rejected". |
| `label` | Get/set a human-readable description of what this Future is waiting on. |
| `waitingCoro` | Return the coroutine waiting on this Future (or nil). |
| `forward` | (Phase 2) Auto-await, then forward the message to the resolved value. |

### Timeouts

Timeouts are JS-side. Io doesn't have its own timer — it can't fire one without JS poking it. The JS side provides the timeout via `Promise.race` or `AbortController`:

```io
// AbortController (preferred for fetch)
controller := JS get("AbortController") new
JS setTimeout(block(controller abort), 5000)
JS fetch("/api", Map clone atPut("signal", controller signal)) await

// Or: convenience wrapper (Io-level, delegates to JS Promise.race)
JS fetch("/api") awaitWithTimeout(5000)
```

`awaitWithTimeout(ms)` could be an Io method that constructs a `Promise.race` on the JS side — wrapping the Future's Promise with a timeout Promise. This keeps all timer logic in JS while giving Io a clean API.

### Lifecycle

1. JS function returns a Promise → bridge creates IoFuture (pending, promiseHandle set)
2. Io code calls `future await` → Future stores `waitingCoro`, sets `FRAME_STATE_AWAIT_JS`
3. Eval loop yields → writes `(promiseHandle, futureIoHandle)` to bridge_buf → returns `IO_EVAL_AWAITING`
4. JS reads bridge_buf, attaches `.then(v => io_resolve(futureH, v))` on the Promise
5. Promise resolves → JS calls `io_resolve(futureH, value)`
6. C side: sets Future's value + state, marks waitingCoro as ready
7. Next scheduler tick: resumes waitingCoro, `await` returns the resolved value

---

## Layer 1: Eval loop yield-to-host

The stackless eval loop already maintains a heap-allocated frame stack. We add:

- **`FRAME_STATE_AWAIT_JS`**: new frame state that breaks out of the eval loop without tearing down the frame stack. The eval loop returns a status code (`IO_EVAL_AWAITING = 2`) to JS.
- **Yield protocol**: before returning, the eval loop writes to bridge_buf:
  - `[promise_handle: i32]` — which JS Promise to watch
  - `[future_io_handle: i32]` — which IoFuture to deliver the result to
- **`io_resolve(future_handle)` WASM export**: JS calls this when the Promise resolves. Value is pre-serialized in bridge_buf. C side deserializes, sets the Future's value/state, marks its waitingCoro as ready.
- **`io_reject(future_handle)` WASM export**: same, but sets state=rejected and raises an Io exception on the waiting coroutine.
- **Resume path**: the scheduler (or `io_scheduler_tick`) picks up the now-ready coroutine and re-enters the eval loop. The `await` frame sees the Future is resolved and returns the value.

This is analogous to coroutine yield, except we yield to the JS host instead of to another Io coroutine.

### Eval loop entry point for resume

`io_eval_input()` → `do_eval()` → `IoState_doCString_()` creates a message and enters the eval loop. On yield, this whole chain returns. On resume, we can't re-enter via `IoState_doCString_` (that would create a new message). We need:

- **`IoState_resumeEval(state)` / `io_scheduler_tick()`**: re-enters the eval loop with the existing frame stack. The current frame is already set up — just restart the `while(frame)` loop.

## Layer 2: Future type + Promise detection

- **JS side**: in `js_call` / `js_call_func`, after getting the result, check `result && typeof result.then === "function"`. If true, serialize as `TYPE_FUTURE` — a new wire type carrying the Promise's JS handle.
- **C side**: `deserialize_buf_to_io` handles `TYPE_FUTURE` by creating an `IoFuture` object (pending, promiseHandle set).
- **No special Io syntax**: `JS fetch(url)` goes through the normal bridge. The only difference is the return value is a Future instead of a JSObject.

### Wire format

```
TYPE_FUTURE = 12  -> [12][promise_handle: i32 LE]    (5 bytes)
```

## Layer 3: JS-side event dispatch + scheduler

> This section is the original design. What was built differs; see
> "Layer 3 as built" at the end of this section.

JS is responsible for calling into Io at the right times:

- **Promise resolve/reject**: wired automatically when the eval loop yields. JS reads `(promiseHandle, futureIoHandle)` from bridge_buf, attaches `.then(v => { serialize(v); io_resolve(futureH); })`.
- **DOM events**: already work via callable IoProxy (Phase 4d). The handler runs synchronously inside `io_send` and returns.
- **Scheduler tick**: new WASM export `io_scheduler_tick()`. JS calls this during idle time / animation frames. Returns: 0 = no more ready coros, 1 = more work to do, 2 = yielded for another Promise.

```js
// JS runtime (now class Io in browser/src/vm.js)
function scheduleIoWork() {
    requestIdleCallback((deadline) => {
        while (deadline.timeRemaining() > 1) {
            const status = wasm.exports.io_scheduler_tick();
            if (status === 2) handleAsyncYield();  // wire up newly yielded Promise
            if (status === 0) break;               // nothing to do
        }
        scheduleIoWork();  // keep polling
    });
}
```

## Layer 3 as built (Phase 4i, November 2026)

The C side is `browser/io_browser.c`; the JS side is `class Io` in `browser/src/vm.js`.

**Ticks.** JS never resumes a particular coroutine. Each entry runs one tick: C starts a fresh coroutine running `Scheduler hostRun` (Io source embedded in `io_browser.c`). `hostRun` wakes expired timers, then resumes coroutines from `yieldingCoros` one at a time, keeping itself on the queue so that a coroutine which yields or pauses always has something to switch to and control comes back. It stops when the queue is empty or after `Scheduler hostBudget` (10 ms), and returns the seconds until it is next needed: 0 (more work now), a timer delay, -1 (waiting on JS Promises) or -2 (nothing due at all).

**Exports.** `io_tick()` runs a tick. `io_next_wake_ms()` reports the above. `class Io` sets one `setTimeout` from it (`schedule`), and calls `pump()` straight away when a Promise settles. `io_resume_eval` is gone.

**Evals.** `io_eval_input` runs the code as a coroutine (`Scheduler hostEval`) at the head of a tick. If the coroutine parks, the export returns 2 and a later tick reports the result; `io_get_eval_output_len` says how much of the output buffer is the eval's, and anything after it goes to `onOutput` listeners. If the eval is parked and a tick ends with nothing due (-2), it fails with "nothing left to resume", as native Io does; the coroutine stays parked and can still be resumed by a later JS callback.

**Promises.** `await` is Io code: it adds the coroutine to the Future's `waiters` list and calls `pause`, in a loop until the Future is ready (a parked coroutine can be woken early). `io_resolve_future`/`io_reject_future` move the waiters back onto `yieldingCoros`. `forward` awaits, then sends just that one message to the value. So several coroutines can await different Promises at once, which the single `suspendedCoro` could not do.

**JS calling Io.** From the JS event loop, `io_send` runs the message as a coroutine at the head of a tick and returns to JS as soon as it finishes or parks. If it parks, JS gets `null` and later ticks carry it on. From inside an Io call (JS calling back synchronously) it runs nested on the caller's coroutine, as before.

**The nested-loop rule.** Switching coroutines works only from frames that the tick's own eval loop is running. A C function that evaluates Io code itself does so in a nested C eval loop, and a switch there would leave the C call half-finished. Two common cases were moved onto ordinary frames in the core VM (`IoState_iterative.c`): Io-level `forward` methods, which actor `FutureProxy` and JS Futures depend on, and `aBlock call(...)`, which is how JS invokes Io callbacks. A third group followed on 2026-11-19 through `IoState_evalInFrame_`, which turns the calling frame into a `DO_EVAL` frame (with an optional fixed result): `call evalArgAt`, `perform`, `performWithArgList`, `do`, and the `init` that `clone` runs. `Number repeat` now runs on the `for` frame state (which accepts a missing counter name) and Sequence `foreach` on the `foreach` frame state; List `select`/`selectInPlace` are written with `repeat`, so they followed. For the rest (the `sortBy` family, `interpolate`, `lexicalDo`, `Date cpuSecondsToRun`, a callback JS makes synchronously during an Io call), `Coroutine resume` checks `Scheduler hostCanSwitch` (`nestedEvalDepth <= 1`) and raises an Io exception rather than hanging.

**Memory.** Three rules keep a tick from leaving anything behind. Every entry from the JS event loop (`io_eval_input`, `io_tick`, a top-level `io_send`, a Promise settling) wraps its work in a retain pool on the main coroutine's stack, since objects allocated outside any coroutine land there and nothing else pops it. Nothing is retained through the Collector: the eval in progress is kept alive by the `Scheduler hostEvalCoro` slot. And a coroutine known to be finished has its retain stack cleared (`release_finished`, `Scheduler hostRelease`), because a finished coroutine otherwise keeps the coroutines its last frames touched, and with the `parentCoroutine` links that can chain back through every earlier tick. Checked by the "ticks, evals and JS calls leave nothing behind" test. Long-lived coroutines and actors no longer grow: the VM's retain-stack growth in loops and on coroutine switches is fixed (`agents/stackless/Debug_Loop_Leak.md`).

**Handles.** A Promise's Io-side handle is released when it settles; its JS-side handle is owned by the Future and released when the Future is collected.

**Removed from the core VM (2026-11-19):** `FRAME_STATE_AWAIT_JS`, `IoState.awaitingJsPromise`, `IoState.suspendedCoro` and the frame's `awaitInfo`, unused since Phase 4i.

## Scheduler introspection

The Scheduler is an Io-level object. It tracks all coroutines and their states. Everything is inspectable from Io code or from JS via the bridge.

```io
// What's running?
Scheduler activeCoros        // list of all non-finished coroutines
Scheduler waitingCoros       // list of coros suspended on Futures
Scheduler readyCoros         // list of coros ready to run

// What's each one waiting on?
Scheduler waitingCoros foreach(coro,
    future := Scheduler futureFor(coro)
    writeln(coro label, " waiting on: ", future label, " (", future state, ")")
)

// Example output:
//   fetchData waiting on: fetch /api/users (pending)
//   loadImage waiting on: fetch /images/hero.png (pending)
//   compute ready to run
```

### Data model

The Scheduler maintains:
- **readyQueue**: list of coroutines ready to run (round-robin)
- **waitingMap**: map of coroutine → Future (coros suspended on Promises)
- **allCoros**: list of all managed coroutines (for introspection)

When `io_resolve` marks a coro as ready, it moves from waitingMap to readyQueue. `io_scheduler_tick` pops from readyQueue and runs.

### Labels

Both coroutines and Futures support `label` — a human-readable string for debugging. Set automatically where possible (e.g., Future label = the URL passed to fetch), settable manually otherwise.

---

## Implementation order

### Step 1: IoFuture proto + TYPE_FUTURE wire format — COMPLETE
- `browser/io_future.c`/`.h`: IoFuture proto with await, isReady, state, label, setLabel, forward, type methods
- TYPE_FUTURE (12) added to C and JS serialization
- JS-side Promise/thenable detection in `serializeToWasm` (before JSREF fallback)
- `js_watch_promise` WASM import: C calls JS to wire `.then()`/`.catch()` on creation
- `io_resolve_future`/`io_reject_future` WASM exports: JS calls C when Promise settles
- `forward` method delegates unrecognized messages to JS Promise (enables `.then(callback)`, `.catch()`, etc.)
- Proto registered in `io_browser.c` `io_init()`
- 9 new browser tests covering: type detection, pending/resolved states, await, reject, error propagation, labels
- **Note**: `Promise.resolve()` is async (microtask queue), so Futures start pending even for already-resolved Promises. They resolve after the next microtask flush.

### Step 2: FRAME_STATE_AWAIT_JS + eval loop yield/resume — COMPLETE
- FRAME_STATE_AWAIT_JS added to eval loop state machine (`IoEvalFrame.h/c`, `IoState_iterative.c`)
- `IoFuture await` (pending case) sets AWAIT_JS frame state + `awaitingJsPromise` + `needsControlFlowHandling`
- Eval loop AWAIT_JS handler: if `awaitingJsPromise`, yields (returns ioNil); on resume, transitions to ACTIVATE
- `IoState.h`: added `awaitingJsPromise` and `suspendedCoro` fields
- `IoCoroutine.c` rawRun: saves *active* coroutine (not `self`) when yielding — critical for `try()` which does child coro frame swap
- `io_browser.c`: `do_eval` returns 2 when yielded; `do_resume_eval` restores suspended coro, re-enters eval loop
- `io_resume_eval` WASM export; chained await support (re-saves active coro if still awaiting)
- JS side: `resumeIfAwaiting()` called on Promise resolve/reject; `pendingAsyncResolve` callback for async test results
- Tests: "await resolved Promise" and "await rejected Promise" (including `try()` across suspend/resume boundary)
- **Key insight**: `try()` creates child coroutine via frame swap. Must save the child coro (active at yield time), not the outer try-coro. Parent's saved frames (with CORO_WAIT_CHILD) are preserved in the parent coro's data.

### Steps 3+4 are subsumed by Step 2
Steps 3 and 4 were originally planned separately but the implementation naturally included all functionality:
- `io_resolve_future`/`io_reject_future` exports (from Step 1)
- Resume path with `io_resume_eval` (Step 3)
- Error handling via `try()` across suspend/resume (Step 4)
- Chained await support in `do_resume_eval` (Step 4)

### Step 5: Implicit forward — auto-await on unrecognized messages — COMPLETE
- `IoFuture forward`: when a message isn't found on a Future, `forward` handles it:
  - **Resolved**: frame redirect — sets `fd->target` to resolved value, `fd->state = LOOKUP_SLOT`, replays message lookup. No nested eval loop needed.
  - **Rejected**: raises error via `IoState_error_`
  - **Pending**: triggers AWAIT_JS yield (same as explicit `await`). On resume, AWAIT_JS transitions to LOOKUP_SLOT (not ACTIVATE), which replays the entire slot lookup — `forward` is called again, now sees resolved state, and does the frame redirect.
- Added `then`, `catch`, `finally` explicit methods delegating to underlying JS Promise via JSObject
- `IoState_iterative.c` LOOKUP_SLOT: added `needsControlFlowHandling` check after `IoObject_forward()` calls (both non-locals and locals-no-scope paths). Without this, LOOKUP_SLOT would overwrite the forward's frame state redirect with CONTINUE_CHAIN.
- `IoState_iterative.c` AWAIT_JS resume: changed from `FRAME_STATE_ACTIVATE` to `FRAME_STATE_LOOKUP_SLOT`. This is correct for both explicit `await` (re-finds "await" slot, re-activates) and implicit forward (re-does forward on resolved Future).
- 5 new tests: resolved forward, pending auto-await, message chain, rejected raises
- **Key insight**: `IoObject_forward()` is a direct C call from LOOKUP_SLOT, not through ACTIVATE. The CFunction it calls can set frame state and `needsControlFlowHandling`, but LOOKUP_SLOT must check the flag to avoid overwriting the redirect. On AWAIT_JS resume, `fd->slotValue` may be stale (from the failed original lookup), so going to LOOKUP_SLOT instead of ACTIVATE replays the full lookup correctly.

### Step 6: Rich JS-side objects for core async concerns (future)

The generic async machinery (Steps 1-5) handles any Promise. But core platform concerns benefit from rich JS objects that encapsulate ugly platform details, with the Io API as a clean facade:

Naming convention: `Js` prefix on Io side = "wraps a JS object." `Io` prefix on JS side = "serves the Io bridge."

| Io side (wrapper) | JS side (implementation) | Purpose |
|-------------------|--------------------------|---------|
| `JsXhrRequest` | `IoXhrRequest` | Network requests with abort, timeout, retry, error taxonomy |
| `JsEvent` | `IoEvent` | User input with timing-aware dispatch |
| `JsPersistence` | `IoPersistence` | IndexedDB with safe-to-write window management |

The JS-side objects handle platform ugliness (XHR quirks, event timing, IndexedDB transaction safety). Inspired by [SvXhrRequest](https://github.com/stevedekorte/strvct.net/blob/master/source/library/services/AiServiceKit/Requests/SvXhrRequest.js) — abort, timeout, retry with exponential backoff, error taxonomy (network vs HTTP vs timeout vs abort), progress tracking, introspectable state.

The Io side stays clean:

```io
request := JsXhrRequest clone setUrl("/api/data") setTimeout(5000)
response := request send await
// Behind the scenes: JS IoXhrRequest handles
// abort, retry, timeout, status codes, etc.
```

JS objects can be tested with standard JS tooling, independent of the Io VM. Design driven by real use cases, not upfront.

---

## Open questions

- **Nested Promises**: if a `.then()` callback triggers another eval that yields for a Promise, the frame stack just grows. Should work but needs testing.
- **Multiple coroutines awaiting**: each coroutine has its own frame stack and its own Future. JS wires each Promise independently. `io_scheduler_tick` runs whichever coros are ready.
- **Scheduler time budget**: `io_scheduler_tick` runs one ready coro until it completes or yields. JS controls how many ticks to run based on `deadline.timeRemaining()`.
- **Node.js**: `requestIdleCallback` doesn't exist. Use `setImmediate` or `setTimeout(0)`. Detect the environment in `browser/src/vm.js`.
- **Already-resolved Promises**: `Promise.resolve(42)` is technically async (`.then` fires on microtask queue) but could be optimized to resolve synchronously in the bridge. TBD.
- **REPL interaction**: resolved. `await io.eval()` settles when the awaited Promise does, and the REPL shows a placeholder until it does.

## Difficulty

Hard (touches eval loop, coroutine system, JS runtime)

## Milestone

`JS fetch(url) await` yields the Io coroutine to JS, resumes when the Promise resolves, returns the value. No callbacks in Io code.
