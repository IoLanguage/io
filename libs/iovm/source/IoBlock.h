/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IOBLOCK_DEFINED
#define IOBLOCK_DEFINED 1

#include "Common.h"
#include "List.h"
#include "IoObject.h"
#include "IoState.h"
#include "IoCall.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISBLOCK(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoBlock_rawClone)

typedef IoObject IoBlock;

typedef struct
{
	IoMessage *message;
	List *argNames;
	IoObject *scope; // if 0x0, then use target as the locals proto
	int passStops;
} IoBlockData;

IoBlock *IoBlock_proto(void *state);
IoBlock *IoBlock_rawClone(IoBlock *self);
IoBlock *IoBlock_new(IoState *state);
void IoBlock_mark(IoBlock *self);
void IoBlock_free(IoBlock *self);
void IoBlock_message_(IoBlock *self, IoMessage *m);

void IoBlock_writeToStream_(IoBlock *self, BStream *stream);
void IoBlock_readFromStream_(IoBlock *self, BStream *stream);

// calling

IoObject *IoBlock_activate(IoBlock *self, IoObject *target, IoObject *locals, IoMessage *m, IoObject *slotContext);
IoObject *IoBlock_print(IoBlock *self, IoObject *locals, IoMessage *m);

UArray *IoBlock_justCode(IoBlock *self);
IoObject *IoBlock_code(IoBlock *self, IoObject *locals, IoMessage *m);
IoObject *IoBlock_code_(IoBlock *self, IoObject *locals, IoMessage *m);

IoObject *IoBlock_message(IoBlock *self, IoObject *locals, IoMessage *m);
IoObject *IoBlock_setMessage(IoBlock *self, IoObject *locals, IoMessage *m);

IoObject *IoBlock_argumentNames(IoBlock *self, IoObject *locals, IoMessage *m);
IoObject *IoBlock_argumentNames_(IoBlock *self, IoObject *locals, IoMessage *m);

IoObject *IoBlock_setScope_(IoBlock *self, IoObject *locals, IoMessage *m);
IoObject *IoBlock_scope(IoBlock *self, IoObject *locals, IoMessage *m);

IoObject *IoBlock_setPassStops_(IoBlock *self, IoObject *locals, IoMessage *m);
IoObject *IoBlock_passStops(IoBlock *self, IoObject *locals, IoMessage *m);

// lobby methods

IoObject *IoBlock_method(IoObject *target, IoObject *locals, IoMessage *m);
IoObject *IoObject_block(IoObject *target, IoObject *locals, IoMessage *m);

IoObject *IoBlock_performOn(IoBlock *self, IoObject *locals, IoMessage *m);
IoObject *IoBlock_call(IoBlock *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif
