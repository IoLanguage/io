//metadoc AsyncRequest copyright Steve Dekorte 2002

#ifndef IoAsyncRequest_DEFINED
#define IoAsyncRequest_DEFINED 1

#include "IoObject.h"

#include <sys/types.h>
#include <aio.h>
#include <fcntl.h>
#include <sys/errno.h>

typedef IoObject IoAsyncRequest;

IoTag *IoAsyncRequest_newTag(void *state);
IoAsyncRequest *IoAsyncRequest_proto(void *state);
IoAsyncRequest *IoAsyncRequest_rawClone(IoAsyncRequest *self);
void IoAsyncRequest_free(IoAsyncRequest *self);

// ------------------------------------------------------------------------------

IoObject *IoAsyncRequest_setDescriptor(IoAsyncRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoAsyncRequest_descriptor(IoAsyncRequest *self, IoObject *locals, IoMessage *m);

IoObject *IoAsyncRequest_numberOfBytes(IoAsyncRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoAsyncRequest_position(IoAsyncRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoAsyncRequest_read(IoAsyncRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoAsyncRequest_write(IoAsyncRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoAsyncRequest_isDone(IoAsyncRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoAsyncRequest_error(IoAsyncRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoAsyncRequest_cancel(IoAsyncRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoAsyncRequest_sync(IoAsyncRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoAsyncRequest_copyBufferTo(IoAsyncRequest *self, IoObject *locals, IoMessage *m);

#endif
