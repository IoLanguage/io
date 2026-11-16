// IoFuture — Io wrapper for JS Promises.
// Created automatically when a JS call returns a thenable.
// Provides await, isReady, state introspection.

#include "IoState.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoTag.h"
#include "IoMessage.h"
#include "IoList.h"
#include "io_future.h"
#include "io_js_bridge.h"
#include "IoEvalFrame.h"
#include <string.h>

// ---- Future states ----

#define FUTURE_PENDING  0
#define FUTURE_RESOLVED 1
#define FUTURE_REJECTED 2

// ---- Future data ----

typedef struct {
	int promiseHandle;      // JS Promise handle (in jsHandles)
	IoObject *value;        // resolved value (NULL while pending)
	IoObject *label;        // human-readable description
	int state;              // FUTURE_PENDING, FUTURE_RESOLVED, FUTURE_REJECTED
} IoFutureData;

#define DATA(self) ((IoFutureData *)IoObject_dataPointer(self))

// ---- Proto ID ----

static const char *protoId = "Future";

// ---- Tag ----

static IoObject *IoFuture_rawClone(IoObject *proto) {
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_tag_(self, IoObject_tag(proto));
	IoFutureData *data = calloc(1, sizeof(IoFutureData));
	IoObject_setDataPointer_(self, data);
	return self;
}

static void IoFuture_free(IoObject *self) {
	IoFutureData *data = DATA(self);
	if (!data) return;
	// The Future owns its Promise's JS handle
	IoJSBridge_releaseJsHandle(data->promiseHandle);
	free(data);
}

static void IoFuture_mark(IoObject *self) {
	IoFutureData *data = DATA(self);
	if (!data) return;
	IoObject_shouldMarkIfNonNull(data->value);
	IoObject_shouldMarkIfNonNull(data->label);
}

static IoTag *IoFuture_newTag(void *state) {
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoFuture_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoFuture_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoFuture_mark);
	return tag;
}

// ---- Methods ----

// rawAwait — if resolved, return value. If rejected or pending, raise an error.
// `await` itself is defined in Io (io_browser.c) on top of this.
IO_METHOD(IoObject, Future_rawAwait) {
	(void)locals; (void)m;
	IoFutureData *data = DATA(self);

	if (data->state == FUTURE_RESOLVED) {
		return data->value ? data->value : IONIL(self);
	}

	if (data->state == FUTURE_REJECTED) {
		IoState_error_(IOSTATE, m, "Future rejected: %s",
			data->value && ISSEQ(data->value) ? CSTRING(data->value) : "unknown error");
		return IONIL(self);
	}

	// Pending. The Io-side await (io_browser.c) parks the coroutine until
	// the Promise settles, so this is only reached by calling rawAwait directly.
	IoState_error_(IOSTATE, m, "Future is pending");
	return IONIL(self);
}

// isReady — return true if resolved or rejected
IO_METHOD(IoObject, Future_isReady) {
	(void)locals; (void)m;
	IoFutureData *data = DATA(self);
	return data->state != FUTURE_PENDING ? IOSTATE->ioTrue : IOSTATE->ioFalse;
}

// state — return "pending", "resolved", or "rejected"
IO_METHOD(IoObject, Future_state) {
	(void)locals; (void)m;
	IoFutureData *data = DATA(self);
	switch (data->state) {
	case FUTURE_RESOLVED: return IoState_symbolWithCString_(IOSTATE, "resolved");
	case FUTURE_REJECTED: return IoState_symbolWithCString_(IOSTATE, "rejected");
	default:              return IoState_symbolWithCString_(IOSTATE, "pending");
	}
}

// label — get description
IO_METHOD(IoObject, Future_label) {
	(void)locals; (void)m;
	IoFutureData *data = DATA(self);
	return data->label ? data->label : IONIL(self);
}

