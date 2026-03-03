
// metadoc Continuation copyright Steve Dekorte 2002, 2025
// metadoc Continuation license BSD revised

#include "IoContinuation.h"
#include "IoState.h"
#include "IoState_eval.h"
#include "IoObject.h"
#include "IoMessage.h"
#include "IoBlock.h"
#include "IoList.h"
#include "IoMap.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "IoEvalFrame.h"
#include "IoMessage_parser.h"
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

    IoState_registerProtoWithId_(state, self, protoId);

    // Methods
    {
        IoMethodTable methodTable[] = {
            {"invoke", IoContinuation_invoke},
            {"isInvoked", IoContinuation_isInvoked},
            {"copy", IoContinuation_copy},
            {"frameCount", IoContinuation_frameCount},
            {"frameStates", IoContinuation_frameStates},
            {"frameMessages", IoContinuation_frameMessages},
            {"asMap", IoContinuation_asMap},
            {"fromMap", IoContinuation_fromMap},
            {NULL, NULL},
        };
        IoObject_addMethodTable_(self, methodTable);
    }

    return self;
}

IoContinuation *IoContinuation_rawClone(IoContinuation *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoContinuationData)));
    DATA(self)->capturedFrame = NULL;
    DATA(self)->capturedLocals = NULL;
    DATA(self)->invoked = 0;
    return self;
}

IoContinuation *IoContinuation_new(void *state) {
    IoObject *proto = IoState_protoWithId_(state, protoId);
    return IOCLONE(proto);
}

void IoContinuation_free(IoContinuation *self) {
    // Frames are GC-managed, just null the pointer
    DATA(self)->capturedFrame = NULL;
    io_free(IoObject_dataPointer(self));
}

void IoContinuation_mark(IoContinuation *self) {
    // Mark captured locals
    IoObject_shouldMarkIfNonNull(DATA(self)->capturedLocals);

    // Mark top captured frame — GC follows the parent chain
    // via IoEvalFrame's own markFunc
    IoObject_shouldMarkIfNonNull(DATA(self)->capturedFrame);
}

static IoEvalFrame *copyFrameChain_(IoState *state, IoEvalFrame *src);

// Capture the current frame stack into this continuation.
// Deep copy the frame chain at capture time. This is necessary because
// popFrame_ zeroes frame data when frames are popped, so a grab-pointer
// capture would lose the frame state after normal return from callcc.
// The deep copy ensures deferred and multi-shot invocations work correctly.
void IoContinuation_captureFrameStack_(IoContinuation *self,
                                        IoEvalFrame *frame,
                                        IoObject *locals) {
    IoState *state = IOSTATE;
    DATA(self)->capturedFrame = copyFrameChain_(state, frame);
    DATA(self)->capturedLocals = locals;
    DATA(self)->invoked = 0;
}

// Deep copy a frame chain (iterative). Used by the copy method.
static IoEvalFrame *copyFrameChain_(IoState *state, IoEvalFrame *src) {
    if (!src) return NULL;

    IoEvalFrame *newTop = NULL;
    IoEvalFrame *prevCopy = NULL;

    while (src) {
        IoEvalFrameData *srcFd = FRAME_DATA(src);

        IoEvalFrame *copy = IoEvalFrame_newWithState(state);
        IoEvalFrameData *copyFd = FRAME_DATA(copy);

        // Copy all data fields
        memcpy(copyFd, srcFd, sizeof(IoEvalFrameData));

        // Deep copy argValues array
        if (srcFd->argValues && srcFd->argCount > 0) {
            copyFd->argValues = io_calloc(srcFd->argCount, sizeof(IoObject *));
            memcpy(copyFd->argValues, srcFd->argValues,
                   srcFd->argCount * sizeof(IoObject *));
        } else {
            copyFd->argValues = NULL;
        }

        copyFd->parent = NULL;

        if (prevCopy) {
            FRAME_DATA(prevCopy)->parent = copy;
        } else {
            newTop = copy;
        }

        prevCopy = copy;
        src = srcFd->parent;
    }

    return newTop;
}

// ============================================================
// Methods
// ============================================================

