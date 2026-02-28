
// metadoc EvalFrame copyright Steve Dekorte 2002, 2025
// metadoc EvalFrame license BSD revised
// metadoc EvalFrame category Core

#include "IoEvalFrame.h"
#include "IoState.h"
#include <stdlib.h>
#include <string.h>

static const char *protoId = "EvalFrame";

IoTag *IoEvalFrame_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoEvalFrame_rawClone);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoEvalFrame_mark);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoEvalFrame_free);
    return tag;
}

IoEvalFrame *IoEvalFrame_proto(void *vState) {
    IoState *state = (IoState *)vState;
    IoObject *self = IoObject_new(state);

    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoEvalFrameData)));
    IoObject_tag_(self, IoEvalFrame_newTag(state));

    IoEvalFrameData *fd = FRAME_DATA(self);
    memset(fd, 0, sizeof(IoEvalFrameData));

    IoState_registerProtoWithId_(state, self, protoId);
    return self;
}

IoEvalFrame *IoEvalFrame_rawClone(IoEvalFrame *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoEvalFrameData)));

    IoEvalFrameData *fd = FRAME_DATA(self);
    memset(fd, 0, sizeof(IoEvalFrameData));

    return self;
}

IoEvalFrame *IoEvalFrame_newWithState(void *vState) {
    IoObject *proto = IoState_protoWithId_((IoState *)vState, protoId);
    return IOCLONE(proto);
}

// Free an evaluation frame's data (tag freeFunc)
void IoEvalFrame_free(IoEvalFrame *self) {
    IoEvalFrameData *fd = (IoEvalFrameData *)IoObject_dataPointer(self);
    if (fd) {
        if (fd->argValues) {
            io_free(fd->argValues);
        }
        io_free(fd);
    }
}

