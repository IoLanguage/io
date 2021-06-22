//--metadoc State copyright Steve Dekorte 2002
//--metadoc State license BSD revised

#include "IoState.h"
#include "IoObject.h"

IoObject *IoState_currentCoroutine(IoState *self) {
    return self->currentCoroutine;
}

void IoState_setCurrentCoroutine_(IoState *self, IoObject *coroutine) {
    self->currentCoroutine = coroutine;
    self->currentIoStack = IoCoroutine_rawIoStack(coroutine);
    Collector_setMarkBeforeSweepValue_(self->collector, coroutine);
}

void IoState_yield(IoState *self) {
    IoMessage_locals_performOn_(self->yieldMessage, self->lobby, self->lobby);
}
