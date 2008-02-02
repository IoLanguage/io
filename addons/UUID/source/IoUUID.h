/*
copyright
	Jonathan Wright, 2006
license
	BSD Revised
*/

#ifndef IOUUID_DEFINED
#define IOUUID_DEFINED 1

#include "IoObject.h"
#include <uuid/uuid.h>

#define ISUUID(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoUUID_rawClone)

typedef IoObject IoUUID;

IoUUID *IoUUID_rawClone(IoUUID *self);
IoUUID *IoUUID_proto(void *state);
IoUUID *IoUUID_new(void *state);

void IoUUID_free(IoUUID *self);
void IoUUID_mark(IoUUID *self);

/* ----------------------------------------------------------- */

IoObject *IoUUID_uuid(IoUUID *self, IoObject *locals, IoMessage *m);
IoObject *IoUUID_uuidRandom(IoUUID *self, IoObject *locals, IoMessage *m);
IoObject *IoUUID_uuidTime(IoUUID *self, IoObject *locals, IoMessage *m);

#endif
