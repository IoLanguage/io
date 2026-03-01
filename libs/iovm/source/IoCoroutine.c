
// metadoc Coroutine category Core
// metadoc Coroutine copyright Steve Dekorte 2002, 2025
// metadoc Coroutine license BSD revised
/*metadoc Coroutine description
Object wrapper for an Io coroutine.
Now implemented using frame-based evaluation (no platform-specific assembly).
*/

#include "IoCoroutine.h"
#include "IoObject.h"
#include "IoState.h"
#include "IoCFunction.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoBlock.h"
#include "IoEvalFrame.h"
#include "IoContinuation.h"
#include <execinfo.h>

//#define DEBUG

// Define DEBUG_CORO_EVAL to enable verbose debug output
// #define DEBUG_CORO_EVAL 1

static const char *protoId = "Coroutine";

#define DATA(self) ((IoCoroutineData *)IoObject_dataPointer(self))

IoCoroutine *IoMessage_locals_coroutineArgAt_(IoMessage *self, void *locals,
                                              int n) {
    IoObject *v = IoMessage_locals_valueArgAt_(self, (IoObject *)locals, n);
    if (!ISCOROUTINE(v))
        IoMessage_locals_numberArgAt_errorForType_(self, (IoObject *)locals, n,
                                                   "Coroutine");
    return v;
}

IoTag *IoCoroutine_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCoroutine_free);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCoroutine_rawClone);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCoroutine_mark);
    return tag;
}

IoCoroutine *IoCoroutine_proto(void *state) {
    IoObject *self = IoObject_new(state);

    IoObject_tag_(self, IoCoroutine_newTag(state));
    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoCoroutineData)));
    DATA(self)->ioStack = Stack_new();
#ifdef STACK_POP_CALLBACK
    Stack_popCallback_(DATA(self)->ioStack, IoObject_freeIfUnreferenced);
#endif
    IoState_registerProtoWithId_((IoState *)state, self, protoId);

    // Main coroutine: frameStack is NULL (its frames live in state->currentFrame)
    DATA(self)->frameStack = NULL;
    DATA(self)->stopStatus = MESSAGE_STOP_STATUS_NORMAL;
    DATA(self)->returnValue = NULL;

    return self;
}

void IoCoroutine_protoFinish(IoCoroutine *self) {
    IoMethodTable methodTable[] = {
        {"ioStack", IoCoroutine_ioStack},
        {"run", IoCoroutine_run},
        {"main", IoCoroutine_main},
        {"resume", IoCoroutine_resume},
        {"isCurrent", IoCoroutine_isCurrent},
        {"currentCoroutine", IoCoroutine_currentCoroutine},
        {"implementation", IoCoroutine_implementation},
        {"setMessageDebugging", IoCoroutine_setMessageDebugging},
        {"freeStack", IoCoroutine_freeStack},
        {"setRecentInChain", IoCoroutine_setRecentInChain},
        {"rawSignalException", IoCoroutine_rawSignalException},
        {"currentFrame", IoCoroutine_currentFrame},
        {NULL, NULL},
    };

    IoObject_addMethodTable_(self, methodTable);
}

IoCoroutine *IoCoroutine_rawClone(IoCoroutine *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoCoroutineData)));
    DATA(self)->ioStack = Stack_new();
#ifdef STACK_POP_CALLBACK
    Stack_popCallback_(DATA(self)->ioStack, IoObject_freeIfUnreferenced);
#endif
    DATA(self)->frameStack = NULL;
    DATA(self)->stopStatus = MESSAGE_STOP_STATUS_NORMAL;
    DATA(self)->returnValue = NULL;
    return self;
}

IoCoroutine *IoCoroutine_new(void *state) {
    IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
    IoObject *self = IOCLONE(proto);
    return self;
}

void IoCoroutine_free(IoCoroutine *self) {
    // Frame stack is GC-managed, just null the pointer
    DATA(self)->frameStack = NULL;
    Stack_free(DATA(self)->ioStack);
    io_free(DATA(self));
}

void IoCoroutine_mark(IoCoroutine *self) {
    IoState *state = IOSTATE;

    // Mark the ioStack (retain stack)
    Stack_do_(DATA(self)->ioStack, (ListDoCallback *)IoObject_shouldMark);

    // Mark the frame stack.
    // Just mark the top frame — GC follows the parent chain
    // via IoEvalFrame's own markFunc.
    IoEvalFrame *frame = DATA(self)->frameStack;
    if (self == state->currentCoroutine && state->currentFrame) {
        frame = state->currentFrame;
    }
    IoObject_shouldMarkIfNonNull(frame);

    // Mark pooled frames so GC doesn't collect them while parked.
    // Only do this when marking the current coroutine (avoids redundant work).
    if (self == state->currentCoroutine) {
        for (int i = 0; i < state->framePoolCount; i++) {
            IoObject_shouldMarkIfNonNull(state->framePool[i]);
        }
        // Mark pooled blockLocals (they have PHash slots already allocated)
        for (int i = 0; i < state->blockLocalsPoolSize; i++) {
            IoObject_shouldMarkIfNonNull(state->blockLocalsPool[i]);
        }
    }

}

