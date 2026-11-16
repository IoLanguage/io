// The `js` import module: calls from Io into JS, plus the handle table they share.

import { serialize, deserialize, serializeError } from "./serialize.js";

// JS holds real object references; WASM only sees integer handles.
export class HandleTable {
	constructor() {
		this.map = new Map();
		this.next = 1;
	}

	register(obj) {
		if (obj == null) return 0;
		const h = this.next++;
		this.map.set(h, obj);
		return h;
	}

	get(h) {
		return this.map.get(h) || null;
	}

	release(h) {
		this.map.delete(h);
	}

	clear() {
		this.map.clear();
	}
}

function readArgs(vm, buf, argc) {
	const args = [];
	let offset = 0;
	for (let i = 0; i < argc; i++) {
		const r = deserialize(vm, buf, offset);
		args.push(r.value);
		offset = r.offset;
	}
	return args;
}

export function createJsImports(vm) {
	return {
		// js_call(handle, namePtr, nameLen, argc) — property get (0 args) or method call (1+ args)
		js_call(handle, namePtr, nameLen, argc) {
			try {
				const obj = vm.handles.get(handle);
				if (obj == null) return serializeError(vm, "js_call: invalid handle");

				const name = vm.readString(namePtr, nameLen);
				const prop = obj[name];
				const { buf } = vm.bridgeBuf();
				const args = readArgs(vm, buf, argc);

				let result;
				if (argc > 0) {
					// Method call: obj.name(args...)
					if (typeof prop !== "function") return serializeError(vm, `${name} is not a function`);
					result = prop.apply(obj, args);
				} else {
					// Property get: if function, auto-call with 0 args
					result = typeof prop === "function" ? prop.call(obj) : prop;
				}

				serialize(vm, result, buf, 0);
				return 0;
			} catch (e) {
				return serializeError(vm, e.message || String(e), e);
			}
		},

		// js_get_prop(handle, namePtr, nameLen) — pure property get, never auto-call
		js_get_prop(handle, namePtr, nameLen) {
			try {
				const obj = vm.handles.get(handle);
				if (obj == null) return serializeError(vm, "js_get_prop: invalid handle");

				const result = obj[vm.readString(namePtr, nameLen)];
				serialize(vm, result, vm.bridgeBuf().buf, 0);
				return 0;
			} catch (e) {
				return serializeError(vm, e.message || String(e), e);
			}
		},

		// js_set_prop(handle, namePtr, nameLen) — value already in bridge_buf
		js_set_prop(handle, namePtr, nameLen) {
			try {
				const obj = vm.handles.get(handle);
				if (obj == null) return;
				const name = vm.readString(namePtr, nameLen);
				obj[name] = deserialize(vm, vm.bridgeBuf().buf, 0).value;
			} catch (e) {
				// silently ignore set errors
			}
		},

		// js_call_func(handle, argc) — invoke handle as function, args in bridge_buf
		js_call_func(handle, argc) {
			try {
				const fn = vm.handles.get(handle);
				if (typeof fn !== "function") return serializeError(vm, "js_call_func: not a function");

				const { buf } = vm.bridgeBuf();
				const result = fn(...readArgs(vm, buf, argc));
				serialize(vm, result, buf, 0);
				return 0;
			} catch (e) {
				return serializeError(vm, e.message || String(e), e);
			}
		},

		// js_typeof(handle, buf, sz) — write typeof string to buf, return length
		js_typeof(handle, buf, sz) {
			return vm.writeString(buf, sz, typeof vm.handles.get(handle));
		},

		// js_get_global() — register globalThis, return handle
		js_get_global() {
			return vm.handles.register(globalThis);
		},

		// js_release(h) — release handle
		js_release(h) {
			vm.handles.release(h);
		},

		// js_watch_promise(promiseHandle, futureIoHandle) — wire .then()/.catch() on a Promise
		js_watch_promise(promiseHandle, futureIoHandle) {
			const promise = vm.handles.get(promiseHandle);
			if (!promise || typeof promise.then !== "function") return;

			promise.then(
				(value) => {
					if (!vm.wasm) return; // disposed while pending
					serialize(vm, value, vm.bridgeBuf().buf, 0);
					vm.wasm.exports.io_resolve_future(futureIoHandle);
					// Coroutines awaiting this Promise are runnable again
					vm.pump();
				},
				(error) => {
					if (!vm.wasm) return;
					const msg = error instanceof Error ? error.message : String(error);
					serialize(vm, msg, vm.bridgeBuf().buf, 0);
					vm.wasm.exports.io_reject_future(futureIoHandle);
					vm.pump();
				}
			);
		},

		// js_new_function(codePtr, codeLen) — create Function from code string, return handle
		js_new_function(codePtr, codeLen) {
			try {
				return vm.handles.register(new Function(vm.readString(codePtr, codeLen)));
			} catch (e) {
				serializeError(vm, e.message || String(e), e);
				return 0;
			}
		},
	};
}