IO_METHOD(IoContinuation, invoke) {
    /*doc Continuation invoke(value)
    Invokes the continuation, restoring the captured execution state.
    The value argument becomes the result of the original callcc call.
    Continuations are one-shot: they can only be invoked once.
    Use copy to create a fresh continuation for multiple invocations.
    */

    IoState *state = IOSTATE;

    if (DATA(self)->invoked) {
        IoState_error_(state, m,
            "Continuation has already been invoked. "
            "Use copy to create a fresh continuation for multiple invocations.");
        return IONIL(self);
    }

    IoObject *value = IoMessage_locals_valueArgAt_(m, locals, 0);

    if (state->currentFrame == NULL) {
        IoState_error_(state, m,
            "Continuation invoke requires iterative evaluation mode");
        return IONIL(self);
    }

    if (!DATA(self)->capturedFrame) {
        IoState_error_(state, m,
            "Continuation has no captured state");
        return IONIL(self);
    }

    DATA(self)->invoked = 1;

    // Replace the entire frame stack.
    // Old frames become GC garbage (nothing references them).
    state->currentFrame = DATA(self)->capturedFrame;
    DATA(self)->capturedFrame = NULL;  // one-shot transfer

    // Count the restored frames
    state->frameDepth = 0;
    IoEvalFrame *f = state->currentFrame;
    while (f) {
        state->frameDepth++;
        f = FRAME_DATA(f)->parent;
    }

    // Set the result of the callcc to be the invoked value
    if (state->currentFrame) {
        FRAME_DATA(state->currentFrame)->result = value;
    }

    // Signal to the eval loop that the frame stack was replaced
    state->continuationInvoked = 1;

    return value;
}

IO_METHOD(IoContinuation, isInvoked) {
    /*doc Continuation isInvoked
    Returns true if this continuation has been invoked.
    */
    return IOBOOL(self, DATA(self)->invoked);
}

IO_METHOD(IoContinuation, copy) {
    /*doc Continuation copy
    Returns a deep copy of this continuation. The copy has an independent
    frame chain that can be invoked separately. Use this for multi-shot
    or delayed invocation patterns.

    Example:
        saved := nil
        callcc(block(cont,
            saved = cont copy  // deep copy while frames are live
            "normal"
        ))
        // Later: saved invoke("delayed")
    */
    IoState *state = IOSTATE;
    IoContinuation *newCont = IoContinuation_new(state);

    if (DATA(self)->capturedFrame) {
        DATA(newCont)->capturedFrame =
            copyFrameChain_(state, DATA(self)->capturedFrame);
    }
    DATA(newCont)->capturedLocals = DATA(self)->capturedLocals;
    DATA(newCont)->invoked = 0;

    return newCont;
}

IO_METHOD(IoContinuation, frameCount) {
    /*doc Continuation frameCount
    Returns the number of frames in the captured continuation stack.
    Returns 0 if no state has been captured.
    */
    int count = 0;
    IoEvalFrame *frame = DATA(self)->capturedFrame;
    while (frame) {
        count++;
        frame = FRAME_DATA(frame)->parent;
    }
    return IONUMBER(count);
}

IO_METHOD(IoContinuation, frameStates) {
    /*doc Continuation frameStates
    Returns a list of strings describing the state of each frame
    in the captured continuation stack (from top to bottom).
    */
    IoState *state = IOSTATE;
    IoList *list = IoList_new(state);
    IoEvalFrame *frame = DATA(self)->capturedFrame;
    while (frame) {
        IoEvalFrameData *fd = FRAME_DATA(frame);
        const char *name = IoEvalFrame_stateName(fd->state);
        IoObject *str = IOSYMBOL(name);
        IoList_rawAppend_(list, str);
        frame = fd->parent;
    }
    return list;
}

IO_METHOD(IoContinuation, frameMessages) {
    /*doc Continuation frameMessages
    Returns a list of strings showing the current message at each
    frame in the captured continuation stack (from top to bottom).
    */
    IoState *state = IOSTATE;
    IoList *list = IoList_new(state);
    IoEvalFrame *frame = DATA(self)->capturedFrame;
    while (frame) {
        IoEvalFrameData *fd = FRAME_DATA(frame);
        if (fd->message) {
            IoObject *str = IOSYMBOL(CSTRING(IoMessage_name(fd->message)));
            IoList_rawAppend_(list, str);
        } else {
            IoList_rawAppend_(list, state->ioNil);
        }
        frame = fd->parent;
    }
    return list;
}

// ============================================================
// Serialization helpers
// ============================================================