// raw

Stack *IoCoroutine_rawIoStack(IoCoroutine *self) { return DATA(self)->ioStack; }

void IoCoroutine_rawShow(IoCoroutine *self) {
    Stack_do_(DATA(self)->ioStack, (StackDoCallback *)IoObject_show);
    printf("\n");
}

IoEvalFrame *IoCoroutine_rawFrameStack(IoCoroutine *self) {
    return DATA(self)->frameStack;
}

// Save coroutine state from IoState
void IoCoroutine_saveState_(IoCoroutine *self, IoState *state) {
    DATA(self)->frameStack = state->currentFrame;
    DATA(self)->stopStatus = state->stopStatus;
    DATA(self)->returnValue = state->returnValue;
    // ioStack is already per-coroutine
}

// Restore coroutine state to IoState
void IoCoroutine_restoreState_(IoCoroutine *self, IoState *state) {
    state->currentFrame = DATA(self)->frameStack;
    state->stopStatus = DATA(self)->stopStatus;
    state->returnValue = DATA(self)->returnValue;
    state->currentIoStack = DATA(self)->ioStack;

    // Recalculate frame depth
    state->frameDepth = 0;
    IoEvalFrame *f = state->currentFrame;
    while (f) {
        state->frameDepth++;
        f = FRAME_DATA(f)->parent;
    }
}

/*
// runTarget

void IoCoroutine_rawSetRunTarget_(IoCoroutine *self, IoObject *v)
{
        IoObject_setSlot_to_(self, IOSTATE->runTargetSymbol, v);
}

IoObject *IoCoroutine_rawRunTarget(IoCoroutine *self)
{
        return IoObject_rawGetSlot_(self, IOSTATE->runTargetSymbol);
}

// runMessage

void IoCoroutine_rawSetRunMessage_(IoCoroutine *self, IoObject *v)
{
        IoObject_setSlot_to_(self, IOSTATE->runMessageSymbol, v);
}

IoObject *IoCoroutine_rawRunMessage(IoCoroutine *self)
{
        return IoObject_rawGetSlot_(self, IOSTATE->runMessageSymbol);
}

// runLocals

void IoCoroutine_rawSetRunLocals_(IoCoroutine *self, IoObject *v)
{
        IoObject_setSlot_to_(self, IOSTATE->runLocalsSymbol, v);
}

IoObject *IoCoroutine_rawRunLocals(IoCoroutine *self)
{
        return IoObject_rawGetSlot_(self, IOSTATE->runLocalsSymbol);
}

// parent

void IoCoroutine_rawSetParentCoroutine_(IoCoroutine *self, IoObject *v)
{
        IoObject_setSlot_to_(self, IOSTATE->parentCoroutineSymbol, v);
}

IoObject *IoCoroutine_rawParentCoroutine(IoCoroutine *self)
{
        return IoObject_getSlot_(self, IOSTATE->parentCoroutineSymbol);
}

// result

void IoCoroutine_rawSetResult_(IoCoroutine *self, IoObject *v)
{
        IoObject_setSlot_to_(self, IOSTATE->resultSymbol, v);
}

IoObject *IoCoroutine_rawResult(IoCoroutine *self)
{
        return IoObject_getSlot_(self, IOSTATE->resultSymbol);
}

// exception

void IoCoroutine_rawRemoveException(IoCoroutine *self)
{
        IoObject_removeSlot_(self, IOSTATE->exceptionSymbol);
}

void IoCoroutine_rawSetException_(IoCoroutine *self, IoObject *v)
{
        IoObject_setSlot_to_(self, IOSTATE->exceptionSymbol, v);
}

IoObject *IoCoroutine_rawException(IoCoroutine *self)
{
        return IoObject_getSlot_(self, IOSTATE->exceptionSymbol);
}
*/

void IoCoroutine_rawSetRunTarget_(IoCoroutine *self, IoObject *v) {
    IoObject_setSlot_to_(self, IOSYMBOL("runTarget"), v);
}

IoObject *IoCoroutine_rawRunTarget(IoCoroutine *self) {
    return IoObject_rawGetSlot_(self, IOSYMBOL("runTarget"));
}

// runMessage

void IoCoroutine_rawSetRunMessage_(IoCoroutine *self, IoObject *v) {
    IoObject_setSlot_to_(self, IOSYMBOL("runMessage"), v);
}

IoObject *IoCoroutine_rawRunMessage(IoCoroutine *self) {
    return IoObject_rawGetSlot_(self, IOSYMBOL("runMessage"));
}

// runLocals

void IoCoroutine_rawSetRunLocals_(IoCoroutine *self, IoObject *v) {
    IoObject_setSlot_to_(self, IOSYMBOL("runLocals"), v);
}

IoObject *IoCoroutine_rawRunLocals(IoCoroutine *self) {
    return IoObject_rawGetSlot_(self, IOSYMBOL("runLocals"));
}

// parent

void IoCoroutine_rawSetParentCoroutine_(IoCoroutine *self, IoObject *v) {
    IoObject_setSlot_to_(self, IOSYMBOL("parentCoroutine"), v);
}

