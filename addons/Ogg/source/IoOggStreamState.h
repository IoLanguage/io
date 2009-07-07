//metadoc copyright Chris Double 2009

#ifndef IOOggStreamState_DEFINED
#define IOOggStreamState_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include <ogg/ogg.h>

#define ISOGGSTREAMSTATE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoOggStreamState_rawClone)

typedef IoObject IoOggStreamState;

IoObject *IoMessage_locals_oggStreamStateArgAt_(IoMessage *self, IoObject *locals, int n);

IoTag *IoOggStreamState_newTag(void *state);
IoOggStreamState *IoOggStreamState_proto(void *state);
IoOggStreamState *IoOggStreamState_rawClone(IoOggStreamState *self);
void IoOggStreamState_free(IoOggStreamState *self);

IoObject *IoOggStreamState_setSerialNumber(IoOggStreamState *self, IoObject *locals, IoMessage *m);
IoObject *IoOggStreamState_clear(IoOggStreamState *self, IoObject *locals, IoMessage *m);
IoObject *IoOggStreamState_reset(IoOggStreamState *self, IoObject *locals, IoMessage *m);
IoObject *IoOggStreamState_eos(IoOggStreamState *self, IoObject *locals, IoMessage *m);
IoObject *IoOggStreamState_pagein(IoOggStreamState *self, IoObject *locals, IoMessage *m);
IoObject *IoOggStreamState_packetout(IoOggStreamState *self, IoObject *locals, IoMessage *m);
IoObject *IoOggStreamState_packetpeek(IoOggStreamState *self, IoObject *locals, IoMessage *m);

#endif
