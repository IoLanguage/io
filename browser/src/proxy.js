// JS-side proxies for Io objects, and message sends into the VM.

import { serialize, deserialize } from "./serialize.js";

export function makeIoProxy(vm, ioHandle) {
	// Use a function target so the Proxy is callable (enables apply trap).
	// This lets Io blocks be passed as JS callbacks (Promise.then, setTimeout, etc.)
	const fn = function() {};
	fn.__ioHandle = ioHandle;
	const proxy = new Proxy(fn, {
		get(target, prop) {
			if (prop === "__ioHandle") return target.__ioHandle;
			if (prop === Symbol.toPrimitive || prop === "valueOf" || prop === "toString") {
				return () => `[IoObject handle=${target.__ioHandle}]`;
			}

			// Return a function that sends a message to the Io object
			return (...args) => ioSend(vm, target.__ioHandle, String(prop), ...args);
		},

		set(target, prop, value) {
			ioSend(vm, target.__ioHandle, "setSlot", String(prop), value);
			return true;
		},

		apply(target, thisArg, args) {
			return ioSend(vm, target.__ioHandle, "call", ...args);
		},
	});

	if (vm.proxyRegistry) vm.proxyRegistry.register(proxy, ioHandle);
	return proxy;
}

export function ioSend(vm, ioHandle, messageName, ...args) {
	if (!vm.wasm) throw new Error("Io VM not loaded");

	const { ptr, buf } = vm.bridgeBuf();

	// Serialize args to bridge_buf
	let offset = 0;
	for (const arg of args) {
		offset = serialize(vm, arg, buf, offset);
	}

	// The message name goes in the bridge buffer right after the args, not in
	// the input buffer, which may still hold the source of an eval that is
	// calling back into JS. io_send copies the name before it runs anything.
	const nameEncoded = new TextEncoder().encode(messageName);
	buf.set(nameEncoded, offset);

	const status = vm.wasm.exports.io_send(ioHandle, ptr + offset, nameEncoded.length, args.length);

	// Output printed by callbacks that run outside an eval (event handlers,
	// timers) goes to onOutput listeners.
	vm.flushCallbackOutput();
	if (vm.evalDepth === 0) vm.schedule(); // the send ran a scheduler tick

	const { value } = deserialize(vm, vm.bridgeBuf().buf, 0);

	if (status !== 0) {
		if (value instanceof Error) throw value;
		throw new Error("Io message send failed");
	}

	return value;
}