IoObject *IoCoroutine_rawParentCoroutine(IoCoroutine *self) {
    return IoObject_getSlot_(self, IOSYMBOL("parentCoroutine"));
}

// recentInChain

void IoCoroutine_rawSetRecentInChain_(IoCoroutine *self, IoObject *v) {
    IoCoroutine *c = self;
    while (!ISNIL(c)) {
        IoObject_setSlot_to_(c, IOSYMBOL("recentInChain"), v);
        c = IoCoroutine_rawParentCoroutine(c);
    }
}

IO_METHOD(IoCoroutine, setRecentInChain) {
    /*doc Coroutine setRecentInChain(aCoro)
    Sets the most recently run coroutine of the chain.
    Sets all the recentInChain in a direct path up to the top of the coro chain
    starting at this coro. Returns self.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);

    IoCoroutine_rawSetRecentInChain_(self, v);

    return self;
}

IoObject *IoCoroutine_rawRecentInChain(IoCoroutine *self) {
    return IoObject_getSlot_(self, IOSYMBOL("recentInChain"));
}

// result

void IoCoroutine_rawSetResult_(IoCoroutine *self, IoObject *v) {
    IoObject_setSlot_to_(self, IOSYMBOL("result"), v);
}

IoObject *IoCoroutine_rawResult(IoCoroutine *self) {
#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_rawResult: self=%p\n", (void*)self);
    fflush(stderr);
#endif

    IoObject *result = IoObject_getSlot_(self, IOSYMBOL("result"));

#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_rawResult: result=%p\n", (void*)result);
    fflush(stderr);
#endif

    return result;
}

// exception

void IoCoroutine_rawRemoveException(IoCoroutine *self) {
    IoObject_removeSlot_(self, IOSYMBOL("exception"));
}

void IoCoroutine_rawSetException_(IoCoroutine *self, IoObject *v) {
    IoObject_setSlot_to_(self, IOSYMBOL("exception"), v);
}

IoObject *IoCoroutine_rawException(IoCoroutine *self) {
    return IoObject_getSlot_(self, IOSYMBOL("exception"));
}

// ioStack

IO_METHOD(IoCoroutine, ioStack) {
    /*doc Coroutine ioStack
    Returns List of values on this coroutine's stack.
    */

    return IoList_newWithList_(IOSTATE, Stack_asList(DATA(self)->ioStack));
}

/*
 * rawReturnToParent - Handle returning from a coroutine to its parent.
 *
 * In the frame-based model, this is called when we want to abort the current
 * coroutine (usually due to an exception). It sets up the state so that when
 * the current CFunction returns, the eval loop will see the parent's frames.
 *
 * NOTE: This does NOT involve any C stack manipulation. It just swaps frame
 * stacks. The eval loop continues on the same C stack, processing parent's frames.
 */
void IoCoroutine_rawReturnToParent(IoCoroutine *self) {
    IoState *state = IOSTATE;

    IoObject *exc = IoCoroutine_rawException(self);
#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_rawReturnToParent: exception=%p, ioNil=%p\n",
            (void*)exc, (void*)state->ioNil);
    fflush(stderr);
#endif

    if (!ISNIL(exc)) {
        // Only print backtrace when NOT in a nested eval.
        // In nested evals (IoCoroutine_try), the caller (tryToPerform)
        // checks for exceptions and handles backtrace printing via
        // IoState_exception_. Printing here would cause infinite
        // recursion: rawPrintBackTrace → tryToPerform → nested eval →
        // error → rawReturnToParent → rawPrintBackTrace → ...
        if (state->nestedEvalDepth == 0) {
#ifdef DEBUG_CORO_EVAL
            fprintf(stderr, "IoCoroutine_rawReturnToParent: printing backtrace\n");
            fflush(stderr);
#endif
            IoCoroutine_rawPrintBackTrace(self);
#ifdef DEBUG_CORO_EVAL
            fprintf(stderr, "IoCoroutine_rawReturnToParent: backtrace done\n");
            fflush(stderr);
#endif
        }
    }

    // In a nested eval (IoCoroutine_try), we need to handle two cases:
    // 1. This coro was started via coro swap (parent has CORO_WAIT_CHILD
    //    frame with us as the child) — must restore parent's saved frames
    //    so the eval loop continues with the parent's frame stack.
    // 2. This coro was started via needOwnEvalLoop (rawRun handles cleanup
    //    after evalLoop_ returns) — just pop all frames and return.
    if (state->nestedEvalDepth > 0) {
#ifdef DEBUG_CORO_EVAL
        fprintf(stderr, "rawReturnToParent: nestedEvalDepth=%d>0, self=%p, popping all frames\n",
                state->nestedEvalDepth, (void*)self);
        fflush(stderr);
#endif
        while (state->currentFrame) {
            IoState_popFrame_(state);
        }

        // Check if we were started via coro swap: parent's saved frameStack
        // should have a CORO_WAIT_CHILD frame with us as the child.
        IoCoroutine *parent = IoCoroutine_rawParentCoroutine(self);
        if (parent && ISCOROUTINE(parent)) {
            IoEvalFrame *parentTopFrame = DATA(parent)->frameStack;
            IoEvalFrameData *parentTopFd = FRAME_DATA(parentTopFrame);
            if (parentTopFrame &&
                parentTopFd->state == FRAME_STATE_CORO_WAIT_CHILD &&
                parentTopFd->controlFlow.coroInfo.childCoroutine == (IoObject *)self) {
#ifdef DEBUG_CORO_EVAL
                fprintf(stderr, "rawReturnToParent: CORO SWAP RESTORE — restoring parent coro\n");
                fflush(stderr);
#endif
                // Save child state (for exception inspection by tryToPerform)
                IoCoroutine_saveState_(self, state);

                // Restore parent's frames
                IoState_setCurrentCoroutine_(state, parent);
                IoCoroutine_restoreState_(parent, state);

                // Transition parent's CORO_WAIT_CHILD to CONTINUE_CHAIN
                IoEvalFrame *parentFrame = state->currentFrame;
                IoEvalFrameData *parentFd = FRAME_DATA(parentFrame);
                if (parentFrame && parentFd->state == FRAME_STATE_CORO_WAIT_CHILD) {
                    parentFd->result = DATA(self)->returnValue
                                              ? DATA(self)->returnValue
                                              : state->ioNil;
                    parentFd->state = FRAME_STATE_CONTINUE_CHAIN;
                }
                // Don't return — let the eval loop continue with parent's frames
                // But we DO need to return here because we're inside a CFunction
                // that was called from the eval loop. The eval loop will pick up
                // the restored parent frames on its next iteration.
                return;
            }
        }
        return;
    }

    // Save our state
