// Io VM — Browser WASM loader + WASI shim + JS bridge + REPL logic

const IO_WASM_URL = "io_browser.wasm";

let wasm = null;    // WebAssembly instance
let memory = null;  // WASM linear memory

// --- Unified handle table ---
// JS holds real object references; WASM only sees integer handles.

const jsHandles = new Map();
let nextHandle = 1;

function registerHandle(obj) {
	if (obj == null) return 0;
	const h = nextHandle++;
	jsHandles.set(h, obj);
	return h;
}

function getHandle(h) {
	return jsHandles.get(h) || null;
}

function releaseHandle(h) {
	jsHandles.delete(h);
}

// --- String helpers ---

function readStringFromWasm(ptr, len) {
	return new TextDecoder().decode(new Uint8Array(memory.buffer, ptr, len));
}

function writeStringToWasm(bufPtr, maxLen, str) {
	const encoded = new TextEncoder().encode(str);
	const bytes = new Uint8Array(memory.buffer);
	const writeLen = Math.min(encoded.length, maxLen - 1);
	bytes.set(encoded.subarray(0, writeLen), bufPtr);
	bytes[bufPtr + writeLen] = 0; // null-terminate
	return writeLen;
}

// --- Minimal WASI shim ---
// The reactor binary still imports WASI symbols.
// We only need to support what the VM actually calls.

function getMemoryView() {
	return new DataView(memory.buffer);
}

function readCString(ptr) {
	const bytes = new Uint8Array(memory.buffer);
	let end = ptr;
	while (bytes[end] !== 0) end++;
	return new TextDecoder().decode(bytes.subarray(ptr, end));
}

function writeCString(ptr, str) {
	const bytes = new Uint8Array(memory.buffer);
	const encoded = new TextEncoder().encode(str);
	bytes.set(encoded, ptr);
	bytes[ptr + encoded.length] = 0;
	return encoded.length;
}

// Captured stdout/stderr from fd_write (before io_init installs printCallback)
let earlyOutput = "";

