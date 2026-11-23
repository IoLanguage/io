# @iolanguage/io

The [Io programming language](https://iolanguage.org) VM, compiled to WebAssembly. It runs in browsers and in Node, with no dependencies, and Io code can call JavaScript and await its Promises.

```sh
npm install @iolanguage/io
```

```js
import { Io } from "@iolanguage/io";

const io = await Io.load();

const { status, output } = await io.eval('list(1, 2, 3) map(x, x * x) sum');
// status 0, output "==> 14\n"
```

## Calling between Io and JavaScript

```js
// Put JS values in Io slots, and send Io messages from JS
io.lobby.greeting = "hello from JS";
await io.eval('double := method(x, x * 2)');
io.lobby.double(21);   // 42

// Io reaches JS globals through JS, and can await Promises
await io.eval('JS Math sqrt(16)');                     // "==> 4\n"
await io.eval('JS fetch("/data.json") await json await');

// Output printed by Io callbacks run later (timers, event handlers)
io.onOutput(text => console.log(text));
await io.eval('JS setTimeout(block("tick" println), 100)');
```

- `eval` resolves to `{ status, output }`: status 0 on success, 1 on an Io exception, -1 if the VM couldn't run the code. Evals run one at a time, in call order.
- `io.eval(code, { onOutput: text => ... })` also delivers the eval's output as it is produced, for code that runs or waits a long time. The pieces add up to `output`.
- `io.lobby` is a proxy for the Lobby: setting a property sets a slot, calling a method sends a message.
- Each `Io.load()` makes an independent VM.
- `io.interrupt()` stops the eval in progress with an "interrupted" error.
- `io.interruptBackground()` stops a background coroutine (one started with `coroDoLater`, or an actor) that is running without ever waiting. Nothing stops such code automatically: `io.backgroundBusy` and `io.onBackgroundBusy(listener)` report it, and coroutines that wait or yield are never reported or stopped. Needs a `timeSlice`.
- `Io.load({ timeSlice: 50 })` makes Io hand control back to JS whenever it has run for 50 ms, so a page stays live during long-running code and `interrupt()` can stop a loop that never waits. Without it, code runs until it waits or finishes, and JS timers and event handlers never run partway through an eval.
- `io.dispose()` stops a VM.

## Coroutines, actors and waiting

Io never blocks the JS thread. An eval that waits resolves when it finishes, and Io's coroutines and actors keep running between evals, driven by JS timers.

```js
await io.eval('wait(1); "done"');   // resolves after a second
await io.eval('Worker := Object clone do(job := method(n, wait(0.1); n * 2))');
await io.eval('a := Worker @job(1); b := Worker @job(2); (a + 1) + (b + 1)');   // "==> 8\n"
await io.eval('coroDoLater(loop(wait(1); "tick" println))');   // runs on; output goes to onOutput
```

An Io block that JS calls from the event loop (an event handler, a timer callback) can wait as well. Code can't wait where its caller needs the result immediately, such as a block JS calls synchronously during an Io call; that raises an Io exception.

## Loading the wasm

With no argument, `Io.load()` loads the package's `io_browser.wasm` using `new URL("../io_browser.wasm", import.meta.url)`, which Node and bundlers such as Vite and webpack resolve on their own. To serve it from somewhere else, pass a URL, a `Response`, bytes or a compiled `WebAssembly.Module`:

```js
const io = await Io.load({ wasm: "/assets/io_browser.wasm" });
```

The file is also exported as `@iolanguage/io/io_browser.wasm`.

## More

- [Io language guide](https://iolanguage.org)
- [Source, and how the JS bridge works](https://github.com/IoLanguage/io/tree/master/browser)

BSD-3-Clause license.
