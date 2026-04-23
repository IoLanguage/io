
// metadoc State copyright Steve Dekorte 2002, 2025
// metadoc State license BSD revised
/*metadoc State description
High-performance iterative evaluator with aggressive optimizations:
- Frame pooling (no malloc/free in hot path)
- Computed gotos (faster than switch)
- Inline fast paths for common cases
- Local variable caching
*/

/*cmetadoc State description
Experimental fast-path evaluator alongside IoState_iterative.c.
Uses a fixed-size thread-local IoFramePool (no malloc per frame) and
— under GCC — a computed-goto dispatch table in place of the main
eval loop's switch. Supports only the minimal set of frame states
(START / EVAL_ARGS / LOOKUP_SLOT / ACTIVATE / CONTINUE_CHAIN / RETURN):
anything that needs special-form handling (if/while/for/callcc/
coroutine switches) is not covered here and must use the primary
iterative evaluator. IoMessage_locals_performOn_fast is the entry
point; it is currently opt-in rather than the default because the
feature coverage is incomplete.
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

/*cdoc State IoFramePool_init(pool)
Resets a FRAME_POOL_SIZE-entry frame pool to empty: initializes the
free-list index stack and zeroes each embedded IoEvalFrame via
IoEvalFrame_reset. Called lazily the first time a thread enters
IoMessage_locals_performOn_fast.
*/
static void IoFramePool_init(IoFramePool *pool) {
    pool->freeCount = FRAME_POOL_SIZE;
    for (int i = 0; i < FRAME_POOL_SIZE; i++) {
        pool->freeList[i] = i;
        IoEvalFrame_reset(&pool->frames[i]);
    }
}

/*cdoc State IoFramePool_alloc(pool)
Pops a free index off the pool's stack and returns the pre-allocated
frame at that slot. Falls back to IoEvalFrame_new (a heap allocation)
when the pool is exhausted; the caller pairs every alloc with an
IoFramePool_free that routes the frame back correctly whether it was
pooled or heap-backed.
*/
static inline IoEvalFrame *IoFramePool_alloc(IoFramePool *pool) {
    if (pool->freeCount > 0) {
        int idx = pool->freeList[--pool->freeCount];
        return &pool->frames[idx];
    }
    // Pool exhausted - fall back to malloc
    return IoEvalFrame_new();
}

/*cdoc State IoFramePool_free(pool, frame)
Pointer-range check decides whether frame lives inside the pool's
array; pool-owned frames get reset and their index pushed back on
the free list, heap-allocated overflow frames are passed to
IoEvalFrame_free. The index is recovered by pointer arithmetic
relative to pool->frames[0].
*/
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

/*cdoc State IoState_pushFrameFast_(state, pool)
Pool-backed analogue of IoState_pushFrame_. Pulls a frame from the
pool, links it to state->currentFrame as the new top, bumps
frameDepth, and raises a stack-overflow error when the configured
max is crossed. Like the slow-path version it leaves newly-pushed
fields uninitialized — callers are expected to stamp message /
target / locals / state immediately after.
*/
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

/*cdoc State IoState_popFrameFast_(state, pool)
Inverse of IoState_pushFrameFast_. Unlinks the current frame from the
parent chain, decrements frameDepth, and returns the frame to the
pool. Safe to call when state->currentFrame is NULL (no-op).
*/
static inline void IoState_popFrameFast_(IoState *state, IoFramePool *pool) {
    IoEvalFrame *frame = state->currentFrame;
    if (frame) {
        state->currentFrame = frame->parent;
        state->frameDepth--;
        IoFramePool_free(pool, frame);
    }
}

/*cdoc State IoState_activateBlockFast_(state, callerFrame, pool)
Inlines block activation on the fast path: builds blockLocals and a
Call object, binds formal args from callerFrame->argValues, then
pushes a new pool frame in FRAME_STATE_START ready to evaluate the
block's body. Differs from IoState_activateBlock_ in IoState_iterative.c
only in its pool-backed allocation and inlined helper calls; feature
coverage (e.g. variadic blocks, TCO) is intentionally narrower.
*/
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

/*cdoc State IoState_evalLoopFast_(state, pool)
Fast-path analogue of IoState_evalLoop_. Drives the same frame state
machine, but uses a GCC computed-goto dispatch table (and a switch
fallback for other compilers) and covers only the six core states,
so it must not be called with frames in special-form states. Polls
state->receivedSignal and stopStatus between iterations and unwinds
via FRAME_STATE_RETURN until the frame chain is empty. Returns the
last frame's result, which becomes the outer IoObject *.
*/
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

/*cdoc State IoMessage_locals_performOn_fast(self, locals, target)
Fast-path entry equivalent of IoMessage_locals_performOn_. Lazily
initializes a thread-local IoFramePool (__thread storage) on first
use, pushes the initial frame with the caller's message/target/locals,
and runs IoState_evalLoopFast_ to completion. Because the pool is
thread-local and the fast loop does not support cross-coroutine
handoff, this is only safe for self-contained evaluations that
never yield or invoke a continuation.
*/
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