const wasi_snapshot_preview1 = {
	// fd_write(fd, iovs_ptr, iovs_len, nwritten_ptr) -> errno
	fd_write(fd, iovs_ptr, iovs_len, nwritten_ptr) {
		const view = getMemoryView();
		const bytes = new Uint8Array(memory.buffer);
		let totalWritten = 0;

		for (let i = 0; i < iovs_len; i++) {
			const base = view.getUint32(iovs_ptr + i * 8, true);
			const len = view.getUint32(iovs_ptr + i * 8 + 4, true);
			const chunk = bytes.subarray(base, base + len);
			const text = new TextDecoder().decode(chunk);

			if (fd === 1 || fd === 2) {
				earlyOutput += text;
			}
			totalWritten += len;
		}

		view.setUint32(nwritten_ptr, totalWritten, true);
		return 0; // ESUCCESS
	},

	// fd_read(fd, iovs_ptr, iovs_len, nread_ptr) -> errno
	fd_read(fd, iovs_ptr, iovs_len, nread_ptr) {
		const view = getMemoryView();
		view.setUint32(nread_ptr, 0, true);
		return 0; // EOF
	},

	// fd_close(fd) -> errno
	fd_close(fd) { return 0; },

	// fd_seek(fd, offset_lo, offset_hi, whence, newoffset_ptr) -> errno
	fd_seek(fd, offset_lo, offset_hi, whence, newoffset_ptr) {
		const view = getMemoryView();
		view.setBigUint64(newoffset_ptr, 0n, true);
		return 0;
	},

	// fd_fdstat_get(fd, buf) -> errno
	fd_fdstat_get(fd, buf) {
		const view = getMemoryView();
		view.setUint8(buf, fd <= 2 ? 2 : 4); // CHARACTER_DEVICE for stdio
		view.setUint16(buf + 2, 0, true);     // flags
		view.setBigUint64(buf + 8, 0n, true); // rights_base
		view.setBigUint64(buf + 16, 0n, true); // rights_inheriting
		return 0;
	},

	// fd_fdstat_set_flags(fd, flags) -> errno
	fd_fdstat_set_flags(fd, flags) { return 0; },

	// fd_filestat_set_size(fd, size) -> errno
	fd_filestat_set_size(fd, size) { return 70; }, // ENOSYS

	// fd_readdir(fd, buf, buf_len, cookie, bufused_ptr) -> errno
	fd_readdir(fd, buf, buf_len, cookie, bufused_ptr) {
		const view = getMemoryView();
		view.setUint32(bufused_ptr, 0, true);
		return 0;
	},

	// fd_renumber(fd, to) -> errno
	fd_renumber(fd, to) { return 70; }, // ENOSYS

	// fd_prestat_get(fd, buf) -> errno
	fd_prestat_get(fd, buf) { return 8; }, // EBADF — no preopened dirs

	// fd_prestat_dir_name(fd, path, path_len) -> errno
	fd_prestat_dir_name(fd, path, path_len) { return 8; }, // EBADF

	// path_open — not supported in browser
	path_open() { return 76; }, // ENOTCAPABLE

	// path_create_directory(fd, path, path_len) -> errno
	path_create_directory(fd, path, path_len) { return 76; },

	// path_filestat_get(fd, flags, path, path_len, buf) -> errno
	path_filestat_get(fd, flags, path, path_len, buf) { return 76; },

	// path_remove_directory(fd, path, path_len) -> errno
	path_remove_directory(fd, path, path_len) { return 76; },

	// path_rename(old_fd, old_path, old_path_len, new_fd, new_path, new_path_len) -> errno
	path_rename(old_fd, old_path, old_path_len, new_fd, new_path, new_path_len) { return 76; },

	// path_unlink_file(fd, path, path_len) -> errno
	path_unlink_file(fd, path, path_len) { return 76; },

	// clock_time_get(clock_id, precision, time_ptr) -> errno
	clock_time_get(clock_id, precision, time_ptr) {
		const view = getMemoryView();
		const now = BigInt(Math.floor(performance.now() * 1_000_000)); // ns
		view.setBigUint64(time_ptr, now, true);
		return 0;
	},

	// environ_sizes_get(count_ptr, buf_size_ptr) -> errno
	environ_sizes_get(count_ptr, buf_size_ptr) {
		const view = getMemoryView();
		view.setUint32(count_ptr, 0, true);
		view.setUint32(buf_size_ptr, 0, true);
		return 0;
	},

	// environ_get(environ_ptr, buf_ptr) -> errno
	environ_get(environ_ptr, buf_ptr) { return 0; },

	// args_sizes_get(argc_ptr, buf_size_ptr) -> errno
	args_sizes_get(argc_ptr, buf_size_ptr) {
		const view = getMemoryView();
		view.setUint32(argc_ptr, 0, true);
		view.setUint32(buf_size_ptr, 0, true);
		return 0;
	},

	// args_get(argv_ptr, buf_ptr) -> errno
	args_get(argv_ptr, buf_ptr) { return 0; },

	// proc_exit(code)
	proc_exit(code) {
		throw new Error(`Io called exit(${code})`);
	},

	// random_get(buf, buf_len) -> errno
	random_get(buf, buf_len) {
		const bytes = new Uint8Array(memory.buffer, buf, buf_len);
		crypto.getRandomValues(bytes);
		return 0;
	},

	// sched_yield() -> errno
	sched_yield() { return 0; },

	// poll_oneoff — stub
	poll_oneoff(in_ptr, out_ptr, nsubscriptions, nevents_ptr) {
		const view = getMemoryView();
		view.setUint32(nevents_ptr, 0, true);
		return 0;
	},
};

// --- JS Bridge: binary serialization ---

const TYPE_NIL        = 0;
const TYPE_TRUE       = 1;
const TYPE_FALSE      = 2;
const TYPE_NUMBER     = 3;
const TYPE_STRING     = 4;
const TYPE_ARRAY      = 5;
const TYPE_OBJECT     = 6;
const TYPE_JSREF      = 7;
const TYPE_IOREF      = 8;
const TYPE_ERROR      = 9;
const TYPE_UNDEFINED  = 10;
const TYPE_TYPEDARRAY = 11;

// TypedArray itemType byte mapping (must match C side)
const TYPED_ARRAY_CTORS = [
	Uint8Array,    // 0
	Uint16Array,   // 1
	Uint32Array,   // 2
	Int8Array,     // 3
	Int16Array,    // 4
	Int32Array,    // 5
	Float32Array,  // 6
	Float64Array,  // 7
];

const TYPED_ARRAY_MAP = new Map();
TYPED_ARRAY_CTORS.forEach((ctor, i) => TYPED_ARRAY_MAP.set(ctor, i));

function getBridgeBuf() {
	const ptr = wasm.exports.io_get_bridge_buf();
	const size = wasm.exports.io_get_bridge_buf_size();
	return { ptr, size };
}

function serializeToWasm(val, buf, offset, visited) {
	if (val === null) {
		buf[offset] = TYPE_NIL;
		return offset + 1;
	}

	if (val === undefined) {
		buf[offset] = TYPE_UNDEFINED;
		return offset + 1;
	}

	if (val === true) {
		buf[offset] = TYPE_TRUE;
		return offset + 1;
	}

	if (val === false) {
		buf[offset] = TYPE_FALSE;
		return offset + 1;
	}

	if (typeof val === "number") {
		buf[offset] = TYPE_NUMBER;
		const view = new DataView(buf.buffer, buf.byteOffset);
		view.setFloat64(offset + 1, val, true);
		return offset + 9;
	}

	if (typeof val === "string") {
		const encoded = new TextEncoder().encode(val);
		buf[offset] = TYPE_STRING;
		const view = new DataView(buf.buffer, buf.byteOffset);
		view.setUint32(offset + 1, encoded.length, true);
		buf.set(encoded, offset + 5);
		return offset + 5 + encoded.length;
	}

	// BigInt and Symbol rejection
	if (typeof val === "bigint") {
		throw new Error("bridge error: BigInt cannot cross the bridge");
	}

	if (typeof val === "symbol") {
		throw new Error("bridge error: JS Symbol cannot cross the bridge");
	}

	// TypedArray → TYPE_TYPEDARRAY
	if (ArrayBuffer.isView(val) && !(val instanceof DataView)) {
		const itemType = TYPED_ARRAY_MAP.get(val.constructor);
		if (itemType !== undefined) {
			const view = new DataView(buf.buffer, buf.byteOffset);
			buf[offset] = TYPE_TYPEDARRAY;
			buf[offset + 1] = itemType;
			view.setUint32(offset + 2, val.length, true);
			const rawBytes = new Uint8Array(val.buffer, val.byteOffset, val.byteLength);
			buf.set(rawBytes, offset + 6);
			return offset + 6 + val.byteLength;
		}
	}

	// Containers: Array, Map, Set — deep copy with cycle detection
	if (!visited) visited = new Set();

	if (Array.isArray(val)) {
		if (visited.has(val)) {
			throw new Error("bridge error: cyclic structure cannot be serialized");
		}
		visited.add(val);
		buf[offset] = TYPE_ARRAY;
		const view = new DataView(buf.buffer, buf.byteOffset);
		view.setUint32(offset + 1, val.length, true);
		let pos = offset + 5;
		for (const item of val) {
			pos = serializeToWasm(item, buf, pos, visited);
		}
		visited.delete(val);
		return pos;
	}

	if (val instanceof Map) {
		if (visited.has(val)) {
			throw new Error("bridge error: cyclic structure cannot be serialized");
		}
		visited.add(val);
		buf[offset] = TYPE_OBJECT;
		const view = new DataView(buf.buffer, buf.byteOffset);
		const entries = Array.from(val.entries());
		view.setUint32(offset + 1, entries.length, true);
		let pos = offset + 5;
		for (const [k, v] of entries) {
			const keyStr = String(k);
			const keyEncoded = new TextEncoder().encode(keyStr);
			const kview = new DataView(buf.buffer, buf.byteOffset);
			kview.setUint32(pos, keyEncoded.length, true);
			pos += 4;
			buf.set(keyEncoded, pos);
			pos += keyEncoded.length;
			pos = serializeToWasm(v, buf, pos, visited);
		}
		visited.delete(val);
		return pos;
	}

	if (val instanceof Set) {
		if (visited.has(val)) {
			throw new Error("bridge error: cyclic structure cannot be serialized");
		}
		visited.add(val);
		const items = Array.from(val);
		buf[offset] = TYPE_ARRAY;
		const view = new DataView(buf.buffer, buf.byteOffset);
		view.setUint32(offset + 1, items.length, true);
		let pos = offset + 5;
		for (const item of items) {
			pos = serializeToWasm(item, buf, pos, visited);
		}
		visited.delete(val);
		return pos;
	}

	// All other objects pass by reference JS→Io
	buf[offset] = TYPE_JSREF;
	const h = registerHandle(val);
	const view = new DataView(buf.buffer, buf.byteOffset);
	view.setInt32(offset + 1, h, true);
	return offset + 5;
}