#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_rawReturnToParent: saving state\n");
    fflush(stderr);
#endif
    IoCoroutine_saveState_(self, state);

    // Switch to parent coroutine
    IoCoroutine *parent = IoCoroutine_rawParentCoroutine(self);
#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_rawReturnToParent: parent=%p\n", (void*)parent);
    fflush(stderr);
#endif

    if (parent && ISCOROUTINE(parent)) {
#ifdef DEBUG_CORO_EVAL
        fprintf(stderr, "IoCoroutine_rawReturnToParent: switching to parent\n");
        fflush(stderr);
#endif
        IoState_setCurrentCoroutine_(state, parent);
        IoCoroutine_restoreState_(parent, state);

        // If parent has a frame waiting for us, give it the result/exception
        IoEvalFrame *parentFrame = state->currentFrame;
        IoEvalFrameData *parentFd = FRAME_DATA(parentFrame);
#ifdef DEBUG_CORO_EVAL
        fprintf(stderr, "IoCoroutine_rawReturnToParent: parentFrame=%p, state=%d\n",
                (void*)parentFrame, parentFrame ? parentFd->state : -1);
        fflush(stderr);
#endif

        if (parentFrame && parentFd->state == FRAME_STATE_CORO_WAIT_CHILD) {
            parentFd->result = DATA(self)->returnValue
                                      ? DATA(self)->returnValue
                                      : state->ioNil;
            parentFd->state = FRAME_STATE_CONTINUE_CHAIN;
        }

        // Signal to eval loop that frame stack changed - don't process stale frame
        state->needsControlFlowHandling = 1;

#ifdef DEBUG_CORO_EVAL
        fprintf(stderr, "IoCoroutine_rawReturnToParent: done, needsControlFlowHandling=1\n");
        fflush(stderr);
#endif
    } else {
        if (self == state->mainCoroutine) {
            printf("IoCoroutine error: attempt to return from main coro\n");
            exit(-1);
        }
        printf("IoCoroutine error: unable to return to parent coro from %p\n",
               (void *)self);
        exit(-1);
    }
}

IO_METHOD(IoCoroutine, rawSignalException) {
    /*doc Coroutine rawSignalException
    Bridges an Io-level exception to the eval loop by setting errorRaised.
    Called by raiseException when there is no parent coroutine to resume.
    The exception should already be set on this coroutine via setException.
    */

    IoState *state = IOSTATE;
    // The exception is already set on this coroutine (by raiseException).
    // Signal the eval loop to unwind frames.
    state->errorRaised = 1;
    return self;
}

IO_METHOD(IoCoroutine, freeStack) {
    /*doc Coroutine freeStack
    Frees all the internal data from the receiver's stack. Returns self.
    */

    IoCoroutine *current = IoState_currentCoroutine(IOSTATE);

    if (current != self && DATA(self)->frameStack) {
        // Frames are GC-managed, just drop the reference
        DATA(self)->frameStack = NULL;
    }

    return self;
}

IO_METHOD(IoCoroutine, main) {
    IoObject *runTarget = IoCoroutine_rawRunTarget(self);
    IoObject *runLocals = IoCoroutine_rawRunLocals(self);
    IoObject *runMessage = IoCoroutine_rawRunMessage(self);

    if (runTarget && runLocals && runMessage) {
        return IoMessage_locals_performOn_(runMessage, runLocals, runTarget);
    } else {
        printf("IoCoroutine_main() missing needed parameters\n");
    }

    return IONIL(self);
}

void IoCoroutine_clearStack(IoCoroutine *self) {
    Stack_clear(DATA(self)->ioStack);
}

