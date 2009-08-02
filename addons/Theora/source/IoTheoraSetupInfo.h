//metadoc copyright Chris Double 2009

#ifndef IOTheoraSetupInfo_DEFINED
#define IOTheoraSetupInfo_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#define ISTHEORASETUPINFO(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoTheoraSetupInfo_rawClone)

typedef IoObject IoTheoraSetupInfo;

IoObject *IoMessage_locals_theoraSetupInfoArgAt_(IoMessage *self, IoObject *locals, int n);

IoTag *IoTheoraSetupInfo_newTag(void *state);
IoTheoraSetupInfo *IoTheoraSetupInfo_proto(void *state);
IoTheoraSetupInfo *IoTheoraSetupInfo_rawClone(IoTheoraSetupInfo *self);
void IoTheoraSetupInfo_free(IoTheoraSetupInfo *self);

#endif
