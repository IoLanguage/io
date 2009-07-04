//metadoc copyright Chris Double 2009

#ifndef IOOggSyncState_DEFINED
#define IOOggSyncState_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include <ogg/ogg.h>

#define ISOggSyncState(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoOggSyncState_rawClone)

typedef IoObject IoOggSyncState;

IoTag *IoOggSyncState_newTag(void *state);
IoOggSyncState *IoOggSyncState_proto(void *state);
IoOggSyncState *IoOggSyncState_rawClone(IoOggSyncState *self);
void IoOggSyncState_free(IoOggSyncState *self);

IoObject *IoOggSyncState_clear(IoOggSyncState *self, IoObject *locals, IoMessage *m);

#endif
