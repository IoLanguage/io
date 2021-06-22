#include "IoState.h"

#include "IoObject.h"
#include "IoCoroutine.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoWeakLink.h"

void IoState_setBindingsInitCallback(IoState *self,
                                     IoStateBindingsInitCallback *callback) {
    self->bindingsInitCallback = callback;
}

// context

void IoState_callbackContext_(IoState *self, void *context) {
    self->callbackContext = context;
}

void *IoState_callbackContext(IoState *self) { return self->callbackContext; }

// print

void IoState_print_(IoState *self, const char *format, ...) {
    UArray *ba;
    va_list ap;
    va_start(ap, format);
    ba = UArray_newWithVargs_(format, ap);
    IoState_justPrintba_(self, ba);
    UArray_free(ba);
    va_end(ap);
}

void IoState_printCallback_(IoState *self, IoStatePrintCallback *callback) {
    self->printCallback = callback;
}

void IoState_justPrint_(IoState *self, const unsigned char *s,
                        const size_t size) {
    UArray *ba = UArray_newWithData_type_size_copy_((uint8_t *)s, CTYPE_uint8_t,
                                                    size, 0);
    IoState_justPrintba_(self, ba);
    UArray_free(ba);
}

void IoState_justPrintba_(IoState *self, UArray *ba) {
    if (self->printCallback) {
        self->printCallback(self->callbackContext, ba);
    } else {
        UArray_print(ba);
    }
}

void IoState_justPrintln_(IoState *self) {
    UArray *ba = UArray_newWithCString_("\n");
    IoState_justPrintba_(self, ba);
    UArray_free(ba);
}

// exception ---------------------------

void IoState_exceptionCallback_(IoState *self,
                                IoStateExceptionCallback *callback) {
    self->exceptionCallback = callback;
}

void IoState_exception_(IoState *self, IoObject *coroutine) {
    if (self->exceptionCallback) {
        self->exceptionCallback(self->callbackContext, coroutine);
    } else {
        IoCoroutine_rawPrintBackTrace(coroutine);
    }
}

// exit ---------------------------

void IoState_exitCallback_(IoState *self, IoStateExitCallback *callback) {
    self->exitCallback = callback;
}

void IoState_exit(IoState *self, int returnCode) {
    self->exitResult = returnCode;
    self->shouldExit = 1;
    fflush(stdout);

    if (self->exitCallback) {
        self->exitCallback(self->callbackContext, returnCode);
    }

    IoCoroutine_rawResume(
        self->mainCoroutine); // this will end up jumping back to main.c
}

// active coro ---------------------------

void IoState_activeCoroCallback_(IoState *self,
                                 IoStateActiveCoroCallback *callback) {
    self->activeCoroCallback = callback;
}

void IoState_schedulerUpdate(IoState *self, int count) {
    if (self->activeCoroCallback) {
        self->activeCoroCallback(self->callbackContext, count);
    }
}
