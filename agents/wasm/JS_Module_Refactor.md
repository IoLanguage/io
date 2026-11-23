# Browser JS Module Refactor

Status: **implemented** (November 2026). See "As built" at the end for where
it differs from the plan below. Line numbers below refer to the old `io.js`.

Companion to [Bridge.md](Bridge.md), which documents the bridge as it exists.
This doc describes turning `browser/io.js` from a REPL page script into an
importable module, so the VM can be embedded — and eventually published to npm.

## Why

The C side of the JS integration is real: `io_js_bridge.c` implements a
`JSObject` proto with forward dispatch, a shared handle table, GC integration,
and futures. The JS side is a page script. Anyone who wants Io in their own
page has to clone the repo, install wasi-sdk, run `make browser`, and hand-copy
two files — and even then they get the site's REPL wired to their DOM.

## Current shape

`browser/io.js` is 1071 lines. Lines 1-889 are library, 891-1071 are the REPL
app. The internal layering is already sound:

```
wasi shim (46-213) ──┐
serialize (215-517) ─┼──> js imports (519-706) ──> resumeIfAwaiting (828)
                     └──> proxy/ioSend (708-786) ──> flushCallbackOutput (974)
loader (803) ──> ioEval (854)                                    |
                                                                 v
                                                                DOM
```

Every arrow points downward except the last one. There is exactly one edge
from the core into the DOM, at line 774.

## What blocks library use

1. **No exports.** It is a classic script; `test.html:99` loads it with
   `<script src="io.js">` and picks up `loadIo`/`ioEval` as globals.

2. **Module-level singletons.** `wasm`, `memory`, `jsHandles`, `nextHandle`,
   `earlyOutput`, and `pendingAsyncResolve` all live at module scope. One VM
   per page, permanently.

3. **Configuration frozen at parse time.** `IO_WASM_URL` is a `const`
   evaluated at line 5 from `window.IO_WASM_URL`. An importer cannot pass a
   URL; it has to set a global before the script tag runs.

4. **The DOM edge is a bug, not just a smell.** `flushCallbackOutput` opens
   with `if (!wasm || !outputEl) return;` (line 975), so with no `#output`
   element, output printed from inside Io callbacks is silently discarded.
   The embedding case fails quietly.

5. **Browser-only loading.** `fetch` + `instantiateStreaming` with no bytes
   path — no Node, no bundler. This is why `run_tests.mjs` boots Chromium
   through Playwright to test a module that has no inherent need for a browser.

Importing the file also starts a VM and wires the UI on `DOMContentLoaded`
(line 1068), whether the importer wants that or not.

## Target shape

```
browser/
  src/wasi.js        wasi_snapshot_preview1 factory (per instance)
  src/serialize.js   type tags + serialize/deserialize (pure, no state)
  src/bridge.js      js_* imports + handle table
  src/proxy.js       makeIoProxy / ioSend
  src/vm.js          class Io — owns wasm, memory, handles, pending state
  src/index.js       exports { Io }
  repl/repl.js       the DOM app (was 891-1071)
  io.js              classic-script shim, keeps existing pages working
```

The API:

```js
import { Io } from "./src/index.js";

const io = await Io.load({ wasm: urlOrBytesOrModule });

const { status, output } = await io.eval('list(1,2,3) map(x, x*x) sum');
io.onOutput(text => process.stdout.write(text));   // replaces the DOM edge
io.lobby.fetch = globalThis.fetch;                 // JS value into Io
io.dispose();

// Added 2026-11-22 (PR #548); see subplans/Async.md "Time slices and interrupt"
const io2 = await Io.load({ timeSlice: 50 });      // give JS a turn every 50 ms
io2.interrupt();                                   // stop the eval in progress
```

`io.lobby.foo = bar` needs no new API — the Proxy `set` trap already forwards
to `setSlot` (lines 736-739).

## The three moves

**1. Thread an instance.** The mechanical bulk. Roughly 40 functions that
close over module globals take a context instead. `serialize.js` is already
pure and moves untouched; `bridge.js` and `proxy.js` need `ctx` threaded
through.

**2. Invert line 774.** `ioSend` calls `this._emitOutput(text)` instead of
`flushCallbackOutput()`. The REPL subscribes through `onOutput` and does its
own DOM append. One edge, inverted — and the silent-drop bug dies with it.

**3. Collapse the status=2 protocol.** Today `ioEval` returns `status: 2`
meaning "yielded on a JS promise", and the *caller* must stash
`pendingAsyncResolve` — the REPL does exactly this at line 951. That is an
internal detail leaking into every consumer. `await io.eval(...)` should
absorb it, settling when `resumeIfAwaiting` reports completion.

## Hazards

- **Input-buffer reentrancy.** `ioSend` writes the message name into the input
  buffer (line 766) — the same buffer `ioEval` uses for source. An Io callback
  that calls back into `ioSend` mid-eval can clobber it. This is a latent bug
  today; per-instance state does not fix it, and it deserves its own buffer.

- **`earlyOutput` ordering.** It exists because `fd_write` output arrives
  before the VM's `printCallback` is installed. The per-instance shim must
  buffer from construction, before `io_init`.

- **FinalizationRegistry lifetime.** The registry at line 710 releases Io
  handles on GC. A per-instance registry must not outlive its VM, or it will
  call `io_release` into a disposed instance.

- **Circular imports.** `bridge` needs `makeIoProxy` (deserialize calls it at
  lines 463 and 506) and `proxy` needs `serialize`. Pass the context object
  rather than importing across.

## What it buys beyond npm

The core becomes instantiable from bytes with no DOM, so most of `test.html`
becomes a plain Node test file and `make check-browser` stops needing a
Playwright/Chromium install for anything but genuine in-page behavior. That is
worth doing on its own terms, independent of publishing.

## Effort and sequencing

Roughly 890 lines relocate largely intact; the genuinely new code is the
instance plumbing, the output callback, and promise-based eval. Call it a
focused day.

Keep `browser/io.js` as a thin classic-script shim that re-exposes `loadIo`
and `ioEval` and auto-boots the REPL, so `/repl/`, `browser/index.html`, and
`test.html` stay green throughout and the whole thing lands as one reviewable
PR. Migrate those pages to `<script type="module">` afterwards, then drop the
shim.

## Related

- [Bridge.md](Bridge.md) — the bridge as built
- [subplans/Async.md](subplans/Async.md) — futures and the await protocol
- `.github/workflows/release.yml` — ships `io-browser-<tag>.tar.gz` today;
  an npm package would replace that kit

## As built

- **Layout** as planned: `browser/src/{wasi,serialize,bridge,proxy,vm,index}.js`,
  `browser/repl/repl.js`, and `browser/io.js` as the classic-script shim.
  `browser/src/package.json` marks `src/` as ES modules for Node (a
  `browser/package.json` would break the CommonJS `test_browser.js`).
- **Context threading** uses the `Io` instance itself as the context: every
  bridge, proxy and serialize function takes the VM. `deserialize` reaches
  `makeIoProxy` through `vm.proxy(h)`, so there are no circular imports.
  `serialize.js` is not quite pure — references register in `vm.handles`.
- **Output.** `ioSend` calls `vm.flushCallbackOutput()`, which hands output to
  `onOutput` listeners. Two fixes came with it: output printed inside an eval
  is left for the eval's result (it used to be shown twice in the REPL), and
  only output since the last delivery is emitted (the old flush re-showed the
  whole buffer, including the previous eval's output).
- **Async eval.** `await io.eval(code)` absorbs status 2. Evals are queued, since
  the C side keeps a single suspended coroutine. `vm.evalNow(code)` is the
  synchronous call that can still return status 2; the shim uses it to keep
  `ioEval` + `pendingAsyncResolve` working for `test.html`. `dispose()` settles
  a pending await with status -1.
- **Input-buffer hazard** fixed without a C change: `ioSend` writes the message
  name into the bridge buffer after the args, and `io_send` copies it before
  running anything.
- **Loading** accepts a URL (string or `URL`), a `Response`, bytes, or a
  `WebAssembly.Module`. The default is `io_browser.wasm` next to `src/`;
  `file:` URLs are read with `node:fs` so Node needs no server.
- **Tests.** `browser/test/vm.test.mjs` (Node, `make check-browser-node`, run in
  CI) covers the module; `test.html` via Playwright still covers in-page
  behavior. Porting the rest of `test.html` to Node is left for later.
- **npm (step 3).** `browser/npm/` holds the package's `package.json`, README
  and `index.d.ts`; `browser/npm/pack.mjs` stages it in `build/npm/` (no
  `browser/package.json`, for the reason above). Io's date tags map to semver
  by dropping leading zeros: `2026.04.20-native-final` → `2026.4.20`;
  prerelease tags keep their suffix and publish to the `next` dist-tag. The
  Release workflow publishes with provenance by npm trusted publishing (no
  token; npm is retiring publish tokens that bypass 2FA); CI runs
  `make check-npm-package` on every push. The first version,
  `2026.11.16-alpha`, was published by hand on 2026-11-16, because a trusted
  publisher can only be added to a package that exists.
- **Since then:** Phase 4i (the browser scheduler) replaced `resumeIfAwaiting`
  with `pump()`/`schedule()` and `io_resume_eval` with `io_tick`; see
  `subplans/Async.md`. `evalNow` is unchanged.
- **Shim removed (2026-11-19):** `browser/index.html`, `test.html` and
  `/repl/` use `<script type="module">` and import `src/index.js` directly;
  `browser/io.js` and `window.IO_WASM_URL` are gone. The release's browser
  kit ships `index.html` in place of `io.js`.