#define SYM(s)  IoState_symbolWithCString_(state, (s))
#define NUM(n)  IoNumber_newWithDouble_(state, (double)(n))

// Helper: serialize an IoObject to a portable representation
static IoObject *serializeObject_(IoState *state, IoObject *obj) {
    if (!obj) return state->ioNil;
    if (obj == state->ioNil) return state->ioNil;
    if (obj == state->ioTrue) return state->ioTrue;
    if (obj == state->ioFalse) return state->ioFalse;
    if (ISNUMBER(obj)) return obj;
    if (ISSEQ(obj)) return obj;

    IoMap *map = IoMap_new(state);
    const char *tagName = IoObject_tag(obj)->name;
    IoMap_rawAtPut(map, SYM("_type"), SYM(tagName));

    if (IoObject_slots(obj)) {
        IoList *slotNames = IoList_new(state);
        PHASH_FOREACH(IoObject_slots(obj), k, v,
            IoList_rawAppend_(slotNames, k);
        );
        IoMap_rawAtPut(map, SYM("_slotNames"), slotNames);
    }
    return map;
}

// Helper: serialize a message tree to a Map
static IoObject *serializeMessage_(IoState *state, IoMessage *msg) {
    if (!msg) return state->ioNil;

    IoMap *map = IoMap_new(state);

    IoMap_rawAtPut(map, SYM("name"),
                   SYM(CSTRING(IoMessage_name(msg))));

    UArray *codeUA = IoMessage_descriptionJustSelfAndArgs(msg);
    IoSeq *code = IoSeq_newWithUArray_copy_(state, codeUA, 0);
    IoMap_rawAtPut(map, SYM("code"), code);

    UArray *chainUA = IoMessage_description(msg);
    IoSeq *chainCode = IoSeq_newWithUArray_copy_(state, chainUA, 0);
    IoMap_rawAtPut(map, SYM("chainCode"), chainCode);

    IoObject *cached = IoMessage_rawCachedResult(msg);
    if (cached) {
        IoMap_rawAtPut(map, SYM("cachedResult"),
                       serializeObject_(state, cached));
    }

    IoMap_rawAtPut(map, SYM("lineNumber"),
                   NUM(IoMessage_rawLineNumber(msg)));
    IoSymbol *label = IoMessage_rawLabel(msg);
    if (label) {
        IoMap_rawAtPut(map, SYM("label"), label);
    }

    IoMap_rawAtPut(map, SYM("argCount"),
                   NUM(IoMessage_argCount(msg)));

    return map;
}

