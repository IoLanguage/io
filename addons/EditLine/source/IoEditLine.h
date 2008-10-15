//metadoc EditLine copyright Jonathan Wright, 2007
//metadoc EditLine license BSD revised

#ifndef IoEditLine_DEFINED
#define IoEditLine_DEFINED 1

#include "Common.h"
#include "IoObject.h"

#include <histedit.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ISEDITLINE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoEditLine_rawClone)

typedef IoObject IoEditLine;

typedef struct
{
	EditLine *editline;
	History *history;
	IoSymbol *prompt;
} IoEditLineData;

IoEditLine *IoEditLine_proto(void *state);
IoEditLine *IoEditLine_rawClone(IoEditLine *self);

void IoEditLine_free(IoEditLine *self);
void IoEditLine_mark(IoEditLine *self);

IoObject *IoEditLine_hasEditLib(IoEditLine *self, IoObject *locals, IoMessage *m);

IoObject *IoEditLine_readLine(IoEditLine *self, IoObject *locals, IoMessage *m);
IoObject *IoEditLine_addHistory(IoEditLine *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif
