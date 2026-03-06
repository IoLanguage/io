// Bidirectional Io-JS bridge.
// Io side: JSObject proto with forward dispatch to JS via binary serialization.
// JS side: Proxy wrapping Io objects via ioHandles.

#include "IoState.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoMap.h"
#include "IoTag.h"
#include "IoMessage.h"
#include "PHash_inline.h"
#include "IoState_symbols.h"
#include "io_js_bridge.h"
#include <string.h>
#include <stdio.h>

// ---- Proto ID ----

static const char *protoId = "JSObject";

#define HANDLE(self) ((int)(intptr_t)IoObject_dataPointer(self))
#define SET_HANDLE(self, h) IoObject_setDataPointer_(self, (void *)(intptr_t)(h))

static IoObject *IoJSObject_rawClone(IoObject *proto);
#define IS_JSOBJECT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoJSObject_rawClone)

// ---- Bridge buffer ----

#define BRIDGE_BUF_SIZE (64 * 1024)
static unsigned char bridge_buf[BRIDGE_BUF_SIZE];

__attribute__((export_name("io_get_bridge_buf")))
unsigned char *io_get_bridge_buf(void) { return bridge_buf; }

__attribute__((export_name("io_get_bridge_buf_size")))
int io_get_bridge_buf_size(void) { return BRIDGE_BUF_SIZE; }

// ---- Type tags (must match JS side) ----

#define TYPE_NIL    0
#define TYPE_TRUE   1
#define TYPE_FALSE  2
#define TYPE_NUMBER 3
#define TYPE_STRING 4
#define TYPE_ARRAY  5
#define TYPE_OBJECT 6
#define TYPE_JSREF  7
#define TYPE_IOREF  8
#define TYPE_ERROR  9

// ---- WASM imports from "js" module ----

#define JS_IMPORT(name) __attribute__((import_module("js"), import_name(#name)))

JS_IMPORT(js_call)       extern int js_call(int handle, const char *name, int name_len, int argc);
JS_IMPORT(js_get_prop)   extern int js_get_prop(int handle, const char *name, int name_len);
JS_IMPORT(js_set_prop)   extern void js_set_prop(int handle, const char *name, int name_len);
JS_IMPORT(js_call_func)  extern int js_call_func(int handle, int argc);
JS_IMPORT(js_typeof)     extern int js_typeof(int handle, char *buf, int sz);
JS_IMPORT(js_get_global) extern int js_get_global(void);
JS_IMPORT(js_release)    extern void js_release(int h);

// ---- Io handle table (for JS→Io references) ----

#define IO_HANDLES_MAX 1024
static IoObject *ioHandles[IO_HANDLES_MAX];
static int ioHandles_next = 1; // 0 = nil/invalid

static int ioHandles_register(IoObject *obj) {
	// Linear scan for free slot (starting from hint)
	for (int i = 0; i < IO_HANDLES_MAX; i++) {
		int idx = (ioHandles_next + i) % IO_HANDLES_MAX;
		if (idx == 0) continue; // reserve 0
		if (ioHandles[idx] == NULL) {
			ioHandles[idx] = obj;
			ioHandles_next = (idx + 1) % IO_HANDLES_MAX;
			return idx;
		}
	}
	return -1; // table full
}

static IoObject *ioHandles_get(int h) {
	if (h <= 0 || h >= IO_HANDLES_MAX) return NULL;
	return ioHandles[h];
}

static void ioHandles_release(int h) {
	if (h > 0 && h < IO_HANDLES_MAX) {
		ioHandles[h] = NULL;
	}
}

void IoJSBridge_markIoHandles(void) {
	for (int i = 1; i < IO_HANDLES_MAX; i++) {
		IoObject_shouldMarkIfNonNull(ioHandles[i]);
	}
}

// ---- Serialization: Io → bridge_buf ----

static int serialize_io_to_buf(IoState *state, IoObject *obj, unsigned char *buf, int maxLen) {
	if (maxLen < 1) return 0;

	if (obj == state->ioNil) {
		buf[0] = TYPE_NIL;
		return 1;
	}

	if (obj == state->ioTrue) {
		buf[0] = TYPE_TRUE;
		return 1;
	}

	if (obj == state->ioFalse) {
		buf[0] = TYPE_FALSE;
		return 1;
	}

	if (ISNUMBER(obj)) {
		if (maxLen < 9) return 0;
		buf[0] = TYPE_NUMBER;
		double val = IoObject_dataDouble(obj);
		memcpy(buf + 1, &val, 8);
		return 9;
	}

	if (ISSEQ(obj)) {
		const char *str = CSTRING(obj);
		int slen = (int)IoSeq_rawSize(obj);
		if (maxLen < 5 + slen) return 0;
		buf[0] = TYPE_STRING;
		memcpy(buf + 1, &slen, 4);
		memcpy(buf + 5, str, slen);
		return 5 + slen;
	}

	if (ISLIST(obj)) {
		List *list = IoList_rawList((IoList *)obj);
		int count = (int)List_size(list);
		if (maxLen < 5) return 0;
		buf[0] = TYPE_ARRAY;
		memcpy(buf + 1, &count, 4);
		int offset = 5;
		for (int i = 0; i < count; i++) {
			IoObject *item = (IoObject *)List_at_(list, i);
			int wrote = serialize_io_to_buf(state, item, buf + offset, maxLen - offset);
			if (wrote == 0) return 0;
			offset += wrote;
		}
		return offset;
	}

	if (ISMAP(obj)) {
		PHash *hash = IoMap_rawHash((IoMap *)obj);
		int count = (int)PHash_count(hash);
		if (maxLen < 5) return 0;
		buf[0] = TYPE_OBJECT;
		memcpy(buf + 1, &count, 4);
		int offset = 5;
		PHASH_FOREACH(hash, k, v,
			const char *kstr = CSTRING((IoObject *)k);
			int klen = (int)IoSeq_rawSize((IoObject *)k);
			if (offset + 4 + klen >= maxLen) return 0;
			memcpy(buf + offset, &klen, 4);
			offset += 4;
			memcpy(buf + offset, kstr, klen);
			offset += klen;
			int wrote = serialize_io_to_buf(state, (IoObject *)v, buf + offset, maxLen - offset);
			if (wrote == 0) return 0;
			offset += wrote;
		);
		return offset;
	}

	if (IS_JSOBJECT(obj)) {
		if (maxLen < 5) return 0;
		buf[0] = TYPE_JSREF;
		int h = HANDLE(obj);
		memcpy(buf + 1, &h, 4);
		return 5;
	}

	// Other Io objects: register as ioHandle reference
	{
		if (maxLen < 5) return 0;
		int h = ioHandles_register(obj);
		if (h < 0) return 0;
		buf[0] = TYPE_IOREF;
		memcpy(buf + 1, &h, 4);
		return 5;
	}
}

// ---- Deserialization: bridge_buf → Io ----

