
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
            {"frameCount", IoContinuation_frameCount},
            {"frameStates", IoContinuation_frameStates},
            {"frameMessages", IoContinuation_frameMessages},
            {"asMap", IoContinuation_asMap},
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
    copy->savedCall = src->savedCall;
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

IO_METHOD(IoContinuation, frameCount) {
    /*doc Continuation frameCount
    Returns the number of frames in the captured continuation stack.
    Returns 0 if no state has been captured.
    */
    int count = 0;
    IoEvalFrame *frame = DATA(self)->capturedFrame;
    while (frame) {
        count++;
        frame = frame->parent;
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
        const char *name = IoEvalFrame_stateName(frame->state);
        IoObject *str = IOSYMBOL(name);
        IoList_rawAppend_(list, str);
        frame = frame->parent;
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
        if (frame->message) {
            IoObject *str = IOSYMBOL(CSTRING(IoMessage_name(frame->message)));
            IoList_rawAppend_(list, str);
        } else {
            IoList_rawAppend_(list, state->ioNil);
        }
        frame = frame->parent;
    }
    return list;
}

// ============================================================
// Serialization helpers
// ============================================================

// Macro shortcuts that take explicit state (unlike IOSYMBOL/IONUMBER which use 'self')
#define SYM(s)  IoState_symbolWithCString_(state, (s))
#define NUM(n)  IoNumber_newWithDouble_(state, (double)(n))

// Helper: serialize an IoObject to a portable representation
// Returns the object itself for primitives, or a Map for complex objects
static IoObject *serializeObject_(IoState *state, IoObject *obj) {
    if (!obj) return state->ioNil;
    if (obj == state->ioNil) return state->ioNil;
    if (obj == state->ioTrue) return state->ioTrue;
    if (obj == state->ioFalse) return state->ioFalse;
    if (ISNUMBER(obj)) return obj;
    if (ISSEQ(obj)) return obj;

    // For complex objects, return a Map with type info and slots
    IoMap *map = IoMap_new(state);
    const char *tagName = IoObject_tag(obj)->name;
    IoMap_rawAtPut(map, SYM("_type"), SYM(tagName));

    // For objects with slots, serialize slot names (not values, to avoid cycles)
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

    // Message name
    IoMap_rawAtPut(map, SYM("name"),
                   SYM(CSTRING(IoMessage_name(msg))));

    // Full code representation (can be reparsed)
    UArray *codeUA = IoMessage_descriptionJustSelfAndArgs(msg);
    IoSeq *code = IoSeq_newWithUArray_copy_(state, codeUA, 0);
    IoMap_rawAtPut(map, SYM("code"), code);

    // Full chain code (including next messages)
    UArray *chainUA = IoMessage_description(msg);
    IoSeq *chainCode = IoSeq_newWithUArray_copy_(state, chainUA, 0);
    IoMap_rawAtPut(map, SYM("chainCode"), chainCode);

    // Cached result (literal value)
    IoObject *cached = IoMessage_rawCachedResult(msg);
    if (cached) {
        IoMap_rawAtPut(map, SYM("cachedResult"),
                       serializeObject_(state, cached));
    }

    // Line number and label
    IoMap_rawAtPut(map, SYM("lineNumber"),
                   NUM(IoMessage_rawLineNumber(msg)));
    IoSymbol *label = IoMessage_rawLabel(msg);
    if (label) {
        IoMap_rawAtPut(map, SYM("label"), label);
    }

    // Argument count
    IoMap_rawAtPut(map, SYM("argCount"),
                   NUM(IoMessage_argCount(msg)));

    return map;
}

