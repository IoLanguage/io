
// metadoc EvalFrame copyright Steve Dekorte 2002, 2025
// metadoc EvalFrame license BSD revised
// metadoc EvalFrame category Core

#include "IoEvalFrame.h"
#include <stdlib.h>
#include <string.h>

// Create a new evaluation frame
IoEvalFrame *IoEvalFrame_new(void) {
    IoEvalFrame *self = (IoEvalFrame *)io_calloc(1, sizeof(IoEvalFrame));
    IoEvalFrame_reset(self);
    return self;
}

// Free an evaluation frame
void IoEvalFrame_free(IoEvalFrame *self) {
    if (!self) {
        return;
    }

    // Free argument values array if allocated
    if (self->argValues) {
        io_free(self->argValues);
        self->argValues = NULL;
    }

    io_free(self);
}

// Mark a single frame's contents for garbage collection (no recursion)
static void IoEvalFrame_markSingle_(IoEvalFrame *self) {
    // Mark all IoObject pointers that might be in the frame
    IoObject_shouldMarkIfNonNull(self->message);
    IoObject_shouldMarkIfNonNull(self->target);
    IoObject_shouldMarkIfNonNull(self->locals);
    IoObject_shouldMarkIfNonNull(self->cachedTarget);
    IoObject_shouldMarkIfNonNull(self->result);
    IoObject_shouldMarkIfNonNull(self->slotValue);
    IoObject_shouldMarkIfNonNull(self->slotContext);
    IoObject_shouldMarkIfNonNull(self->call);
    IoObject_shouldMarkIfNonNull(self->blockLocals);

    // Mark evaluated arguments
    if (self->argValues) {
        int i;
        for (i = 0; i < self->currentArgIndex; i++) {
            IoObject_shouldMarkIfNonNull(self->argValues[i]);
        }
    }

    // Mark control flow continuation info based on current state
    switch (self->state) {
        case FRAME_STATE_IF_EVAL_CONDITION:
        case FRAME_STATE_IF_CONVERT_BOOLEAN:
        case FRAME_STATE_IF_EVAL_BRANCH:
            IoObject_shouldMarkIfNonNull(self->controlFlow.ifInfo.conditionMsg);
            IoObject_shouldMarkIfNonNull(self->controlFlow.ifInfo.trueBranch);
            IoObject_shouldMarkIfNonNull(self->controlFlow.ifInfo.falseBranch);
            break;

        case FRAME_STATE_WHILE_EVAL_CONDITION:
        case FRAME_STATE_WHILE_CHECK_CONDITION:
        case FRAME_STATE_WHILE_DECIDE:
        case FRAME_STATE_WHILE_EVAL_BODY:
            IoObject_shouldMarkIfNonNull(self->controlFlow.whileInfo.conditionMsg);
            IoObject_shouldMarkIfNonNull(self->controlFlow.whileInfo.bodyMsg);
            IoObject_shouldMarkIfNonNull(self->controlFlow.whileInfo.lastResult);
            break;

        case FRAME_STATE_LOOP_EVAL_BODY:
        case FRAME_STATE_LOOP_AFTER_BODY:
            IoObject_shouldMarkIfNonNull(self->controlFlow.loopInfo.bodyMsg);
            IoObject_shouldMarkIfNonNull(self->controlFlow.loopInfo.lastResult);
            break;

        case FRAME_STATE_FOR_EVAL_SETUP:
        case FRAME_STATE_FOR_EVAL_BODY:
        case FRAME_STATE_FOR_AFTER_BODY:
            IoObject_shouldMarkIfNonNull(self->controlFlow.forInfo.bodyMsg);
            IoObject_shouldMarkIfNonNull(self->controlFlow.forInfo.counterName);
            IoObject_shouldMarkIfNonNull(self->controlFlow.forInfo.lastResult);
            break;

        case FRAME_STATE_FOREACH_EVAL_BODY:
        case FRAME_STATE_FOREACH_AFTER_BODY:
            IoObject_shouldMarkIfNonNull(self->controlFlow.foreachInfo.collection);
            IoObject_shouldMarkIfNonNull(self->controlFlow.foreachInfo.bodyMsg);
            IoObject_shouldMarkIfNonNull(self->controlFlow.foreachInfo.indexName);
            IoObject_shouldMarkIfNonNull(self->controlFlow.foreachInfo.valueName);
            IoObject_shouldMarkIfNonNull(self->controlFlow.foreachInfo.lastResult);
            IoObject_shouldMarkIfNonNull(self->controlFlow.foreachInfo.mapSource);
            break;

        case FRAME_STATE_CALLCC_EVAL_BLOCK:
            IoObject_shouldMarkIfNonNull(self->controlFlow.callccInfo.continuation);
            IoObject_shouldMarkIfNonNull(self->controlFlow.callccInfo.blockLocals);
            break;

        case FRAME_STATE_CORO_WAIT_CHILD:
        case FRAME_STATE_CORO_YIELDED:
            IoObject_shouldMarkIfNonNull(self->controlFlow.coroInfo.childCoroutine);
            break;

        case FRAME_STATE_DO_EVAL:
        case FRAME_STATE_DO_WAIT:
            IoObject_shouldMarkIfNonNull(self->controlFlow.doInfo.codeMessage);
            IoObject_shouldMarkIfNonNull(self->controlFlow.doInfo.evalTarget);
            IoObject_shouldMarkIfNonNull(self->controlFlow.doInfo.evalLocals);
            break;

        default:
            break;
    }
}

// Mark frame chain for garbage collection (iterative, not recursive)
void IoEvalFrame_mark(IoEvalFrame *self) {
    IoEvalFrame *frame = self;
    while (frame) {
        IoEvalFrame_markSingle_(frame);
        frame = frame->parent;
    }
}

// Reset frame to initial state (for reuse)
void IoEvalFrame_reset(IoEvalFrame *self) {
    if (!self) {
        return;
    }

    self->magic = IOEVAL_FRAME_MAGIC;
    self->message = NULL;
    self->target = NULL;
    self->locals = NULL;
    self->cachedTarget = NULL;
    self->parent = NULL;
    self->state = FRAME_STATE_START;
    self->argCount = 0;
    self->currentArgIndex = 0;
    self->result = NULL;
    self->slotValue = NULL;
    self->slotContext = NULL;
    self->call = NULL;
    self->blockLocals = NULL;
    self->passStops = 0;
    self->isNestedEvalRoot = 0;

    // Clear control flow union
    memset(&self->controlFlow, 0, sizeof(self->controlFlow));

    // Free argument values if allocated
    if (self->argValues) {
        io_free(self->argValues);
        self->argValues = NULL;
    }
}
