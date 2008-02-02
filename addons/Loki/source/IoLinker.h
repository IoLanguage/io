
//metadoc Linker copyright Marc Fauconneau, 2006

#ifndef IOLINKER_DEFINED
#define IOLINKER_DEFINED 1

#include "IoObject.h"
#include "IoNumber.h"

#define ISLinker(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoLinker_rawClone)

typedef IoObject IoLinker;
IoLinker *IoLinker_proto(void *state);

/* ----------------------------------------------------------- */

IoObject *IoLinker_makeCFunction(IoLinker *self, IoObject *locals, IoMessage *m);
IoObject *IoLinker_bytesToHexSeq(IoLinker *self, IoObject *locals, IoMessage *m);
IoObject *IoLinker_hexSeqToBytes(IoLinker *self, IoObject *locals, IoMessage *m);

#endif
