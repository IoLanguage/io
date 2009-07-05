//metadoc copyright Chris Double 2009

#ifndef IOTheoraDecodeContext_DEFINED
#define IOTheoraDecodeContext_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#define ISTHEORADECODECONTEXT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoTheoraDecodeContext_rawClone)

typedef IoObject IoTheoraDecodeContext;

IoObject *IoMessage_locals_theoraDecodeContextArgAt_(IoMessage *self, IoObject *locals, int n);

IoTag *IoTheoraDecodeContext_newTag(void *state);
IoTheoraDecodeContext *IoTheoraDecodeContext_proto(void *state);
IoTheoraDecodeContext *IoTheoraDecodeContext_rawClone(IoTheoraDecodeContext *self);
void IoTheoraDecodeContext_free(IoTheoraDecodeContext *self);

IoObject *IoTheoraDecodeContext_headerin(IoTheoraDecodeContext *self, IoObject *locals, IoMessage *m);

#endif
