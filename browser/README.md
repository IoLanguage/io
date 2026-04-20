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

The JS side (`io.js`) provides a minimal WASI shim — just enough for the VM to run without filesystem or process access.

## Files

- `io_browser.c` — C entry point with exported functions
- `io.js` — WASM loader, WASI shim, REPL logic
- `index.html` — UI
- `io_browser.wasm` — built artifact (gitignored)
