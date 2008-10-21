//metadoc ReadLine copyright Jonaa Eschenburg, 2007
//metadoc ReadLine license BSD revised

#ifndef IoReadLine_DEFINED
#define IoReadLine_DEFINED 1

#include "Common.h"
#include "IoObject.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISREADLINE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoReadLine_rawClone)

typedef IoObject IoReadLine;

IoReadLine *IoReadLine_proto(void *state);
IoReadLine *IoReadLine_rawClone(IoReadLine *self);

IoObject *IoReadLine_readLine(IoReadLine *self, IoObject *locals, IoMessage *m);
IoObject *IoReadLine_addHistory(IoReadLine *self, IoObject *locals, IoMessage *m);
IoObject *IoReadLine_loadHistory(IoReadLine *self, IoObject *locals, IoMessage *m);
IoObject *IoReadLine_saveHistory(IoReadLine *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif
