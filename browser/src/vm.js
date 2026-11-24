// class Io — one VM instance: owns its wasm instance, memory, handle table and
// pending-eval state, so several can run on one page (or in Node).

import { createWasi } from "./wasi.js";
import { HandleTable, createJsImports } from "./bridge.js";
import { makeIoProxy, ioSend } from "./proxy.js";

const DEFAULT_WASM = new URL("../io_browser.wasm", import.meta.url);

export class Io {
	// Io.load({ wasm }) — wasm may be a URL (string or URL object), a Response,
	// bytes (ArrayBuffer or typed array), or a compiled WebAssembly.Module.
	// Defaults to io_browser.wasm next to this package's src/ folder.
	// timeSlice: the longest Io code runs before the page (or Node's event
	// loop) gets a turn, in milliseconds. Without it, code runs until it
	// waits or finishes, and interrupt() can only stop an eval that is waiting.
	static async load({ wasm = DEFAULT_WASM, timeSlice = 0 } = {}) {
		const vm = new Io();
		const imports = { wasi_snapshot_preview1: createWasi(vm), js: createJsImports(vm) };
		vm.wasm = await instantiate(wasm, imports);

		// Reactor model: call _initialize to run static constructors
		if (vm.wasm.exports._initialize) vm.wasm.exports._initialize();
		vm.wasm.exports.io_init();
		if (timeSlice > 0) vm.wasm.exports.io_set_time_slice_ms(timeSlice);
		return vm;
	}

	constructor() {
		this.wasm = null;
		this.handles = new HandleTable();
		this.stdout = "";           // fd_write output not yet returned by an eval
		this.awaiting = null;       // settles the eval that is still running (status 2)
		this.evalPending = false;   // an eval returned status 2 and hasn't finished
		this.evalDepth = 0;         // >0 while Io code is running
		this.wakeTimer = null;      // timer for the next scheduler tick
		this.wakeAt = 0;            // when that timer fires (ms since the epoch)
		this.outputSeen = 0;        // bytes of the C output buffer already delivered
		this.outputListeners = new Set();
		this.evalListener = null;   // onOutput of the eval in progress
		this.evalSeen = 0;          // bytes of its output already given to it
		this.queue = Promise.resolve();
		// Releases Io objects whose JS proxies were collected. Checks this.wasm
		// so it never calls into a disposed VM.
		this.proxyRegistry = typeof FinalizationRegistry !== "undefined"
			? new FinalizationRegistry((handle) => {
				if (this.wasm && this.wasm.exports.io_release) this.wasm.exports.io_release(handle);
			})
			: null;
	}

	get memory() {
		return this.wasm.exports.memory;
	}

	// --- Public API ---

	// Evaluate Io source. Resolves to { status, output } once the code has
	// finished, including code that waits (on JS Promises, `wait`, actor
	// futures). Other Io coroutines keep running meanwhile. status is 0 on
	// success, 1 on an Io error, -1 if the VM couldn't run it. Evals run one
	// at a time, in call order.
	// onOutput, if given, receives the eval's output as it is produced: what
	// it prints while it runs, piece by piece, then the rest when it ends (the
	// "==> value" line or the error). The pieces add up to result.output.
	eval(code, { onOutput = null } = {}) {
		const run = () => new Promise((resolve) => {
			this.evalListener = onOutput;
			this.evalSeen = 0;
			const result = this.evalNow(code);
			if (result.status === 2) this.awaiting = resolve;
			else resolve(result);
		});
		return (this.queue = this.queue.then(run, run));
	}

	// The Lobby as a JS proxy: io.lobby.foo = bar sets a slot, io.lobby.foo(1)
	// sends a message.
	get lobby() {
		if (!this.wasm) throw new Error("Io VM not loaded");
		return this.proxy(this.wasm.exports.io_get_lobby_handle());
	}

	send(handle, messageName, ...args) {
		return ioSend(this, handle, messageName, ...args);
	}

