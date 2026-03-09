// IoFuture — Io wrapper for JS Promises.
// Created automatically when a JS call returns a thenable.
// Provides await, isReady, state introspection.

#include "IoState.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoTag.h"
#include "IoMessage.h"
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
	IoObject *waitingCoro;  // coroutine that called await (NULL if none)
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
	if (data) free(data);
}

static void IoFuture_mark(IoObject *self) {
	IoFutureData *data = DATA(self);
	if (!data) return;
	IoObject_shouldMarkIfNonNull(data->waitingCoro);
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

// await — if resolved, return value. If rejected, raise error. If pending, yield to JS.
IO_METHOD(IoObject, Future_await) {
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

	// Pending — suspend eval loop, yield to JS host
	{
		IoEvalFrame *frame = IOSTATE->currentFrame;
		if (frame) {
			IoEvalFrameData *fd = FRAME_DATA(frame);
			fd->controlFlow.awaitInfo.future = self;
			fd->state = FRAME_STATE_AWAIT_JS;
			IOSTATE->awaitingJsPromise = 1;
			IOSTATE->needsControlFlowHandling = 1;
			return IONIL(self);  // Return value ignored; frame state takes over
		}
		// No frame (recursive eval fallback) — can't yield
		IoState_error_(IOSTATE, m, "Future is pending (cannot yield outside eval loop)");
		return IONIL(self);
	}
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

// forward — delegate unrecognized messages (like then/catch) to the JS Promise
IO_METHOD(IoObject, Future_forward) {
	IoFutureData *data = DATA(self);
	// Create a JSObject wrapping the same Promise handle and forward to it
	IoObject *jsObj = IoJSObject_newWithHandle_(IOSTATE, data->promiseHandle);
	return IoMessage_locals_performOn_(m, jsObj, jsObj);
}

// type — Io convention
IO_METHOD(IoObject, Future_type) {
	(void)locals; (void)m;
	return IoState_symbolWithCString_(IOSTATE, "Future");
}

// ---- Public API ----

void IoFuture_resolve(IoObject *self, IoObject *value) {
	IoFutureData *data = DATA(self);
	data->state = FUTURE_RESOLVED;
	data->value = value;
}

void IoFuture_reject(IoObject *self, IoObject *error) {
	IoFutureData *data = DATA(self);
	data->state = FUTURE_REJECTED;
	data->value = error;
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
		{"await",    IoObject_Future_await},
		{"isReady",  IoObject_Future_isReady},
		{"state",    IoObject_Future_state},
		{"label",    IoObject_Future_label},
		{"setLabel", IoObject_Future_setLabel},
		{"forward",  IoObject_Future_forward},
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
