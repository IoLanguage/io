
// metadoc State copyright Steve Dekorte 2002, 2025
// metadoc State license BSD revised
/*metadoc State description
High-performance iterative evaluator with aggressive optimizations:
- Frame pooling (no malloc/free in hot path)
- Computed gotos (faster than switch)
- Inline fast paths for common cases
- Local variable caching
*/

#include "IoState.h"
#include "IoEvalFrame.h"
#include "IoMessage.h"
#include "IoObject.h"
#include "IoBlock.h"
#include "IoCall.h"

// Frame pool for fast allocation
#define FRAME_POOL_SIZE 256

typedef struct IoFramePool {
    IoEvalFrame frames[FRAME_POOL_SIZE];
    int freeList[FRAME_POOL_SIZE];
    int freeCount;
} IoFramePool;

// Initialize frame pool
static void IoFramePool_init(IoFramePool *pool) {
    pool->freeCount = FRAME_POOL_SIZE;
    for (int i = 0; i < FRAME_POOL_SIZE; i++) {
        pool->freeList[i] = i;
        IoEvalFrame_reset(&pool->frames[i]);
    }
}

// Allocate frame from pool (inline for speed)
static inline IoEvalFrame *IoFramePool_alloc(IoFramePool *pool) {
    if (pool->freeCount > 0) {
        int idx = pool->freeList[--pool->freeCount];
        return &pool->frames[idx];
    }
    // Pool exhausted - fall back to malloc
    return IoEvalFrame_new();
}

// Return frame to pool (inline for speed)
static inline void IoFramePool_free(IoFramePool *pool, IoEvalFrame *frame) {
    // Check if frame is from pool
    if (frame >= &pool->frames[0] &&
        frame < &pool->frames[FRAME_POOL_SIZE]) {
        int idx = frame - &pool->frames[0];
        IoEvalFrame_reset(frame);
        pool->freeList[pool->freeCount++] = idx;
    } else {
        IoEvalFrame_free(frame);
    }
}

// Push frame using pool
static inline IoEvalFrame *IoState_pushFrameFast_(IoState *state, IoFramePool *pool) {
    IoEvalFrame *frame = IoFramePool_alloc(pool);
    frame->parent = state->currentFrame;
    state->currentFrame = frame;
    state->frameDepth++;

    if (state->frameDepth > state->maxFrameDepth) {
        IoState_error_(state, NULL, "Stack overflow: frame depth exceeded %d",
                      state->maxFrameDepth);
    }

    return frame;
}

// Pop frame using pool
static inline void IoState_popFrameFast_(IoState *state, IoFramePool *pool) {
    IoEvalFrame *frame = state->currentFrame;
    if (frame) {
        state->currentFrame = frame->parent;
        state->frameDepth--;
        IoFramePool_free(pool, frame);
    }
}

// Fast activation for blocks
static inline void IoState_activateBlockFast_(IoState *state, IoEvalFrame *callerFrame, IoFramePool *pool) {
    IoBlock *block = (IoBlock *)callerFrame->slotValue;
    IoBlockData *blockData = (IoBlockData *)IoObject_dataPointer(block);

    IoObject *blockLocals = IOCLONE(state->localsProto);
    IoObject_isLocals_(blockLocals, 1);

    IoObject *scope = blockData->scope ? blockData->scope : callerFrame->target;

    IoCall *callObject = IoCall_with(
        state, callerFrame->locals, callerFrame->target,
        callerFrame->message, callerFrame->slotContext,
        block, state->currentCoroutine
    );

    IoObject_createSlotsIfNeeded(blockLocals);
    PHash *bslots = IoObject_slots(blockLocals);
    PHash_at_put_(bslots, state->callSymbol, callObject);
    PHash_at_put_(bslots, state->selfSymbol, scope);
    PHash_at_put_(bslots, state->updateSlotSymbol, state->localsUpdateSlotCFunc);

    List *argNames = blockData->argNames;
    if (callerFrame->argValues) {
        LIST_FOREACH(argNames, i, name,
                     if ((int)i < callerFrame->argCount) {
                         IoObject *arg = callerFrame->argValues[i];
                         IoObject_setSlot_to_(blockLocals, name, arg);
                     });
    }

    IoObject_isReferenced_(blockLocals, 0);
    IoObject_isReferenced_(callObject, 0);

    IoEvalFrame *blockFrame = IoState_pushFrameFast_(state, pool);
    blockFrame->message = blockData->message;
    blockFrame->target = blockLocals;
    blockFrame->locals = blockLocals;
    blockFrame->cachedTarget = blockLocals;
    blockFrame->state = FRAME_STATE_START;
    blockFrame->call = callObject;
    blockFrame->blockLocals = blockLocals;
    blockFrame->passStops = blockData->passStops;
}

