#ifndef IO_FUTURE_H
#define IO_FUTURE_H

#include "IoObject.h"

// IoFuture — wraps a JS Promise. Created automatically when a JS call returns a thenable.
// State: pending (0), resolved (1), rejected (2).

IoObject *IoFuture_proto(void *state);
IoObject *IoFuture_newWithPromiseHandle(void *state, int promiseHandle);

// Called from io_resolve/io_reject WASM exports
void IoFuture_resolve(IoObject *self, IoObject *value);
void IoFuture_reject(IoObject *self, IoObject *error);

int IoFuture_isPending(IoObject *self);
int IoFuture_promiseHandle(IoObject *self);

#endif