function deserializeFromWasm(buf, offset) {
	const type = buf[offset];
	offset++;

	switch (type) {
	case TYPE_NIL:
		return { value: null, offset };

	case TYPE_TRUE:
		return { value: true, offset };

	case TYPE_FALSE:
		return { value: false, offset };

	case TYPE_NUMBER: {
		const view = new DataView(buf.buffer, buf.byteOffset);
		const val = view.getFloat64(offset, true);
		return { value: val, offset: offset + 8 };
	}

	case TYPE_STRING: {
		const view = new DataView(buf.buffer, buf.byteOffset);
		const len = view.getUint32(offset, true);
		offset += 4;
		const str = new TextDecoder().decode(buf.subarray(offset, offset + len));
		return { value: str, offset: offset + len };
	}

	case TYPE_ARRAY: {
		const view = new DataView(buf.buffer, buf.byteOffset);
		const count = view.getUint32(offset, true);
		offset += 4;
		const arr = [];
		for (let i = 0; i < count; i++) {
			const r = deserializeFromWasm(buf, offset);
			arr.push(r.value);
			offset = r.offset;
		}
		return { value: arr, offset };
	}

	case TYPE_OBJECT: {
		const view = new DataView(buf.buffer, buf.byteOffset);
		const count = view.getUint32(offset, true);
		offset += 4;
		const map = new Map();
		for (let i = 0; i < count; i++) {
			const klen = view.getUint32(offset, true);
			offset += 4;
			const key = new TextDecoder().decode(buf.subarray(offset, offset + klen));
			offset += klen;
			const r = deserializeFromWasm(buf, offset);
			map.set(key, r.value);
			offset = r.offset;
		}
		return { value: map, offset };
	}

	case TYPE_JSREF: {
		const view = new DataView(buf.buffer, buf.byteOffset);
		const h = view.getInt32(offset, true);
		return { value: getHandle(h), offset: offset + 4 };
	}

	case TYPE_IOREF: {
		const view = new DataView(buf.buffer, buf.byteOffset);
		const h = view.getInt32(offset, true);
		return { value: makeIoProxy(h), offset: offset + 4 };
	}

	case TYPE_UNDEFINED:
		return { value: undefined, offset };

	case TYPE_TYPEDARRAY: {
		const itemType = buf[offset];
		offset++;
		const view = new DataView(buf.buffer, buf.byteOffset);
		const count = view.getUint32(offset, true);
		offset += 4;
		const Ctor = TYPED_ARRAY_CTORS[itemType] || Uint8Array;
		const bytesPerElem = Ctor.BYTES_PER_ELEMENT;
		const byteLen = count * bytesPerElem;
		// Copy bytes out and create typed array
		const copy = new Uint8Array(byteLen);
		copy.set(buf.subarray(offset, offset + byteLen));
		const arr = new Ctor(copy.buffer, 0, count);
		return { value: arr, offset: offset + byteLen };
	}

	case TYPE_ERROR: {
		const view = new DataView(buf.buffer, buf.byteOffset);
		const len = view.getUint32(offset, true);
		offset += 4;
		const msg = new TextDecoder().decode(buf.subarray(offset, offset + len));
		offset += len;
		// Read error handle (exception proxy)
		let ioError = null;
		if (offset + 4 <= buf.length) {
			const errHandle = view.getInt32(offset, true);
			offset += 4;
			if (errHandle) {
				ioError = makeIoProxy(errHandle);
			}
		}
		const err = new Error(msg);
		if (ioError) err.ioError = ioError;
		return { value: err, offset };
	}

	default:
		return { value: null, offset };
	}
}

// --- JS Bridge: WASM imports (js module) ---

