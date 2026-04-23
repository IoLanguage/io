/*cmetadoc State description
Embedder-facing callback plumbing. The VM keeps function pointers for
print, exception, active-coroutine notification, exit, and a generic
bindings-init hook; embedders install their own via the setter pair
for each callback and optionally set a callbackContext that every
callback receives. When a callback is NULL the VM falls back to a
reasonable default (stdout print, backtrace on exception, no-op for
active coro). IoState_exit runs the exit callback then resumes the
main coroutine so control unwinds back to whichever host called
IoState_runCLI / IoState_doCString_.
*/

#include "IoState.h"

#include "IoObject.h"
#include <stdio.h>
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

/*cdoc State IoState_print_(self, format, ...)
printf-style wrapper that routes through whichever print callback the
embedder has installed. Builds the UArray with UArray_newWithVargs_
and hands it to IoState_justPrintba_.
*/
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

/*cdoc State IoState_justPrint_(self, s, size)
Raw byte-slice print: wraps the buffer in a non-owning UArray (copy=0)
so the VM does not take ownership, routes it through the print
callback, then frees the UArray header. Used by IoObject_print paths
that already own the bytes.
*/
void IoState_justPrint_(IoState *self, const unsigned char *s,
                        const size_t size) {
    UArray *ba = UArray_newWithData_type_size_copy_((uint8_t *)s, CTYPE_uint8_t,
                                                    size, 0);
    IoState_justPrintba_(self, ba);
    UArray_free(ba);
}

/*cdoc State IoState_justPrintba_(self, ba)
The actual print dispatcher. Calls the installed printCallback if one
is set (so embedders can capture Io output into a host UI) and
otherwise falls through to UArray_print which writes to stdout.
*/
void IoState_justPrintba_(IoState *self, UArray *ba) {
    if (self->printCallback) {
        self->printCallback(self->callbackContext, ba);
    } else {
        UArray_print(ba);
    }
}

/*cdoc State IoState_justPrintln_(self)
Emits a single newline through the print callback. Split out so
IoObject_print paths can end a line without building a full format
string.
*/
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

/*cdoc State IoState_exception_(self, coroutine)
Reports an uncaught exception living on coroutine. Delegates to the
embedder's exceptionCallback if one is installed — GUIs typically
route to an error panel — otherwise prints a backtrace via
IoCoroutine_rawPrintBackTrace. Called from IoState_tryToPerform when
the try coroutine finishes with an exception set.
*/
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

/*cdoc State IoState_exit(self, returnCode)
Orchestrates a clean VM shutdown: records the return code, flushes
stdout, invokes the embedder's exit callback, and then resumes the
main coroutine so execution unwinds back to whichever C frame invoked
the VM (typically main.c). Nothing after IoCoroutine_rawResume here
should run in the same coroutine.
*/
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

/*cdoc State IoState_schedulerUpdate(self, count)
Notifies the embedder how many coroutines are active. Used by UI
integrations to drive a progress indicator. Count is advisory; the
VM itself does not react to it.
*/
void IoState_schedulerUpdate(IoState *self, int count) {
    if (self->activeCoroCallback) {
        self->activeCoroCallback(self->callbackContext, count);
    }
}