// setLabel(str) — set description
IO_METHOD(IoObject, Future_setLabel) {
	IoObject *label = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (IOSTATE->errorRaised) return IONIL(self);
	DATA(self)->label = label;
	return self;
}

// then/catch/finally — delegate to the underlying JS Promise, through a
// temporary JSObject that borrows the Future's handle without owning it.
// IoObject_perform sends just this message, with the arguments the eval loop
// already evaluated in the caller's context. (Evaluating `m` as a chain would
// also run whatever follows it in the statement, a second time.)
static IoObject *IoFuture_sendToPromise(IoObject *self, IoObject *locals, IoMessage *m) {
	IoObject *jsObj = IoJSObject_newWithHandle_(IOSTATE, DATA(self)->promiseHandle);
	IoObject *result = IoObject_perform(jsObj, locals, m);
	IoJSObject_disownHandle(jsObj);
	return result;
}

IO_METHOD(IoObject, Future_then) {
	return IoFuture_sendToPromise(self, locals, m);
}

IO_METHOD(IoObject, Future_catch) {
	return IoFuture_sendToPromise(self, locals, m);
}

IO_METHOD(IoObject, Future_finally) {
	return IoFuture_sendToPromise(self, locals, m);
}

// type — Io convention
IO_METHOD(IoObject, Future_type) {
	(void)locals; (void)m;
	return IoState_symbolWithCString_(IOSTATE, "Future");
}

// ---- Public API ----

// Coroutines parked in await (the Io-side `waiters` list) go back on the
// run queue; the next scheduler tick resumes them.
extern void io_browser_make_runnable(IoObject *coro);

static void IoFuture_wakeWaiters(IoObject *self) {
	IoObject *waiters = IoObject_rawGetSlot_(self, IoState_symbolWithCString_(IOSTATE, "waiters"));
	if (!waiters || !ISLIST(waiters)) return;
	size_t n = IoList_rawSize(waiters);
	for (size_t i = 0; i < n; i++) {
		io_browser_make_runnable(IoList_rawAt_(waiters, (int)i));
	}
	List_removeAll(IoList_rawList(waiters));
}

void IoFuture_resolve(IoObject *self, IoObject *value) {
	IoFutureData *data = DATA(self);
	data->state = FUTURE_RESOLVED;
	data->value = value;
	IoFuture_wakeWaiters(self);
}

void IoFuture_reject(IoObject *self, IoObject *error) {
	IoFutureData *data = DATA(self);
	data->state = FUTURE_REJECTED;
	data->value = error;
	IoFuture_wakeWaiters(self);
}

int IoFuture_isPending(IoObject *self) {
	return DATA(self)->state == FUTURE_PENDING;
}

int IoFuture_promiseHandle(IoObject *self) {
	return DATA(self)->promiseHandle;
}

// ---- Proto ----

IoObject *IoFuture_proto(void *state) {
	IoMethodTable methodTable[] = {
		{"rawAwait", IoObject_Future_rawAwait},
		{"isReady",  IoObject_Future_isReady},
		{"state",    IoObject_Future_state},
		{"label",    IoObject_Future_label},
		{"setLabel", IoObject_Future_setLabel},
		{"then",     IoObject_Future_then},
		{"catch",    IoObject_Future_catch},
		{"finally",  IoObject_Future_finally},
		{"type",     IoObject_Future_type},
		{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoFuture_newTag(state));
	IoFutureData *data = calloc(1, sizeof(IoFutureData));
	IoObject_setDataPointer_(self, data);
	IoState_registerProtoWithId_((IoState *)state, self, protoId);
	IoObject_addMethodTable_(self, methodTable);
	return self;
}

IoObject *IoFuture_newWithPromiseHandle(void *state, int promiseHandle) {
	IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
	IoObject *self = IOCLONE(proto);
	DATA(self)->promiseHandle = promiseHandle;
	DATA(self)->state = FUTURE_PENDING;
	return self;
}
