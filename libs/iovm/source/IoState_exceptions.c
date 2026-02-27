
//--metadoc State copyright Steve Dekorte 2002
//--metadoc State license BSD revised

#include "IoState.h"
#include "IoObject.h"
#include "IoCoroutine.h"
#include "IoSeq.h"
#include "IoEvalFrame.h"
#include <stdio.h>

//#define IOSTATE_SHOW_ERRORS 1

// Define DEBUG_CORO_EVAL to enable verbose debug output
// #define DEBUG_CORO_EVAL 1

void IoState_fatalError_(IoState *self, char *error) {
    fputs(error, stderr);
    fputs("\n", stderr);
    exit(-1);
}

void IoState_error_(IoState *self, IoMessage *m, const char *format, ...) {
    IoSymbol *description;

    va_list ap;
    va_start(ap, format);
    description = IoState_symbolWithUArray_copy_(
        self, UArray_newWithVargs_(format, ap), 0);
    va_end(ap);

#ifdef IOSTATE_SHOW_ERRORS
    fputs("\nIoState_error_: ", stderr);
    fputs(CSTRING(description), stderr);
    fputs("\n\n", stderr);
#endif

#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoState_error_: entering, msg=%s\n", CSTRING(description));
    // Print frame stack trace
    fprintf(stderr, "  Frame stack trace:\n");
    IoEvalFrame *frame = self->currentFrame;
    int frameNum = 0;
    while (frame && frameNum < 30) {
        if (frame->message) {
            IoSymbol *label = IoMessage_rawLabel(frame->message);
            int line = IoMessage_rawLineNumber(frame->message);
            IoSymbol *name = IoMessage_name(frame->message);
            const char *targetType = frame->target ? IoObject_name(frame->target) : "(null)";
            fprintf(stderr, "    [%d] %s at %s:%d (state=%d, target=%s)\n",
                    frameNum,
                    name ? CSTRING(name) : "(null)",
                    label ? CSTRING(label) : "(null)",
                    line,
                    frame->state,
                    targetType);
        } else {
            fprintf(stderr, "    [%d] (null message)\n", frameNum);
        }
        frame = frame->parent;
        frameNum++;
    }
    fflush(stderr);
#endif

    while (Collector_isPaused(self->collector)) {
        Collector_popPause(self->collector);
    }

    // Create exception on current coroutine (lightweight — no unwinding).
    // The eval loop handles frame unwinding when it sees errorRaised.
    // Note: Can't use IOSYMBOL() here — it expands IOSTATE via IoObject_tag(self)
    // but self is IoState*, not IoObject*.
    {
        IoCoroutine *coroutine = IoState_currentCoroutine(self);
        IoObject *e = IoObject_rawGetSlot_(coroutine,
            IoState_symbolWithCString_(self, "Exception"));
        if (e) {
            e = IOCLONE(e);
            IoObject_setSlot_to_(e,
                IoState_symbolWithCString_(self, "error"), description);
            if (m) IoObject_setSlot_to_(e,
                IoState_symbolWithCString_(self, "caughtMessage"), m);
            IoObject_setSlot_to_(e,
                IoState_symbolWithCString_(self, "coroutine"), coroutine);
            IoCoroutine_rawSetException_(coroutine, e);
        }
    }

    self->errorRaised = 1;

#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoState_error_: errorRaised=1\n");
    fflush(stderr);
#endif
    // IoState_error_ returns normally. Callers must check
    // state->errorRaised and return early to avoid continuing
    // with invalid state. The eval loop checks errorRaised after
    // each CFunction call and unwinds frames.
}