static IoObject *deserialize_buf_to_io(IoState *state, unsigned char **ptr, unsigned char *end) {
	if (*ptr >= end) return state->ioNil;

	unsigned char type = **ptr;
	(*ptr)++;

	switch (type) {
	case TYPE_NIL:
		return state->ioNil;

	case TYPE_TRUE:
		return state->ioTrue;

	case TYPE_FALSE:
		return state->ioFalse;

	case TYPE_NUMBER: {
		if (*ptr + 8 > end) return state->ioNil;
		double val;
		memcpy(&val, *ptr, 8);
		*ptr += 8;
		return (IoObject *)IoNumber_newWithDouble_(state, val);
	}

	case TYPE_STRING: {
		if (*ptr + 4 > end) return state->ioNil;
		int slen;
		memcpy(&slen, *ptr, 4);
		*ptr += 4;
		if (*ptr + slen > end) return state->ioNil;
		IoObject *seq = (IoObject *)IoSeq_newWithData_length_(state, *ptr, slen);
		*ptr += slen;
		return seq;
	}

	case TYPE_ARRAY: {
		if (*ptr + 4 > end) return state->ioNil;
		int count;
		memcpy(&count, *ptr, 4);
		*ptr += 4;
		IoList *list = IoList_new(state);
		for (int i = 0; i < count; i++) {
			IoObject *item = deserialize_buf_to_io(state, ptr, end);
			IoList_rawAppend_(list, item);
		}
		return (IoObject *)list;
	}

	case TYPE_OBJECT: {
		if (*ptr + 4 > end) return state->ioNil;
		int count;
		memcpy(&count, *ptr, 4);
		*ptr += 4;
		IoMap *map = IoMap_new(state);
		for (int i = 0; i < count; i++) {
			if (*ptr + 4 > end) break;
			int klen;
			memcpy(&klen, *ptr, 4);
			*ptr += 4;
			if (*ptr + klen > end) break;
			*ptr += klen;
			IoSymbol *key = IoState_symbolWithCString_length_(state, (const char *)(*ptr - klen), klen);
			IoObject *val = deserialize_buf_to_io(state, ptr, end);
			IoMap_rawAtPut(map, key, val);
		}
		return (IoObject *)map;
	}

	case TYPE_JSREF: {
		if (*ptr + 4 > end) return state->ioNil;
		int h;
		memcpy(&h, *ptr, 4);
		*ptr += 4;
		return IoJSObject_newWithHandle_(state, h);
	}

	case TYPE_IOREF: {
		if (*ptr + 4 > end) return state->ioNil;
		int h;
		memcpy(&h, *ptr, 4);
		*ptr += 4;
		IoObject *obj = ioHandles_get(h);
		return obj ? obj : state->ioNil;
	}

	case TYPE_ERROR: {
		if (*ptr + 4 > end) return state->ioNil;
		int slen;
		memcpy(&slen, *ptr, 4);
		*ptr += 4;
		if (*ptr + slen > end) return state->ioNil;
		// We don't raise here — caller checks for TYPE_ERROR before deserializing
		*ptr += slen;
		return state->ioNil;
	}

	default:
		return state->ioNil;
	}
}

// ---- JSObject tag ----

static IoObject *IoJSObject_rawClone(IoObject *proto) {
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_tag_(self, IoObject_tag(proto));
	SET_HANDLE(self, 0);
	return self;
}

static void IoJSObject_free(IoObject *self) {
	int h = HANDLE(self);
	if (h) js_release(h);
}

static IoTag *IoJSObject_newTag(void *state) {
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoJSObject_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoJSObject_free);
	return tag;
}

// ---- JSObject instance factory ----

IoObject *IoJSObject_newWithHandle_(void *state, int handle) {
	if (handle == 0) return ((IoState *)state)->ioNil;
	IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
	IoObject *self = IOCLONE(proto);
	SET_HANDLE(self, handle);
	return self;
}

// ---- JSObject methods ----

// forward: dispatched for any unrecognized message on JSObject
IO_METHOD(IoObject, JSObject_forward) {
	IoSymbol *name = IoMessage_name(m);
	const char *cname = CSTRING(name);
	int nameLen = (int)IoSeq_rawSize(name);
	int argc = IoMessage_argCount(m);

	// Evaluate all args into a local array first (buffer re-entrancy safety)
	IoObject *args[32];
	int argCount = argc < 32 ? argc : 32;
	for (int i = 0; i < argCount; i++) {
		args[i] = IoMessage_locals_valueArgAt_(m, locals, i);
		if (IOSTATE->errorRaised) return IONIL(self);
	}

	// Serialize args to bridge_buf
	int offset = 0;
	for (int i = 0; i < argCount; i++) {
		int wrote = serialize_io_to_buf(IOSTATE, args[i], bridge_buf + offset, BRIDGE_BUF_SIZE - offset);
		if (wrote == 0) {
			IoState_error_(IOSTATE, m, "JS bridge: argument serialization overflow");
			return IONIL(self);
		}
		offset += wrote;
	}

	// Call into JS
	int resultType = js_call(HANDLE(self), cname, nameLen, argCount);

	// Check for error
	if (resultType < 0 || bridge_buf[0] == TYPE_ERROR) {
		if (bridge_buf[0] == TYPE_ERROR) {
			unsigned char *p = bridge_buf + 1;
			int errLen = 0;
			if (p + 4 <= bridge_buf + BRIDGE_BUF_SIZE) {
				memcpy(&errLen, p, 4);
				p += 4;
			}
			char errBuf[1024];
			int copyLen = errLen < (int)sizeof(errBuf) - 1 ? errLen : (int)sizeof(errBuf) - 1;
			memcpy(errBuf, p, copyLen);
			errBuf[copyLen] = '\0';
			IoState_error_(IOSTATE, m, "JS error: %s", errBuf);
		} else {
			IoState_error_(IOSTATE, m, "JS bridge: call failed");
		}
		return IONIL(self);
	}

	// Deserialize result
	unsigned char *ptr = bridge_buf;
	unsigned char *end = bridge_buf + BRIDGE_BUF_SIZE;
	return deserialize_buf_to_io(IOSTATE, &ptr, end);
}