const js = {
	// js_call(handle, namePtr, nameLen, argc) — property get (0 args) or method call (1+ args)
	js_call(handle, namePtr, nameLen, argc) {
		try {
			const obj = getHandle(handle);
			if (obj == null) {
				return serializeError("js_call: invalid handle");
			}

			const name = readStringFromWasm(namePtr, nameLen);
			const prop = obj[name];

			const { ptr: bufPtr, size: bufSize } = getBridgeBuf();
			const buf = new Uint8Array(memory.buffer, bufPtr, bufSize);

			// Deserialize args from bridge_buf
			const args = [];
			let offset = 0;
			for (let i = 0; i < argc; i++) {
				const r = deserializeFromWasm(buf, offset);
				args.push(r.value);
				offset = r.offset;
			}

			let result;
			if (argc > 0) {
				// Method call: obj.name(args...)
				if (typeof prop !== "function") {
					return serializeError(`${name} is not a function`);
				}
				result = prop.apply(obj, args);
			} else {
				// Property get: if function, auto-call with 0 args
				if (typeof prop === "function") {
					result = prop.call(obj);
				} else {
					result = prop;
				}
			}

			// Serialize result back to bridge_buf
			serializeToWasm(result, buf, 0);
			return 0;
		} catch (e) {
			return serializeError(e.message || String(e), e);
		}
	},

	// js_get_prop(handle, namePtr, nameLen) — pure property get, never auto-call
	js_get_prop(handle, namePtr, nameLen) {
		try {
			const obj = getHandle(handle);
			if (obj == null) {
				return serializeError("js_get_prop: invalid handle");
			}

			const name = readStringFromWasm(namePtr, nameLen);
			const result = obj[name];

			const { ptr: bufPtr, size: bufSize } = getBridgeBuf();
			const buf = new Uint8Array(memory.buffer, bufPtr, bufSize);
			serializeToWasm(result, buf, 0);
			return 0;
		} catch (e) {
			return serializeError(e.message || String(e), e);
		}
	},

	// js_set_prop(handle, namePtr, nameLen) — value already in bridge_buf
	js_set_prop(handle, namePtr, nameLen) {
		try {
			const obj = getHandle(handle);
			if (obj == null) return;

			const name = readStringFromWasm(namePtr, nameLen);

			const { ptr: bufPtr, size: bufSize } = getBridgeBuf();
			const buf = new Uint8Array(memory.buffer, bufPtr, bufSize);
			const { value } = deserializeFromWasm(buf, 0);

			obj[name] = value;
		} catch (e) {
			// silently ignore set errors
		}
	},

	// js_call_func(handle, argc) — invoke handle as function, args in bridge_buf
	js_call_func(handle, argc) {
		try {
			const fn = getHandle(handle);
			if (typeof fn !== "function") {
				return serializeError("js_call_func: not a function");
			}

			const { ptr: bufPtr, size: bufSize } = getBridgeBuf();
			const buf = new Uint8Array(memory.buffer, bufPtr, bufSize);

			const args = [];
			let offset = 0;
			for (let i = 0; i < argc; i++) {
				const r = deserializeFromWasm(buf, offset);
				args.push(r.value);
				offset = r.offset;
			}

			const result = fn(...args);
			serializeToWasm(result, buf, 0);
			return 0;
		} catch (e) {
			return serializeError(e.message || String(e), e);
		}
	},

	// js_typeof(handle, buf, sz) — write typeof string to buf, return length
	js_typeof(handle, buf, sz) {
		const obj = getHandle(handle);
		const typeStr = typeof obj;
		return writeStringToWasm(buf, sz, typeStr);
	},

	// js_get_global() — register globalThis, return handle
	js_get_global() {
		return registerHandle(globalThis);
	},

	// js_release(h) — release handle
	js_release(h) {
		releaseHandle(h);
	},

	// js_new_function(codePtr, codeLen) — create Function from code string, return handle
	js_new_function(codePtr, codeLen) {
		try {
			const code = readStringFromWasm(codePtr, codeLen);
			const fn = new Function(code);
			const h = registerHandle(fn);
			return h;
		} catch (e) {
			serializeError(e.message || String(e), e);
			return 0;
		}
	},
};

function serializeError(msg, originalError) {
	const { ptr: bufPtr, size: bufSize } = getBridgeBuf();
	const buf = new Uint8Array(memory.buffer, bufPtr, bufSize);
	const encoded = new TextEncoder().encode(msg);
	buf[0] = TYPE_ERROR;
	const view = new DataView(memory.buffer, bufPtr);
	view.setUint32(1, encoded.length, true);
	buf.set(encoded, 5);
	// Write error handle for exception proxy
	let errHandle = 0;
	if (originalError != null) {
		errHandle = registerHandle(originalError);
	}
	view.setInt32(5 + encoded.length, errHandle, true);
	return -1;
}

// --- Io Proxy factory (for JS→Io object references) ---

const ioProxyRegistry = typeof FinalizationRegistry !== "undefined"
	? new FinalizationRegistry((handle) => {
		if (wasm && wasm.exports.io_release) {
			wasm.exports.io_release(handle);
		}
	})
	: null;

function makeIoProxy(ioHandle) {
	const proxy = new Proxy({__ioHandle: ioHandle}, {
		get(target, prop) {
			if (prop === "__ioHandle") return target.__ioHandle;
			if (prop === Symbol.toPrimitive || prop === "valueOf" || prop === "toString") {
				return () => `[IoObject handle=${target.__ioHandle}]`;
			}

			// Return a function that sends a message to the Io object
			return (...args) => {
				return ioSend(target.__ioHandle, String(prop), ...args);
			};
		},

		set(target, prop, value) {
			ioSend(target.__ioHandle, "setSlot", String(prop), value);
			return true;
		},
	});

	if (ioProxyRegistry) {
		ioProxyRegistry.register(proxy, ioHandle);
	}

	return proxy;
}

function ioSend(ioHandle, messageName, ...args) {
	if (!wasm) throw new Error("Io VM not loaded");

	const { ptr: bufPtr, size: bufSize } = getBridgeBuf();
	const buf = new Uint8Array(memory.buffer, bufPtr, bufSize);

	// Serialize args to bridge_buf
	let offset = 0;
	for (const arg of args) {
		offset = serializeToWasm(arg, buf, offset);
	}

	// Write message name to WASM memory (reuse input buf area)
	const inputBufPtr = wasm.exports.io_get_input_buf();
	const nameEncoded = new TextEncoder().encode(messageName);
	const nameBytes = new Uint8Array(memory.buffer, inputBufPtr, nameEncoded.length);
	nameBytes.set(nameEncoded);

	const status = wasm.exports.io_send(ioHandle, inputBufPtr, nameEncoded.length, args.length);

	// Deserialize result from bridge_buf
	const resultBuf = new Uint8Array(memory.buffer, bufPtr, bufSize);
	const { value } = deserializeFromWasm(resultBuf, 0);

	if (status !== 0) {
		if (value instanceof Error) throw value;
		throw new Error("Io message send failed");
	}

	return value;
}

// Public API: io.lobby and io.send
const io = {
	get lobby() {
		if (!wasm) throw new Error("Io VM not loaded");
		const h = wasm.exports.io_get_lobby_handle();
		return makeIoProxy(h);
	},

	send(handle, msg, ...args) {
		return ioSend(handle, msg, ...args);
	},
};

// --- WASM loader ---

async function loadIo() {
	const importObject = { wasi_snapshot_preview1, js };

	const response = await fetch(IO_WASM_URL);
	const { instance } = await WebAssembly.instantiateStreaming(response, importObject);

	wasm = instance;
	memory = wasm.exports.memory;

	// Reactor model: call _initialize to run static constructors
	if (wasm.exports._initialize) {
		wasm.exports._initialize();
	}

	// Initialize the Io VM
	wasm.exports.io_init();

	return wasm;
}

// --- REPL interface ---

function ioEval(code) {
	if (!wasm) throw new Error("Io VM not loaded");

	// Write code into the C-side input buffer
	const inputBufPtr = wasm.exports.io_get_input_buf();
	const inputBufSize = wasm.exports.io_get_input_buf_size();
	const encoded = new TextEncoder().encode(code);

	if (encoded.length >= inputBufSize) {
		return { status: -1, output: "Error: input too long (max " + (inputBufSize - 1) + " bytes)" };
	}

	const bytes = new Uint8Array(memory.buffer);
	bytes.set(encoded, inputBufPtr);
	bytes[inputBufPtr + encoded.length] = 0; // null-terminate

	earlyOutput = "";
	const status = wasm.exports.io_eval_input();

	// Read output
	const outPtr = wasm.exports.io_get_output();
	const outLen = wasm.exports.io_get_output_len();
	let output = "";

	if (outLen > 0) {
		output = readCString(outPtr);
	}

	// Include any early fd_write output (from before printCallback was set)
	if (earlyOutput) {
		output = earlyOutput + output;
		earlyOutput = "";
	}

	return { status, output };
}

// --- UI wiring ---

let outputEl, inputEl;
const history = [];
let historyIndex = -1;

function appendReplPair(inputText, outputText, isError) {
	const pair = document.createElement("div");
	pair.className = "replPair";

	const inputLine = document.createElement("div");
	inputLine.className = "input-echo";
	inputLine.textContent = inputText;
	pair.appendChild(inputLine);

	if (outputText) {
		const resultLine = document.createElement("div");
		resultLine.className = isError ? "result-error" : "result";
		resultLine.textContent = outputText;
		pair.appendChild(resultLine);
	}

	outputEl.appendChild(pair);
	outputEl.scrollTop = outputEl.scrollHeight;
}

function autoResize() {
	inputEl.style.height = "auto";
	inputEl.style.height = inputEl.scrollHeight + "px";
}

function handleEval() {
	const code = inputEl.value.trim();
	if (!code) return;

	history.push(code);
	historyIndex = history.length;

	inputEl.value = "";
	autoResize();

	try {
		const { status, output } = ioEval(code);
		appendReplPair(code, output || null, status !== 0);
	} catch (e) {
		appendReplPair(code, "Error: " + e.message, true);
	}
}

function clearOutput() {
	outputEl.innerHTML = "";
}

function initUI() {
	outputEl = document.getElementById("output");
	inputEl = document.getElementById("input");

	// Guard UI wiring: test.html imports loadIo/ioEval but has no REPL elements
	if (!outputEl || !inputEl) return;

	inputEl.addEventListener("input", autoResize);

	inputEl.addEventListener("keydown", (e) => {
		if (e.key === "Enter" && (e.ctrlKey || e.metaKey)) {
			e.preventDefault();
			handleEval();
		} else if (e.key === "Enter" && !e.shiftKey && inputEl.value.indexOf("\n") === -1) {
			// Single-line: Enter evals. Multi-line (shift+enter): newline.
			e.preventDefault();
			handleEval();
		} else if (e.key === "ArrowUp" && inputEl.selectionStart === 0) {
			if (historyIndex > 0) {
				historyIndex--;
				inputEl.value = history[historyIndex];
				autoResize();
			}
			e.preventDefault();
		} else if (e.key === "ArrowDown" && inputEl.selectionStart === inputEl.value.length) {
			if (historyIndex < history.length - 1) {
				historyIndex++;
				inputEl.value = history[historyIndex];
			} else {
				historyIndex = history.length;
				inputEl.value = "";
			}
			autoResize();
			e.preventDefault();
		} else if (e.key === "u" && e.ctrlKey && e.shiftKey) {
			e.preventDefault();
			clearOutput();
		}
	});

	document.getElementById("eval-btn").addEventListener("click", handleEval);
	document.getElementById("clear-btn").addEventListener("click", clearOutput);

	document.querySelectorAll(".topic").forEach((el) => {
		el.addEventListener("click", () => {
			inputEl.value = el.dataset.code;
			autoResize();
			handleEval();
		});
	});
}

// --- Boot ---

async function boot() {
	const statusEl = document.getElementById("status");
	const replEl = document.getElementById("repl");

	// Only run REPL boot when REPL elements exist (not on test.html)
	if (!statusEl || !replEl) return;

	try {
		await loadIo();
		statusEl.className = "status hidden";
		replEl.style.opacity = "1";
		inputEl.disabled = false;
		inputEl.placeholder = "";
		inputEl.focus();
	} catch (e) {
		statusEl.textContent = "Failed to load: " + e.message;
		statusEl.className = "status error";
		console.error(e);
	}
}

document.addEventListener("DOMContentLoaded", () => {
	initUI();
	boot();
});