	// Stop the eval in progress: it fails with an "interrupted" Io error,
	// which `try` can't catch. Returns false if no eval is running. Code that
	// never waits can only be stopped if the VM was loaded with a timeSlice,
	// since otherwise no JS runs until it finishes.
	interrupt() {
		if (!this.wasm || !this.wasm.exports.io_interrupt()) return false;
		this.wakeSoon(0);
		return true;
	}

	// Output printed outside an eval, by Io callbacks run from event handlers,
	// timers and so on. Returns a function that unsubscribes.
	onOutput(listener) {
		this.outputListeners.add(listener);
		return () => this.outputListeners.delete(listener);
	}

	dispose() {
		if (!this.wasm) return;
		this.wasm = null;
		clearTimeout(this.wakeTimer);
		this.wakeTimer = null;
		this.evalPending = false;
		this.handles.clear();
		this.outputListeners.clear();
		if (this.awaiting) {
			const resolve = this.awaiting;
			this.awaiting = null;
			resolve({ status: -1, output: "Io VM disposed" });
		}
	}

	// --- Lower level ---

	// Run code now. Returns { status, output }; status 2 means the eval is
	// waiting on something (a JS Promise, `wait`, another coroutine) and
	// this.awaiting will be called with the final result. eval() wraps this;
	// test.html uses it directly.
	evalNow(code) {
		if (!this.wasm) throw new Error("Io VM not loaded");
		if (this.evalPending) {
			return { status: -1, output: "Error: the previous eval is still running" };
		}

		// Write code into the C-side input buffer
		const inputBufPtr = this.wasm.exports.io_get_input_buf();
		const inputBufSize = this.wasm.exports.io_get_input_buf_size();
		const encoded = new TextEncoder().encode(code);

		if (encoded.length >= inputBufSize) {
			return { status: -1, output: "Error: input too long (max " + (inputBufSize - 1) + " bytes)" };
		}

		const bytes = new Uint8Array(this.memory.buffer);
		bytes.set(encoded, inputBufPtr);
		bytes[inputBufPtr + encoded.length] = 0; // null-terminate

		if (this.evalDepth > 0) {
			// Called from JS that Io called: runs to completion inside that
			// call. Its output is what it adds to the buffer of the eval or
			// tick it is running in.
			const before = this.wasm.exports.io_get_output_len();
			const status = this.run(() => this.wasm.exports.io_eval_input());
			const len = this.wasm.exports.io_get_output_len() - before;
			return { status, output: len > 0 ? this.readString(this.wasm.exports.io_get_output() + before, len) : "" };
		}

		// The eval clears the C output buffer; deliver what's in it first
		this.flushCallbackOutput();
		this.outputSeen = 0;
		this.stdout = "";
		const status = this.run(() => this.wasm.exports.io_eval_input());
		if (status === 2) {
			this.evalPending = true;
			this.streamEvalOutput();
			this.schedule();
			return { status, output: "" };
		}
		const result = { status, output: this.takeEvalOutput() };
		this.schedule();
		return result;
	}

	// Run the Io scheduler for one time slice: resumes coroutines that are
	// ready (timers expired, Promises settled, actors with messages), then
	// arranges the next tick. Called from timers and when a Promise settles.
	pump() {
		if (!this.wasm || this.evalDepth > 0) return;
		const status = this.run(() => this.wasm.exports.io_tick());
		if (this.evalPending && status !== 2) {
			this.evalPending = false;
			const result = { status, output: this.takeEvalOutput() };
			const resolve = this.awaiting;
			this.awaiting = null;
			if (resolve) resolve(result);
		} else if (this.evalPending) {
			this.streamEvalOutput();
		} else {
			this.flushCallbackOutput();
		}
		this.schedule();
	}

	// Set the timer for the next tick, from when Io says it next has work.
	schedule() {
		if (!this.wasm) return;
		clearTimeout(this.wakeTimer);
		this.wakeTimer = null;
		const ms = this.wasm.exports.io_next_wake_ms();
		if (ms >= 0) this.wakeSoon(ms);
	}