// get(name) — pure property get, never auto-calls functions
IO_METHOD(IoObject, JSObject_get) {
	IoSymbol *name = IoMessage_locals_symbolArgAt_(m, locals, 0);
	if (IOSTATE->errorRaised) return IONIL(self);
	const char *cname = CSTRING(name);
	int nameLen = (int)IoSeq_rawSize(name);

	js_get_prop(HANDLE(self), cname, nameLen);

	if (bridge_buf[0] == TYPE_ERROR) {
		IoState_error_(IOSTATE, m, "JS error in get");
		return IONIL(self);
	}

	unsigned char *ptr = bridge_buf;
	unsigned char *end = bridge_buf + BRIDGE_BUF_SIZE;
	return deserialize_buf_to_io(IOSTATE, &ptr, end);
}

// set(name, value) — property set, returns self
IO_METHOD(IoObject, JSObject_set) {
	IoSymbol *name = IoMessage_locals_symbolArgAt_(m, locals, 0);
	if (IOSTATE->errorRaised) return IONIL(self);
	IoObject *val = IoMessage_locals_valueArgAt_(m, locals, 1);
	if (IOSTATE->errorRaised) return IONIL(self);

	const char *cname = CSTRING(name);
	int nameLen = (int)IoSeq_rawSize(name);

	// Serialize value into bridge_buf
	int wrote = serialize_io_to_buf(IOSTATE, val, bridge_buf, BRIDGE_BUF_SIZE);
	if (wrote == 0) {
		IoState_error_(IOSTATE, m, "JS bridge: value serialization overflow");
		return IONIL(self);
	}

	js_set_prop(HANDLE(self), cname, nameLen);
	return self;
}

// at(index) — array index access: obj[index]
IO_METHOD(IoObject, JSObject_at) {
	IoObject *idx = IoMessage_locals_numberArgAt_(m, locals, 0);
	if (IOSTATE->errorRaised) return IONIL(self);
	int index = (int)IoObject_dataDouble(idx);

	// Use js_call with numeric index converted to string
	char numBuf[32];
	int numLen = snprintf(numBuf, sizeof(numBuf), "%d", index);

	js_get_prop(HANDLE(self), numBuf, numLen);

	if (bridge_buf[0] == TYPE_ERROR) {
		IoState_error_(IOSTATE, m, "JS error in at");
		return IONIL(self);
	}

	unsigned char *ptr = bridge_buf;
	unsigned char *end = bridge_buf + BRIDGE_BUF_SIZE;
	return deserialize_buf_to_io(IOSTATE, &ptr, end);
}

// call(args...) — invoke self as function: obj(args...)
IO_METHOD(IoObject, JSObject_call) {
	int argc = IoMessage_argCount(m);

	IoObject *args[32];
	int argCount = argc < 32 ? argc : 32;
	for (int i = 0; i < argCount; i++) {
		args[i] = IoMessage_locals_valueArgAt_(m, locals, i);
		if (IOSTATE->errorRaised) return IONIL(self);
	}

	int offset = 0;
	for (int i = 0; i < argCount; i++) {
		int wrote = serialize_io_to_buf(IOSTATE, args[i], bridge_buf + offset, BRIDGE_BUF_SIZE - offset);
		if (wrote == 0) {
			IoState_error_(IOSTATE, m, "JS bridge: argument serialization overflow");
			return IONIL(self);
		}
		offset += wrote;
	}

	js_call_func(HANDLE(self), argCount);

	if (bridge_buf[0] == TYPE_ERROR) {
		IoState_error_(IOSTATE, m, "JS error in call");
		return IONIL(self);
	}

	unsigned char *ptr = bridge_buf;
	unsigned char *end = bridge_buf + BRIDGE_BUF_SIZE;
	return deserialize_buf_to_io(IOSTATE, &ptr, end);
}

