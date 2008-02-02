//metadoc Random copyright Steve Dekorte 2002
//metadoc Random license BSD revised

#ifndef IoRandom_DEFINED
#define IoRandom_DEFINED 1

#include "IoObject.h"
#include "IoNumber.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef IoObject IoRandom;

IoRandom *IoRandom_proto(void *state);
IoRandom *IoRandom_rawClone(IoRandom *self);
void IoRandom_free(IoMessage *self);

IoObject *IoRandom_flip(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoRandom_value(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoRandom_setSeed(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoRandom_gaussian(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoRandom_bytes(IoObject *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif
