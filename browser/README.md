# Io Browser REPL

The Io language VM running in the browser via WebAssembly. Type Io expressions, see results immediately. State persists between evaluations — variables, methods, and objects survive across inputs.

## Quick start

```bash
make browser   # build browser/io_browser.wasm (requires wasi-sdk)
make serve     # start local server on http://localhost:8000
```

Open http://localhost:8000 in your browser.

## Try it

```
"hello world" println
1 + 2
x := 42
x * 2
list(1, 2, 3) map(x, x * x)
Dog := Object clone do(name ::= nil; speak := method(name .. " says woof"))
Dog clone setName("Rex") speak
```

## How it works

The VM is compiled as a WASM **reactor module** (`-mexec-model=reactor`) — instead of running `main()` and exiting, it exports functions that JS calls repeatedly:

| Export | Purpose |
|--------|---------|
| `io_init()` | Create and initialize the Io VM (call once) |
| `io_get_input_buf()` | Pointer to 64KB input buffer in WASM memory |
| `io_eval_input()` | Evaluate the code in the input buffer |
| `io_get_output()` | Pointer to captured output string |
| `io_get_output_len()` | Length of captured output |

All Io output (`write`, `writeln`, `println`) is redirected to an in-memory buffer via the VM's `printCallback` mechanism. If an expression produces no printed output but returns a non-nil value, the REPL displays it as `==> <value>`.

The JS side (`src/`) provides a minimal WASI shim — just enough for the VM to run without filesystem or process access — plus the Io↔JS bridge.

## Using Io from JavaScript

`src/index.js` is an ES module that works in browsers and Node, with no DOM dependency:

```js
import { Io } from "./src/index.js";

const io = await Io.load();          // or Io.load({ wasm: urlOrBytesOrModule })

const { status, output } = await io.eval('list(1, 2, 3) map(x, x * x) sum');
// status 0, output "==> 14\n"

io.lobby.fetch = globalThis.fetch;   // put a JS value in an Io slot
await io.eval('JS get("Promise") resolve(42) await');   // settles once the Promise does

io.onOutput(text => console.log(text));   // output from Io callbacks run by timers, events…
io.dispose();
```

- `eval` resolves to `{ status, output }`: status 0 on success, 1 on an Io exception, -1 if the VM couldn't run the code. Evals run one at a time, in call order.
- Io never blocks the JS thread. An eval that waits (`wait(1)`, a JS Promise, an actor future) resolves when it finishes, and Io coroutines and `@` actors keep running between evals, driven by JS timers.
- `io.lobby` is a proxy for the Lobby: setting a property sets a slot, calling a method sends a message.
- Each `Io.load()` makes an independent VM, so a page can run several.
- With no argument, `Io.load()` loads `io_browser.wasm` from this folder.

### Coroutines and waiting

```js
await io.eval('wait(1); "done"');                       // resolves after a second; the page stays live
await io.eval('Worker := Object clone do(job := method(n, wait(0.1); n * 2))');
await io.eval('a := Worker @job(1); b := Worker @job(2); (a + 1) + (b + 1)');   // actors, "==> 8"
await io.eval('coroDoLater(loop(wait(1); "tick" println))');   // keeps running; output goes to onOutput
```

- A JS call into Io from the event loop (`io.lobby.foo()`, an Io block used as an event handler or timer callback) may wait too. If it does, JS gets `null` straight away and the Io code finishes later.
- Code can't wait where its caller needs the result immediately: an Io block that JS calls synchronously during an Io call (`someArray map(block(...))`), or the comparison block given to `sortBy` or `sortInPlaceBy`, which runs inside the sort itself. Waiting there raises an Io exception. Loops (`for`, `while`, `loop`, `repeat`, `foreach` on a List, Map or Sequence), `map`, `select`, `detect`, `reduce`, `perform`, `do`, `lexicalDo`, `interpolate`, `sortInPlace(expression)`, `call evalArgAt` and an object's `init` can all wait.
- An eval that is paused with nothing due to wake it (no coroutine, timer or Promise) fails with "nothing left to resume".
- A coroutine that never waits or yields can't be interrupted, as in JS.

## Tests

```bash
make check-browser-node   # the module in Node (no browser)
make check-browser        # that, plus test.html in headless Chromium via Playwright
```

## Files

- `io_browser.c` — C entry point with exported functions
- `src/` — the JS module: `vm.js` (class `Io`), `bridge.js` (calls from Io into JS), `proxy.js` (JS proxies for Io objects), `serialize.js` (bridge wire format), `wasi.js`
- `repl/repl.js` — the REPL UI, used by `index.html` and `/repl/`
- `test/vm.test.mjs` — Node tests for the module
- `npm/` — the `@iolanguage/io` package: its `package.json`, README and types. `make npm-package` stages it in `build/npm/` with `src/` and the wasm; `make check-npm-package` packs, installs and smoke-tests it. The Release workflow publishes it on a release tag, by npm trusted publishing (no token).
- `index.html` — UI
- `io_browser.wasm` — built artifact (gitignored; the Pages workflow builds it for deploys)

The site's public Live Interpreter at `/repl/` imports `src/index.js` and
`repl/repl.js` from this folder. The module finds `io_browser.wasm` relative to
its own URL, so keep the wasm next to `src/`.