/*
 * rawRun - Start running a coroutine.
 *
 * FRAME-BASED ARCHITECTURE:
 * There is ONE eval loop running on the main C stack. This function does NOT
 * call evalLoop_ for child coroutines - it just sets up their frame stack
 * and returns. The existing eval loop will continue processing.
 *
 * For the MAIN coroutine (called at startup), we DO call evalLoop_ since
 * there's no existing loop to return to.
 *
 * For CHILD coroutines (called from Io code), we:
 *   1. Mark caller's frame as waiting for child
 *   2. Save caller's frame stack
 *   3. Push child's initial frame
 *   4. Return - the eval loop continues with child's frames
 *   5. When child's stack empties, eval loop returns to parent (see IoState_evalLoop_)
 */
void IoCoroutine_rawRun(IoCoroutine *self) {
    IoState *state = IOSTATE;

#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_rawRun: self=%p, mainCoro=%p\n",
            (void*)self, (void*)state->mainCoroutine);
    fflush(stderr);
#endif

    IoCoroutine_rawSetRecentInChain_(self, self);

    // Get the run parameters
    IoObject *runTarget = IoCoroutine_rawRunTarget(self);
    IoObject *runLocals = IoCoroutine_rawRunLocals(self);
    IoMessage *runMessage = IoCoroutine_rawRunMessage(self);

#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_rawRun: target=%p, locals=%p, msg=%p\n",
            (void*)runTarget, (void*)runLocals, (void*)runMessage);
    fflush(stderr);
#endif

    if (!runTarget || !runLocals || !runMessage) {
        printf("IoCoroutine_rawRun: missing runTarget, runLocals, or runMessage\n");
        return;
    }

    // Validate that runMessage is actually a Message
    if (!ISMESSAGE(runMessage)) {
        fprintf(stderr, "BUG: IoCoroutine_rawRun: runMessage is NOT a Message!\n");
        fprintf(stderr, "  runMessage=%p, tag=%s\n",
                (void*)runMessage,
                IoObject_tag((IoObject*)runMessage) ? IoObject_tag((IoObject*)runMessage)->name : "NULL");
        fprintf(stderr, "  runTarget=%p, runLocals=%p\n",
                (void*)runTarget, (void*)runLocals);
        fprintf(stderr, "  self=%p (coro)\n", (void*)self);
        fflush(stderr);
        abort();
    }

    IoCoroutine *current = IoState_currentCoroutine(state);

#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_rawRun: current=%p, currentFrame=%p\n",
            (void*)current, (void*)state->currentFrame);
    fflush(stderr);
#endif

    // Check if we need to run our own eval loop.
    // This happens when:
    // - This is the main coroutine being started
    // - No coroutine is current
    // - The current coroutine has no active frames (no eval loop running)
    int needOwnEvalLoop = (self == state->mainCoroutine) ||
                          (current == NULL) ||
                          (state->currentFrame == NULL);

#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_rawRun: needOwnEvalLoop=%d\n", needOwnEvalLoop);
    fflush(stderr);
#endif

    if (needOwnEvalLoop) {
        // No eval loop is running - we need to run one ourselves
        IoCoroutine *previousCoro = current;

        DATA(self)->frameStack = NULL;
        DATA(self)->stopStatus = MESSAGE_STOP_STATUS_NORMAL;
        DATA(self)->returnValue = NULL;

        IoState_setCurrentCoroutine_(state, self);
        state->currentFrame = NULL;
        state->frameDepth = 0;
        state->currentIoStack = DATA(self)->ioStack;

        // Set parent if we have one
        if (previousCoro && previousCoro != self) {
            IoCoroutine_rawSetParentCoroutine_(self, previousCoro);
        }

        // Push initial frame
        IoEvalFrame *frame = IoState_pushFrame_(state);
        IoEvalFrameData *fd = FRAME_DATA(frame);
        fd->message = runMessage;
        fd->target = runTarget;
        fd->locals = runLocals;
        fd->cachedTarget = runTarget;
        fd->state = FRAME_STATE_START;

        // Run the eval loop
        state->nestedEvalDepth++;
        IoObject *result = IoState_evalLoop_(state);
        state->nestedEvalDepth--;
        IoCoroutine_rawSetResult_(self, result);

        // Restore previous coroutine if any
        if (previousCoro && previousCoro != self) {
            IoState_setCurrentCoroutine_(state, previousCoro);
            IoCoroutine_restoreState_(previousCoro, state);
        }
        return;
    }

    // Child coroutine - set up frame stack swap
#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_rawRun: setting up child coro swap\n");
    fflush(stderr);
#endif

    // Mark caller's frame as waiting for this child to complete
    IoEvalFrame *callerFrame = state->currentFrame;
    IoEvalFrameData *callerFd = FRAME_DATA(callerFrame);
#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_rawRun: callerFrame=%p\n", (void*)callerFrame);
    fflush(stderr);
#endif

    if (callerFrame) {
        callerFd->state = FRAME_STATE_CORO_WAIT_CHILD;
        callerFd->controlFlow.coroInfo.childCoroutine = self;
    }

    // Save current coroutine's frame stack
#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_rawRun: saving current coro state\n");
    fflush(stderr);
