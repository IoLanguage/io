//metadoc copyright Chris Double 2009

#ifndef IOOggSyncState_DEFINED
#define IOOggSyncState_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include <ogg/ogg.h>

#define ISOGGSYNCSTATE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoOggSyncState_rawClone)

typedef IoObject IoOggSyncState;

IoObject *IoMessage_locals_oggSyncStateArgAt_(IoMessage *self, IoObject *locals, int n);

IoTag *IoOggSyncState_newTag(void *state);
IoOggSyncState *IoOggSyncState_proto(void *state);
IoOggSyncState *IoOggSyncState_rawClone(IoOggSyncState *self);
void IoOggSyncState_free(IoOggSyncState *self);

IoObject *IoOggSyncState_clear(IoOggSyncState *self, IoObject *locals, IoMessage *m);
IoObject *IoOggSyncState_reset(IoOggSyncState *self, IoObject *locals, IoMessage *m);
IoObject *IoOggSyncState_write(IoOggSyncState *self, IoObject *locals, IoMessage *m);
IoObject *IoOggSyncState_pageseek(IoOggSyncState *self, IoObject *locals, IoMessage *m);
IoObject *IoOggSyncState_pageout(IoOggSyncState *self, IoObject *locals, IoMessage *m);

#endif