// Helper: serialize control flow state for a frame
static void serializeControlFlow_(IoState *state, IoEvalFrame *frame,
                                    IoMap *map) {
    IoEvalFrameData *fd = FRAME_DATA(frame);
    switch (fd->state) {
        case FRAME_STATE_IF_EVAL_CONDITION:
        case FRAME_STATE_IF_CONVERT_BOOLEAN:
        case FRAME_STATE_IF_EVAL_BRANCH: {
            IoMap_rawAtPut(map, SYM("conditionResult"),
                           NUM(fd->controlFlow.ifInfo.conditionResult));
            if (fd->controlFlow.ifInfo.conditionMsg)
                IoMap_rawAtPut(map, SYM("conditionMsg"),
                    serializeMessage_(state, fd->controlFlow.ifInfo.conditionMsg));
            if (fd->controlFlow.ifInfo.trueBranch)
                IoMap_rawAtPut(map, SYM("trueBranch"),
                    serializeMessage_(state, fd->controlFlow.ifInfo.trueBranch));
            if (fd->controlFlow.ifInfo.falseBranch)
                IoMap_rawAtPut(map, SYM("falseBranch"),
                    serializeMessage_(state, fd->controlFlow.ifInfo.falseBranch));
            break;
        }

        case FRAME_STATE_WHILE_EVAL_CONDITION:
        case FRAME_STATE_WHILE_CHECK_CONDITION:
        case FRAME_STATE_WHILE_DECIDE:
        case FRAME_STATE_WHILE_EVAL_BODY: {
            IoMap_rawAtPut(map, SYM("conditionResult"),
                           NUM(fd->controlFlow.whileInfo.conditionResult));
            if (fd->controlFlow.whileInfo.conditionMsg)
                IoMap_rawAtPut(map, SYM("conditionMsg"),
                    serializeMessage_(state, fd->controlFlow.whileInfo.conditionMsg));
            if (fd->controlFlow.whileInfo.bodyMsg)
                IoMap_rawAtPut(map, SYM("bodyMsg"),
                    serializeMessage_(state, fd->controlFlow.whileInfo.bodyMsg));
            break;
        }

        case FRAME_STATE_LOOP_EVAL_BODY:
        case FRAME_STATE_LOOP_AFTER_BODY: {
            if (fd->controlFlow.loopInfo.bodyMsg)
                IoMap_rawAtPut(map, SYM("bodyMsg"),
                    serializeMessage_(state, fd->controlFlow.loopInfo.bodyMsg));
            break;
        }

        case FRAME_STATE_FOR_EVAL_SETUP:
        case FRAME_STATE_FOR_EVAL_BODY:
        case FRAME_STATE_FOR_AFTER_BODY: {
            IoMap_rawAtPut(map, SYM("startValue"),
                           NUM(fd->controlFlow.forInfo.startValue));
            IoMap_rawAtPut(map, SYM("endValue"),
                           NUM(fd->controlFlow.forInfo.endValue));
            IoMap_rawAtPut(map, SYM("increment"),
                           NUM(fd->controlFlow.forInfo.increment));
            IoMap_rawAtPut(map, SYM("currentValue"),
                           NUM(fd->controlFlow.forInfo.currentValue));
            IoMap_rawAtPut(map, SYM("initialized"),
                           NUM(fd->controlFlow.forInfo.initialized));
            if (fd->controlFlow.forInfo.counterName)
                IoMap_rawAtPut(map, SYM("counterName"),
                               fd->controlFlow.forInfo.counterName);
            if (fd->controlFlow.forInfo.bodyMsg)
                IoMap_rawAtPut(map, SYM("bodyMsg"),
                    serializeMessage_(state, fd->controlFlow.forInfo.bodyMsg));
            break;
        }

        case FRAME_STATE_FOREACH_EVAL_BODY:
        case FRAME_STATE_FOREACH_AFTER_BODY: {
            IoMap_rawAtPut(map, SYM("currentIndex"),
                           NUM(fd->controlFlow.foreachInfo.currentIndex));
            IoMap_rawAtPut(map, SYM("collectionSize"),
                           NUM(fd->controlFlow.foreachInfo.collectionSize));
            IoMap_rawAtPut(map, SYM("direction"),
                           NUM(fd->controlFlow.foreachInfo.direction));
            IoMap_rawAtPut(map, SYM("isEach"),
                           NUM(fd->controlFlow.foreachInfo.isEach));
            if (fd->controlFlow.foreachInfo.indexName)
                IoMap_rawAtPut(map, SYM("indexName"),
                               fd->controlFlow.foreachInfo.indexName);
            if (fd->controlFlow.foreachInfo.valueName)
                IoMap_rawAtPut(map, SYM("valueName"),
                               fd->controlFlow.foreachInfo.valueName);
            if (fd->controlFlow.foreachInfo.bodyMsg)
                IoMap_rawAtPut(map, SYM("bodyMsg"),
                    serializeMessage_(state, fd->controlFlow.foreachInfo.bodyMsg));
            break;
        }

        case FRAME_STATE_CALLCC_EVAL_BLOCK: {
            if (fd->controlFlow.callccInfo.continuation)
                IoMap_rawAtPut(map, SYM("hasContinuation"), state->ioTrue);
            break;
        }

        default:
            break;
    }
}