// typeof — JS typeof string
IO_METHOD(IoObject, JSObject_typeof) {
	(void)locals; (void)m;
	char typeBuf[64];
	int len = js_typeof(HANDLE(self), typeBuf, sizeof(typeBuf));
	if (len < 0) len = 0;
	typeBuf[len] = '\0';
	return IoState_symbolWithCString_(IOSTATE, typeBuf);
}

// type — standard Io convention, returns "JSObject"
IO_METHOD(IoObject, JSObject_type) {
	(void)locals; (void)m;
	return IoState_symbolWithCString_(IOSTATE, "JSObject");
}

// ---- JSObject proto ----

// Set GC marking hook (defined in IoCoroutine.c)
extern void (*IoJSBridge_markIoHandlesFunc)(void);

IoObject *IoJSObject_proto(void *state) {
	// Wire up GC marking for ioHandles
	IoJSBridge_markIoHandlesFunc = IoJSBridge_markIoHandles;

	IoMethodTable methodTable[] = {
		{"forward", IoObject_JSObject_forward},
		{"get",     IoObject_JSObject_get},
		{"set",     IoObject_JSObject_set},
		{"at",      IoObject_JSObject_at},
		{"call",    IoObject_JSObject_call},
		{"typeof",  IoObject_JSObject_typeof},
		{"type",    IoObject_JSObject_type},
		{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoJSObject_newTag(state));
	SET_HANDLE(self, 0);
	IoState_registerProtoWithId_((IoState *)state, self, protoId);
	IoObject_addMethodTable_(self, methodTable);
	return self;
}

// ---- WASM exports for JS→Io direction ----

// Reference to the global IoState (defined in io_browser.c)
extern IoState *io_bridge_state;

__attribute__((export_name("io_send")))
int io_send(int io_handle, const char *name_ptr, int name_len, int argc) {
	IoState *state = io_bridge_state;
	if (!state) return -1;

	IoObject *target = ioHandles_get(io_handle);
	if (!target) return -1;

	// Create symbol for message name
	char nameBuf[256];
	int copyLen = name_len < (int)sizeof(nameBuf) - 1 ? name_len : (int)sizeof(nameBuf) - 1;
	memcpy(nameBuf, name_ptr, copyLen);
	nameBuf[copyLen] = '\0';

	IoSymbol *msgName = IoState_symbolWithCString_(state, nameBuf);

	// Build message with cached (pre-evaluated) args
	IoMessage *msg = IoMessage_newWithName_(state, msgName);

	// Deserialize args from bridge_buf
	unsigned char *ptr = bridge_buf;
	unsigned char *end = bridge_buf + BRIDGE_BUF_SIZE;
	for (int i = 0; i < argc; i++) {
		IoObject *arg = deserialize_buf_to_io(state, &ptr, end);
		IoMessage_addCachedArg_(msg, arg);
	}

	// Perform the message send
	state->errorRaised = 0;
	IoObject *result = IoMessage_locals_performOn_(msg, target, target);

	if (state->errorRaised) {
		// Serialize error
		const char *errMsg = "Io error";
		int errLen = (int)strlen(errMsg);
		bridge_buf[0] = TYPE_ERROR;
		memcpy(bridge_buf + 1, &errLen, 4);
		memcpy(bridge_buf + 5, errMsg, errLen);
		state->errorRaised = 0;
		return 1;
	}

	// Serialize result to bridge_buf
	int wrote = serialize_io_to_buf(state, result, bridge_buf, BRIDGE_BUF_SIZE);
	if (wrote == 0) {
		bridge_buf[0] = TYPE_NIL;
	}
	return 0;
}

__attribute__((export_name("io_release")))
void io_release(int io_handle) {
	ioHandles_release(io_handle);
}

__attribute__((export_name("io_get_lobby_handle")))
int io_get_lobby_handle(void) {
	IoState *state = io_bridge_state;
	if (!state) return 0;
	IoObject *lobby = state->lobby;
	return ioHandles_register(lobby);
}
