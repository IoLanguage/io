
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
#include "IoEvalFrame.h"
#include "IoCoroutine.h"
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
static IoObject *IoState_tryToPerform_exception_(IoState *self,
                                                 IoObject *target,
                                                 IoObject *locals,
                                                 IoMessage *m,
                                                 IoObject **exception) {
    IoCoroutine *tryCoro = IoCoroutine_newWithTry(self, target, locals, m);

    IoObject *exc = IoCoroutine_rawException(tryCoro);

    if (exc != self->ioNil) {
        IoState_exception_(self, tryCoro);
    }

    if (exception) *exception = exc;

    IoObject *result = IoCoroutine_rawResult(tryCoro);
    return result;
}

IoObject *IoState_tryToPerform(IoState *self, IoObject *target,
                               IoObject *locals, IoMessage *m) {
    return IoState_tryToPerform_exception_(self, target, locals, m, NULL);
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
    return IoState_on_doCString_withLabel_exception_(self, target, s, label,
                                                     NULL);
}

/*cdoc State IoState_on_doCString_withLabel_exception_(self, target, s, label, exception)
IoState_on_doCString_withLabel_, and when exception is not NULL, sets
*exception to the exception the code raised and did not catch, or to
nil. The code runs in its own try coroutine, so the exception is not
left on the caller's coroutine; this is how a caller can learn of it.
*/
IoObject *IoState_on_doCString_withLabel_exception_(IoState *self,
                                                    IoObject *target,
                                                    const char *s,
                                                    const char *label,
                                                    IoObject **exception) {
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

        result = IoState_tryToPerform_exception_(self, target, target, m,
                                                 exception);
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

static double IoState_secondsNow(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + ((double)tv.tv_usec / 1000000.0);
}

/*cdoc State IoState_sandboxPoll(self)
The host poll callback (see hostPollCallback in IoState.h) installed
for a sandboxed run. Counts evaluator steps off messageCount and
compares the clock with endTime. Once either limit is passed it raises
an error, and raises it again at every following step, so the code
being run can't carry on by catching it; the run ends when its last
frame has unwound.
*/
static void IoState_sandboxPoll(IoState *self) {
    if (!self->sandboxExceeded) {
        if (self->messageCountLimit > 0) {
            self->messageCount -= self->sandboxPollSpan;
            if (self->messageCount <= 0) {
                self->sandboxExceeded = IOSTATE_SANDBOX_MESSAGES;
            } else if (self->messageCount < IOSTATE_HOST_POLL_INTERVAL) {
                self->hostPollCountdown = self->messageCount;
            }
            self->sandboxPollSpan = self->hostPollCountdown;
        }
        if (!self->sandboxExceeded && self->timeLimit > 0 &&
            IoState_secondsNow() > self->endTime) {
            self->sandboxExceeded = IOSTATE_SANDBOX_TIME;
        }
        if (!self->sandboxExceeded) return;
    }

    self->hostPollCountdown = 1;
    IoState_error_(self,
        self->currentFrame ? FRAME_DATA(self->currentFrame)->message : NULL,
        self->sandboxExceeded == IOSTATE_SANDBOX_TIME
            ? "Sandbox: time limit exceeded"
            : "Sandbox: message count limit exceeded");
}

/*cdoc State IoState_doSandboxCString_(self, s)
Runs a snippet under the sandbox limits. With a time limit or a message
count limit set, IoState_sandboxPoll is installed for the run and ends
it when either is passed; self->sandboxExceeded then says which, and
the exception is not reported here (the caller reports it). Used by the
Sandbox proto to execute untrusted code with bounded time and work.
*/
IoObject *IoState_doSandboxCString_(IoState *self, const char *s) {
    IoMessage *m = IoMessage_newWithName_andCachedArg_(
        self, SIOSYMBOL("doString"), SIOSYMBOL(s));
    void (*savedPoll)(IoState *) = self->hostPollCallback;

    IoState_resetSandboxCounts(self);
    self->sandboxExceeded = 0;
    if (self->timeLimit > 0 || self->messageCountLimit > 0) {
        self->hostPollCallback = IoState_sandboxPoll;
        self->hostPollCountdown = IOSTATE_HOST_POLL_INTERVAL;
        if (self->messageCountLimit > 0 &&
            self->messageCountLimit < IOSTATE_HOST_POLL_INTERVAL) {
            self->hostPollCountdown = self->messageCountLimit;
        }
        self->sandboxPollSpan = self->hostPollCountdown;
    }

    IoCoroutine *tryCoro =
        IoCoroutine_newWithTry(self, self->lobby, self->lobby, m);

    // Off before anything else runs here, such as the backtrace printer
    self->hostPollCallback = savedPoll;

    if (!self->sandboxExceeded &&
        IoCoroutine_rawException(tryCoro) != self->ioNil) {
        IoState_exception_(self, tryCoro);
    }
    self->errorRaised = 0;

    return IoCoroutine_rawResult(tryCoro);
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
