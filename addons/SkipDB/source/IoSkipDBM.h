
//metadoc SkipDB copyright Steve Dekorte 2002
//metadoc SkipDB license BSD revised

#ifndef IoSkipDBM_DEFINED
#define IoSkipDBM_DEFINED 1

#include "Common.h"
#include "IoState.h"
#include "IoObject.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISSKIPDBM(self) \
	IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSkipDBM_rawClone)

typedef IoObject IoSkipDBM;

IoSkipDBM *IoSkipDBM_proto(void *state);
IoSkipDBM *IoSkipDBM_rawClone(IoObject *self);
IoSkipDBM *IoSkipDBM_new(void *state);
void IoSkipDBM_free(IoObject *self);
void IoSkipDBM_mark(IoObject *self);

//void IoSkipDBM_writeToStream_(IoObject *self, BStream *stream);
//void IoSkipDBM_readFromStream_(IoObject *self, BStream *stream);

// --------------------------------------------------------

IoObject *IoSkipDBM_setPath(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDBM_path(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDBM_open(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDBM_close(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDBM_isOpen(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDBM_delete(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDBM_root(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDBM_at(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDBM_compact(IoObject *self, IoObject *locals, IoMessage *m);

IoObject *IoSkipDBM_beginTransaction(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoSkipDBM_commitnTransaction(IoObject *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif
