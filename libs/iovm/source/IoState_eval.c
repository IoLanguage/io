
//--metadoc State copyright Steve Dekorte 2002
//--metadoc State license BSD revised

#include "IoState.h"
#include "IoObject.h"
#include <time.h>
#include "PortableGettimeofday.h"

IoObject *IoState_tryToPerform(IoState *self, IoObject *target,
                               IoObject *locals, IoMessage *m) {
    IoCoroutine *tryCoro = IoCoroutine_newWithTry(self, target, locals, m);

    if (IoCoroutine_rawException(tryCoro) != self->ioNil) {
        IoState_exception_(self, tryCoro);
    }

    return IoCoroutine_rawResult(tryCoro);
}

void IoState_zeroSandboxCounts(IoState *self) {
    self->messageCount = 0;
    self->endTime = 0;
}

void IoState_resetSandboxCounts(IoState *self) {
    struct timeval startTv;
    double start;

    // Get the start and current time
    gettimeofday(&startTv, NULL);
    start = (double)startTv.tv_sec + ((double)startTv.tv_usec / 1000000.0);

    // Calculate the end of time
    self->endTime = start + self->timeLimit;

    self->messageCount = self->messageCountLimit;
}

IoObject *IoState_on_doCString_withLabel_(IoState *self, IoObject *target,
                                          const char *s, const char *label) {
    IoObject *result;

    IoState_pushRetainPool(self);

    {
        IoMessage *m = IoMessage_newWithName_andCachedArg_(
            self, SIOSYMBOL("doString"), SIOSYMBOL(s));

        if (label) {
            IoMessage_addCachedArg_(m, SIOSYMBOL(label));
        }

        IoState_zeroSandboxCounts(self);

        result = IoState_tryToPerform(self, target, target, m);
    }

    IoState_popRetainPoolExceptFor_(self, result);

    return result;
}

IoObject *IoState_doCString_(IoState *self, const char *s) {
    return IoState_on_doCString_withLabel_(self, self->lobby, s,
                                           "IoState_doCString");
}

IoObject *IoState_doSandboxCString_(IoState *self, const char *s) {
    IoMessage *m = IoMessage_newWithName_andCachedArg_(
        self, SIOSYMBOL("doString"), SIOSYMBOL(s));
    IoState_resetSandboxCounts(self);
    return IoState_tryToPerform(self, self->lobby, self->lobby, m);
}

double IoState_endTime(IoState *self) { return self->endTime; }

IoObject *IoState_doFile_(IoState *self, const char *path) {
    IoMessage *m = IoMessage_newWithName_andCachedArg_(
        self, SIOSYMBOL("doFile"), SIOSYMBOL(path));
    return IoState_tryToPerform(self, self->lobby, self->lobby, m);
}