// Helper: serialize control flow state for a frame
static void serializeControlFlow_(IoState *state, IoEvalFrame *frame,
                                    IoMap *map) {
    switch (frame->state) {
        case FRAME_STATE_IF_EVAL_CONDITION:
        case FRAME_STATE_IF_CONVERT_BOOLEAN:
        case FRAME_STATE_IF_EVAL_BRANCH: {
            IoMap_rawAtPut(map, SYM("conditionResult"),
                           NUM(frame->controlFlow.ifInfo.conditionResult));
            if (frame->controlFlow.ifInfo.conditionMsg) {
                IoMap_rawAtPut(map, SYM("conditionMsg"),
                    serializeMessage_(state, frame->controlFlow.ifInfo.conditionMsg));
            }
            if (frame->controlFlow.ifInfo.trueBranch) {
                IoMap_rawAtPut(map, SYM("trueBranch"),
                    serializeMessage_(state, frame->controlFlow.ifInfo.trueBranch));
            }
            if (frame->controlFlow.ifInfo.falseBranch) {
                IoMap_rawAtPut(map, SYM("falseBranch"),
                    serializeMessage_(state, frame->controlFlow.ifInfo.falseBranch));
            }
            break;
        }

        case FRAME_STATE_WHILE_EVAL_CONDITION:
        case FRAME_STATE_WHILE_CHECK_CONDITION:
        case FRAME_STATE_WHILE_DECIDE:
        case FRAME_STATE_WHILE_EVAL_BODY: {
            IoMap_rawAtPut(map, SYM("conditionResult"),
                           NUM(frame->controlFlow.whileInfo.conditionResult));
            if (frame->controlFlow.whileInfo.conditionMsg) {
                IoMap_rawAtPut(map, SYM("conditionMsg"),
                    serializeMessage_(state, frame->controlFlow.whileInfo.conditionMsg));
            }
            if (frame->controlFlow.whileInfo.bodyMsg) {
                IoMap_rawAtPut(map, SYM("bodyMsg"),
                    serializeMessage_(state, frame->controlFlow.whileInfo.bodyMsg));
            }
            break;
        }

        case FRAME_STATE_LOOP_EVAL_BODY:
        case FRAME_STATE_LOOP_AFTER_BODY: {
            if (frame->controlFlow.loopInfo.bodyMsg) {
                IoMap_rawAtPut(map, SYM("bodyMsg"),
                    serializeMessage_(state, frame->controlFlow.loopInfo.bodyMsg));
            }
            break;
        }

        case FRAME_STATE_FOR_EVAL_SETUP:
        case FRAME_STATE_FOR_EVAL_BODY:
        case FRAME_STATE_FOR_AFTER_BODY: {
            IoMap_rawAtPut(map, SYM("startValue"),
                           NUM(frame->controlFlow.forInfo.startValue));
            IoMap_rawAtPut(map, SYM("endValue"),
                           NUM(frame->controlFlow.forInfo.endValue));
            IoMap_rawAtPut(map, SYM("increment"),
                           NUM(frame->controlFlow.forInfo.increment));
            IoMap_rawAtPut(map, SYM("currentValue"),
                           NUM(frame->controlFlow.forInfo.currentValue));
            IoMap_rawAtPut(map, SYM("initialized"),
                           NUM(frame->controlFlow.forInfo.initialized));
            if (frame->controlFlow.forInfo.counterName) {
                IoMap_rawAtPut(map, SYM("counterName"),
                               frame->controlFlow.forInfo.counterName);
            }
            if (frame->controlFlow.forInfo.bodyMsg) {
                IoMap_rawAtPut(map, SYM("bodyMsg"),
                    serializeMessage_(state, frame->controlFlow.forInfo.bodyMsg));
            }
            break;
        }

        case FRAME_STATE_FOREACH_EVAL_BODY:
        case FRAME_STATE_FOREACH_AFTER_BODY: {
            IoMap_rawAtPut(map, SYM("currentIndex"),
                           NUM(frame->controlFlow.foreachInfo.currentIndex));
            IoMap_rawAtPut(map, SYM("collectionSize"),
                           NUM(frame->controlFlow.foreachInfo.collectionSize));
            IoMap_rawAtPut(map, SYM("direction"),
                           NUM(frame->controlFlow.foreachInfo.direction));
            IoMap_rawAtPut(map, SYM("isEach"),
                           NUM(frame->controlFlow.foreachInfo.isEach));
            if (frame->controlFlow.foreachInfo.indexName) {
                IoMap_rawAtPut(map, SYM("indexName"),
                               frame->controlFlow.foreachInfo.indexName);
            }
            if (frame->controlFlow.foreachInfo.valueName) {
                IoMap_rawAtPut(map, SYM("valueName"),
                               frame->controlFlow.foreachInfo.valueName);
            }
            if (frame->controlFlow.foreachInfo.bodyMsg) {
                IoMap_rawAtPut(map, SYM("bodyMsg"),
                    serializeMessage_(state, frame->controlFlow.foreachInfo.bodyMsg));
            }
            break;
        }

        case FRAME_STATE_CALLCC_EVAL_BLOCK: {
            if (frame->controlFlow.callccInfo.continuation) {
                IoMap_rawAtPut(map, SYM("hasContinuation"),
                               state->ioTrue);
            }
            break;
        }

        default:
            break;
    }
}

