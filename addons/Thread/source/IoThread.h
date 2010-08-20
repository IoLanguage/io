
//metadoc Thread copyright Steve Dekorte 2002


#ifndef IOTHREAD_DEFINED
#define IOTHREAD_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
//#include <zlib.h>

#define ISThread(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoThread_rawClone)

typedef IoObject IoThread;

IoTag *IoThread_newTag(void *state);
IoThread *IoThread_proto(void *state);
IoThread *IoThread_rawClone(IoThread *self);
void IoThread_free(IoThread *self);

IoObject *IoThread_createThread(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoThread_threadCount(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoThread_endCurrentThread(IoObject *self, IoObject *locals, IoMessage *m);

#endif
