
// metadoc Continuation copyright Steve Dekorte 2002, 2025
// metadoc Continuation license BSD revised

#include "IoContinuation.h"
#include "IoState.h"
#include "IoState_eval.h"
#include "IoObject.h"
#include "IoMessage.h"
#include "IoBlock.h"
#include "IoEvalFrame.h"
#include <stdlib.h>
#include <string.h>

#define DATA(self) ((IoContinuationData *)IoObject_dataPointer(self))

static const char *protoId = "Continuation";

IoTag *IoContinuation_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoContinuation_free);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoContinuation_rawClone);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoContinuation_mark);
    return tag;
}

IoContinuation *IoContinuation_proto(void *state) {
    IoObject *self = IoObject_new(state);
    IoObject_tag_(self, IoContinuation_newTag(state));

    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoContinuationData)));
    DATA(self)->capturedFrame = NULL;
    DATA(self)->capturedLocals = NULL;
    DATA(self)->invoked = 0;
    DATA(self)->multiShot = 0;

    IoState_registerProtoWithId_(state, self, protoId);

    // Methods
    {
        IoMethodTable methodTable[] = {
            {"invoke", IoContinuation_invoke},
            {"isInvoked", IoContinuation_isInvoked},
            {"setMultiShot", IoContinuation_setMultiShot},
            {NULL, NULL},
        };
        IoObject_addMethodTable_(self, methodTable);
    }

    return self;
}

IoContinuation *IoContinuation_rawClone(IoContinuation *proto) {
    IoState *state = IoObject_state(proto);
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoContinuationData)));
    DATA(self)->capturedFrame = NULL;
    DATA(self)->capturedLocals = NULL;
    DATA(self)->invoked = 0;
    DATA(self)->multiShot = 0;
    return self;
}

IoContinuation *IoContinuation_new(void *state) {
    IoObject *proto = IoState_protoWithId_(state, protoId);
    return IOCLONE(proto);
}

void IoContinuation_free(IoContinuation *self) {
    if (DATA(self)->capturedFrame) {
        IoContinuation_freeFrameStack_(DATA(self)->capturedFrame);
        DATA(self)->capturedFrame = NULL;
    }
    io_free(IoObject_dataPointer(self));
}

void IoContinuation_mark(IoContinuation *self) {
    // Mark captured locals
    if (DATA(self)->capturedLocals) {
        IoObject_shouldMark(DATA(self)->capturedLocals);
    }

    // Mark all objects in captured frame stack
    IoEvalFrame *frame = DATA(self)->capturedFrame;
    while (frame) {
        IoEvalFrame_mark(frame);
        frame = frame->parent;
    }
}

// Deep copy a single frame
static IoEvalFrame *copyFrame_(IoEvalFrame *src) {
    if (!src) return NULL;

    IoEvalFrame *copy = IoEvalFrame_new();

    // Copy all fields
    copy->message = src->message;
    copy->target = src->target;
    copy->locals = src->locals;
    copy->cachedTarget = src->cachedTarget;
    copy->state = src->state;
    copy->argCount = src->argCount;
    copy->currentArgIndex = src->currentArgIndex;
    copy->result = src->result;
    copy->slotValue = src->slotValue;
    copy->slotContext = src->slotContext;
    copy->call = src->call;
    copy->blockLocals = src->blockLocals;
    copy->passStops = src->passStops;

    // Copy argValues array if present
    if (src->argValues && src->argCount > 0) {
        copy->argValues = io_calloc(src->argCount, sizeof(IoObject *));
        memcpy(copy->argValues, src->argValues,
               src->argCount * sizeof(IoObject *));
    }

    // Copy control flow union
    memcpy(&copy->controlFlow, &src->controlFlow, sizeof(src->controlFlow));

    // Note: parent will be set by the recursive copy

    return copy;
}

// Deep copy the entire frame stack
IoEvalFrame *IoContinuation_copyFrameStack_(IoEvalFrame *frame) {
    if (!frame) return NULL;

    // Copy this frame
    IoEvalFrame *copy = copyFrame_(frame);

    // Recursively copy parent and link
    copy->parent = IoContinuation_copyFrameStack_(frame->parent);

    return copy;
}