// Serialize a single frame to a Map
static IoMap *serializeFrame_(IoState *state, IoEvalFrame *frame) {
    IoEvalFrameData *fd = FRAME_DATA(frame);
    IoMap *map = IoMap_new(state);

    IoMap_rawAtPut(map, SYM("state"),
                   SYM(IoEvalFrame_stateName(fd->state)));

    if (fd->message)
        IoMap_rawAtPut(map, SYM("message"),
                       serializeMessage_(state, fd->message));

    IoMap_rawAtPut(map, SYM("passStops"), NUM(fd->passStops));
    IoMap_rawAtPut(map, SYM("isNestedEvalRoot"),
                   NUM(fd->isNestedEvalRoot));

    IoMap_rawAtPut(map, SYM("argCount"), NUM(fd->argCount));
    IoMap_rawAtPut(map, SYM("currentArgIndex"),
                   NUM(fd->currentArgIndex));

    if (fd->argValues && fd->currentArgIndex > 0) {
        IoList *args = IoList_new(state);
        for (int i = 0; i < fd->currentArgIndex; i++) {
            IoList_rawAppend_(args,
                serializeObject_(state, fd->argValues[i]));
        }
        IoMap_rawAtPut(map, SYM("argValues"), args);
    }

    if (fd->target)
        IoMap_rawAtPut(map, SYM("targetType"),
                       SYM(IoObject_tag(fd->target)->name));
    if (fd->locals)
        IoMap_rawAtPut(map, SYM("localsType"),
                       SYM(IoObject_tag(fd->locals)->name));
    if (fd->result)
        IoMap_rawAtPut(map, SYM("result"),
                       serializeObject_(state, fd->result));
    if (fd->slotValue)
        IoMap_rawAtPut(map, SYM("slotValue"),
                       serializeObject_(state, fd->slotValue));

    if (fd->blockLocals) {
        IoMap_rawAtPut(map, SYM("hasBlockLocals"), state->ioTrue);
        if (IoObject_slots(fd->blockLocals)) {
            IoMap *slotsMap = IoMap_new(state);
            PHASH_FOREACH(IoObject_slots(fd->blockLocals), k, v,
                IoMap_rawAtPut(slotsMap, k, serializeObject_(state, v));
            );
            IoMap_rawAtPut(map, SYM("blockLocalsSlots"), slotsMap);
        }
    }

    if (fd->call) {
        int stopStatus = IoCall_rawStopStatus((IoCall *)fd->call);
        IoMap_rawAtPut(map, SYM("callStopStatus"), NUM(stopStatus));
    }
    if (fd->savedCall) {
        int stopStatus = IoCall_rawStopStatus((IoCall *)fd->savedCall);
        IoMap_rawAtPut(map, SYM("savedCallStopStatus"), NUM(stopStatus));
    }

    serializeControlFlow_(state, frame, map);

    return map;
}

IO_METHOD(IoContinuation, asMap) {
    /*doc Continuation asMap
    Returns a Map representation of the captured continuation state.
    */
    IoState *state = IOSTATE;
    IoMap *result = IoMap_new(state);

    IoMap_rawAtPut(result, IOSYMBOL("invoked"),
                   IOBOOL(self, DATA(self)->invoked));

    int count = 0;
    IoEvalFrame *frame = DATA(self)->capturedFrame;
    while (frame) {
        count++;
        frame = FRAME_DATA(frame)->parent;
    }
    IoMap_rawAtPut(result, IOSYMBOL("frameCount"), IONUMBER(count));

    IoList *frames = IoList_new(state);
    frame = DATA(self)->capturedFrame;
    while (frame) {
        IoList_rawAppend_(frames, serializeFrame_(state, frame));
        frame = FRAME_DATA(frame)->parent;
    }
    IoMap_rawAtPut(result, IOSYMBOL("frames"), frames);

    return result;
}

#undef SYM
#undef NUM

// ============================================================
// Deserialization helpers
// ============================================================

#define SYM(s)  IoState_symbolWithCString_(state, (s))
#define NUM(n)  IoNumber_newWithDouble_(state, (double)(n))

static double mapNumberAt_(IoState *state, IoMap *map, const char *key, double defaultVal) {
	IoObject *val = IoMap_rawAt(map, SYM(key));
	if (!val || val == state->ioNil) return defaultVal;
	if (ISNUMBER(val)) return CNUMBER(val);
	return defaultVal;
}

static const char *mapStringAt_(IoState *state, IoMap *map, const char *key) {
	IoObject *val = IoMap_rawAt(map, SYM(key));
	if (!val || val == state->ioNil) return NULL;
	if (ISSEQ(val)) return CSTRING(val);
	return NULL;
}

static IoMessage *deserializeMessage_(IoState *state, IoObject *msgMap) {
	if (!msgMap || msgMap == state->ioNil) return NULL;
	if (!ISMAP(msgMap)) return NULL;

	const char *chainCode = mapStringAt_(state, (IoMap *)msgMap, "chainCode");
	if (!chainCode) {
		chainCode = mapStringAt_(state, (IoMap *)msgMap, "code");
	}
	if (!chainCode) return NULL;

	const char *label = mapStringAt_(state, (IoMap *)msgMap, "label");
	if (!label) label = "fromMap";

	return IoMessage_newFromText_label_(state, chainCode, label);
}

static void deserializeControlFlow_(IoState *state, IoEvalFrame *frame,
                                     IoMap *map) {
	IoEvalFrameData *fd = FRAME_DATA(frame);
	switch (fd->state) {
		case FRAME_STATE_IF_EVAL_CONDITION:
		case FRAME_STATE_IF_CONVERT_BOOLEAN:
		case FRAME_STATE_IF_EVAL_BRANCH: {
			fd->controlFlow.ifInfo.conditionResult =
				(int)mapNumberAt_(state, map, "conditionResult", 0);
			IoObject *condMap = IoMap_rawAt(map, SYM("conditionMsg"));
			fd->controlFlow.ifInfo.conditionMsg =
				deserializeMessage_(state, condMap);
			IoObject *trueMap = IoMap_rawAt(map, SYM("trueBranch"));
			fd->controlFlow.ifInfo.trueBranch =
				deserializeMessage_(state, trueMap);
			IoObject *falseMap = IoMap_rawAt(map, SYM("falseBranch"));
			fd->controlFlow.ifInfo.falseBranch =
				deserializeMessage_(state, falseMap);
			break;
		}

		case FRAME_STATE_WHILE_EVAL_CONDITION:
		case FRAME_STATE_WHILE_CHECK_CONDITION:
		case FRAME_STATE_WHILE_DECIDE:
		case FRAME_STATE_WHILE_EVAL_BODY: {
			fd->controlFlow.whileInfo.conditionResult =
				(int)mapNumberAt_(state, map, "conditionResult", 0);
			IoObject *condMap = IoMap_rawAt(map, SYM("conditionMsg"));
			fd->controlFlow.whileInfo.conditionMsg =
				deserializeMessage_(state, condMap);
			IoObject *bodyMap = IoMap_rawAt(map, SYM("bodyMsg"));
			fd->controlFlow.whileInfo.bodyMsg =
				deserializeMessage_(state, bodyMap);
			break;
		}

		case FRAME_STATE_LOOP_EVAL_BODY:
		case FRAME_STATE_LOOP_AFTER_BODY: {
			IoObject *bodyMap = IoMap_rawAt(map, SYM("bodyMsg"));
			fd->controlFlow.loopInfo.bodyMsg =
				deserializeMessage_(state, bodyMap);
			break;
		}

		case FRAME_STATE_FOR_EVAL_SETUP:
		case FRAME_STATE_FOR_EVAL_BODY:
		case FRAME_STATE_FOR_AFTER_BODY: {
			fd->controlFlow.forInfo.startValue =
				mapNumberAt_(state, map, "startValue", 0);
			fd->controlFlow.forInfo.endValue =
				mapNumberAt_(state, map, "endValue", 0);
			fd->controlFlow.forInfo.increment =
				mapNumberAt_(state, map, "increment", 1);
			fd->controlFlow.forInfo.currentValue =
				mapNumberAt_(state, map, "currentValue", 0);
			fd->controlFlow.forInfo.initialized =
				(int)mapNumberAt_(state, map, "initialized", 0);
			const char *counterName = mapStringAt_(state, map, "counterName");
			if (counterName)
				fd->controlFlow.forInfo.counterName = SYM(counterName);
			IoObject *bodyMap = IoMap_rawAt(map, SYM("bodyMsg"));
			fd->controlFlow.forInfo.bodyMsg =
				deserializeMessage_(state, bodyMap);
			break;
		}

		case FRAME_STATE_FOREACH_EVAL_BODY:
		case FRAME_STATE_FOREACH_AFTER_BODY: {
			fd->controlFlow.foreachInfo.currentIndex =
				(int)mapNumberAt_(state, map, "currentIndex", 0);
			fd->controlFlow.foreachInfo.collectionSize =
				(int)mapNumberAt_(state, map, "collectionSize", 0);
			fd->controlFlow.foreachInfo.direction =
				(int)mapNumberAt_(state, map, "direction", 1);
			fd->controlFlow.foreachInfo.isEach =
				(int)mapNumberAt_(state, map, "isEach", 0);
			const char *indexName = mapStringAt_(state, map, "indexName");
			if (indexName)
				fd->controlFlow.foreachInfo.indexName = SYM(indexName);
			const char *valueName = mapStringAt_(state, map, "valueName");
			if (valueName)
				fd->controlFlow.foreachInfo.valueName = SYM(valueName);
			IoObject *bodyMap = IoMap_rawAt(map, SYM("bodyMsg"));
			fd->controlFlow.foreachInfo.bodyMsg =
				deserializeMessage_(state, bodyMap);
			break;
		}

		case FRAME_STATE_CALLCC_EVAL_BLOCK:
			break;

		default:
			break;
	}
}