// Mark a frame's contents for garbage collection (tag markFunc)
// Marks the parent frame as an IoObject — GC follows the chain transitively.
void IoEvalFrame_mark(IoEvalFrame *self) {
    IoEvalFrameData *fd = (IoEvalFrameData *)IoObject_dataPointer(self);
    if (!fd) return;

    // Mark parent frame (GC follows the chain via parent's own markFunc)
    IoObject_shouldMarkIfNonNull(fd->parent);

    // Mark all IoObject pointers
    IoObject_shouldMarkIfNonNull(fd->message);
    IoObject_shouldMarkIfNonNull(fd->target);
    IoObject_shouldMarkIfNonNull(fd->locals);
    IoObject_shouldMarkIfNonNull(fd->cachedTarget);
    IoObject_shouldMarkIfNonNull(fd->result);
    IoObject_shouldMarkIfNonNull(fd->slotValue);
    IoObject_shouldMarkIfNonNull(fd->slotContext);
    IoObject_shouldMarkIfNonNull(fd->call);
    IoObject_shouldMarkIfNonNull(fd->savedCall);
    IoObject_shouldMarkIfNonNull(fd->blockLocals);

    // Mark evaluated arguments
    if (fd->argValues) {
        int i;
        for (i = 0; i < fd->currentArgIndex; i++) {
            IoObject_shouldMarkIfNonNull(fd->argValues[i]);
        }
    }

    // Mark control flow continuation info based on current state
    switch (fd->state) {
        case FRAME_STATE_IF_EVAL_CONDITION:
        case FRAME_STATE_IF_CONVERT_BOOLEAN:
        case FRAME_STATE_IF_EVAL_BRANCH:
            IoObject_shouldMarkIfNonNull(fd->controlFlow.ifInfo.conditionMsg);
            IoObject_shouldMarkIfNonNull(fd->controlFlow.ifInfo.trueBranch);
            IoObject_shouldMarkIfNonNull(fd->controlFlow.ifInfo.falseBranch);
            break;

        case FRAME_STATE_WHILE_EVAL_CONDITION:
        case FRAME_STATE_WHILE_CHECK_CONDITION:
        case FRAME_STATE_WHILE_DECIDE:
        case FRAME_STATE_WHILE_EVAL_BODY:
            IoObject_shouldMarkIfNonNull(fd->controlFlow.whileInfo.conditionMsg);
            IoObject_shouldMarkIfNonNull(fd->controlFlow.whileInfo.bodyMsg);
            IoObject_shouldMarkIfNonNull(fd->controlFlow.whileInfo.lastResult);
            break;

        case FRAME_STATE_LOOP_EVAL_BODY:
        case FRAME_STATE_LOOP_AFTER_BODY:
            IoObject_shouldMarkIfNonNull(fd->controlFlow.loopInfo.bodyMsg);
            IoObject_shouldMarkIfNonNull(fd->controlFlow.loopInfo.lastResult);
            break;

        case FRAME_STATE_FOR_EVAL_SETUP:
        case FRAME_STATE_FOR_EVAL_BODY:
        case FRAME_STATE_FOR_AFTER_BODY:
            IoObject_shouldMarkIfNonNull(fd->controlFlow.forInfo.bodyMsg);
            IoObject_shouldMarkIfNonNull(fd->controlFlow.forInfo.counterName);
            IoObject_shouldMarkIfNonNull(fd->controlFlow.forInfo.lastResult);
            break;

        case FRAME_STATE_FOREACH_EVAL_BODY:
        case FRAME_STATE_FOREACH_AFTER_BODY:
            IoObject_shouldMarkIfNonNull(fd->controlFlow.foreachInfo.collection);
            IoObject_shouldMarkIfNonNull(fd->controlFlow.foreachInfo.bodyMsg);
            IoObject_shouldMarkIfNonNull(fd->controlFlow.foreachInfo.indexName);
            IoObject_shouldMarkIfNonNull(fd->controlFlow.foreachInfo.valueName);
            IoObject_shouldMarkIfNonNull(fd->controlFlow.foreachInfo.lastResult);
            IoObject_shouldMarkIfNonNull(fd->controlFlow.foreachInfo.mapSource);
            break;

        case FRAME_STATE_CALLCC_EVAL_BLOCK:
            IoObject_shouldMarkIfNonNull(fd->controlFlow.callccInfo.continuation);
            IoObject_shouldMarkIfNonNull(fd->controlFlow.callccInfo.blockLocals);
            break;

        case FRAME_STATE_CORO_WAIT_CHILD:
        case FRAME_STATE_CORO_YIELDED:
            IoObject_shouldMarkIfNonNull(fd->controlFlow.coroInfo.childCoroutine);
            break;

        case FRAME_STATE_DO_EVAL:
        case FRAME_STATE_DO_WAIT:
            IoObject_shouldMarkIfNonNull(fd->controlFlow.doInfo.codeMessage);
            IoObject_shouldMarkIfNonNull(fd->controlFlow.doInfo.evalTarget);
            IoObject_shouldMarkIfNonNull(fd->controlFlow.doInfo.evalLocals);
            break;

        default:
            break;
    }
}

// Return a human-readable name for a frame state
const char *IoEvalFrame_stateName(IoFrameState state) {
    switch (state) {
        case FRAME_STATE_START: return "start";
        case FRAME_STATE_EVAL_ARGS: return "evalArgs";
        case FRAME_STATE_LOOKUP_SLOT: return "lookupSlot";
        case FRAME_STATE_ACTIVATE: return "activate";
        case FRAME_STATE_CONTINUE_CHAIN: return "continueChain";
        case FRAME_STATE_RETURN: return "return";
        case FRAME_STATE_IF_EVAL_CONDITION: return "if:evalCondition";
        case FRAME_STATE_IF_CONVERT_BOOLEAN: return "if:convertBoolean";
        case FRAME_STATE_IF_EVAL_BRANCH: return "if:evalBranch";
        case FRAME_STATE_WHILE_EVAL_CONDITION: return "while:evalCondition";
        case FRAME_STATE_WHILE_CHECK_CONDITION: return "while:checkCondition";
        case FRAME_STATE_WHILE_DECIDE: return "while:decide";
        case FRAME_STATE_WHILE_EVAL_BODY: return "while:evalBody";
        case FRAME_STATE_LOOP_EVAL_BODY: return "loop:evalBody";
        case FRAME_STATE_LOOP_AFTER_BODY: return "loop:afterBody";
        case FRAME_STATE_FOR_EVAL_SETUP: return "for:evalSetup";
        case FRAME_STATE_FOR_EVAL_BODY: return "for:evalBody";
        case FRAME_STATE_FOR_AFTER_BODY: return "for:afterBody";
        case FRAME_STATE_FOREACH_EVAL_BODY: return "foreach:evalBody";
        case FRAME_STATE_FOREACH_AFTER_BODY: return "foreach:afterBody";
        case FRAME_STATE_CALLCC_EVAL_BLOCK: return "callcc:evalBlock";
        case FRAME_STATE_CORO_WAIT_CHILD: return "coro:waitChild";
        case FRAME_STATE_CORO_YIELDED: return "coro:yielded";
        case FRAME_STATE_DO_EVAL: return "do:eval";
        case FRAME_STATE_DO_WAIT: return "do:wait";
        default: return "unknown";
    }
}

