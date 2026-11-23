// Types for @iolanguage/io.

export interface EvalResult {
	/** 0 on success, 1 on an Io exception, -1 if the VM couldn't run the code. */
	status: number;
	/** Printed output followed by the result ("==> value\n"), or the error message. */
	output: string;
}

export type WasmSource =
	| string
	| URL
	| Response
	| Promise<Response>
	| ArrayBuffer
	| ArrayBufferView
	| WebAssembly.Module;

export interface LoadOptions {
	/** Where to load the VM from. Defaults to the package's io_browser.wasm. */
	wasm?: WasmSource;
	/**
	 * The longest Io code runs before the page (or Node's event loop) gets a
	 * turn, in milliseconds. By default code runs until it waits or finishes.
	 * With a time slice, JS timers and event handlers can run partway through
	 * a long eval, and `interrupt()` can stop one that never waits.
	 */
	timeSlice?: number;
}

export class Io {
	/** Load and start a new, independent VM. */
	static load(options?: LoadOptions): Promise<Io>;

	/**
	 * Evaluate Io source. Resolves once the code has finished, including code
	 * that waits (on JS Promises, `wait`, actor futures); Io never blocks the
	 * JS thread. Evals run one at a time, in call order.
	 */
	eval(code: string): Promise<EvalResult>;

	/**
	 * The Lobby as a JS proxy: setting a property sets a slot, calling a
	 * method sends a message.
	 */
	readonly lobby: any;

	/**
	 * Send a message to the Io object behind a handle. If the Io code waits,
	 * this returns null and the code finishes later.
	 */
	send(handle: number, messageName: string, ...args: unknown[]): unknown;

	/**
	 * Stop the eval in progress: it resolves with status 1 and an
	 * "interrupted" error, which Io's `try` can't catch. Returns false if no
	 * eval is running. Code that never waits can only be stopped if the VM was
	 * loaded with a `timeSlice`.
	 */
	interrupt(): boolean;

	/**
	 * Receive output printed outside an eval, by Io callbacks run from timers,
	 * event handlers and so on. Returns a function that unsubscribes.
	 */
	onOutput(listener: (text: string) => void): () => void;

	/** Stop the VM. A pending eval resolves with status -1. */
	dispose(): void;
}
