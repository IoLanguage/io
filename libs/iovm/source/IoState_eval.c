
//--metadoc State copyright Steve Dekorte 2002
//--metadoc State license BSD revised

/*cmetadoc State description
Top-level entry points for evaluating Io source from C. Every "run
some code" path embedders expose eventually funnels through
IoState_tryToPerform, which spawns a fresh try-coroutine so uncaught
exceptions are captured instead of reaching the host. Sandbox-mode
state (message count limit, deadline endTime) is reset here before
the evaluation starts so the iterative evaluator can enforce the
limits per-run. This file is intentionally small — the actual step
machinery lives in IoState_iterative.c.
*/

#include "IoState.h"
#include "IoObject.h"
#include <time.h>
#include <stdio.h>
#include "PortableGettimeofday.h"

// Define DEBUG_CORO_EVAL to enable verbose debug output
// #define DEBUG_CORO_EVAL 1

/*cdoc State IoState_tryToPerform(self, target, locals, m)
Runs a message inside a freshly-spawned try coroutine so any raised
exception is caught rather than unwinding past the C caller. If the
try coroutine recorded an exception, re-reports it via
IoState_exception_ (which invokes the embedder's exception callback
or prints the backtrace) and returns the try coroutine's raw result.
The central safety net for every doCString / doFile entry point.
*/
IoObject *IoState_tryToPerform(IoState *self, IoObject *target,
                               IoObject *locals, IoMessage *m) {
    IoCoroutine *tryCoro = IoCoroutine_newWithTry(self, target, locals, m);

    IoObject *exc = IoCoroutine_rawException(tryCoro);

    if (exc != self->ioNil) {
        IoState_exception_(self, tryCoro);
    }

    IoObject *result = IoCoroutine_rawResult(tryCoro);
    return result;
}

/*cdoc State IoState_zeroSandboxCounts(self)
Clears the per-run sandbox accounting (messageCount and endTime) so
normal non-sandboxed evaluation is not accidentally limited. Called
at the top of IoState_on_doCString_withLabel_ since that path is the
default, unsandboxed evaluator.
*/
void IoState_zeroSandboxCounts(IoState *self) {
    self->messageCount = 0;
    self->endTime = 0;
}

/*cdoc State IoState_resetSandboxCounts(self)
Primes the sandbox counters for a single sandboxed run: records an
endTime deadline derived from state->timeLimit and resets messageCount
to messageCountLimit. The iterative evaluator checks both values on
each step and raises an error if either is exceeded.
*/
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

/*cdoc State IoState_on_doCString_withLabel_(self, target, s, label)
The canonical "evaluate a string of Io source" entry point. Builds a
doString message with the source and optional label as cached args,
wraps the call in a retain pool so intermediates released while the
iterative evaluator runs are not collected, and performs it via
IoState_tryToPerform so exceptions stay inside Io. The label drives
error-message source locations.
*/
IoObject *IoState_on_doCString_withLabel_(IoState *self, IoObject *target,
                                          const char *s, const char *label) {
#ifdef DEBUG_CORO_EVAL
    static int callDepth = 0;
    Stack *ioStackBefore;
    Stack *ioStackAfter;
    callDepth++;
    fprintf(stderr, ">>> on_doCString ENTER (depth=%d, label=%s)\n", callDepth, label ? label : "NULL");
    fflush(stderr);
    ioStackBefore = self->currentIoStack;
    fprintf(stderr, "on_doCString: ioStack before push = %p\n", (void*)ioStackBefore);
    fflush(stderr);
#endif

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

#ifdef DEBUG_CORO_EVAL
    ioStackAfter = self->currentIoStack;
    fprintf(stderr, "on_doCString: ioStack after tryToPerform = %p (was %p)\n",
            (void*)ioStackAfter, (void*)ioStackBefore);
    fflush(stderr);

    if (ioStackBefore != ioStackAfter) {
        fprintf(stderr, "WARNING: ioStack changed during tryToPerform!\n");
        fflush(stderr);
    }
#endif

    IoState_popRetainPoolExceptFor_(self, result);

#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "<<< on_doCString EXIT (depth=%d, result=%p)\n", callDepth, (void*)result);
    callDepth--;
    fflush(stderr);
#endif

    return result;
}

/*cdoc State IoState_doCString_(self, s)
Convenience shortcut: evaluate a C string against the lobby with a
fixed label. Equivalent to IoState_on_doCString_withLabel_(self,
lobby, s, "IoState_doCString").
*/
IoObject *IoState_doCString_(IoState *self, const char *s) {
    return IoState_on_doCString_withLabel_(self, self->lobby, s,
                                           "IoState_doCString");
}

/*cdoc State IoState_doSandboxCString_(self, s)
Runs a snippet under sandbox limits: resets the message-count and
deadline counters before dispatching through IoState_tryToPerform.
Used by the Sandbox proto to execute untrusted code with bounded
time and work.
*/
IoObject *IoState_doSandboxCString_(IoState *self, const char *s) {
    IoMessage *m = IoMessage_newWithName_andCachedArg_(
        self, SIOSYMBOL("doString"), SIOSYMBOL(s));
    IoState_resetSandboxCounts(self);
    return IoState_tryToPerform(self, self->lobby, self->lobby, m);
}

double IoState_endTime(IoState *self) { return self->endTime; }

/*cdoc State IoState_doFile_(self, path)
Asks the lobby to perform "doFile(path)" so file loading goes through
the Io-side implementation (which handles search paths, File proto,
and imports) rather than duplicating that logic in C.
*/
IoObject *IoState_doFile_(IoState *self, const char *path) {
    IoMessage *m = IoMessage_newWithName_andCachedArg_(
        self, SIOSYMBOL("doFile"), SIOSYMBOL(path));
    return IoState_tryToPerform(self, self->lobby, self->lobby, m);
}
