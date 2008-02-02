//metadoc SkipDB copyright Steve Dekorte 2002
//metadoc SkipDB license BSD revised

#ifndef IoSkipDB_DEFINED
#define IoSkipDB_DEFINED 1

#include "Common.h"
#include "IoState.h"
#include "IoObject.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISSKIPDB(self) \
	IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSkipDB_rawClone)

typedef IoObject IoSkipDB;

IoSkipDB *IoSkipDB_proto(void *state);
IoSkipDB *IoSkipDB_rawClone(IoObject *self);
IoSkipDB *IoSkipDB_new(void *state);
IoSkipDB *IoSkipDB_newWithSDB(void *state, SkipDB *sdb);
void IoSkipDB_free(IoObject *self);
void IoSkipDB_mark(IoObject *self);

//void IoSkipDB_writeToStream_(IoObject *self, BStream *stream);
//void IoSkipDB_readFromStream_(IoObject *self, BStream *stream);

// --------------------------------------------------------

IoObject *IoSkipDB_headerPid(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDB_atPut(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDB_at(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDB_removeAt(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDB_size(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDB_cursor(IoObject *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif
