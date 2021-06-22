
// metadoc Call copyright Steve Dekorte 2002
// metadoc Call license BSD revised

#ifndef IoCall_DEFINED
#define IoCall_DEFINED 1

#include "Common.h"
#include "IoState.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISACTIVATIONCONTEXT(self)                                              \
    IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCall_rawClone)

typedef IoObject IoCall;

typedef struct {
    IoObject *sender;
    IoObject *message;
    IoObject *target;
    IoObject *slotContext;
    IoObject *activated;
    IoObject *coroutine;
    int stopStatus;
} IoCallData;

IoCall *IoCall_with(void *state, IoObject *sender, IoObject *target,
                    IoObject *message, IoObject *slotContext,
                    IoObject *activated, IoObject *coroutine);

IoCall *IoCall_proto(void *state);
IoCall *IoCall_rawClone(IoCall *self);
IoCall *IoCall_new(IoState *state);

void IoCall_mark(IoCall *self);
void IoCall_free(IoCall *self);

IO_METHOD(IoCall, sender);
IO_METHOD(IoCall, message);
IO_METHOD(IoCall, target);
IO_METHOD(IoCall, slotContext);
IO_METHOD(IoCall, activated);
IO_METHOD(IoCall, coroutine);
IO_METHOD(IoCall, evalArgAt);
IO_METHOD(IoCall, argAt);

int IoCall_rawStopStatus(IoCall *self);
IO_METHOD(IoCall, stopStatus);
IO_METHOD(IoCall, setStopStatus);

#ifdef __cplusplus
}
#endif
#endif
