# Phase 4e: Async — Run Loop, Futures, Scheduler

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

JS is responsible for calling into Io at the right times:

- **Promise resolve/reject**: wired automatically when the eval loop yields. JS reads `(promiseHandle, futureIoHandle)` from bridge_buf, attaches `.then(v => { serialize(v); io_resolve(futureH); })`.
- **DOM events**: already work via callable IoProxy (Phase 4d). The handler runs synchronously inside `io_send` and returns.
- **Scheduler tick**: new WASM export `io_scheduler_tick()`. JS calls this during idle time / animation frames. Returns: 0 = no more ready coros, 1 = more work to do, 2 = yielded for another Promise.

```js
// JS runtime (io.js)
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

### Step 2: FRAME_STATE_AWAIT_JS + yield protocol
- Add AWAIT_JS to eval loop state machine
- `IoFuture await` sets the state when pending
- Eval loop breaks out, writes (promiseHandle, futureIoHandle) to bridge_buf
- `io_eval_input` returns `IO_EVAL_AWAITING = 2`
- Test: verify eval loop yields and frame stack is preserved

### Step 3: io_resolve + resume
- New WASM exports: `io_resolve(future_handle)`, `io_reject(future_handle)`
- `io_resolve` deserializes value from bridge_buf, sets Future state, marks coro ready
- New `io_scheduler_tick()` export: re-enters eval loop for ready coros
- JS side: on yield, wire `.then()` and call `io_resolve` when done; schedule tick
- Test: `JS get("Promise") resolve(42) await` works end-to-end (async path)

### Step 4: Error handling + chaining
- `io_reject` sets rejected state, raises Io exception on waiting coro
- Chained awaits: `response await text await`
- `try(JS fetch(badUrl) await)` catches rejection as Io exception
- Test: rejection, chaining, error propagation

### Step 5: Implicit forward (optional, layered on top)
- `IoFuture forward`: auto-await + replay message on resolved value
- Makes `JS fetch(url) text` work without explicit `await`

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
- **Node.js**: `requestIdleCallback` doesn't exist. Use `setImmediate` or `setTimeout(0)`. Detect environment in io.js.
- **Already-resolved Promises**: `Promise.resolve(42)` is technically async (`.then` fires on microtask queue) but could be optimized to resolve synchronously in the bridge. TBD.
- **REPL interaction**: the REPL calls `ioEval()` synchronously. If the eval yields for a Promise, the REPL needs to handle the `IO_EVAL_AWAITING` status and show the result when it arrives later.

## Difficulty

Hard (touches eval loop, coroutine system, JS runtime)

## Milestone

`JS fetch(url) await` yields the Io coroutine to JS, resumes when the Promise resolves, returns the value. No callbacks in Io code.