// Return a frame state enum from its string name (reverse of IoEvalFrame_stateName)
IoFrameState IoEvalFrame_stateFromName(const char *name) {
	if (!name) return FRAME_STATE_START;
	if (strcmp(name, "start") == 0) return FRAME_STATE_START;
	if (strcmp(name, "evalArgs") == 0) return FRAME_STATE_EVAL_ARGS;
	if (strcmp(name, "lookupSlot") == 0) return FRAME_STATE_LOOKUP_SLOT;
	if (strcmp(name, "activate") == 0) return FRAME_STATE_ACTIVATE;
	if (strcmp(name, "continueChain") == 0) return FRAME_STATE_CONTINUE_CHAIN;
	if (strcmp(name, "return") == 0) return FRAME_STATE_RETURN;
	if (strcmp(name, "if:evalCondition") == 0) return FRAME_STATE_IF_EVAL_CONDITION;
	if (strcmp(name, "if:convertBoolean") == 0) return FRAME_STATE_IF_CONVERT_BOOLEAN;
	if (strcmp(name, "if:evalBranch") == 0) return FRAME_STATE_IF_EVAL_BRANCH;
	if (strcmp(name, "while:evalCondition") == 0) return FRAME_STATE_WHILE_EVAL_CONDITION;
	if (strcmp(name, "while:checkCondition") == 0) return FRAME_STATE_WHILE_CHECK_CONDITION;
	if (strcmp(name, "while:decide") == 0) return FRAME_STATE_WHILE_DECIDE;
	if (strcmp(name, "while:evalBody") == 0) return FRAME_STATE_WHILE_EVAL_BODY;
	if (strcmp(name, "loop:evalBody") == 0) return FRAME_STATE_LOOP_EVAL_BODY;
	if (strcmp(name, "loop:afterBody") == 0) return FRAME_STATE_LOOP_AFTER_BODY;
	if (strcmp(name, "for:evalSetup") == 0) return FRAME_STATE_FOR_EVAL_SETUP;
	if (strcmp(name, "for:evalBody") == 0) return FRAME_STATE_FOR_EVAL_BODY;
	if (strcmp(name, "for:afterBody") == 0) return FRAME_STATE_FOR_AFTER_BODY;
	if (strcmp(name, "foreach:evalBody") == 0) return FRAME_STATE_FOREACH_EVAL_BODY;
	if (strcmp(name, "foreach:afterBody") == 0) return FRAME_STATE_FOREACH_AFTER_BODY;
	if (strcmp(name, "callcc:evalBlock") == 0) return FRAME_STATE_CALLCC_EVAL_BLOCK;
	if (strcmp(name, "coro:waitChild") == 0) return FRAME_STATE_CORO_WAIT_CHILD;
	if (strcmp(name, "coro:yielded") == 0) return FRAME_STATE_CORO_YIELDED;
	if (strcmp(name, "do:eval") == 0) return FRAME_STATE_DO_EVAL;
	if (strcmp(name, "do:wait") == 0) return FRAME_STATE_DO_WAIT;
	return FRAME_STATE_START;
}

// Reset frame data to initial state (for reuse)
void IoEvalFrame_reset(IoEvalFrame *self) {
    if (!self) return;

    IoEvalFrameData *fd = FRAME_DATA(self);
    if (!fd) return;

    // Free argument values if allocated
    if (fd->argValues) {
        io_free(fd->argValues);
    }

    // Zero all fields
    memset(fd, 0, sizeof(IoEvalFrameData));
}
