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
	static async load({ wasm = DEFAULT_WASM } = {}) {
		const vm = new Io();
		const imports = { wasi_snapshot_preview1: createWasi(vm), js: createJsImports(vm) };
		vm.wasm = await instantiate(wasm, imports);

		// Reactor model: call _initialize to run static constructors
		if (vm.wasm.exports._initialize) vm.wasm.exports._initialize();
		vm.wasm.exports.io_init();
		return vm;
	}

	constructor() {
		this.wasm = null;
		this.handles = new HandleTable();
		this.stdout = "";           // fd_write output not yet returned by an eval
		this.awaiting = null;       // settles the eval suspended on a JS Promise
		this.evalDepth = 0;         // >0 while an eval (or its resume) is running
		this.outputSeen = 0;        // bytes of the C output buffer already delivered
		this.outputListeners = new Set();
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
	// finished, including code that awaits JS Promises. status is 0 on
	// success, 1 on an Io error, -1 if the VM couldn't run it. Evals run one
	// at a time, in call order.
	eval(code) {
		const run = () => new Promise((resolve) => {
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

	// Output printed outside an eval, by Io callbacks run from event handlers,
	// timers and so on. Returns a function that unsubscribes.
	onOutput(listener) {
		this.outputListeners.add(listener);
		return () => this.outputListeners.delete(listener);
	}

	dispose() {
		if (!this.wasm) return;
		this.wasm = null;
		this.handles.clear();
		this.outputListeners.clear();
		if (this.awaiting) {
			const resolve = this.awaiting;
			this.awaiting = null;
			resolve({ status: -1, output: "Io VM disposed" });
		}
	}

	// --- Lower level ---

	// Run code synchronously. Returns { status, output }; status 2 means the
	// eval yielded on a JS Promise and this.awaiting will be called with the
	// final result. eval() wraps this; the classic-script shim uses it directly.
	evalNow(code) {
		if (!this.wasm) throw new Error("Io VM not loaded");

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

		this.stdout = "";
		this.evalDepth++;
		let status;
		try {
			status = this.wasm.exports.io_eval_input();
		} finally {
			this.evalDepth--;
		}

		// Include any fd_write output (from before printCallback was set)
		const output = this.stdout + this.takeOutput();
		this.stdout = "";
		return { status, output };
	}

	resumeIfAwaiting() {
		if (!this.wasm) return;
		this.evalDepth++;
		let status;
		try {
			status = this.wasm.exports.io_resume_eval();
		} finally {
			this.evalDepth--;
		}

		// Still awaiting (chained await) — next resolve will resume again
		if (status === 2) return;

		const output = this.takeOutput();
		if (this.awaiting) {
			const resolve = this.awaiting;
			this.awaiting = null;
			resolve({ status, output });
		}
	}

	// Deliver output printed since the last eval to onOutput listeners. Inside
	// an eval it is left for the eval's result instead.
	flushCallbackOutput() {
		if (!this.wasm || this.evalDepth > 0) return;
		const len = this.wasm.exports.io_get_output_len();
		if (len <= this.outputSeen) return;
		const text = this.readString(this.wasm.exports.io_get_output() + this.outputSeen, len - this.outputSeen);
		this.outputSeen = len;
		for (const listener of this.outputListeners) listener(text);
	}

	takeOutput() {
		const len = this.wasm.exports.io_get_output_len();
		this.outputSeen = len;
		return len > 0 ? this.readString(this.wasm.exports.io_get_output(), len) : "";
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
