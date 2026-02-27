
#include "IoObject.h"
#include "IoNumber.h"
#include "IoEvalFrame.h"

// loops ---------------------------------------

IO_METHOD(IoObject, while) {
    /*doc Object while(<condition>, expression)
    Keeps evaluating message until condition return Nil.
    Returns the result of the last message evaluated or Nil if none were
    evaluated.
    */

    IoMessage_assertArgCount_receiver_(m, 2, self);
    if (IOSTATE->errorRaised) return IONIL(self);

    IoState *state = IOSTATE;

    // Bootstrap fallback: no eval loop running
    if (!state->currentFrame) {
        IoObject *result = IONIL(self);
        IoState_resetStopStatus(state);
        IoState_pushRetainPool(state);
        for (;;) {
            IoObject *v;
            IoState_clearTopPool(state);
            IoState_stackRetain_(state, result);
            v = IoMessage_locals_valueArgAt_(m, locals, 0);
            if (!ISTRUE(IoMessage_locals_performOn_(state->asBooleanMessage, v, v)))
                break;
            result = IoMessage_locals_valueArgAt_(m, locals, 1);
            if (IoState_handleStatus(state)) break;
        }
        IoState_popRetainPoolExceptFor_(state, result);
        return result;
    }

    IoEvalFrame *frame = state->currentFrame;

    // Get the argument messages (not evaluated yet)
    IoMessage *conditionMsg = IoMessage_rawArgAt_(m, 0);
    IoMessage *bodyMsg = IoMessage_rawArgAt_(m, 1);

    // Set up control flow info
    frame->controlFlow.whileInfo.conditionMsg = conditionMsg;
    frame->controlFlow.whileInfo.bodyMsg = bodyMsg;
    frame->controlFlow.whileInfo.lastResult = NULL;

    // Change frame state to start WHILE evaluation
    frame->state = FRAME_STATE_WHILE_EVAL_CONDITION;

    // Signal that we've set up control flow handling
    state->needsControlFlowHandling = 1;

    return state->ioNil;
}

IO_METHOD(IoObject, loop) {
    /*doc Object loop(expression)
    Keeps evaluating message until a break.
    */

    IoMessage_assertArgCount_receiver_(m, 1, self);
    if (IOSTATE->errorRaised) return IONIL(self);

    IoState *state = IOSTATE;

    // Bootstrap fallback: no eval loop running
    if (!state->currentFrame) {
        IoObject *result;
        IoState_resetStopStatus(state);
        IoState_pushRetainPool(state);
        for (;;) {
            IoState_clearTopPool(state);
            result = IoMessage_locals_valueArgAt_(m, locals, 0);
            if (IoState_handleStatus(state)) break;
        }
        IoState_popRetainPoolExceptFor_(state, result);
        return result;
    }

    IoEvalFrame *frame = state->currentFrame;

    // Get the body message (not evaluated yet)
    IoMessage *bodyMsg = IoMessage_rawArgAt_(m, 0);

    // Set up control flow info
    frame->controlFlow.loopInfo.bodyMsg = bodyMsg;
    frame->controlFlow.loopInfo.lastResult = NULL;

    // Change frame state to start LOOP evaluation
    frame->state = FRAME_STATE_LOOP_EVAL_BODY;

    // Signal that we've set up control flow handling
    state->needsControlFlowHandling = 1;

    return state->ioNil;
}

IO_METHOD(IoObject, for)
{
    /*doc Object for(<counter>, <start>, <end>, <do message>)
    A for-loop control structure. See the io Programming Guide for a full
    description.
    */

    IoMessage_assertArgCount_receiver_(m, 4, self);
    if (IOSTATE->errorRaised) return IONIL(self);

    IoState *state = IOSTATE;

    // Bootstrap fallback: no eval loop running
    if (!state->currentFrame) {
        IoMessage *indexMessage = IoMessage_rawArgAt_(m, 0);
        IoMessage *doMessage;
        IoObject *result = IONIL(self);
        double i;
        IoSymbol *slotName = IoMessage_name(indexMessage);
        double startValue = IoMessage_locals_doubleArgAt_(m, locals, 1);
        double endValue = IoMessage_locals_doubleArgAt_(m, locals, 2);
        double increment = 1;

        if (IoMessage_argCount(m) > 4) {
            increment = IoMessage_locals_doubleArgAt_(m, locals, 3);
            doMessage = IoMessage_rawArgAt_(m, 4);
        } else {
            doMessage = IoMessage_rawArgAt_(m, 3);
        }

        IoState_resetStopStatus(state);
        IoState_pushRetainPool(state);

        for (i = startValue;; i += increment) {
            if (increment > 0) { if (i > endValue) break; }
            else { if (i < endValue) break; }
            IoState_clearTopPool(state);
            IoObject_addingRef_(locals, IONUMBER(i));
            PHash_at_put_(IoObject_slots(locals), slotName, IONUMBER(i));
            result = IoMessage_locals_performOn_(doMessage, locals, self);
            if (IoState_handleStatus(state)) break;
        }
        IoState_popRetainPoolExceptFor_(state, result);
        return result;
    }

    IoEvalFrame *frame = state->currentFrame;

    IoMessage *indexMessage = IoMessage_rawArgAt_(m, 0);
    IoSymbol *counterName = IoMessage_name(indexMessage);

    double startValue = IoMessage_locals_doubleArgAt_(m, locals, 1);
    if (state->errorRaised) return IONIL(self);
    double endValue = IoMessage_locals_doubleArgAt_(m, locals, 2);
    if (state->errorRaised) return IONIL(self);
    double increment = 1;
    IoMessage *doMessage;

    if (IoMessage_argCount(m) > 4) {
        increment = IoMessage_locals_doubleArgAt_(m, locals, 3);
        if (state->errorRaised) return IONIL(self);
        doMessage = IoMessage_rawArgAt_(m, 4);
    } else {
        doMessage = IoMessage_rawArgAt_(m, 3);
    }

    // Set up control flow info
    frame->controlFlow.forInfo.bodyMsg = doMessage;
    frame->controlFlow.forInfo.counterName = counterName;
    frame->controlFlow.forInfo.startValue = startValue;
    frame->controlFlow.forInfo.endValue = endValue;
    frame->controlFlow.forInfo.increment = increment;
    frame->controlFlow.forInfo.currentValue = startValue;
    frame->controlFlow.forInfo.lastResult = NULL;
    frame->controlFlow.forInfo.initialized = 0;

    // Change frame state to start FOR evaluation
    frame->state = FRAME_STATE_FOR_EVAL_BODY;

    // Signal that we've set up control flow handling
    state->needsControlFlowHandling = 1;

    return state->ioNil;
}