#endif
    IoCoroutine_saveState_(current, state);

    // Set up child coroutine
    IoCoroutine_rawSetParentCoroutine_(self, current);
    DATA(self)->frameStack = NULL;
    DATA(self)->stopStatus = MESSAGE_STOP_STATUS_NORMAL;
    DATA(self)->returnValue = NULL;

    // Switch to child coroutine
#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_rawRun: switching to child\n");
    fflush(stderr);
#endif
    IoState_setCurrentCoroutine_(state, self);
    state->currentFrame = NULL;
    state->frameDepth = 0;
    state->currentIoStack = DATA(self)->ioStack;

    // Push child's initial frame
    IoEvalFrame *frame = IoState_pushFrame_(state);
    IoEvalFrameData *fd = FRAME_DATA(frame);
    fd->message = runMessage;
    fd->target = runTarget;
    fd->locals = runLocals;
    fd->cachedTarget = runTarget;
    fd->state = FRAME_STATE_START;

#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "rawRun CHILD ROOT FRAME SET: frame=%p, msg=%p, target=%p, locals=%p\n",
            (void*)frame, (void*)runMessage, (void*)runTarget, (void*)runLocals);
    fflush(stderr);
#endif

    // Signal that we set up control flow - don't process return value normally
    state->needsControlFlowHandling = 1;

    // Return - the eval loop will continue with child's frame stack
    // When child's stack empties, eval loop will switch back to parent
}

IO_METHOD(IoCoroutine, run) {
    /*doc Coroutine run
    Runs receiver and returns self.
    */

    IoState *state = IOSTATE;
#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_run: calling rawRun, currentFrame=%p\n",
            (void*)state->currentFrame);
    fflush(stderr);
#endif
    IoCoroutine_rawRun(self);
#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_run: rawRun returned, getting result\n");
    fflush(stderr);
#endif
    IoObject *result = IoCoroutine_rawResult(self);
#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_run: got result=%p, returning\n", (void*)result);
    fflush(stderr);
#endif
    return result;
}

void IoCoroutine_try(IoCoroutine *self, IoObject *target, IoObject *locals,
                     IoMessage *message) {
    IoState *state = IOSTATE;

#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_try: self=%p, entering\n", (void*)self);
    fflush(stderr);
#endif

    IoCoroutine *currentCoro = (IoCoroutine *)IoState_currentCoroutine(state);

    // Validate message parameter
    if (!ISMESSAGE(message)) {
        fprintf(stderr, "BUG: IoCoroutine_try: message is NOT a Message!\n");
        fprintf(stderr, "  message=%p, tag=%s\n",
                (void*)message,
                IoObject_tag((IoObject*)message) ? IoObject_tag((IoObject*)message)->name : "NULL");
        fflush(stderr);
        abort();
    }

    IoCoroutine_rawSetRunTarget_(self, target);
    IoCoroutine_rawSetRunLocals_(self, locals);
    IoCoroutine_rawSetRunMessage_(self, message);
    IoCoroutine_rawSetParentCoroutine_(self, currentCoro);

    // Verify runMessage was stored correctly
    IoMessage *storedMsg = IoCoroutine_rawRunMessage(self);
    if (storedMsg != message) {
        fprintf(stderr, "BUG: IoCoroutine_try: stored runMessage differs!\n");
        fprintf(stderr, "  expected=%p, got=%p\n", (void*)message, (void*)storedMsg);
        if (storedMsg && !ISMESSAGE(storedMsg)) {
            fprintf(stderr, "  stored tag=%s\n",
                    IoObject_tag((IoObject*)storedMsg) ? IoObject_tag((IoObject*)storedMsg)->name : "NULL");
        }
        fflush(stderr);
        abort();
    }

    // Check if we're inside an existing eval loop.
    // If so, we need to run a nested loop to ensure synchronous completion.
    // IoCoroutine_try semantics require the coroutine to be done when this returns.
    int needNestedLoop = (state->currentFrame != NULL);

    if (needNestedLoop) {
#ifdef DEBUG_CORO_EVAL
        fprintf(stderr, "IoCoroutine_try: running nested eval for synchronous completion\n");
        fflush(stderr);
#endif

        // Save current coroutine's state
        IoCoroutine_saveState_(currentCoro, state);

        // Initialize the try coroutine
        DATA(self)->frameStack = NULL;
        DATA(self)->stopStatus = MESSAGE_STOP_STATUS_NORMAL;
        DATA(self)->returnValue = NULL;

        // Switch to try coroutine
        IoState_setCurrentCoroutine_(state, self);
        state->currentFrame = NULL;
        state->frameDepth = 0;
        state->currentIoStack = DATA(self)->ioStack;

        // Push initial frame
        IoEvalFrame *frame = IoState_pushFrame_(state);
        IoEvalFrameData *fd = FRAME_DATA(frame);
        fd->message = message;
        fd->target = target;
        fd->locals = locals;
        fd->cachedTarget = target;
        fd->state = FRAME_STATE_START;

#ifdef DEBUG_CORO_EVAL
        fprintf(stderr, "try NESTED ROOT FRAME SET: frame=%p, msg=%p, target=%p, locals=%p\n",
                (void*)frame, (void*)message, (void*)target, (void*)locals);
        fflush(stderr);
#endif

        // Mark that we're in a nested eval - so evalLoop knows not to do coro switching.
        state->nestedEvalDepth++;

        // Run nested eval loop - this will complete the try coroutine
        IoObject *result = IoState_evalLoop_(state);
        IoCoroutine_rawSetResult_(self, result);

        // Done with nested eval
        state->nestedEvalDepth--;

        // Restore parent coroutine's state
        IoState_setCurrentCoroutine_(state, currentCoro);
        IoCoroutine_restoreState_(currentCoro, state);

#ifdef DEBUG_CORO_EVAL
        fprintf(stderr, "IoCoroutine_try: nested eval completed, result=%p\n", (void*)result);
        fprintf(stderr, "IoCoroutine_try: restored to coro=%p, frame=%p\n",
                (void*)currentCoro, (void*)state->currentFrame);
        fflush(stderr);
#endif
    } else {
        // Must increment nestedEvalDepth so rawReturnToParent pops frames
        // instead of doing a full parent-coro switch (which corrupts state
        // while CFunction is still on C stack). rawRun's own eval loop will
        // exit naturally, and rawRun handles coro restoration itself.
        state->nestedEvalDepth++;
        IoCoroutine_rawRun(self);
        state->nestedEvalDepth--;
    }
}