// Free a copied frame stack
void IoContinuation_freeFrameStack_(IoEvalFrame *frame) {
    while (frame) {
        IoEvalFrame *parent = frame->parent;
        IoEvalFrame_free(frame);
        frame = parent;
    }
}

// Capture the current frame stack into this continuation
void IoContinuation_captureFrameStack_(IoContinuation *self,
                                        IoEvalFrame *frame,
                                        IoObject *locals) {
    // Free any existing captured frame
    if (DATA(self)->capturedFrame) {
        IoContinuation_freeFrameStack_(DATA(self)->capturedFrame);
    }

    // Deep copy the frame stack
    DATA(self)->capturedFrame = IoContinuation_copyFrameStack_(frame);
    DATA(self)->capturedLocals = locals;
    DATA(self)->invoked = 0;
}

// ============================================================
// Methods
// ============================================================

IO_METHOD(IoContinuation, invoke) {
    /*doc Continuation invoke(value)
    Invokes the continuation, restoring the captured execution state.
    The value argument becomes the result of the original callcc call.
    By default, a continuation can only be invoked once.
    */

    IoState *state = IOSTATE;

    // Check if already invoked (for one-shot continuations)
    if (DATA(self)->invoked && !DATA(self)->multiShot) {
        IoState_error_(state, m,
            "Continuation has already been invoked. "
            "Use setMultiShot(true) to allow multiple invocations.");
        return IONIL(self);
    }

    // Get the value to return
    IoObject *value = IoMessage_locals_valueArgAt_(m, locals, 0);

    // Check if we're in iterative evaluation mode
    if (state->currentFrame == NULL) {
        IoState_error_(state, m,
            "Continuation invoke requires iterative evaluation mode");
        return IONIL(self);
    }

    // Check if we have a captured frame
    if (!DATA(self)->capturedFrame) {
        IoState_error_(state, m,
            "Continuation has no captured state");
        return IONIL(self);
    }

    DATA(self)->invoked = 1;

    // Pop all current frames back to the pool
    while (state->currentFrame) {
        IoEvalFrame *frame = state->currentFrame;
        state->currentFrame = frame->parent;
        state->frameDepth--;

        // Return frame to pool
        if (state->framePoolCount < 256) {
            IoEvalFrame_reset(frame);
            state->framePool[state->framePoolCount++] = frame;
        } else {
            IoEvalFrame_free(frame);
        }
    }

    // Restore the captured frame stack (deep copy again if multi-shot)
    if (DATA(self)->multiShot) {
        state->currentFrame = IoContinuation_copyFrameStack_(
            DATA(self)->capturedFrame);
    } else {
        // Transfer ownership - faster for one-shot
        state->currentFrame = DATA(self)->capturedFrame;
        DATA(self)->capturedFrame = NULL;
    }

    // Count the restored frames
    state->frameDepth = 0;
    IoEvalFrame *f = state->currentFrame;
    while (f) {
        state->frameDepth++;
        f = f->parent;
    }

    // Set the result of the callcc to be the invoked value
    // The top frame should be the callcc frame in CALLCC_EVAL_BLOCK state
    if (state->currentFrame) {
        state->currentFrame->result = value;
        // The state should already be CALLCC_EVAL_BLOCK, which will
        // transition to CONTINUE_CHAIN when the eval loop processes it
    }

    // Signal to the eval loop that the frame stack was replaced
    // The eval loop will restart with the new frame stack
    state->continuationInvoked = 1;

    // Return the value (this return value goes back to the eval loop
    // but won't be used because continuationInvoked is set)
    return value;
}

IO_METHOD(IoContinuation, isInvoked) {
    /*doc Continuation isInvoked
    Returns true if this continuation has been invoked.
    */
    return IOBOOL(self, DATA(self)->invoked);
}

IO_METHOD(IoContinuation, setMultiShot) {
    /*doc Continuation setMultiShot(aBool)
    If true, allows this continuation to be invoked multiple times.
    By default, continuations are one-shot (can only be invoked once).
    */
    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    DATA(self)->multiShot = ISTRUE(v);
    return self;
}

// ============================================================
// callcc - Call With Current Continuation
// ============================================================

