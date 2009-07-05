//metadoc copyright Chris Double 2009

#ifndef IOVorbisComment_DEFINED
#define IOVorbisComment_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#define ISVORBISDSPSTATE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoVorbisDspState_rawClone)

typedef IoObject IoVorbisDspState;

IoObject *IoMessage_locals_vorbisDspStateArgAt_(IoMessage *self, IoObject *locals, int n);

IoTag *IoVorbisDspState_newTag(void *state);
IoVorbisDspState *IoVorbisDspState_proto(void *state);
IoVorbisDspState *IoVorbisDspState_rawClone(IoVorbisDspState *self);
void IoVorbisDspState_free(IoVorbisDspState *self);

#endif