IoCoroutine *IoCoroutine_newWithTry(void *state, IoObject *target,
                                    IoObject *locals, IoMessage *message) {
    IoCoroutine *self = IoCoroutine_new(state);
    IoCoroutine_try(self, target, locals, message);
    return self;
}

void IoCoroutine_raiseError(IoCoroutine *self, IoSymbol *description,
                            IoMessage *m) {
#ifdef DEBUG_CORO_EVAL
    fprintf(stderr, "IoCoroutine_raiseError: self=%p, error=%s\n",
            (void*)self, CSTRING(description));
    fflush(stderr);
#endif

    // Just create the exception — no unwinding.
    // The eval loop handles frame unwinding when it sees errorRaised.
    IoObject *e = IoObject_rawGetSlot_(self, IOSYMBOL("Exception"));

    if (e) {
        e = IOCLONE(e);
        IoObject_setSlot_to_(e, IOSYMBOL("error"), description);
        if (m)
            IoObject_setSlot_to_(e, IOSYMBOL("caughtMessage"), m);
        IoObject_setSlot_to_(e, IOSYMBOL("coroutine"), self);
        IoCoroutine_rawSetException_(self, e);
    }
}

// methods

/*
 * rawResume - Resume a suspended coroutine.
 *
 * This swaps frame stacks: the current coroutine is suspended, and the
 * target coroutine is resumed. The eval loop continues processing
 * whichever frames are now in state->currentFrame.
 */
IoObject *IoCoroutine_rawResume(IoCoroutine *self) {
    IoState *state = IOSTATE;
    IoCoroutine *current = IoState_currentCoroutine(state);

    IoCoroutine_rawSetRecentInChain_(self, self);

    // Can't resume self
    if (self == current) {
        return self;
    }

    if (DATA(self)->frameStack || DATA(self)->frameStack == NULL) {
        // Either resuming a suspended coro OR starting a fresh one

        // Mark current frame as yielded (so we resume here when switched back)
        IoEvalFrame *callerFrame = state->currentFrame;
        IoEvalFrameData *callerFd = FRAME_DATA(callerFrame);
        if (callerFrame) {
            callerFd->state = FRAME_STATE_CORO_YIELDED;
        }

        // Save current coroutine's state
        IoCoroutine_saveState_(current, state);

        // Restore target coroutine's state
        IoState_setCurrentCoroutine_(state, self);
        IoCoroutine_restoreState_(self, state);

        // If target has no frames (never run), start it
        if (state->currentFrame == NULL) {
            IoObject *runTarget = IoCoroutine_rawRunTarget(self);
            IoObject *runLocals = IoCoroutine_rawRunLocals(self);
            IoMessage *runMessage = IoCoroutine_rawRunMessage(self);

            if (runTarget && runLocals && runMessage) {
                if (!ISMESSAGE(runMessage)) {
                    // Can't start this coro - no valid runMessage.
                    // This can happen if 'resume' is called on the Coroutine proto
                    // (which has nil run parameters from ::= declarations).
                    // Just restore the caller and skip.
#ifdef DEBUG_CORO_EVAL
                    fprintf(stderr, "WARNING: rawResume: can't start coro %p (runMessage not a Message, is %s). Restoring caller.\n",
                            (void*)self,
                            IoObject_tag((IoObject*)runMessage) ? IoObject_tag((IoObject*)runMessage)->name : "NULL");
                    fflush(stderr);
#endif
                    // Restore calling coroutine
                    IoState_setCurrentCoroutine_(state, current);
                    IoCoroutine_restoreState_(current, state);
                    state->needsControlFlowHandling = 0;
                    return self;
                }
                IoCoroutine_rawSetParentCoroutine_(self, current);
                IoEvalFrame *frame = IoState_pushFrame_(state);
                IoEvalFrameData *fd = FRAME_DATA(frame);
                fd->message = runMessage;
                fd->target = runTarget;
                fd->locals = runLocals;
                fd->cachedTarget = runTarget;
                fd->state = FRAME_STATE_START;
            }
        }

        // Signal control flow handling
        state->needsControlFlowHandling = 1;
    }

    return self;
}

