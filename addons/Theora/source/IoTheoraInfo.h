//metadoc copyright Chris Double 2009

#ifndef IOTheoraInfo_DEFINED
#define IOTheoraInfo_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#define ISTHEORAINFO(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoTheoraInfo_rawClone)

typedef IoObject IoTheoraInfo;

IoObject *IoMessage_locals_theoraInfoArgAt_(IoMessage *self, IoObject *locals, int n);

IoTag *IoTheoraInfo_newTag(void *state);
IoTheoraInfo *IoTheoraInfo_proto(void *state);
IoTheoraInfo *IoTheoraInfo_rawClone(IoTheoraInfo *self);
void IoTheoraInfo_free(IoTheoraInfo *self);

IoObject *IoTheoraInfo_frameWidth(IoTheoraInfo *self, IoObject *locals, IoMessage *m);
IoObject *IoTheoraInfo_frameHeight(IoTheoraInfo *self, IoObject *locals, IoMessage *m);
IoObject *IoTheoraInfo_frameRate(IoTheoraInfo *self, IoObject *locals, IoMessage *m);

#endif
