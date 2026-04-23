/*cmetadoc State description
Debugger and signal-handler glue. The "debugging" toggles rewrite every
registered proto's tag->performFunc between IoObject_perform (normal
path) and IoObject_performWithDebugger (hook path that notifies the
Io-side Debugger on every message send). The Ctrl-C handler writes a
single flag on the IoState rather than doing work in the signal context
— the eval loop polls state->receivedSignal between steps and calls
IoState_callUserInterruptHandler when it notices. Multi-state processes
can't disambiguate which VM the signal belonged to and currently exit.
*/

#include "IoState.h"
#include "IoObject.h"
#include "IoSystem.h"

/*cdoc State IoState_show(self)
Prints a placeholder dump of the state — most of the interesting
detail (per-color object groups) is commented out after the collector
rewrite. Kept as a hook for ad-hoc debugging.
*/
void IoState_show(IoState *self) {
    printf("--- state ----------------------------------\n");
    printf("State:\n");
    /*
    printf("black:\n");
    IoObjectGroup_show(self->blackGroup);
    printf("\n");

    printf("gray:\n");
    IoObjectGroup_show(self->grayGroup);
    printf("\n");

    printf("white:\n");
    IoObjectGroup_show(self->whiteGroup);
    printf("\n");
    */
    printf("stacks:\n");
    printf("\n");
}

/*cdoc State IoState_replacePerformFunc_with_(self, oldFunc, newFunc)
Walks every registered proto's tag and rewrites its performFunc
in place. Used by IoState_debuggingOn / IoState_debuggingOff to swap
the whole VM between the plain perform path and the debugger-hooking
one. Tags with a NULL performFunc are also updated so newly-registered
tags inherit the currently-active mode.
*/
IoObject *IoState_replacePerformFunc_with_(IoState *self,
                                           IoTagPerformFunc *oldFunc,
                                           IoTagPerformFunc *newFunc) {
    POINTERHASH_FOREACH(self->primitives, k, v, {
        IoObject *proto = v;
        IoTag *tag = IoObject_tag(proto);
        if (tag->performFunc == oldFunc || !tag->performFunc) {
            tag->performFunc = newFunc;
        }
    });

    return NULL;
}

/*cdoc State IoState_debuggingOn(self)
Switches every tag to IoObject_performWithDebugger so each message
send is observable by the Io-side Debugger proto. Irreversible without
a matching IoState_debuggingOff.
*/
void IoState_debuggingOn(IoState *self) {
    IoState_replacePerformFunc_with_(
        self, (IoTagPerformFunc *)IoObject_perform,
        (IoTagPerformFunc *)IoObject_performWithDebugger);
}

/*cdoc State IoState_debuggingOff(self)
Inverse of IoState_debuggingOn — restores the plain IoObject_perform
dispatch across all tags.
*/
void IoState_debuggingOff(IoState *self) {
    IoState_replacePerformFunc_with_(
        self, (IoTagPerformFunc *)IoObject_performWithDebugger,
        (IoTagPerformFunc *)IoObject_perform);
}

/*cdoc State IoState_hasDebuggingCoroutine(self)
Currently a constant-1 stub: the old multi-coroutine design tracked
a dedicated debugging coro, but the rewrite hasn't re-implemented it
yet, so IoState_updateDebuggingMode always enables the hook.
*/
int IoState_hasDebuggingCoroutine(IoState *self) {
    return 1; // hack awaiting decision on how to change this
}

/*cdoc State IoState_updateDebuggingMode(self)
Re-evaluates whether the debugger hook should be active and applies
the result. Given the constant-1 stub above this currently always
turns debugging on; kept as the right call site for when tracking is
re-added.
*/
void IoState_updateDebuggingMode(IoState *self) {
    if (IoState_hasDebuggingCoroutine(self)) {
        IoState_debuggingOn(self);
    } else {
        IoState_debuggingOff(self);
    }
}


static IoState *stateToReceiveControlC = NULL;
static int multipleIoStates = 0;

/*cdoc State IoState_UserInterruptHandler(sig)
Installed as the SIGINT handler when there is a single IoState in the
process. Sets state->receivedSignal = 1 on the target state so the
iterative eval loop can handle the interrupt safely between steps.
A second signal arriving before the first is drained is treated as
"C code is stuck" and calls exit directly. When multiple IoStates
coexist the function has no way to route the signal and aborts.
*/
void IoState_UserInterruptHandler(int sig) {
    printf("\nIOVM:\n");

    if (multipleIoStates) {
        // what could we do here to tell which IoState we are in?
        // send the interrupt to all of them? interactively let the user choose
        // one? interrupt the first stat created
        printf("	Received signal but since multiple Io states are in "
               "use\n");
        printf(
            "	we don't know which state to send the signal to. Exiting.\n");
        exit(0);
    } else {
        IoState *self = stateToReceiveControlC;

        if (self->receivedSignal) {
            printf("	Second signal received before first was handled. \n");
            printf("	Assuming control is stuck in a C call and isn't "
                   "returning\n");
            printf("	to Io so we're exiting without stack trace.\n\n");
            exit(-1);
        } else {
            self->receivedSignal = 1;

            /*
            IoObject *system = IoState_protoWithName_(self, "System");
            if (system)
            {
                    IoObject *handlerMethod = IoObject_rawGetSlot_(system,
            IOSYMBOL("interuptHandler")); if (handlerMethod)
                    {
                            return;
                    }
            }
            */

            printf("	Received signal. Setting interrupt flag.\n");
        }
    }
}

/*cdoc State IoState_callUserInterruptHandler(self)
Drains the pending signal flag and dispatches to the Io-side
System userInterruptHandler slot so user code can decide how to react.
Invoked from the eval loop's between-steps check, not from inside the
signal handler itself, so full VM allocation is safe here.
*/
void IoState_callUserInterruptHandler(IoState *self) {
    self->receivedSignal = 0;

    {
        IoObject *system = IoState_doCString_(self, "System");
        IoMessage *m =
            IoMessage_newWithName_(self, SIOSYMBOL("userInterruptHandler"));
        IoMessage_locals_performOn_(m, system, system);
    }
}

/*cdoc State IoState_setupUserInterruptHandler(self)
Records self as the Ctrl-C target, and flips the multiple-states flag
if another IoState has already registered. Under WASM there is no
signal API to wire up, so the function stops at bookkeeping — the
hook is plumbed by host platforms that provide signal().
*/
void IoState_setupUserInterruptHandler(IoState *self) {
    if (stateToReceiveControlC) {
        multipleIoStates = 1;
    }

    stateToReceiveControlC = self;
    /* WASM: no signal support */
}
