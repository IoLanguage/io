//metadoc copyright Chris Double 2009

#ifndef IOOggPacket_DEFINED
#define IOOggPacket_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include <ogg/ogg.h>

#define ISOGGPACKET(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoOggPacket_rawClone)

typedef IoObject IoOggPacket;

IoObject *IoMessage_locals_oggPacketArgAt_(IoMessage *self, IoObject *locals, int n);

IoTag *IoOggPacket_newTag(void *state);
IoOggPacket *IoOggPacket_proto(void *state);
IoOggPacket *IoOggPacket_rawClone(IoOggPacket *self);
void IoOggPacket_free(IoOggPacket *self);

#endif