IO_METHOD(IoObject, callcc) {
    /*doc Object callcc(aBlock)
    Calls aBlock with the current continuation as its argument.
    The continuation captures the execution state at the point of
    the callcc call. If the block returns normally, callcc returns
    the block's return value. If the continuation is invoked with
    a value, callcc returns that value instead.

    Example:
        result := callcc(block(cont,
            // This is the "escape" pattern
            if(someCondition,
                cont invoke("early exit")
            )
            "normal return"
        ))
        // result is either "early exit" or "normal return"
    */

    IoState *state = IOSTATE;
    IoEvalFrame *frame = state->currentFrame;

    // Check if we're in iterative evaluation mode
    if (frame == NULL) {
        IoState_error_(state, m,
            "callcc requires iterative evaluation mode. "
            "Use IoMessage_locals_performOn_iterative instead of "
            "IoMessage_locals_performOn_");
        return IONIL(self);
    }

    // Get the block argument (unevaluated - we evaluate it ourselves)
    IoMessage *blockMsg = IoMessage_rawArgAt_(m, 0);
    if (!blockMsg) {
        IoState_error_(state, m, "callcc requires a block argument");
        return IONIL(self);
    }

    // Evaluate the block argument to get the actual block
    // We need to do this synchronously since we need the block to set up
    IoObject *blockArg = IoMessage_locals_performOn_(blockMsg, locals, locals);

    if (!ISBLOCK(blockArg)) {
        IoState_error_(state, m, "callcc requires a block argument");
        return IONIL(self);
    }

    // Set up block activation (similar to IoState_activateBlock_)
    IoBlock *block = (IoBlock *)blockArg;
    IoBlockData *blockData = (IoBlockData *)IoObject_dataPointer(block);

    // Create block locals
    IoObject *blockLocals = IOCLONE(state->localsProto);
    IoObject_isLocals_(blockLocals, 1);

    // Determine scope
    IoObject *scope = blockData->scope ? blockData->scope : self;

    // Create Call object
    IoCall *callObject = IoCall_with(
        state, locals,      // sender
        self,               // target
        m,                  // message
        self,               // slotContext (callcc is on Object)
        block,              // activated
        state->currentCoroutine
    );

    // Set up block locals
    IoObject_createSlotsIfNeeded(blockLocals);
    PHash *bslots = IoObject_slots(blockLocals);
    PHash_at_put_(bslots, state->callSymbol, callObject);
    PHash_at_put_(bslots, state->selfSymbol, scope);
    PHash_at_put_(bslots, state->updateSlotSymbol, state->localsUpdateSlotCFunc);

    // Mark as unreferenced for potential recycling
    IoObject_isReferenced_(blockLocals, 0);
    IoObject_isReferenced_(callObject, 0);

    // Change frame state to wait for block body
    // IMPORTANT: Must set state BEFORE capturing continuation
    // so when restored, the frame is in the right state
    frame->state = FRAME_STATE_CALLCC_EVAL_BLOCK;

    // NOW create continuation and capture the frame stack
    // (frame is now in CALLCC_EVAL_BLOCK state)
    IoContinuation *cont = IoContinuation_new(state);
    IoContinuation_captureFrameStack_(cont, frame, locals);

    // Store callcc info in frame for later use
    frame->controlFlow.callccInfo.continuation = cont;
    frame->controlFlow.callccInfo.blockLocals = blockLocals;

    // Bind the continuation to the first parameter
    List *argNames = blockData->argNames;
    if (argNames && List_size(argNames) > 0) {
        IoSymbol *paramName = List_at_(argNames, 0);
        IoObject_setSlot_to_(blockLocals, paramName, cont);
    }

    // Push frame to evaluate block body
    IoEvalFrame *blockFrame = IoState_pushFrame_(state);
    blockFrame->message = blockData->message;
    blockFrame->target = blockLocals;
    blockFrame->locals = blockLocals;
    blockFrame->cachedTarget = blockLocals;
    blockFrame->state = FRAME_STATE_START;
    blockFrame->call = callObject;
    blockFrame->blockLocals = blockLocals;
    blockFrame->passStops = blockData->passStops;

    // Signal that we've set up control flow handling
    state->needsControlFlowHandling = 1;

    // Return placeholder (will be replaced by block result or invoke value)
    return state->ioNil;
}
