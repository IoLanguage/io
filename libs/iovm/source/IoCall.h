/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IoCall_DEFINED
#define IoCall_DEFINED 1

#include "Common.h"
#include "IoState.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISACTIVATIONCONTEXT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCall_rawClone)

typedef IoObject IoCall;

typedef struct
{
	IoObject *sender;
	IoObject *message;
	IoObject *target;
	IoObject *slotContext;
	IoObject *activated;
	IoObject *coroutine;
        int stopStatus;
} IoCallData;

IoCall *IoCall_with(void *state, 
									 IoObject *sender,
									 IoObject *target,
									 IoObject *message,
									 IoObject *slotContext,
									 IoObject *activated,
									 IoObject *coroutine);

IoCall *IoCall_proto(void *state);
IoCall *IoCall_rawClone(IoCall *self);
IoCall *IoCall_new(IoState *state);

void IoCall_mark(IoCall *self);
void IoCall_free(IoCall *self);

void IoCall_writeToStream_(IoCall *self, BStream *stream);
void IoCall_readFromStream_(IoCall *self, BStream *stream);

IoObject *IoCall_sender(IoCall *self, IoObject *locals, IoMessage *m);
IoObject *IoCall_message(IoCall *self, IoObject *locals, IoMessage *m);
IoObject *IoCall_target(IoCall *self, IoObject *locals, IoMessage *m);
IoObject *IoCall_slotContext(IoCall *self, IoObject *locals, IoMessage *m);
IoObject *IoCall_activated(IoCall *self, IoObject *locals, IoMessage *m);
IoObject *IoCall_coroutine(IoCall *self, IoObject *locals, IoMessage *m);
IoObject *IoCall_evalArgAt(IoCall *self, IoObject *locals, IoMessage *m);
IoObject *IoCall_argAt(IoCall *self, IoObject *locals, IoMessage *m);

int IoCall_rawStopStatus(IoCall *self);
IoObject *IoCall_stopStatus(IoCall *self, IoObject *locals, IoMessage *m);
IoObject *IoCall_setStopStatus(IoCall *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif
