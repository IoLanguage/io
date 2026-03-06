# Browser Target

Io runs in the browser as a WebAssembly module. The browser build compiles the full VM into a WASM reactor module that JS loads and drives via exported functions.

## Building

```bash
make browser        # build browser/io_browser.wasm
make serve          # serve REPL at http://localhost:8000
make check-browser  # run headless Playwright tests
```

Requires [wasi-sdk](https://github.com/WebAssembly/wasi-sdk) for cross-compilation and [Node.js](https://nodejs.org/) + Playwright for headless tests.

## Architecture

```
┌──────────────────────────────────────────────┐
│  Browser (JS)                                │
│                                              │
│  io.js                                       │
│  ├── WASI shim (fd_write, clock, etc.)       │
│  ├── DOM handle table (Map<int, Element>)    │
│  ├── DOM bridge functions (dom namespace)    │
│  ├── WASM loader (loadIo / ioEval)           │
│  └── REPL UI wiring                          │
│                                              │
│  ┌────────────────────────────────────────┐  │
│  │  WebAssembly (io_browser.wasm)         │  │
│  │                                        │  │
│  │  io_browser.c — init, eval, I/O        │  │
│  │  io_dom.c    — DOM/Element protos      │  │
│  │  iovm/*      — full Io VM              │  │
│  └────────────────────────────────────────┘  │
└──────────────────────────────────────────────┘
```

The WASM module is built with `-mexec-model=reactor` (no `main()`). JS calls exported functions:

| Export | Purpose |
|--------|---------|
| `io_init()` | Initialize the Io VM (call once) |
| `io_get_input_buf()` | Pointer to 64KB input buffer |
| `io_eval_input()` | Evaluate code written to input buffer |
| `io_get_output()` | Pointer to output string |
| `io_get_output_len()` | Length of output |

## WASI Shim

The browser has no filesystem or OS. `io.js` provides a minimal WASI shim:

- **stdout/stderr** — captured to a JS string
- **clock** — `performance.now()` in nanoseconds
- **filesystem** — all path operations return `ENOTCAPABLE`
- **proc_exit** — throws a JS Error
- **random** — `crypto.getRandomValues()`

## Files

| File | Purpose |
|------|---------|
| `browser/io_browser.c` | Reactor entry point: init, eval, output capture |
| `browser/io_dom.c` | DOM and Element proto implementation |
| `browser/io_dom.h` | Header for DOM protos |
| `browser/io.js` | WASI shim, DOM bridge, WASM loader, REPL UI |
| `browser/index.html` | REPL page |
| `browser/test.html` | Automated test page |
| `browser/run_tests.mjs` | Headless Playwright test runner |
