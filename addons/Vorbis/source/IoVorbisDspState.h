//metadoc copyright Chris Double 2009

#ifndef IOVorbisDspState_DEFINED
#define IOVorbisDspState_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#define ISVORBISDSPSTATE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoVorbisDspState_rawClone)

typedef IoObject IoVorbisDspState;

IoObject *IoMessage_locals_vorbisDspStateArgAt_(IoMessage *self, IoObject *locals, int n);

IoTag *IoVorbisDspState_newTag(void *state);
IoVorbisDspState *IoVorbisDspState_proto(void *state);
IoVorbisDspState *IoVorbisDspState_rawClone(IoVorbisDspState *self);
void IoVorbisDspState_free(IoVorbisDspState *self);

IoObject *IoVorbisDspState_setup(IoVorbisDspState *self, IoObject *locals, IoMessage *m);
IoObject *IoVorbisDspState_headerin(IoVorbisDspState *self, IoObject *locals, IoMessage *m);
IoObject *IoVorbisDspState_blockin(IoVorbisDspState *self, IoObject *locals, IoMessage *m);
IoObject *IoVorbisDspState_pcmout(IoVorbisDspState *self, IoObject *locals, IoMessage *m);

#endif