IO_METHOD(IoCoroutine, resume) {
    /*doc Coroutine resume
    Yields to the receiver. Runs the receiver if it is not running yet.
    Returns self.
    */

    return IoCoroutine_rawResume(self);
}

IO_METHOD(IoCoroutine, implementation) {
    /*doc Coroutine implementation
    Returns coroutine implementation type: "frame-based" (portable, no assembly)
    */

    return IOSYMBOL("frame-based");
}

IO_METHOD(IoCoroutine, isCurrent) {
    /*doc Coroutine isCurrent
    Returns true if the receiver is currently running coroutine.
    */

    IoObject *v = IOBOOL(self, self == IoState_currentCoroutine(IOSTATE));
    return v;
}

IO_METHOD(IoCoroutine, currentCoroutine) {
    /*doc Coroutine currentCoroutine
    Returns currently running coroutine in Io state.
    */

    return IoState_currentCoroutine(IOSTATE);
}

// stack trace

int IoCoroutine_rawIoStackSize(IoCoroutine *self) {
    return Stack_count(DATA(self)->ioStack);
}

void IoCoroutine_rawPrint(IoCoroutine *self) {
    int frameCount = 0;
    IoEvalFrame *f = DATA(self)->frameStack;
    while (f) {
        frameCount++;
        f = FRAME_DATA(f)->parent;
    }
    printf("Coroutine_%p frameDepth %d ioStackSize %i\n", (void *)self,
           frameCount, (int)Stack_count(DATA(self)->ioStack));
}

// debugging

int IoCoroutine_rawDebuggingOn(IoCoroutine *self) {
    return DATA(self)->debuggingOn;
}

IO_METHOD(IoCoroutine, setMessageDebugging) {
    /*doc Coroutine setMessageDebugging(aBoolean)
    Turns on message level debugging for this coro. When on, this
    coro will send a vmWillSendMessage message to the Debugger object before
    each message send and pause itself. See the Debugger object documentation
    for more information.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);

    DATA(self)->debuggingOn = ISTRUE(v);
    IoState_updateDebuggingMode(IOSTATE);

    return self;
}

IO_METHOD(IoCoroutine, currentFrame) {
    /*doc Coroutine currentFrame
    Returns the current (topmost) EvalFrame of this coroutine,
    or nil if no frames are active. Only works for the currently
    executing coroutine.
    */
    IoState *state = IOSTATE;
    if (self == state->currentCoroutine && state->currentFrame) {
        return state->currentFrame;
    }
    // For non-current coroutines, the frame stack is saved in the coro data
    IoEvalFrame *f = DATA(self)->frameStack;
    return f ? (IoObject *)f : IONIL(self);
}

IoObject *IoObject_performWithDebugger(IoCoroutine *self, IoObject *locals,
                                       IoMessage *m) {
    IoState *state = IOSTATE;
    IoObject *currentCoroutine = IoState_currentCoroutine(state);

    if (IoCoroutine_rawDebuggingOn(currentCoroutine)) {
        IoObject *debugger = state->debugger; // stack retain it?

        if (debugger) {
            IoObject_setSlot_to_(debugger, IOSYMBOL("messageCoroutine"),
                                 currentCoroutine);
            IoObject_setSlot_to_(debugger, IOSYMBOL("messageSelf"), self);
            IoObject_setSlot_to_(debugger, IOSYMBOL("messageLocals"), locals);
            IoObject_setSlot_to_(debugger, IOSYMBOL("message"), m);

            {
                IoObject *context;
                IoCoroutine *c = IoObject_rawGetSlot_context_(
                    debugger, IOSYMBOL("debuggerCoroutine"), &context);
                IOASSERT(c, "Debugger needs a debuggerCoroutine slot");
                IoCoroutine_rawResume(c);
            }
        }
    }

    return IoObject_perform(self, locals, m);
}

void IoCoroutine_rawPrintBackTrace(IoCoroutine *self) {
    IoState *state = IOSTATE;
    IoObject *e = IoCoroutine_rawException(self);
    IoMessage *caughtMessage =
        IoObject_rawGetSlot_(e, IOSYMBOL("caughtMessage"));

    if (IoObject_rawGetSlot_(e, IOSYMBOL("showStack"))) // sanity check
    {
        IoState_on_doCString_withLabel_(state, e, "showStack", "[Coroutine]");
    } else {
        IoSymbol *error = IoObject_rawGetSlot_(e, IOSYMBOL("error"));

        if (error) {
            fputs(CSTRING(error), stderr);
            fputs("\n", stderr);
        } else {
            fputs("error: [missing error slot in Exception object]\n", stderr);
        }

        if (caughtMessage) {
            UArray *ba =
                IoMessage_asMinimalStackEntryDescription(caughtMessage);
            fputs(UArray_asCString(ba), stderr);
            fputs("\n", stderr);
            UArray_free(ba);
        }
    }
}