	// Make sure a tick happens within ms. JS calls into Io (proxy sends) use
	// this, since they can make coroutines runnable outside a tick.
	wakeSoon(ms = 0) {
		if (!this.wasm) return;
		const at = Date.now() + ms;
		if (this.wakeTimer !== null && this.wakeAt <= at) return;
		clearTimeout(this.wakeTimer);
		this.wakeAt = at;
		this.wakeTimer = setTimeout(() => {
			this.wakeTimer = null;
			this.pump();
		}, ms);
	}

	run(callIntoIo) {
		this.evalDepth++;
		try {
			return callIntoIo();
		} finally {
			this.evalDepth--;
		}
	}

	// Deliver output printed outside an eval to onOutput listeners. While an
	// eval is running, output is kept for its result instead.
	flushCallbackOutput() {
		if (!this.wasm || this.evalDepth > 0 || this.evalPending) return;
		const len = this.wasm.exports.io_get_output_len();
		if (len <= this.outputSeen) return;
		const text = this.readString(this.wasm.exports.io_get_output() + this.outputSeen, len - this.outputSeen);
		this.outputSeen = len;
		for (const listener of this.outputListeners) listener(text);
	}

	// Give the eval in progress what it has printed since last time.
	streamEvalOutput() {
		if (!this.evalListener) return;
		const len = this.wasm.exports.io_get_output_len();
		if (len <= this.evalSeen) return;
		const text = this.readString(this.wasm.exports.io_get_output() + this.evalSeen, len - this.evalSeen);
		this.evalSeen = len;
		this.evalListener(text);
	}

	// The finished eval's output. Anything printed after it finished, by
	// other coroutines in the same tick, goes to the listeners.
	takeEvalOutput() {
		const len = this.wasm.exports.io_get_eval_output_len();
		const text = this.stdout + (len > 0 ? this.readString(this.wasm.exports.io_get_output(), len) : "");
		const listener = this.evalListener;
		this.evalListener = null;
		if (listener) {
			const seen = Math.min(this.evalSeen, len);
			const rest = this.stdout + (len > seen ? this.readString(this.wasm.exports.io_get_output() + seen, len - seen) : "");
			if (rest) listener(rest);
		}
		this.stdout = "";
		this.outputSeen = len;
		this.flushCallbackOutput();
		return text;
	}

	proxy(ioHandle) {
		return makeIoProxy(this, ioHandle);
	}

	bridgeBuf() {
		const ptr = this.wasm.exports.io_get_bridge_buf();
		const size = this.wasm.exports.io_get_bridge_buf_size();
		return { ptr, size, buf: new Uint8Array(this.memory.buffer, ptr, size) };
	}

	readString(ptr, len) {
		return new TextDecoder().decode(new Uint8Array(this.memory.buffer, ptr, len));
	}

	writeString(bufPtr, maxLen, str) {
		const encoded = new TextEncoder().encode(str);
		const bytes = new Uint8Array(this.memory.buffer);
		const writeLen = Math.min(encoded.length, maxLen - 1);
		bytes.set(encoded.subarray(0, writeLen), bufPtr);
		bytes[bufPtr + writeLen] = 0; // null-terminate
		return writeLen;
	}
}

async function instantiate(source, imports) {
	if (source instanceof WebAssembly.Module) {
		return WebAssembly.instantiate(source, imports);
	}
	if (source instanceof ArrayBuffer || ArrayBuffer.isView(source)) {
		return (await WebAssembly.instantiate(source, imports)).instance;
	}

	let response = source;
	if (typeof source === "string" || source instanceof URL) {
		const url = String(source);
		// Node's fetch can't read file: URLs (the default when loaded from disk)
		if (url.startsWith("file:")) {
			const { readFile } = await import("node:fs/promises");
			return (await WebAssembly.instantiate(await readFile(new URL(url)), imports)).instance;
		}
		response = await fetch(url);
	}
	response = await response;
	if (!response.ok) throw new Error(`couldn't load ${response.url}: ${response.status}`);

	try {
		return (await WebAssembly.instantiateStreaming(response.clone(), imports)).instance;
	} catch (e) {
		// Servers that don't send application/wasm
		if (!(e instanceof TypeError)) throw e;
		return (await WebAssembly.instantiate(await response.arrayBuffer(), imports)).instance;
	}
}