IO_METHOD(IoObject, return ) {
    /*doc Object return(anObject)
    Return anObject from the current execution block.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoState_return(IOSTATE, v);
    return self;
}

IO_METHOD(IoObject, returnIfNonNil) {
    /*doc Object returnIfNonNil
    Returns the receiver from the current execution block if it is non nil.
    Otherwise returns the receiver locally.
    */

    if (!ISNIL(self)) {
        IoState_return(IOSTATE, self);
    }

    return self;
}

IO_METHOD(IoObject, break) {
    /*doc Object break(optionalReturnValue)
    Break the current loop, if any.
    */

    IoObject *v = IONIL(self);

    if (IoMessage_argCount(m) > 0) {
        v = IoMessage_locals_valueArgAt_(m, locals, 0);
    }

    IoState_break(IOSTATE, v);
    return self;
}

IO_METHOD(IoObject, continue) {
    /*doc Object continue
    Skip the rest of the current loop iteration and start on
    the next, if any.
    */

    IoState_continue(IOSTATE);
    return self;
}

IO_METHOD(IoObject, stopStatus) {
    /*doc Object stopStatus
    Returns the internal IoState->stopStatus.
    */

    int stopStatus;

    IoMessage_locals_valueArgAt_(m, locals, 0);

    stopStatus = IOSTATE->stopStatus;
    IoState_resetStopStatus(IOSTATE);

    return IoState_stopStatusObject(IOSTATE, stopStatus);
}

IO_METHOD(IoObject, if) {
    /*doc Object if(<condition>, <trueMessage>, <optionalFalseMessage>)
    Evaluates trueMessage if condition evaluates to a non-Nil.
    Otherwise evaluates optionalFalseMessage if it is present.
    Returns the result of the evaluated message or Nil if none was evaluated.
    */

    IoState *state = IOSTATE;

    // Bootstrap fallback: if no eval loop is running (currentFrame == NULL),
    // evaluate inline. This only happens during VM initialization.
    if (!state->currentFrame) {
        IoObject *r = IoMessage_locals_valueArgAt_(m, locals, 0);
        const int condition =
            ISTRUE(IoMessage_locals_performOn_(state->asBooleanMessage, r, r));
        const int index = condition ? 1 : 2;
        if (index < IoMessage_argCount(m))
            return IoMessage_locals_valueArgAt_(m, locals, index);
        return IOBOOL(self, condition);
    }

    IoEvalFrame *frame = state->currentFrame;

    // Get the argument messages (not evaluated yet)
    IoMessage *conditionMsg = IoMessage_rawArgAt_(m, 0);
    IoMessage *trueBranch = IoMessage_rawArgAt_(m, 1);
    IoMessage *falseBranch = IoMessage_argCount(m) > 2 ? IoMessage_rawArgAt_(m, 2) : NULL;

    // DEBUG
    if (state->showAllMessages) {
        printf("IF primitive: condition=%s, true=%s, false=%s\n",
               conditionMsg ? CSTRING(IoMessage_name(conditionMsg)) : "NULL",
               trueBranch ? CSTRING(IoMessage_name(trueBranch)) : "NULL",
               falseBranch ? CSTRING(IoMessage_name(falseBranch)) : "NULL");
    }

    // Set up control flow info
    frame->controlFlow.ifInfo.conditionMsg = conditionMsg;
    frame->controlFlow.ifInfo.trueBranch = trueBranch;
    frame->controlFlow.ifInfo.falseBranch = falseBranch;

    // Change frame state to start IF evaluation
    frame->state = FRAME_STATE_IF_EVAL_CONDITION;

    // Signal that we've set up control flow handling
    state->needsControlFlowHandling = 1;

    // Return placeholder (will be replaced by actual result)
    return state->ioNil;
}
