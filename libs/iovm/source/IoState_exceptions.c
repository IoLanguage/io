
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
#include "IoList.h"
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

// Longest stack trace recorded on an exception, in entries.
#define IO_STACK_TRACE_MAX 64

/*cdoc State IoState_recordStackTraceOn_(self, exception)
Records where an exception was raised, as a List in its stackTrace slot.
The eval loop unwinds the frames before anything can print them, so the
trace has to be taken here, while the frame chain is intact.

The list is flat, two items per entry, innermost first: the message being
evaluated and the type name of its target. The entries are the frame that
raised, then each frame waiting on a method or block it called. A frame
reused by a tail call has only its latest call, so tail calls leave no
entry for their caller. An exception that already has a trace keeps it,
so re-raising with pass still shows where it first came from.
*/
void IoState_recordStackTraceOn_(IoState *self, IoObject *exception) {
    IoSymbol *slotName = IoState_symbolWithCString_(self, "stackTrace");
    IoSymbol *typeSlot = self->typeSymbol;
    IoObject *existing;
    IoList *trace;
    IoEvalFrame *frame = self->currentFrame;
    int isTop = 1;
    int count = 0;

    if (!exception) return;
    existing = IoObject_rawGetSlot_(exception, slotName);
    if (existing && ISLIST(existing)) return;

    trace = IoList_new(self);
    IoObject_setSlot_to_(exception, slotName, trace);

    while (frame && count < IO_STACK_TRACE_MAX) {
        IoEvalFrameData *fd = FRAME_DATA(frame);
        if (fd->message && (isTop || fd->state == FRAME_STATE_ACTIVATE)) {
            // The target's type slot when it is a string (no new
            // symbol needed), else its tag name, as IoObject_name does.
            IoObject *typeName =
                fd->target ? IoObject_rawGetSlot_(fd->target, typeSlot) : NULL;
            if (!typeName || !ISSEQ(typeName)) {
                typeName = IoState_symbolWithCString_(
                    self, fd->target ? IoTag_name(IoObject_tag(fd->target))
                                     : "?");
            }
            IoList_rawAppend_(trace, fd->message);
            IoList_rawAppend_(trace, typeName);
            count++;
        }
        isTop = 0;
        frame = fd->parent;
    }
}

/*cdoc State IoState_error_(self, m, format, ...)
Raises an Io-level exception without longjmp. Formats the description
with printf-style vargs, clones the current coroutine's Exception proto,
and stores description / caughtMessage / coroutine slots on it. Collector
pauses are left to the code that pushed them. Sets state->errorRaised to 1; the caller MUST return early
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

    // Collector pauses are left alone. When errors longjmp'd, this popped
    // them all because the code that pushed them never got to pop; now the
    // caller returns normally and pops its own, so popping here made the
    // count go negative (an assert, or with NDEBUG a collector that never
    // runs again).

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
            IoState_recordStackTraceOn_(self, e);
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
