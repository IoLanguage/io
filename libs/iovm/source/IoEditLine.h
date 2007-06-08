/*#io
docCopyright("Jonathan Wright", 2007)
docLicense("BSD revised")
*/

#ifndef IoEditLine_DEFINED
#define IoEditLine_DEFINED 1

#include "Common.h"
#include "IoObject.h"

#ifdef IO_HAS_EDITLIB
#include <histedit.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ISEDITLINE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoEditLine_rawClone)

typedef IoObject IoEditLine;

#ifdef IO_HAS_EDITLIB
typedef struct
{
	EditLine *editline;
	History *history;
	IoSymbol *prompt;
} IoEditLineData;
#endif

IoEditLine *IoEditLine_proto(void *state);
IoEditLine *IoEditLine_rawClone(IoEditLine *self);

void IoEditLine_free(IoEditLine *self);
void IoEditLine_mark(IoEditLine *self);

IoObject *IoEditLine_hasEditLib(IoEditLine *self, IoObject *locals, IoMessage *m);

#ifdef IO_HAS_EDITLIB
IoObject *IoEditLine_readLine(IoEditLine *self, IoObject *locals, IoMessage *m);
IoObject *IoEditLine_addHistory(IoEditLine *self, IoObject *locals, IoMessage *m);
#endif

#ifdef __cplusplus
}
#endif
#endif
