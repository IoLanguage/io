// Binary serialization across the bridge buffer.
// Values by reference (JS objects, promises, Io objects) go through the VM's
// handle table and proxy factory, so both functions take the VM.

export const TYPE_NIL        = 0;
export const TYPE_TRUE       = 1;
export const TYPE_FALSE      = 2;
export const TYPE_NUMBER     = 3;
export const TYPE_STRING     = 4;
export const TYPE_ARRAY      = 5;
export const TYPE_OBJECT     = 6;
export const TYPE_JSREF      = 7;
export const TYPE_IOREF      = 8;
export const TYPE_ERROR      = 9;
export const TYPE_UNDEFINED  = 10;
export const TYPE_TYPEDARRAY = 11;
export const TYPE_FUTURE     = 12;
export const TYPE_BIGINT     = 13;

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

export function serialize(vm, val, buf, offset, visited) {
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

	// BigInt → TYPE_BIGINT (decimal string wire format)
	if (typeof val === "bigint") {
		const encoded = new TextEncoder().encode(val.toString());
		buf[offset] = TYPE_BIGINT;
		const view = new DataView(buf.buffer, buf.byteOffset);
		view.setUint32(offset + 1, encoded.length, true);
		buf.set(encoded, offset + 5);
		return offset + 5 + encoded.length;
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
			buf.set(new Uint8Array(val.buffer, val.byteOffset, val.byteLength), offset + 6);
			return offset + 6 + val.byteLength;
		}
	}

	// Containers: Array, Map, Set — deep copy with cycle detection
	if (!visited) visited = new Set();

	if (Array.isArray(val) || val instanceof Set) {
		if (visited.has(val)) {
			throw new Error("bridge error: cyclic structure cannot be serialized");
		}
		visited.add(val);
		const items = Array.isArray(val) ? val : Array.from(val);
		buf[offset] = TYPE_ARRAY;
		const view = new DataView(buf.buffer, buf.byteOffset);
		view.setUint32(offset + 1, items.length, true);
		let pos = offset + 5;
		for (const item of items) {
			pos = serialize(vm, item, buf, pos, visited);
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
			const keyEncoded = new TextEncoder().encode(String(k));
			view.setUint32(pos, keyEncoded.length, true);
			pos += 4;
			buf.set(keyEncoded, pos);
			pos += keyEncoded.length;
			pos = serialize(vm, v, buf, pos, visited);
		}
		visited.delete(val);
		return pos;
	}

	// Promise/thenable → TYPE_FUTURE (pass Promise by handle, C side creates IoFuture)
	// All other objects pass by reference JS→Io
	buf[offset] = typeof val.then === "function" ? TYPE_FUTURE : TYPE_JSREF;
	const view = new DataView(buf.buffer, buf.byteOffset);
	view.setInt32(offset + 1, vm.handles.register(val), true);
	return offset + 5;
}

export function deserialize(vm, buf, offset) {
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
		return { value: view.getFloat64(offset, true), offset: offset + 8 };
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
			const r = deserialize(vm, buf, offset);
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
			const r = deserialize(vm, buf, offset);
			map.set(key, r.value);
			offset = r.offset;
		}
		return { value: map, offset };
	}

	case TYPE_JSREF: {
		const view = new DataView(buf.buffer, buf.byteOffset);
		return { value: vm.handles.get(view.getInt32(offset, true)), offset: offset + 4 };
	}

	case TYPE_IOREF: {
		const view = new DataView(buf.buffer, buf.byteOffset);
		return { value: vm.proxy(view.getInt32(offset, true)), offset: offset + 4 };
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
		const byteLen = count * Ctor.BYTES_PER_ELEMENT;
		// Copy bytes out and create typed array
		const copy = new Uint8Array(byteLen);
		copy.set(buf.subarray(offset, offset + byteLen));
		return { value: new Ctor(copy.buffer, 0, count), offset: offset + byteLen };
	}

	case TYPE_BIGINT: {
		const view = new DataView(buf.buffer, buf.byteOffset);
		const len = view.getUint32(offset, true);
		offset += 4;
		const str = new TextDecoder().decode(buf.subarray(offset, offset + len));
		return { value: BigInt(str), offset: offset + len };
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
			if (errHandle) ioError = vm.proxy(errHandle);
		}
		const err = new Error(msg);
		if (ioError) err.ioError = ioError;
		return { value: err, offset };
	}

	default:
		return { value: null, offset };
	}
}

// Write an error into the bridge buffer; the C side sees TYPE_ERROR.
export function serializeError(vm, msg, originalError) {
	const { buf } = vm.bridgeBuf();
	const encoded = new TextEncoder().encode(msg);
	buf[0] = TYPE_ERROR;
	const view = new DataView(buf.buffer, buf.byteOffset);
	view.setUint32(1, encoded.length, true);
	buf.set(encoded, 5);
	// Write error handle for exception proxy
	const errHandle = originalError != null ? vm.handles.register(originalError) : 0;
	view.setInt32(5 + encoded.length, errHandle, true);
	return -1;
}