// Helper: deserialize a single frame from a Map
static IoEvalFrame *deserializeFrame_(IoState *state, IoObject *frameMap) {
	if (!frameMap || frameMap == state->ioNil || !ISMAP(frameMap)) {
		return NULL;
	}

	IoMap *map = (IoMap *)frameMap;
	IoEvalFrame *frame = IoEvalFrame_newWithState(state);
	IoEvalFrameData *fd = FRAME_DATA(frame);

	const char *stateName = mapStringAt_(state, map, "state");
	fd->state = IoEvalFrame_stateFromName(stateName);

	IoObject *msgMap = IoMap_rawAt(map, SYM("message"));
	if (msgMap && msgMap != state->ioNil) {
		fd->message = deserializeMessage_(state, msgMap);
	}

	fd->passStops = (int)mapNumberAt_(state, map, "passStops", 0);
	fd->isNestedEvalRoot = (int)mapNumberAt_(state, map, "isNestedEvalRoot", 0);

	fd->argCount = (int)mapNumberAt_(state, map, "argCount", 0);
	fd->currentArgIndex = (int)mapNumberAt_(state, map, "currentArgIndex", 0);

	IoObject *argList = IoMap_rawAt(map, SYM("argValues"));
	if (argList && argList != state->ioNil && ISLIST(argList)) {
		int argCount = (int)IoList_rawSize((IoList *)argList);
		if (argCount > 0) {
			fd->argValues = (IoObject **)io_calloc(argCount, sizeof(IoObject *));
			for (int i = 0; i < argCount; i++) {
				fd->argValues[i] = IoList_rawAt_((IoList *)argList, i);
			}
			fd->currentArgIndex = argCount;
		}
	}

	fd->target = state->lobby;
	fd->locals = state->lobby;
	fd->cachedTarget = fd->target;

	IoObject *result = IoMap_rawAt(map, SYM("result"));
	if (result && result != state->ioNil) {
		fd->result = result;
	}

	IoObject *slotValue = IoMap_rawAt(map, SYM("slotValue"));
	if (slotValue && slotValue != state->ioNil) {
		fd->slotValue = slotValue;
	}

	IoObject *hasBlockLocals = IoMap_rawAt(map, SYM("hasBlockLocals"));
	if (hasBlockLocals && hasBlockLocals == state->ioTrue) {
		IoObject *blockLocals = IOCLONE(state->localsProto);
		IoObject_isLocals_(blockLocals, 1);

		IoObject *slotsMap = IoMap_rawAt(map, SYM("blockLocalsSlots"));
		if (slotsMap && slotsMap != state->ioNil && ISMAP(slotsMap)) {
			IoObject_createSlotsIfNeeded(blockLocals);
			PHash *bslots = IoObject_slots(blockLocals);
			PHASH_FOREACH(IoObject_slots((IoObject *)slotsMap), k, v,
				PHash_at_put_(bslots, k, v);
			);
		}

		fd->blockLocals = blockLocals;
	}

	deserializeControlFlow_(state, frame, map);

	return frame;
}