// Serialize a single frame to a Map
static IoMap *serializeFrame_(IoState *state, IoEvalFrame *frame) {
    IoMap *map = IoMap_new(state);

    // Frame state
    IoMap_rawAtPut(map, SYM("state"),
                   SYM(IoEvalFrame_stateName(frame->state)));

    // Message info
    if (frame->message) {
        IoMap_rawAtPut(map, SYM("message"),
                       serializeMessage_(state, frame->message));
    }

    // Frame flags
    IoMap_rawAtPut(map, SYM("passStops"), NUM(frame->passStops));
    IoMap_rawAtPut(map, SYM("isNestedEvalRoot"),
                   NUM(frame->isNestedEvalRoot));

    // Argument evaluation state
    IoMap_rawAtPut(map, SYM("argCount"), NUM(frame->argCount));
    IoMap_rawAtPut(map, SYM("currentArgIndex"),
                   NUM(frame->currentArgIndex));

    // Evaluated arguments
    if (frame->argValues && frame->currentArgIndex > 0) {
        IoList *args = IoList_new(state);
        for (int i = 0; i < frame->currentArgIndex; i++) {
            IoList_rawAppend_(args,
                serializeObject_(state, frame->argValues[i]));
        }
        IoMap_rawAtPut(map, SYM("argValues"), args);
    }

    // Object references (type info + identity)
    if (frame->target) {
        IoMap_rawAtPut(map, SYM("targetType"),
                       SYM(IoObject_tag(frame->target)->name));
    }
    if (frame->locals) {
        IoMap_rawAtPut(map, SYM("localsType"),
                       SYM(IoObject_tag(frame->locals)->name));
    }
    if (frame->result) {
        IoMap_rawAtPut(map, SYM("result"),
                       serializeObject_(state, frame->result));
    }
    if (frame->slotValue) {
        IoMap_rawAtPut(map, SYM("slotValue"),
                       serializeObject_(state, frame->slotValue));
    }

    // Block activation info
    if (frame->blockLocals) {
        IoMap_rawAtPut(map, SYM("hasBlockLocals"), state->ioTrue);

        // Serialize the block locals' slot names and values where possible
        if (IoObject_slots(frame->blockLocals)) {
            IoMap *slotsMap = IoMap_new(state);
            PHASH_FOREACH(IoObject_slots(frame->blockLocals), k, v,
                IoMap_rawAtPut(slotsMap, k, serializeObject_(state, v));
            );
            IoMap_rawAtPut(map, SYM("blockLocalsSlots"), slotsMap);
        }
    }

    // Call stop status
    if (frame->call) {
        int stopStatus = IoCall_rawStopStatus((IoCall *)frame->call);
        IoMap_rawAtPut(map, SYM("callStopStatus"), NUM(stopStatus));
    }
    if (frame->savedCall) {
        int stopStatus = IoCall_rawStopStatus((IoCall *)frame->savedCall);
        IoMap_rawAtPut(map, SYM("savedCallStopStatus"),
                       NUM(stopStatus));
    }

    // Control flow specific state
    serializeControlFlow_(state, frame, map);

    return map;
}

IO_METHOD(IoContinuation, asMap) {
    /*doc Continuation asMap
    Returns a Map representation of the captured continuation state.
    The Map contains a "frames" key with a list of frame Maps (top to bottom),
    and metadata about the continuation itself.

    Each frame Map includes:
    - "state": the frame state machine state name
    - "message": Map with message name, code, lineNumber, label
    - "passStops", "isNestedEvalRoot": frame flags
    - "argCount", "currentArgIndex": argument evaluation state
    - "targetType", "localsType": type names of target/locals objects
    - "result": the frame's current result value (if primitive)
    - "blockLocalsSlots": Map of slot name→value for block locals
    - Control flow fields (conditionMsg, bodyMsg, counters, etc.)

    This is the foundation for continuation serialization. Message trees
    can be round-tripped via code strings and reparsing.
    */
    IoState *state = IOSTATE;
    IoMap *result = IoMap_new(state);

    // Metadata
    IoMap_rawAtPut(result, IOSYMBOL("invoked"),
                   IOBOOL(self, DATA(self)->invoked));
    IoMap_rawAtPut(result, IOSYMBOL("multiShot"),
                   IOBOOL(self, DATA(self)->multiShot));

    // Frame count
    int count = 0;
    IoEvalFrame *frame = DATA(self)->capturedFrame;
    while (frame) { count++; frame = frame->parent; }
    IoMap_rawAtPut(result, IOSYMBOL("frameCount"), IONUMBER(count));

    // Frames list
    IoList *frames = IoList_new(state);
    frame = DATA(self)->capturedFrame;
    while (frame) {
        IoList_rawAppend_(frames, serializeFrame_(state, frame));
        frame = frame->parent;
    }
    IoMap_rawAtPut(result, IOSYMBOL("frames"), frames);

    return result;
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
