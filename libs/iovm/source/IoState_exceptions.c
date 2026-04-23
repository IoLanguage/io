
//--metadoc State copyright Steve Dekorte 2002
//--metadoc State license BSD revised

/*cmetadoc State description
Error-raising helpers for the iterative evaluator. IoState_error_ is
the workhorse: it formats the message, builds an Exception on the
current coroutine, and sets state->errorRaised so the eval loop
unwinds on its next step. Crucially this function does NOT longjmp
or abort — the stackless rewrite relies on normal returns, so every
CFunction caller must check state->errorRaised after this helper and
bail out explicitly. IoState_fatalError_ is reserved for non-recoverable
bootstrap failures where no Io-visible error path exists yet.
*/

#include "IoState.h"
#include "IoObject.h"
#include "IoCoroutine.h"
#include "IoSeq.h"
#include "IoEvalFrame.h"
#include <stdio.h>

//#define IOSTATE_SHOW_ERRORS 1

// Define DEBUG_CORO_EVAL to enable verbose debug output
// #define DEBUG_CORO_EVAL 1

/*cdoc State IoState_fatalError_(self, error)
Prints a message to stderr and terminates the process. Used only from
bootstrap paths (missing proto, broken init ordering) where there is
no running coroutine for an exception to land on.
*/
void IoState_fatalError_(IoState *self, char *error) {
    fputs(error, stderr);
    fputs("\n", stderr);
    exit(-1);
}

/*cdoc State IoState_error_(self, m, format, ...)
Raises an Io-level exception without longjmp. Formats the description
with printf-style vargs, clones the current coroutine's Exception proto,
and stores description / caughtMessage / coroutine slots on it. Any
paused collector is resumed first so exception allocation is not
deferred. Sets state->errorRaised to 1; the caller MUST return early
and the enclosing iterative eval loop will unwind frames on the next
step. If m is NULL (C-side error with no Io message context) the
caughtMessage slot is omitted.
*/
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
    IoEvalFrameData *fd = FRAME_DATA(frame);
    int frameNum = 0;
    while (frame && frameNum < 30) {
        fd = FRAME_DATA(frame);
        if (fd->message) {
            IoSymbol *label = IoMessage_rawLabel(fd->message);
            int line = IoMessage_rawLineNumber(fd->message);
            IoSymbol *name = IoMessage_name(fd->message);
            const char *targetType = fd->target ? IoObject_name(fd->target) : "(null)";
            fprintf(stderr, "    [%d] %s at %s:%d (state=%d, target=%s)\n",
                    frameNum,
                    name ? CSTRING(name) : "(null)",
                    label ? CSTRING(label) : "(null)",
                    line,
                    fd->state,
                    targetType);
        } else {
            fprintf(stderr, "    [%d] (null message)\n", frameNum);
        }
        frame = fd->parent;
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