IO_METHOD(IoContinuation, fromMap) {
	/*doc Continuation fromMap(aMap)
	Restores a Continuation from a Map representation produced by asMap.
	Returns self with the restored continuation state.
	*/
	IoState *state = IOSTATE;

	IoObject *mapArg = IoMessage_locals_valueArgAt_(m, locals, 0);
	if (state->errorRaised) return IONIL(self);

	if (!ISMAP(mapArg)) {
		IoState_error_(state, m, "fromMap requires a Map argument");
		return IONIL(self);
	}
	IoMap *map = (IoMap *)mapArg;

	// Clear existing captured frames (GC handles old frames)
	DATA(self)->capturedFrame = NULL;

	// Restore metadata
	IoObject *invokedVal = IoMap_rawAt(map, SYM("invoked"));
	DATA(self)->invoked = (invokedVal && invokedVal == state->ioTrue) ? 1 : 0;

	// Get frames list
	IoObject *framesList = IoMap_rawAt(map, SYM("frames"));
	if (!framesList || framesList == state->ioNil || !ISLIST(framesList)) {
		DATA(self)->capturedFrame = NULL;
		DATA(self)->capturedLocals = NULL;
		return self;
	}

	int frameCount = (int)IoList_rawSize((IoList *)framesList);
	if (frameCount == 0) {
		DATA(self)->capturedFrame = NULL;
		DATA(self)->capturedLocals = NULL;
		return self;
	}

	// Build from the end (oldest) to front (newest)
	IoEvalFrame *childFrame = NULL;

	for (int i = frameCount - 1; i >= 0; i--) {
		IoObject *fMap = IoList_rawAt_((IoList *)framesList, i);
		IoEvalFrame *frame = deserializeFrame_(state, fMap);
		if (!frame) continue;

		FRAME_DATA(frame)->parent = childFrame;
		childFrame = frame;
	}

	DATA(self)->capturedFrame = childFrame;

	if (childFrame) {
		DATA(self)->capturedLocals = FRAME_DATA(childFrame)->locals;
	}

	return self;
}

#undef SYM
#undef NUM

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
    IoEvalFrameData *fd = FRAME_DATA(frame);

    if (frame == NULL) {
        IoState_error_(state, m,
            "callcc requires iterative evaluation mode.");
        return IONIL(self);
    }

    IoMessage *blockMsg = IoMessage_rawArgAt_(m, 0);
    if (!blockMsg) {
        IoState_error_(state, m, "callcc requires a block argument");
        return IONIL(self);
    }

    IoObject *blockArg = IoMessage_locals_performOn_(blockMsg, locals, locals);

    if (!ISBLOCK(blockArg)) {
        IoState_error_(state, m, "callcc requires a block argument");
        return IONIL(self);
    }

    IoBlock *block = (IoBlock *)blockArg;
    IoBlockData *blockData = (IoBlockData *)IoObject_dataPointer(block);

    IoObject *blockLocals = IOCLONE(state->localsProto);
    IoObject_isLocals_(blockLocals, 1);

    IoObject *scope = blockData->scope ? blockData->scope : self;

    IoCall *callObject = IoCall_with(
        state, locals, self, m, self, block, state->currentCoroutine
    );

    IoObject_createSlotsIfNeeded(blockLocals);
    PHash *bslots = IoObject_slots(blockLocals);
    PHash_at_put_(bslots, state->callSymbol, callObject);
    PHash_at_put_(bslots, state->selfSymbol, scope);
    PHash_at_put_(bslots, state->updateSlotSymbol, state->localsUpdateSlotCFunc);

    IoObject_isReferenced_(blockLocals, 0);
    IoObject_isReferenced_(callObject, 0);

    // Set state BEFORE capturing so restored frame is in correct state
    fd->state = FRAME_STATE_CALLCC_EVAL_BLOCK;

    // Capture frame stack — just grabs the pointer (no deep copy)
    IoContinuation *cont = IoContinuation_new(state);
    IoContinuation_captureFrameStack_(cont, frame, locals);

    fd->controlFlow.callccInfo.continuation = cont;
    fd->controlFlow.callccInfo.blockLocals = blockLocals;

    List *argNames = blockData->argNames;
    if (argNames && List_size(argNames) > 0) {
        IoSymbol *paramName = List_at_(argNames, 0);
        IoObject_setSlot_to_(blockLocals, paramName, cont);
    }

    IoEvalFrame *blockFrame = IoState_pushFrame_(state);
    IoEvalFrameData *blockFd = FRAME_DATA(blockFrame);
    blockFd->message = blockData->message;
    blockFd->target = blockLocals;
    blockFd->locals = blockLocals;
    blockFd->cachedTarget = blockLocals;
    blockFd->state = FRAME_STATE_START;
    blockFd->call = callObject;
    blockFd->blockLocals = blockLocals;
    blockFd->passStops = blockData->passStops;

    state->needsControlFlowHandling = 1;

    return state->ioNil;
}