// Ultra-fast evaluation loop with computed gotos
IoObject *IoState_evalLoopFast_(IoState *state, IoFramePool *pool) {
    IoEvalFrame *frame;
    IoObject *result = state->ioNil;

    // Local cache to reduce indirection
    IoMessage *m;
    IoMessageData *md;
    IoObject *target;
    IoObject *slotValue;
    IoObject *slotContext;
    IoSymbol *messageName;

#ifdef __GNUC__
    // Computed goto labels (GCC extension - much faster than switch)
    static void *dispatch_table[] = {
        &&STATE_START,
        &&STATE_EVAL_ARGS,
        &&STATE_LOOKUP_SLOT,
        &&STATE_ACTIVATE,
        &&STATE_CONTINUE_CHAIN,
        &&STATE_RETURN
    };

    #define DISPATCH() goto *dispatch_table[frame->state]
    #define CASE(label) label:
#else
    // Fallback to switch for non-GCC compilers
    #define DISPATCH() goto dispatch_switch; dispatch_switch: switch(frame->state)
    #define CASE(label) case label:
#endif

    while ((frame = state->currentFrame) != NULL) {
        if (state->receivedSignal) {
            IoState_callUserInterruptHandler(state);
        }

        if (state->stopStatus != MESSAGE_STOP_STATUS_NORMAL) {
            // Handle stop status...
            IoEvalFrame *f = frame;
            while (f) {
                if (f->blockLocals && !f->passStops) {
                    f->result = state->returnValue;
                    f->state = FRAME_STATE_RETURN;
                    state->stopStatus = MESSAGE_STOP_STATUS_NORMAL;
                    break;
                }
                f = f->parent;
            }
            continue;
        }

        DISPATCH();

        CASE(STATE_START) {
            m = frame->message;
            if (!m) {
                frame->result = state->ioNil;
                frame->state = FRAME_STATE_RETURN;
                DISPATCH();
            }

            md = IOMESSAGEDATA(m);

            // FAST PATH: Semicolon
            if (md->name == state->semicolonSymbol) {
                frame->target = frame->cachedTarget;
                frame->message = md->next;
                if (md->next) {
                    frame->state = FRAME_STATE_START;
                } else {
                    frame->result = frame->target;
                    frame->state = FRAME_STATE_RETURN;
                }
                DISPATCH();
            }

            // FAST PATH: Cached result (literals)
            if (md->cachedResult) {
                frame->result = md->cachedResult;
                if (!md->next) {
                    frame->state = FRAME_STATE_RETURN;
                } else {
                    frame->state = FRAME_STATE_CONTINUE_CHAIN;
                }
                DISPATCH();
            }

            // Need argument evaluation
            frame->argCount = List_size(md->args);
            frame->currentArgIndex = 0;

            if (frame->argCount > 0) {
                frame->argValues = io_calloc(frame->argCount, sizeof(IoObject *));
                frame->state = FRAME_STATE_EVAL_ARGS;
            } else {
                frame->state = FRAME_STATE_LOOKUP_SLOT;
            }
            DISPATCH();
        }

        CASE(STATE_EVAL_ARGS) {
            if (frame->currentArgIndex >= frame->argCount) {
                frame->state = FRAME_STATE_LOOKUP_SLOT;
                DISPATCH();
            }

            IoMessage *argMsg = List_at_(IOMESSAGEDATA(frame->message)->args,
                                         frame->currentArgIndex);

            if (!argMsg) {
                frame->argValues[frame->currentArgIndex] = state->ioNil;
                frame->currentArgIndex++;
                DISPATCH();
            }

            // FAST PATH: Cached argument
            IoMessageData *argMd = IOMESSAGEDATA(argMsg);
            if (argMd->cachedResult && !argMd->next) {
                frame->argValues[frame->currentArgIndex] = argMd->cachedResult;
                frame->currentArgIndex++;
                DISPATCH();
            }

            // Need to evaluate - push frame
            IoEvalFrame *argFrame = IoState_pushFrameFast_(state, pool);
            argFrame->message = argMsg;
            argFrame->target = frame->locals;
            argFrame->locals = frame->locals;
            argFrame->cachedTarget = frame->locals;
            argFrame->state = FRAME_STATE_START;
            DISPATCH();
        }

        CASE(STATE_LOOKUP_SLOT) {
            messageName = IoMessage_name(frame->message);
            slotValue = IoObject_rawGetSlot_context_(frame->target, messageName, &slotContext);

            if (slotValue) {
                frame->slotValue = slotValue;
                frame->slotContext = slotContext;
                frame->state = FRAME_STATE_ACTIVATE;
            } else {
                if (IoObject_isLocals(frame->target)) {
                    frame->result = IoObject_localsForward(frame->target, frame->locals, frame->message);
                } else {
                    frame->result = IoObject_forward(frame->target, frame->locals, frame->message);
                }
                frame->state = FRAME_STATE_CONTINUE_CHAIN;
            }
            DISPATCH();
        }

        CASE(STATE_ACTIVATE) {
            slotValue = frame->slotValue;

            if (IoObject_isActivatable(slotValue)) {
                if (ISBLOCK(slotValue)) {
                    IoState_activateBlockFast_(state, frame, pool);
                    DISPATCH();
                } else {
                    // CFunction
                    IoTagActivateFunc *activateFunc = IoObject_tag(slotValue)->activateFunc;
                    IoState_pushRetainPool(state);
                    frame->result = activateFunc(slotValue, frame->target, frame->locals,
                                                 frame->message, frame->slotContext);
                    IoState_popRetainPoolExceptFor_(state, frame->result);
                    frame->state = FRAME_STATE_CONTINUE_CHAIN;
                }
            } else {
                frame->result = slotValue;
                frame->state = FRAME_STATE_CONTINUE_CHAIN;
            }
            DISPATCH();
        }

        CASE(STATE_CONTINUE_CHAIN) {
            IoMessage *next = IOMESSAGEDATA(frame->message)->next;

            if (next) {
                frame->target = frame->result;
                frame->message = next;
                frame->state = FRAME_STATE_START;

                if (frame->argValues) {
                    io_free(frame->argValues);
                    frame->argValues = NULL;
                }
                frame->argCount = 0;
                frame->currentArgIndex = 0;
            } else {
                frame->state = FRAME_STATE_RETURN;
            }
            DISPATCH();
        }

        CASE(STATE_RETURN) {
            result = frame->result;
            IoEvalFrame *parent = frame->parent;

            if (parent) {
                if (parent->state == FRAME_STATE_EVAL_ARGS) {
                    parent->argValues[parent->currentArgIndex] = result;
                    parent->currentArgIndex++;
                } else if (parent->state == FRAME_STATE_ACTIVATE) {
                    parent->result = result;
                    parent->state = FRAME_STATE_CONTINUE_CHAIN;
                }
            }

            IoState_popFrameFast_(state, pool);
            DISPATCH();
        }
    }

    return result;
}

// Fast entry point
IoObject *IoMessage_locals_performOn_fast(IoMessage *self, IoObject *locals, IoObject *target) {
    IoState *state = IOSTATE;

    // Initialize thread-local frame pool
    static __thread IoFramePool pool;
    static __thread int pool_initialized = 0;
    if (!pool_initialized) {
        IoFramePool_init(&pool);
        pool_initialized = 1;
    }

    // Push initial frame
    IoEvalFrame *frame = IoState_pushFrameFast_(state, &pool);
    frame->message = self;
    frame->target = target;
    frame->locals = locals;
    frame->cachedTarget = target;
    frame->state = FRAME_STATE_START;

    // Run optimized evaluation loop
    IoObject *result = IoState_evalLoopFast_(state, &pool);

    return result;
}
