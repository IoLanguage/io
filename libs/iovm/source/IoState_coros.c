//--metadoc State copyright Steve Dekorte 2002
//--metadoc State license BSD revised

/*cmetadoc State description
Glue between IoState and the currently-running coroutine. Only three
responsibilities live here: track which IoCoroutine is "current",
swap the retain stack to match, and expose an IoState_yield entry
point that evaluates the cached yield message. Under the stackless
design coroutine switches do not swap C stacks — the switch is
expressed as a frame transition in IoState_iterative.c — but the
retain-stack pointer still has to move so each coroutine has its own
GC root set.
*/

#include "IoState.h"
#include "IoObject.h"

IoObject *IoState_currentCoroutine(IoState *self) {
    return self->currentCoroutine;
}

/*cdoc State IoState_setCurrentCoroutine_(self, coroutine)
Switches the VM's active coroutine: points state->currentCoroutine at
it, swaps state->currentIoStack to the coroutine's retain stack so
subsequent stackRetain calls protect that coroutine's locals, and
tells the collector which marker to use as a GC root before sweeping.
Must be called every time the iterative evaluator transitions from
one coroutine's frame chain to another's.
*/
void IoState_setCurrentCoroutine_(IoState *self, IoObject *coroutine) {
    self->currentCoroutine = coroutine;
    self->currentIoStack = IoCoroutine_rawIoStack(coroutine);
    Collector_setMarkBeforeSweepValue_(self->collector, coroutine);
}

/*cdoc State IoState_yield(self)
Sends the cached yield message to the lobby, which bottoms out in the
coroutine yield primitive that parks the current coroutine and resumes
another. Called from C hosts that want to cooperatively pump the Io
scheduler between native callbacks.
*/
void IoState_yield(IoState *self) {
    IoMessage_locals_performOn_(self->yieldMessage, self->lobby, self->lobby);
}
