
//metadoc User copyright Steve Dekorte 2002
//metadoc User license BSD revised

#ifndef IOUSER_DEFINED
#define IOUSER_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#define ISUSER(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoUser_rawClone)

typedef IoObject IoUser;

IoUser *IoUser_rawClone(IoUser *self);
IoUser *IoUser_proto(void *state);
IoUser *IoUser_new(void *state);

/* ----------------------------------------------------------- */
IoObject *IoUser_protoName(IoUser *self, IoObject *locals, IoMessage *m);
IoObject *IoUser_homeDirectory(IoUser *self, IoObject *locals, IoMessage *m);

#endif
