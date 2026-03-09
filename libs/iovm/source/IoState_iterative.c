
// metadoc State copyright Steve Dekorte 2002, 2025
// metadoc State license BSD revised
// metadoc State category Core
/*metadoc State description
Iterative (non-recursive) message evaluation for Io.
This provides a C-stack-independent evaluator that enables
first-class continuations, serializable execution state,
and network-portable coroutines.
*/

#include "IoState.h"
#include "IoEvalFrame.h"
#include "IoMessage.h"
#include "IoObject.h"
#include "IoBlock.h"
#include "IoCall.h"
#include "IoCFunction.h"
#include "IoNumber.h"
#include "IoCoroutine.h"
#include "IoMap.h"
#include "IoList.h"
#include <stdio.h>
#include <string.h>

// Forward declarations
static void IoState_activateBlock_(IoState *state, IoEvalFrame *callerFrame);
static void IoState_activateBlockTCO_(IoState *state, IoEvalFrame *blockFrame);


// Debug: Validate that a frame's message field is a Message.
// This should be called after every frame setup where message is assigned.
#define VALIDATE_FRAME(f, location) do { \
    IoEvalFrameData *_vfd = FRAME_DATA(f); \
    if (_vfd->message && !ISMESSAGE(_vfd->message)) { \
        fprintf(stderr, "FRAME VALIDATION FAILED at %s\n", location); \
        fprintf(stderr, "  frame=%p, message=%p (tag=%s), target=%p, locals=%p\n", \
                (void*)(f), (void*)_vfd->message, \
                IoObject_tag((IoObject*)_vfd->message) ? IoObject_tag((IoObject*)_vfd->message)->name : "NULL", \
                (void*)_vfd->target, (void*)_vfd->locals); \
        fprintf(stderr, "  parent=%p, state=%d\n", (void*)_vfd->parent, _vfd->state); \
        fflush(stderr); \
        abort(); \
    } \
} while(0)

// Check for pre-evaluated argument in the current eval frame.
// Called from IoMessage_locals_quickValueArgAt_ (IoState_inline.h).
// Separated into a function to avoid circular IoEvalFrame.h includes.
IoObject *IoState_preEvalArgAt_(IoState *self, IoMessage *msg, int n) {
    IoEvalFrame *frame = self->currentFrame;
    if (!frame) return NULL;
    IoEvalFrameData *fd = FRAME_DATA(frame);
    if (
        fd->argValues &&
        msg == fd->message &&
        n < fd->argCount &&
        fd->argValues[n] != NULL) {
        return fd->argValues[n];
    }
    return NULL;
}

// Push a new frame onto the evaluation stack.
// Reuses pooled frames when available; allocates new IoObject otherwise.
IoEvalFrame *IoState_pushFrame_(IoState *state) {
    IoEvalFrame *frame;
    IoEvalFrameData *fd;

    if (state->framePoolCount > 0) {
        // Reuse a pooled frame (already a valid collector object)
        // Pointer fields were cleared on pool return (GC safety).
        // Only initialize the fields we actually use.
        frame = state->framePool[--state->framePoolCount];
        fd = FRAME_DATA(frame);
    } else {
        frame = IoEvalFrame_newWithState(state);
        fd = FRAME_DATA(frame);
    }

    // Selective initialization: set only the fields that matter.
    // The controlFlow union is left uninitialized — it's set by
    // control flow primitives before use.
    fd->message = NULL;
    fd->target = NULL;
    fd->locals = NULL;
    fd->cachedTarget = NULL;
    fd->parent = NULL;
    fd->state = FRAME_STATE_START;
    fd->argCount = 0;
    fd->currentArgIndex = 0;
    fd->argValues = NULL;
    // inlineArgs left uninitialized — set when used
    fd->result = NULL;
    fd->slotValue = NULL;
    fd->slotContext = NULL;
    fd->call = NULL;
    fd->savedCall = NULL;
    fd->blockLocals = NULL;
    fd->passStops = 0;
    fd->isNestedEvalRoot = 0;
    fd->retainPoolMark = 0;

    fd->parent = state->currentFrame;
    state->currentFrame = frame;
    state->frameDepth++;

    if (state->frameDepth > state->maxFrameDepth) {
        IoState_error_(state, NULL, "Stack overflow: frame depth exceeded %d",
                      state->maxFrameDepth);
    }

    return frame;
}

// Pop a frame from the evaluation stack.
// Returns the frame to the pool if space; otherwise lets GC reclaim it.
void IoState_popFrame_(IoState *state) {
    IoEvalFrame *frame = state->currentFrame;
    if (frame) {
        IoEvalFrameData *fd = FRAME_DATA(frame);
        state->currentFrame = fd->parent;
        state->frameDepth--;

        // Free heap-allocated argValues (not inline buffer)
        if (fd->argValues && fd->argValues != fd->inlineArgs) {
            io_free(fd->argValues);
        }
        fd->argValues = NULL;

        // Clear only pointer fields so pooled frames don't hold stale
        // GC references. Without this, GC marking of pooled frames keeps
        // dead objects alive (e.g. WeakLink targets that should be collected).
        fd->message = NULL;
        fd->target = NULL;
        fd->locals = NULL;
        fd->cachedTarget = NULL;
        fd->parent = NULL;
        fd->inlineArgs[0] = NULL;
        fd->inlineArgs[1] = NULL;
        fd->inlineArgs[2] = NULL;
        fd->inlineArgs[3] = NULL;
        fd->result = NULL;
        fd->slotValue = NULL;
        fd->slotContext = NULL;
        fd->call = NULL;
        fd->savedCall = NULL;
        fd->blockLocals = NULL;
        // Reset state so GC mark function won't walk stale controlFlow pointers.
        // This is cheaper than memset of the ~80-byte controlFlow union.
        fd->state = FRAME_STATE_START;

        // Return to pool if space available
        if (state->framePoolCount < FRAME_POOL_SIZE) {
            state->framePool[state->framePoolCount++] = frame;
        }
        // else: frame stays in collector, GC reclaims when unreferenced
    }
}

// Unwind frames for error handling, respecting nested eval boundaries.
//
// When IoMessage_locals_performOn_iterative creates a nested eval loop,
// it marks its root frame with isNestedEvalRoot. If an error occurs
// inside the nested eval, we must NOT pop frames beyond the boundary —
// doing so would corrupt the outer eval loop's frame pointers, especially
// when coroutine switches are involved.
//
// Returns 1 if a nested eval boundary was found (caller should return
// from the eval loop with errorRaised re-set).
// Returns 0 if all frames were popped (caller should fall through to
// the frame=NULL handler for coro switching / exit).
static int IoState_unwindFramesForError_(IoState *state) {
    while (state->currentFrame) {
        IoEvalFrameData *ufd = FRAME_DATA(state->currentFrame);
        int isRoot = ufd->isNestedEvalRoot;
        if (ufd->retainPoolMark) {
            IoState_popRetainPool(state);
        }
        IoState_popFrame_(state);
        if (isRoot) {
            // Hit a nested eval boundary. Re-set errorRaised so the
            // C caller (IoMessage_locals_performOn_iterative) propagates
            // the error to the outer eval loop.
            state->errorRaised = 1;
            return 1;
        }
    }
    return 0;  // All frames popped
}

// Main iterative evaluation loop
//
// COROUTINE ARCHITECTURE:
// This is the ONE eval loop that runs on the main C stack. It processes
// whatever frames are in state->currentFrame. Coroutine "switching" just
// changes which frame stack is current - the loop keeps running.
//
// When a coroutine's frame stack becomes empty:
// - If it has a parent, switch back to parent and continue
// - If no parent (main coro), exit the loop
//
// Define DEBUG_EVAL_LOOP to enable verbose debug output
// #define DEBUG_EVAL_LOOP 1

IoObject *IoState_evalLoop_(IoState *state) {
    IoEvalFrame *frame;
    IoEvalFrameData *fd;
    IoObject *result = state->ioNil;

#ifdef DEBUG_EVAL_LOOP
    static int loopIter = 0;
#endif

    while (1) {
        frame = state->currentFrame;
        fd = frame ? FRAME_DATA(frame) : NULL;

        // Check if System exit was called
        if (state->shouldExit) {
            while (state->currentFrame) IoState_popFrame_(state);
            return result;
        }

#ifdef DEBUG_EVAL_LOOP
        loopIter++;
        // Always print to trace coro issues
        fprintf(stderr, "evalLoop iter %d: frame=%p, coro=%p, msg=%s, state=%d, nestedDepth=%d\n",
                loopIter, (void*)frame, (void*)state->currentCoroutine,
                (frame && fd->message) ? CSTRING(IoMessage_name(fd->message)) : "NULL",
                frame ? fd->state : -1,
                state->nestedEvalDepth);
        fflush(stderr);
#endif

        // Check if current coroutine's frame stack is empty
        if (!frame) {
#ifdef DEBUG_EVAL_LOOP
            fprintf(stderr, "evalLoop: frame=NULL, coro=%p, nestedDepth=%d, result=%p\n",
                    (void*)state->currentCoroutine, state->nestedEvalDepth, (void*)result);
            fflush(stderr);
#endif

            IoCoroutine *current = state->currentCoroutine;
            IoCoroutine *parent = IoCoroutine_rawParentCoroutine(current);

            // Check if this is a child coro started via coro swap that has
            // finished. The parent's saved frameStack will have a
            // CORO_WAIT_CHILD or CORO_YIELDED frame. We must check
            // this BEFORE the nestedEvalDepth check, because a coro swap
            // child can finish within a nested eval loop.
            if (parent && ISCOROUTINE(parent)) {
                IoEvalFrame *parentTopFrame = ((IoCoroutineData *)IoObject_dataPointer(parent))->frameStack;
                if (parentTopFrame) {
                IoEvalFrameData *parentTopFd = FRAME_DATA(parentTopFrame);
                if (parentTopFd->state == FRAME_STATE_CORO_WAIT_CHILD ||
                    parentTopFd->state == FRAME_STATE_CORO_YIELDED) {
#ifdef DEBUG_EVAL_LOOP
                    fprintf(stderr, "evalLoop: coro finished, returning to parent coro (parent state=%d)\n",
                            parentTopFd->state);
                    fflush(stderr);
#endif
                    // Child coro finished - restore parent
                    IoCoroutine_rawSetResult_(current, result);
                    IoCoroutine_saveState_(current, state);

                    IoCoroutine_restoreState_(parent, state);
                    IoState_setCurrentCoroutine_(state, parent);

                    // Parent's top frame: transition to CONTINUE_CHAIN
                    frame = state->currentFrame;
                    fd = FRAME_DATA(frame);
                    if (frame && (fd->state == FRAME_STATE_CORO_WAIT_CHILD ||
                                  fd->state == FRAME_STATE_CORO_YIELDED)) {
                        fd->result = result;
                        fd->state = FRAME_STATE_CONTINUE_CHAIN;
                    }
                    continue;  // Continue with parent's frames
                }
                }
            }

            // If we're in a nested eval (e.g., from IoCoroutine_try), return
            // immediately. The C caller (IoCoroutine_try or rawRun) handles
            // cleanup and coroutine state restoration.
            if (state->nestedEvalDepth > 0) {
                return result;
            }

            // Not nested, not a coro swap child — check for regular parent
            if (parent && ISCOROUTINE(parent)) {
#ifdef DEBUG_EVAL_LOOP
                fprintf(stderr, "evalLoop: returning to parent coro\n");
                fflush(stderr);
#endif

                // Child coroutine finished - return to parent
                IoCoroutine_rawSetResult_(current, result);

                // Restore parent's frame stack
                IoCoroutine_restoreState_(parent, state);
                IoState_setCurrentCoroutine_(state, parent);

                // Parent's top frame should be waiting for us
                frame = state->currentFrame;
                fd = frame ? FRAME_DATA(frame) : NULL;
#ifdef DEBUG_EVAL_LOOP
                fprintf(stderr, "evalLoop: parent frame=%p, state=%d\n",
                        (void*)frame, fd ? fd->state : -1);
                fflush(stderr);
#endif

                if (frame && fd && fd->state == FRAME_STATE_CORO_WAIT_CHILD) {
                    fd->result = result;
                    fd->state = FRAME_STATE_CONTINUE_CHAIN;
                }
                continue;  // Continue with parent's frames
            }

#ifdef DEBUG_EVAL_LOOP
            fprintf(stderr, "evalLoop: exiting (no parent)\n");
            fflush(stderr);
#endif
            // Main coroutine done - check for uncaught exception
            {
                IoObject *exc = IoCoroutine_rawException(current);
                if (exc != state->ioNil) {
                    IoCoroutine_rawPrintBackTrace(current);
                }
            }
            return result;
        }

        IoMessage *m;
        IoMessageData *md;

#ifdef DEBUG_FRAME_VALIDATION
        // Validate frame integrity (enabled by -DDEBUG_FRAME_VALIDATION)
        if (!ISEVALFRAME(frame)) {
            fprintf(stderr, "CORRUPTION: currentFrame is not an EvalFrame! frame=%p, tag=%s\n",
                    (void*)frame,
                    IoObject_tag(frame) ? IoObject_tag(frame)->name : "NULL");
            fflush(stderr);
            abort();
        }

        if (fd->message && !ISMESSAGE(fd->message)) {
            fprintf(stderr, "CORRUPTION: fd->message is not a Message!\n");
            fprintf(stderr, "  frame=%p, state=%d, message=%p, tag=%s\n",
                    (void*)frame, fd->state,
                    (void*)fd->message,
                    IoObject_tag(fd->message) ? IoObject_tag(fd->message)->name : "NULL");
            fprintf(stderr, "  target=%p, locals=%p, result=%p, cachedTarget=%p\n",
                    (void*)fd->target, (void*)fd->locals,
                    (void*)fd->result, (void*)fd->cachedTarget);
            fprintf(stderr, "  slotValue=%p, slotContext=%p, blockLocals=%p\n",
                    (void*)fd->slotValue, (void*)fd->slotContext,
                    (void*)fd->blockLocals);
            fprintf(stderr, "  argValues=%p, argCount=%d, currentArgIndex=%d\n",
                    (void*)fd->argValues, fd->argCount, fd->currentArgIndex);
            fprintf(stderr, "  parent=%p\n", (void*)fd->parent);
            // Walk parent chain
            IoEvalFrame *p = fd->parent;
            int depth = 0;
            while (p && depth < 10) {
                IoEvalFrameData *pd = FRAME_DATA(p);
                fprintf(stderr, "  parent[%d]: frame=%p, state=%d, msg=%p (%s)\n",
                        depth, (void*)p, pd->state, (void*)pd->message,
                        (pd->message && ISMESSAGE(pd->message))
                            ? CSTRING(IoMessage_name(pd->message)) : "INVALID/UNKNOWN");
                fprintf(stderr, "    target=%p, locals=%p, blockLocals=%p\n",
                        (void*)pd->target, (void*)pd->locals, (void*)pd->blockLocals);
                p = pd->parent;
                depth++;
            }
            fflush(stderr);
            abort();
        }
#endif

        // Check for signals (Ctrl-C, etc.)
        if (state->receivedSignal) {
            IoState_callUserInterruptHandler(state);
        }

        // Stop status (return/break/continue) propagates naturally through
        // CONTINUE_CHAIN → RETURN at each frame level. Method frames
        // (blockLocals && !passStops) catch all stop statuses in
        // CONTINUE_CHAIN. This matches the recursive evaluator where
        // performOn_ checks stopStatus after each message.
        //
        // NOTE: We intentionally do NOT eagerly intercept 'return' here.
        // Eager interception would bypass frames that need to observe
        // state->stopStatus (e.g., IoObject_stopStatus used by
        // relayStopStatus). Instead, return propagates frame-by-frame
        // like break/continue.

        // Generic errorRaised check: catches errors raised outside of ACTIVATE
        // (e.g., from forward handlers in LOOKUP_SLOT, or other CFunction calls).
        // The ACTIVATE case has its own errorRaised handler that also pops the
        // retain pool — errors raised during ACTIVATE are caught there first.
        if (state->errorRaised) {
#ifdef DEBUG_EVAL_LOOP
            fprintf(stderr, "evalLoop: top-level errorRaised, unwinding frames\n");
            fflush(stderr);
#endif
            state->errorRaised = 0;
            if (IoState_unwindFramesForError_(state)) {
                return state->ioNil;  // Hit nested eval boundary
            }
            continue;  // frame=NULL handler takes over
        }

        // Show message if debugging
        if (state->showAllMessages && fd->message) {
            printf("M:%s:%s:%i\n", CSTRING(IoMessage_name(fd->message)),
                   CSTRING(IoMessage_rawLabel(fd->message)),
                   IoMessage_rawLineNumber(fd->message));
        }

        switch (fd->state) {

        start_message:
        case FRAME_STATE_START: {
            // Starting evaluation of a message
            m = fd->message;
            if (!m) {
                // No message to evaluate, return nil
                fd->result = state->ioNil;
                fd->state = FRAME_STATE_RETURN;
                break;
            }

#ifdef DEBUG_EVAL_LOOP
            {
                // Print full message chain for debugging
                IoMessage *dbgMsg = m;
                fprintf(stderr, "  START chain: ");
                while (dbgMsg) {
                    IoSymbol *dbgLabel = IoMessage_rawLabel(dbgMsg);
                    fprintf(stderr, "%s@%s:%d -> ",
                            CSTRING(IoMessage_name(dbgMsg)),
                            dbgLabel ? CSTRING(dbgLabel) : "(null)",
                            IoMessage_rawLineNumber(dbgMsg));
                    dbgMsg = IOMESSAGEDATA(dbgMsg)->next;
                    if (!dbgMsg) fprintf(stderr, "(end)");
                }
                fprintf(stderr, "\n");
                fflush(stderr);
            }
#endif

            md = IOMESSAGEDATA(m);

            if (!md) {
                fprintf(stderr, "FATAL: NULL message data for message %p, frame=%p, state=%d\n",
                        (void*)m, (void*)frame, fd->state);
                fprintf(stderr, "  fd->target=%p, fd->locals=%p\n",
                        (void*)fd->target, (void*)fd->locals);
                fprintf(stderr, "  fd->argValues=%p, fd->argCount=%d, fd->currentArgIndex=%d\n",
                        (void*)fd->argValues, fd->argCount, fd->currentArgIndex);
                fprintf(stderr, "  IoObject_tag(m)->name=%s\n",
                        IoObject_tag(m) ? IoObject_tag(m)->name : "NULL");
                IoEvalFrame *p = fd->parent;
                int depth = 0;
                while (p && depth < 10) {
                    IoEvalFrameData *pd = FRAME_DATA(p);
                    fprintf(stderr, "  parent[%d]: state=%d, msg=%p", depth,
                            pd->state, (void*)pd->message);
                    if (pd->message) {
                        IoMessageData *pmd = IOMESSAGEDATA(pd->message);
                        if (pmd) {
                            fprintf(stderr, " name=%s", CSTRING(IoMessage_name(pd->message)));
                        } else {
                            fprintf(stderr, " (no msg data, tag=%s)",
                                    IoObject_tag(pd->message) ? IoObject_tag(pd->message)->name : "NULL");
                        }
                    }
                    fprintf(stderr, " argValues=%p argCount=%d\n",
                            (void*)pd->argValues, pd->argCount);
                    p = pd->parent;
                    depth++;
                }
                fprintf(stderr, "  (total parent depth: searched %d)\n", depth);
                fflush(stderr);
                fd->result = state->ioNil;
                fd->state = FRAME_STATE_RETURN;
                break;
            }

            // Check if this is a semicolon (resets target)
            if (md->name == state->semicolonSymbol) {
                fd->target = fd->cachedTarget;
                fd->message = md->next;
                if (md->next) {
                    // Continue with next message
                    fd->state = FRAME_STATE_START;
                } else {
                    // End of chain after semicolon
                    fd->result = fd->target;
                    fd->state = FRAME_STATE_RETURN;
                }
                break;
            }

            // Check if message has a cached result (literal)
            if (md->cachedResult) {
                fd->result = md->cachedResult;
                // If there's no next message, we can return immediately
                if (!md->next) {
                    fd->state = FRAME_STATE_RETURN;
                    break;
                } else {
                    fd->state = FRAME_STATE_CONTINUE_CHAIN;
                    goto continue_chain;  // Fast path: skip loop restart
                }
            }

            // Skip to slot lookup — special form detection is done in
            // ACTIVATE where it actually matters for arg pre-evaluation.
            fd->state = FRAME_STATE_LOOKUP_SLOT;
            goto lookup_slot;  // Fast path: skip loop restart
        }

        case FRAME_STATE_EVAL_ARGS: {
            // Evaluate arguments one at a time iteratively (no C recursion).
            // Entered from FRAME_STATE_ACTIVATE when args need pre-evaluation.
            IoMessage *argMsg;

            if (fd->currentArgIndex >= fd->argCount) {
                // All arguments evaluated, return to ACTIVATE to call the function
                fd->state = FRAME_STATE_ACTIVATE;
                break;
            }

            // Get the next argument message
            argMsg = IoMessage_rawArgAt_(fd->message, fd->currentArgIndex);

            if (!argMsg) {
                // Shouldn't happen, but handle gracefully
                fd->argValues[fd->currentArgIndex] = state->ioNil;
                fd->currentArgIndex++;
                break;
            }

            // Check if argument has a cached result
            if (IOMESSAGEDATA(argMsg)->cachedResult &&
                !IOMESSAGEDATA(argMsg)->next) {
                // Use cached value, no need to push frame
                fd->argValues[fd->currentArgIndex] =
                    IOMESSAGEDATA(argMsg)->cachedResult;
                fd->currentArgIndex++;
                break;
            }

            // Need to evaluate this argument - push a new frame
            IoEvalFrame *argFrame = IoState_pushFrame_(state);
            IoEvalFrameData *argFd = FRAME_DATA(argFrame);
            argFd->message = argMsg;
            argFd->target = fd->locals; // Args eval in sender context
            argFd->locals = fd->locals;
            argFd->cachedTarget = fd->locals;
            argFd->state = FRAME_STATE_START;

            // When argFrame returns, we'll resume at EVAL_ARGS state
            // and currentArgIndex will be incremented
            break;
        }

        lookup_slot:
        case FRAME_STATE_LOOKUP_SLOT: {
            // Perform slot lookup on target
            IoMessageData *lookupMd = IOMESSAGEDATA(fd->message);
            IoSymbol *messageName = lookupMd->name;
            IoObject *slotValue;
            IoObject *slotContext;

            // Inline cache: if target has the same tag and global slot
            // version hasn't changed since we cached, reuse the result.
            // Only caches proto-chain hits (not direct slot hits on target).
            // Guard: a local slot on target shadows the cached proto value
            // (e.g. false.isTrue overrides Object.isTrue).
            if (lookupMd->inlineCacheTag == IoObject_tag(fd->target) &&
                lookupMd->inlineCacheVersion == state->slotVersion &&
                !(IoObject_ownsSlots(fd->target) &&
                  PHash_at_(IoObject_slots(fd->target), messageName))) {
                slotValue = lookupMd->inlineCacheValue;
                slotContext = lookupMd->inlineCacheContext;
            } else {
                slotValue = IoObject_rawGetSlot_context_(fd->target, messageName,
                                                        &slotContext);
                // Cache only proto-chain hits (method lookups).
                // Direct hits (local vars) change frequently and aren't worth caching.
                if (slotValue && slotContext != fd->target) {
                    lookupMd->inlineCacheTag = IoObject_tag(fd->target);
                    lookupMd->inlineCacheValue = slotValue;
                    lookupMd->inlineCacheContext = slotContext;
                    lookupMd->inlineCacheVersion = state->slotVersion;
                }
            }

            if (slotValue) {
                fd->slotValue = slotValue;
                fd->slotContext = slotContext;
                fd->state = FRAME_STATE_ACTIVATE;
                goto activate;  // Fast path: skip loop restart
            } else if (IoObject_isLocals(fd->target)) {
                // Slot not found on block locals — look up 'self' (the scope)
                // and re-do the lookup there. This is the iterative equivalent
                // of IoObject_localsForward which would call performOn_
                // recursively. By retargeting here, we avoid C stack growth.
                IoObject *scope = IoObject_rawGetSlot_(fd->target,
                                                       state->selfSymbol);
                if (scope) {
                    fd->target = scope;
                    // Retry lookup on the scope (stays in LOOKUP_SLOT)
                } else {
                    // No scope — use regular forward
                    fd->result = IoObject_forward(fd->target, fd->locals,
                                                     fd->message);
                    if (state->errorRaised) {
                        state->errorRaised = 0;
                        if (IoState_unwindFramesForError_(state)) {
                            return state->ioNil;
                        }
                        break;
                    }
                    fd->state = FRAME_STATE_CONTINUE_CHAIN;
                }
            } else {
                // Slot not found on non-locals target
                fd->result = IoObject_forward(fd->target, fd->locals,
                                                 fd->message);
                if (state->errorRaised) {
                    state->errorRaised = 0;
                    if (IoState_unwindFramesForError_(state)) {
                        return state->ioNil;
                    }
                    break;
                }
                fd->state = FRAME_STATE_CONTINUE_CHAIN;
            }
            break;
        }

        activate:
        case FRAME_STATE_ACTIVATE: {
            // Activate the slot value (if activatable)
            IoObject *slotValue = fd->slotValue;

            if (IoObject_isActivatable(slotValue)) {
                // ============================================================
                // ARG PRE-EVALUATION
                // Before calling any Block or CFunction, pre-evaluate all
                // arguments iteratively. This eliminates C stack re-entrancy
                // for argument evaluation (~95% of all re-entrant call sites).
                //
                // Skip pre-eval for CFunctions with isLazyArgs flag set.
                // The flag is set at init time on all special-form CFunctions
                // (if, while, for, foreach, method, block, etc.) and
                // automatically covers aliases (e.g., elseif := getSlot("if"))
                // since they share the same CFunction object.
                //
                // Also skip for CFunctions using IoObject_self (thisContext,
                // ifNil, etc.) which ignore their arguments entirely.
                // ============================================================
                // Check cached flag (0=unchecked, 1=normal, 2=special)
                IoMessageData *activateMd = IOMESSAGEDATA(fd->message);
                int isSpecialForm;
                if (activateMd->isSpecialForm == 0) {
                    isSpecialForm = 0;
                    if (ISCFUNCTION(slotValue)) {
                        IoCFunctionData *cfData =
                            (IoCFunctionData *)IoObject_dataPointer(slotValue);
                        if (cfData->isLazyArgs ||
                            cfData->func == (IoUserFunction *)IoObject_self) {
                            isSpecialForm = 1;
                        }
                    }
                    activateMd->isSpecialForm = isSpecialForm ? 2 : 1;
                } else {
                    isSpecialForm = (activateMd->isSpecialForm == 2);
                }

                if (!isSpecialForm && !fd->argValues) {
                    // Pre-evaluate arguments iteratively (no C stack re-entrancy).
                    //
                    // For CFunctions: pre-evaluate ALL args.
                    // For Blocks: pre-evaluate only the named formal parameters.
                    //   Extra args beyond named params remain unevaluated for
                    //   lazy access via call argAt() / call evalArgAt()
                    //   (e.g., map(asUTF8) passes the message, not a value).
                    int msgArgCount = IoMessage_argCount(fd->message);
                    int preEvalCount = msgArgCount;

                    if (ISBLOCK(slotValue) && msgArgCount > 0) {
                        IoBlockData *bd = (IoBlockData *)IoObject_dataPointer(slotValue);
                        int namedCount = (int)List_size(bd->argNames);
                        if (namedCount < preEvalCount) {
                            preEvalCount = namedCount;
                        }
                    }

                    if (preEvalCount > 0) {
                        fd->argCount = preEvalCount;
                        fd->currentArgIndex = 0;
                        // Use inline buffer for small arg counts (avoids heap alloc)
                        if (preEvalCount <= FRAME_INLINE_ARG_MAX) {
                            fd->argValues = fd->inlineArgs;
                            memset(fd->inlineArgs, 0, preEvalCount * sizeof(IoObject *));
                        } else {
                            fd->argValues = (IoObject **)io_calloc(preEvalCount, sizeof(IoObject *));
                        }

                        // Fast path: check if ALL args are simple cached literals
                        int allCached = 1;
                        int i;
                        for (i = 0; i < msgArgCount; i++) {
                            IoMessage *argMsg = IoMessage_rawArgAt_(fd->message, i);
                            if (argMsg) {
                                IoMessageData *argMd = IOMESSAGEDATA(argMsg);
                                if (argMd->cachedResult && !argMd->next) {
                                    fd->argValues[i] = argMd->cachedResult;
                                } else {
                                    allCached = 0;
                                    break;
                                }
                            } else {
                                fd->argValues[i] = state->ioNil;
                            }
                        }

                        if (allCached) {
                            // All args were cached literals - done
                            fd->currentArgIndex = msgArgCount;
                        } else {
                            // Slow path: evaluate args iteratively via EVAL_ARGS
                            // Reset to evaluate from the first non-cached arg
                            fd->currentArgIndex = 0;
                            // Clear fast-path partial results
                            memset(fd->argValues, 0, msgArgCount * sizeof(IoObject *));
                            fd->state = FRAME_STATE_EVAL_ARGS;
                            break;
                        }
                    }
                }

                // ============================================================
                // ACTIVATION (args are pre-evaluated or not needed)
                // ============================================================
                if (ISBLOCK(slotValue)) {
                    // Fast path: trivial method body (single cached literal).
                    // Skip entire block activation (locals clone, Call object,
                    // PHash creation, frame push/pop) and return directly.
                    {
                        IoBlockData *blockData =
                            (IoBlockData *)IoObject_dataPointer(slotValue);
                        IoMessage *bodyMsg = blockData->message;
                        if (bodyMsg && BODY_IS_CACHED_LITERAL(bodyMsg)) {
                            fd->result = CACHED_LITERAL_RESULT(bodyMsg);
                            fd->state = FRAME_STATE_CONTINUE_CHAIN;
                            goto continue_chain;
                        }
                    }

                    // Tail Call Optimization: if this is the last message
                    // in a block body frame, reuse the frame instead of
                    // pushing a new one. This prevents stack overflow for
                    // recursive methods like factorial.
                    if (fd->blockLocals &&
                        !IOMESSAGEDATA(fd->message)->next) {
                        IoState_activateBlockTCO_(state, frame);
                    } else {
                        IoState_activateBlock_(state, frame);
                    }
                } else {
                    // CFunction - call directly
                    IoTagActivateFunc *activateFunc =
                        IoObject_tag(slotValue)->activateFunc;

                    // Save ioStack pointer before CFunction call.
                    // Coroutine operations (rawRun, rawReturnToParent,
                    // rawResume) switch state->currentIoStack to a
                    // different coroutine's stack. We must NOT pop the
                    // retain pool from the wrong stack - that causes
                    // buffer underflow and ioStack corruption.
                    Stack *savedIoStack = state->currentIoStack;

                    IoState_pushRetainPool(state);
                    fd->result =
                        activateFunc(slotValue, fd->target, fd->locals,
                                    fd->message, fd->slotContext);


#ifdef DEBUG_EVAL_LOOP
                    fprintf(stderr, "evalLoop ACTIVATE: CFunction %s returned, result=%p, frame=%p\n",
                            CSTRING(IoMessage_name(fd->message)),
                            (void*)fd->result, (void*)frame);
                    fflush(stderr);
#endif

                    // Check if an error was raised during CFunction execution.
                    // IoState_error_ creates the Exception and sets errorRaised.
                    // If the error was raised outside the recursive evaluator,
                    // we got here via longjmp. If inside, we got here via the
                    // recursive evaluator's error check returning ioNil.
                    if (state->errorRaised) {
#ifdef DEBUG_EVAL_LOOP
                        fprintf(stderr, "evalLoop ACTIVATE: errorRaised, unwinding frames. frame=%p, currentFrame=%p, coro=%p\n",
                                (void*)frame, (void*)state->currentFrame, (void*)state->currentCoroutine);
                        fflush(stderr);
#endif
                        state->errorRaised = 0;

                        // Pop retain pool if ioStack hasn't been switched
                        if (state->currentIoStack == savedIoStack) {
                            IoState_popRetainPoolExceptFor_(state, state->ioNil);
                        }

                        // Unwind frames, respecting nested eval boundaries.
                        // If we hit an isNestedEvalRoot, return from this eval
                        // loop with errorRaised re-set. The C caller will
                        // propagate the error to the outer eval loop.
                        if (IoState_unwindFramesForError_(state)) {
                            return state->ioNil;
                        }
                        break;  // loop restarts, frame=NULL handler takes over
                    }

#ifdef IO_CALLCC
                    // Check if a continuation was invoked (frame stack replaced)
                    if (state->continuationInvoked) {
#ifdef DEBUG_EVAL_LOOP
                        fprintf(stderr, "evalLoop ACTIVATE: continuationInvoked, breaking\n");
                        fflush(stderr);
#endif
                        state->continuationInvoked = 0;
                        break;
                    }
#endif

                    // Only pop the retain pool if the ioStack hasn't been
                    // switched by a coroutine operation. If it HAS been
                    // switched, the retain pool mark is on the OLD stack
                    // (saved coroutine) and popping from the NEW stack
                    // would corrupt it (Stack_popMark underflow).
                    if (state->currentIoStack == savedIoStack) {
                        IoState_popRetainPoolExceptFor_(state, fd->result);
                    }

                    // Check if primitive set up control flow handling
                    if (state->needsControlFlowHandling) {
#ifdef DEBUG_EVAL_LOOP
                        fprintf(stderr, "evalLoop ACTIVATE: needsControlFlowHandling, breaking\n");
                        fprintf(stderr, "  old frame=%p, state->currentFrame=%p, coro=%p\n",
                                (void*)frame, (void*)state->currentFrame,
                                (void*)state->currentCoroutine);
                        fflush(stderr);
#endif
                        state->needsControlFlowHandling = 0;
                        break;
                    }

#ifdef DEBUG_EVAL_LOOP
                    fprintf(stderr, "evalLoop ACTIVATE: normal return\n");
                    fflush(stderr);
#endif

                    // Normal return, continue chain
                    fd->state = FRAME_STATE_CONTINUE_CHAIN;
                    goto continue_chain;  // Fast path: skip loop restart
                }
            } else {
                // Not activatable - just return the value
                fd->result = slotValue;
                fd->state = FRAME_STATE_CONTINUE_CHAIN;
                goto continue_chain;  // Fast path: skip loop restart
            }
#ifdef DEBUG_EVAL_LOOP
            fprintf(stderr, "evalLoop ACTIVATE: case done, frame=%p, state=%d\n",
                    (void*)frame, fd->state);
            fflush(stderr);
#endif
            break;
        }

        continue_chain:
        case FRAME_STATE_CONTINUE_CHAIN: {
            // Check for non-normal stop status (break, continue, return).
            // Stop status can be set by CFunctions like break/continue/return
            // that were called in a child frame (e.g., break inside if inside loop).
            if (state->stopStatus != MESSAGE_STOP_STATUS_NORMAL) {
                // If this frame is a block activation with passStops=false,
                // it catches the stop status here (return is contained within the block).
                if (fd->blockLocals && !fd->passStops) {
                    fd->result = state->returnValue;
                    state->stopStatus = MESSAGE_STOP_STATUS_NORMAL;
                } else {
                    // For non-catching frames, set result to returnValue.
                    // This matches the recursive evaluator behavior where
                    // IoMessage_locals_performOn_ returns state->returnValue
                    // when stopStatus is non-normal. This ensures nested eval
                    // roots and intermediate frames propagate the correct value.
                    fd->result = state->returnValue;
                }
                // Propagate upward.
                fd->state = FRAME_STATE_RETURN;
                break;
            }

            // Move to next message in chain
            IoMessage *next = IOMESSAGEDATA(fd->message)->next;

#ifdef DEBUG_EVAL_LOOP
            fprintf(stderr, "  CONTINUE_CHAIN: current=%s, next=%s\n",
                    CSTRING(IoMessage_name(fd->message)),
                    next ? CSTRING(IoMessage_name(next)) : "(null)");
            fflush(stderr);
#endif

            if (next) {
                // Update target to be the result
                fd->target = fd->result;
                fd->message = next;
                fd->state = FRAME_STATE_START;

                // Reset arg state
                if (fd->argValues) {
                    if (fd->argValues != fd->inlineArgs) {
                        io_free(fd->argValues);
                    }
                    fd->argValues = NULL;
                }
                fd->argCount = 0;
                fd->currentArgIndex = 0;
                goto start_message;  // Fast path: skip loop restart
            } else {
                // End of chain - return
                fd->state = FRAME_STATE_RETURN;
            }
            break;
        }

        case FRAME_STATE_RETURN: {
            // Pop this frame and return result to parent
            result = fd->result;
            IoEvalFrame *parent = fd->parent;
            int isNestedRoot = fd->isNestedEvalRoot;

#ifdef DEBUG_EVAL_LOOP
            fprintf(stderr, "  RETURN: msg=%s, parent=%p, isNestedRoot=%d\n",
                    fd->message ? CSTRING(IoMessage_name(fd->message)) : "(null)",
                    (void*)parent, isNestedRoot);
            if (parent) {
                IoEvalFrameData *pd = FRAME_DATA(parent);
                fprintf(stderr, "  RETURN: parent msg=%s, parent state=%d\n",
                        pd->message ? CSTRING(IoMessage_name(pd->message)) : "(null)",
                        pd->state);
            }
            fflush(stderr);
#endif

            if (parent && !isNestedRoot) {
                IoEvalFrameData *pd = FRAME_DATA(parent);
                // Store result in parent's current arg slot
                if (pd->state == FRAME_STATE_EVAL_ARGS) {
                    pd->argValues[pd->currentArgIndex] = result;
                    pd->currentArgIndex++;
                }
                // If parent is waiting for a block to return, store result
                else if (pd->state == FRAME_STATE_ACTIVATE) {
                    pd->result = result;
                    pd->state = FRAME_STATE_CONTINUE_CHAIN;
                }
                // If parent is in a control flow state or waiting for result, store it
                else if (pd->state == FRAME_STATE_IF_CONVERT_BOOLEAN ||
                         pd->state == FRAME_STATE_IF_EVAL_BRANCH ||
                         pd->state == FRAME_STATE_WHILE_EVAL_CONDITION ||
                         pd->state == FRAME_STATE_WHILE_CHECK_CONDITION ||
                         pd->state == FRAME_STATE_WHILE_DECIDE ||
                         pd->state == FRAME_STATE_WHILE_EVAL_BODY ||
                         pd->state == FRAME_STATE_LOOP_EVAL_BODY ||
                         pd->state == FRAME_STATE_LOOP_AFTER_BODY ||
                         pd->state == FRAME_STATE_FOR_EVAL_SETUP ||
                         pd->state == FRAME_STATE_FOR_EVAL_BODY ||
                         pd->state == FRAME_STATE_FOR_AFTER_BODY ||
                         pd->state == FRAME_STATE_FOREACH_AFTER_BODY ||
#ifdef IO_CALLCC
                         pd->state == FRAME_STATE_CALLCC_EVAL_BLOCK ||
#endif
                         pd->state == FRAME_STATE_DO_WAIT ||
                         pd->state == FRAME_STATE_CONTINUE_CHAIN) {
                    pd->result = result;
                    // State already set by parent before pushing child, don't change it
                }
            }

            // Mirror IoBlock_activate lines 249-252: when a block frame
            // with passStops=0 finishes, propagate per-Call stopStatus
            // back to state->stopStatus. This is how Io-level code like
            // relayStopStatus (which does call setStopStatus(ss)) gets
            // its stop status propagated to the eval loop.
            if (fd->blockLocals && !fd->passStops) {
                int callStopStatus = MESSAGE_STOP_STATUS_NORMAL;
                if (fd->call) {
                    callStopStatus = IoCall_rawStopStatus((IoCall *)fd->call);
                }
                // Also check savedCall: when in-place if optimization
                // fires and then TCO replaces fd->call, the original
                // Call (with stop status set by relayStopStatus) is here.
                if (callStopStatus == MESSAGE_STOP_STATUS_NORMAL && fd->savedCall) {
                    callStopStatus = IoCall_rawStopStatus((IoCall *)fd->savedCall);
                }
                if (callStopStatus != MESSAGE_STOP_STATUS_NORMAL) {
                    state->stopStatus = callStopStatus;
                    state->returnValue = result;
                }
            }

            // Return blockLocals to pool for reuse (before retain pool pop)
            if (fd->blockLocals &&
                state->blockLocalsPoolSize < BLOCK_LOCALS_POOL_MAX) {
                state->blockLocalsPool[state->blockLocalsPoolSize++] =
                    fd->blockLocals;
            }

            // Return Call object to pool for reuse
            if (fd->call &&
                state->callPoolSize < CALL_POOL_MAX) {
                // Clear pointer fields for GC safety (pooled objects
                // are marked, so stale pointers would keep dead objects alive)
                IoCallData *cd = (IoCallData *)IoObject_dataPointer(fd->call);
                cd->sender = state->ioNil;
                cd->target = state->ioNil;
                cd->message = state->ioNil;
                cd->slotContext = state->ioNil;
                cd->activated = state->ioNil;
                cd->coroutine = state->ioNil;
                state->callPool[state->callPoolSize++] = fd->call;
            }

            if (fd->retainPoolMark) {
                IoState_popRetainPoolExceptFor_(state, result);
            }

            IoState_popFrame_(state);

            // If this was a nested eval root, exit the eval loop
            if (isNestedRoot) {
                return result;
            }

            // Fast path: if parent is a hot loop state, jump directly
            // to it instead of restarting the eval loop (saves frame
            // fetch, safety checks, and switch dispatch per iteration).
            frame = state->currentFrame;
            if (frame) {
                fd = FRAME_DATA(frame);
                if (fd->state == FRAME_STATE_FOR_AFTER_BODY) {
                    goto for_after_body;
                }
                if (fd->state == FRAME_STATE_FOREACH_AFTER_BODY) {
                    goto foreach_after_body;
                }
                if (fd->state == FRAME_STATE_LOOP_AFTER_BODY) {
                    goto loop_after_body;
                }
                if (fd->state == FRAME_STATE_WHILE_EVAL_BODY) {
                    goto while_eval_body;
                }
            }
            break;
        }

        case FRAME_STATE_IF_EVAL_CONDITION: {
            // Push a frame to evaluate the condition
            IoEvalFrame *condFrame = IoState_pushFrame_(state);
            IoEvalFrameData *condFd = FRAME_DATA(condFrame);
            condFd->message = fd->controlFlow.ifInfo.conditionMsg;
            condFd->target = fd->locals;
            condFd->locals = fd->locals;
            condFd->cachedTarget = fd->locals;
            condFd->state = FRAME_STATE_START;

            // When condition returns, convert it to boolean
            fd->state = FRAME_STATE_IF_CONVERT_BOOLEAN;
            break;
        }

        case FRAME_STATE_IF_CONVERT_BOOLEAN: {
            // Condition has been evaluated, now convert to boolean
            IoObject *condResult = fd->result;

            // Fast path: ioTrue, ioFalse, ioNil already are booleans.
            // Skip the asBoolean frame push for these common cases
            // (comparisons like <=, ==, != always return ioTrue/ioFalse).
            if (condResult == state->ioTrue ||
                condResult == state->ioFalse ||
                condResult == state->ioNil) {
                fd->state = FRAME_STATE_IF_EVAL_BRANCH;
                break;
            }

            // Push frame to send 'asBoolean' message to result
            IoEvalFrame *boolFrame = IoState_pushFrame_(state);
            IoEvalFrameData *boolFd = FRAME_DATA(boolFrame);
            boolFd->message = state->asBooleanMessage;
            boolFd->target = condResult;
            boolFd->locals = condResult;
            boolFd->cachedTarget = condResult;
            boolFd->state = FRAME_STATE_START;

            // When boolean conversion returns, evaluate branch
            fd->state = FRAME_STATE_IF_EVAL_BRANCH;
            break;
        }

        case FRAME_STATE_IF_EVAL_BRANCH: {
            // Boolean result is in fd->result
            int condition = ISTRUE(fd->result);

            // Store the condition result for potential use
            fd->controlFlow.ifInfo.conditionResult = condition;

            // DEBUG
            if (state->showAllMessages) {
                IoMessage *trueBr = fd->controlFlow.ifInfo.trueBranch;
                IoMessage *falseBr = fd->controlFlow.ifInfo.falseBranch;
                printf("IF_EVAL_BRANCH: condition=%d, evaluating %s branch\n",
                       condition,
                       condition ? "TRUE" : "FALSE");
                printf("  trueBranch=%s, falseBranch=%s\n",
                       trueBr ? CSTRING(IoMessage_name(trueBr)) : "NULL",
                       falseBr ? CSTRING(IoMessage_name(falseBr)) : "NULL");
            }

            // Determine which branch to take
            IoMessage *branch = condition ? fd->controlFlow.ifInfo.trueBranch
                                           : fd->controlFlow.ifInfo.falseBranch;

            if (branch) {
                // Tail position optimization: if the if() is the last
                // message in the chain, evaluate the branch directly in
                // this frame instead of pushing a child. This enables TCO
                // for the common pattern:
                //   method(n, if(n <= 0, acc, recurse(n - 1, acc)))
                if (!IOMESSAGEDATA(fd->message)->next) {
                    // Preserve original Call so RETURN can check its
                    // stop status even after TCO replaces fd->call
                    // (needed for relayStopStatus / ? operator)
                    if (fd->call) {
                        fd->savedCall = fd->call;
                    }
                    fd->message = branch;
                    fd->target = fd->locals;
                    fd->cachedTarget = fd->locals;
                    fd->state = FRAME_STATE_START;
                    if (fd->argValues) {
                        if (fd->argValues != fd->inlineArgs) {
                            io_free(fd->argValues);
                        }
                        fd->argValues = NULL;
                    }
                    fd->argCount = 0;
                    fd->currentArgIndex = 0;
                } else {
                    // Not tail position: push branch frame
                    IoEvalFrame *branchFrame = IoState_pushFrame_(state);
                    IoEvalFrameData *branchFd = FRAME_DATA(branchFrame);
                    branchFd->message = branch;
                    branchFd->target = fd->locals;
                    branchFd->locals = fd->locals;
                    branchFd->cachedTarget = fd->locals;
                    branchFd->state = FRAME_STATE_START;
                    fd->state = FRAME_STATE_CONTINUE_CHAIN;
                }
            } else {
                // No branch for this condition, return boolean
                fd->result = IOBOOL(fd->target, condition);
                fd->state = FRAME_STATE_CONTINUE_CHAIN;
            }
            break;
        }

        // ============================================================
        // WHILE LOOP STATE MACHINE
        // ============================================================

        case FRAME_STATE_WHILE_EVAL_CONDITION: {
            // Push a frame to evaluate the condition
            IoEvalFrame *condFrame = IoState_pushFrame_(state);
            IoEvalFrameData *condFd = FRAME_DATA(condFrame);
            condFd->message = fd->controlFlow.whileInfo.conditionMsg;
            condFd->target = fd->locals;
            condFd->locals = fd->locals;
            condFd->cachedTarget = fd->locals;
            condFd->state = FRAME_STATE_START;

            // When condition returns, we need to convert it to boolean
            fd->state = FRAME_STATE_WHILE_CHECK_CONDITION;
            break;
        }

        case FRAME_STATE_WHILE_CHECK_CONDITION: {
            // Condition has been evaluated, result is in fd->result
            // Now convert to boolean by sending asBoolean message
            IoObject *condResult = fd->result;

            // Fast path: skip asBoolean for true/false/nil singletons
            if (condResult == state->ioTrue ||
                condResult == state->ioFalse ||
                condResult == state->ioNil) {
                fd->state = FRAME_STATE_WHILE_DECIDE;
                break;
            }

            IoEvalFrame *boolFrame = IoState_pushFrame_(state);
            IoEvalFrameData *boolFd = FRAME_DATA(boolFrame);
            boolFd->message = state->asBooleanMessage;
            boolFd->target = condResult;
            boolFd->locals = condResult;
            boolFd->cachedTarget = condResult;
            boolFd->state = FRAME_STATE_START;

            // When boolean conversion returns, check result and maybe eval body
            fd->state = FRAME_STATE_WHILE_DECIDE;
            break;
        }

        case FRAME_STATE_WHILE_DECIDE: {
            // Boolean result is in fd->result
            int condition = ISTRUE(fd->result);

            if (state->showAllMessages) {
                printf("WHILE_DECIDE: condition=%d\n", condition);
            }

            if (!condition) {
                // Condition is false - exit loop
                // Return the last body result (stored in whileInfo) or nil
                fd->result = fd->controlFlow.whileInfo.lastResult
                    ? fd->controlFlow.whileInfo.lastResult
                    : state->ioNil;
                fd->state = FRAME_STATE_CONTINUE_CHAIN;
            } else {
                // Fast path: body is a cached literal
                if (BODY_IS_CACHED_LITERAL(fd->controlFlow.whileInfo.bodyMsg)) {
                    fd->result = CACHED_LITERAL_RESULT(fd->controlFlow.whileInfo.bodyMsg);
                    fd->state = FRAME_STATE_WHILE_EVAL_BODY;
                    break;
                }

                // Condition is true - evaluate body
                IoEvalFrame *bodyFrame = IoState_pushFrame_(state);
                IoEvalFrameData *bodyFd = FRAME_DATA(bodyFrame);
                bodyFd->message = fd->controlFlow.whileInfo.bodyMsg;
                bodyFd->target = fd->locals;
                bodyFd->locals = fd->locals;
                bodyFd->cachedTarget = fd->locals;
                bodyFd->state = FRAME_STATE_START;
                bodyFd->passStops = 1;  // Let break/continue propagate to us

                fd->state = FRAME_STATE_WHILE_EVAL_BODY;
            }
            break;
        }

        while_eval_body:
        case FRAME_STATE_WHILE_EVAL_BODY: {
            // Body has been evaluated, result is in fd->result
            // Store the result for potential return
            fd->controlFlow.whileInfo.lastResult = fd->result;

            // Check for break/continue
            if (state->stopStatus == MESSAGE_STOP_STATUS_BREAK) {
                // Break - exit loop with break value
                IoState_resetStopStatus(state);
                fd->result = state->returnValue ? state->returnValue : state->ioNil;
                fd->state = FRAME_STATE_CONTINUE_CHAIN;
                break;
            }

            if (state->stopStatus == MESSAGE_STOP_STATUS_CONTINUE) {
                // Continue - go back to condition
                IoState_resetStopStatus(state);
                fd->state = FRAME_STATE_WHILE_EVAL_CONDITION;
                break;
            }

            if (state->stopStatus == MESSAGE_STOP_STATUS_RETURN) {
                // Return - route through CONTINUE_CHAIN so method
                // frames can catch it (sets result = returnValue)
                fd->state = FRAME_STATE_CONTINUE_CHAIN;
                break;
            }

            // Normal - loop back to condition
            fd->state = FRAME_STATE_WHILE_EVAL_CONDITION;
            break;
        }

        // ============================================================
        // LOOP (infinite) STATE MACHINE
        // ============================================================

        case FRAME_STATE_LOOP_EVAL_BODY: {
            // For first iteration, just push body frame
            // For subsequent iterations, we arrive here after body returns

            // Check if this is first iteration (lastResult is NULL)
            if (fd->controlFlow.loopInfo.lastResult == NULL) {
                // Mark that we've started (use a non-null value)
                fd->controlFlow.loopInfo.lastResult = state->ioNil;

                // Fast path: body is a cached literal
                if (BODY_IS_CACHED_LITERAL(fd->controlFlow.loopInfo.bodyMsg)) {
                    fd->result = CACHED_LITERAL_RESULT(fd->controlFlow.loopInfo.bodyMsg);
                    fd->state = FRAME_STATE_LOOP_AFTER_BODY;
                    break;
                }

                // First iteration - push body frame
                IoEvalFrame *bodyFrame = IoState_pushFrame_(state);
                IoEvalFrameData *bodyFd = FRAME_DATA(bodyFrame);
                bodyFd->message = fd->controlFlow.loopInfo.bodyMsg;
                bodyFd->target = fd->locals;
                bodyFd->locals = fd->locals;
                bodyFd->cachedTarget = fd->locals;
                bodyFd->state = FRAME_STATE_START;
                bodyFd->passStops = 1;

                fd->state = FRAME_STATE_LOOP_AFTER_BODY;
                break;
            }

            // Should not reach here - use LOOP_AFTER_BODY instead
            fd->state = FRAME_STATE_LOOP_AFTER_BODY;
            break;
        }

        loop_after_body:
        case FRAME_STATE_LOOP_AFTER_BODY: {
            // Body has been evaluated
            fd->controlFlow.loopInfo.lastResult = fd->result;

            // Check for break
            if (state->stopStatus == MESSAGE_STOP_STATUS_BREAK) {
                IoState_resetStopStatus(state);
                fd->result = state->returnValue ? state->returnValue : state->ioNil;
                fd->state = FRAME_STATE_CONTINUE_CHAIN;
                break;
            }

            // Check for continue (just loop again)
            if (state->stopStatus == MESSAGE_STOP_STATUS_CONTINUE) {
                IoState_resetStopStatus(state);
            }

            // Check for return (propagate through CONTINUE_CHAIN)
            if (state->stopStatus == MESSAGE_STOP_STATUS_RETURN) {
                fd->state = FRAME_STATE_CONTINUE_CHAIN;
                break;
            }

            // Fast path: body is a cached literal
            if (BODY_IS_CACHED_LITERAL(fd->controlFlow.loopInfo.bodyMsg)) {
                fd->result = CACHED_LITERAL_RESULT(fd->controlFlow.loopInfo.bodyMsg);
                // Stay in LOOP_AFTER_BODY
                break;
            }

            // Loop again - push another body frame
            IoEvalFrame *bodyFrame = IoState_pushFrame_(state);
            IoEvalFrameData *bodyFd = FRAME_DATA(bodyFrame);
            bodyFd->message = fd->controlFlow.loopInfo.bodyMsg;
            bodyFd->target = fd->locals;
            bodyFd->locals = fd->locals;
            bodyFd->cachedTarget = fd->locals;
            bodyFd->state = FRAME_STATE_START;
            bodyFd->passStops = 1;

            // Stay in LOOP_AFTER_BODY to handle next iteration
            break;
        }

        // ============================================================
        // FOR LOOP STATE MACHINE
        // ============================================================

        case FRAME_STATE_FOR_EVAL_SETUP:
            // Note: Currently unused - for primitive evaluates setup synchronously
            // and jumps directly to FOR_EVAL_BODY. This case exists for potential
            // future async setup evaluation.
            fd->state = FRAME_STATE_FOR_EVAL_BODY;
            break;

        case FRAME_STATE_FOR_EVAL_BODY: {
            // Check if we need to initialize or continue
            if (!fd->controlFlow.forInfo.initialized) {
                // First time - just evaluate body for first iteration
                fd->controlFlow.forInfo.initialized = 1;

                // Check if we should even start
                double i = fd->controlFlow.forInfo.currentValue;
                double end = fd->controlFlow.forInfo.endValue;
                double incr = fd->controlFlow.forInfo.increment;

                if ((incr > 0 && i > end) || (incr < 0 && i < end)) {
                    // Range is empty, return nil
                    fd->result = state->ioNil;
                    fd->state = FRAME_STATE_CONTINUE_CHAIN;
                    break;
                }

                // Set the counter variable
                IoObject *num = IoState_numberWithDouble_(state, i);
#ifdef COLLECTOR_USE_REFCOUNT
                // Opt-in to RC tracking for for-loop counter Numbers
                ((CollectorMarker *)num)->refCount = 1;
#endif
                IoObject_setSlot_to_(fd->locals,
                    fd->controlFlow.forInfo.counterName, num);
                // Counter is now reachable via PHash slot; pop retain stack
                Stack_pop(state->currentIoStack);

                // Fast path: body is a cached literal — run entire loop inline
                if (BODY_IS_CACHED_LITERAL(fd->controlFlow.forInfo.bodyMsg)) {
                    IoObject *cachedBody = CACHED_LITERAL_RESULT(fd->controlFlow.forInfo.bodyMsg);
                    double loopIncr = fd->controlFlow.forInfo.increment;
                    double loopEnd = fd->controlFlow.forInfo.endValue;
                    IoSymbol *ctrName = fd->controlFlow.forInfo.counterName;
                    PHash *localSlots = IoObject_slots(fd->locals);
                    fd->controlFlow.forInfo.currentValue += loopIncr;
                    for (;;) {
                        double li = fd->controlFlow.forInfo.currentValue;
                        if ((loopIncr > 0 && li > loopEnd) || (loopIncr < 0 && li < loopEnd)) {
                            break;
                        }
#ifdef COLLECTOR_USE_REFCOUNT
                        IoObject *oldCtr1 = (IoObject *)PHash_at_(localSlots, ctrName);
#endif
                        {
                        IoObject *newCtr1 = IoState_numberWithDouble_(state, li);
#ifdef COLLECTOR_USE_REFCOUNT
                        ((CollectorMarker *)newCtr1)->refCount = 1;
#endif
                        PHash_at_put_(localSlots, ctrName, newCtr1);
                        }
                        // Counter is in PHash slot; pop retain to prevent
                        // unbounded ioStack growth
                        Stack_pop(state->currentIoStack);
#ifdef COLLECTOR_USE_REFCOUNT
                        if (oldCtr1) {
                            Collector_value_removingRefTo_(state->collector, oldCtr1);
                            Collector_rcDrainFreeList_(state->collector);
                        }
#endif
                        fd->controlFlow.forInfo.currentValue += loopIncr;
                    }
                    fd->result = cachedBody;
                    fd->state = FRAME_STATE_CONTINUE_CHAIN;
                    break;
                }

                // Push body frame
                IoEvalFrame *bodyFrame = IoState_pushFrame_(state);
                IoEvalFrameData *bodyFd = FRAME_DATA(bodyFrame);
                bodyFd->message = fd->controlFlow.forInfo.bodyMsg;
                bodyFd->target = fd->locals;
                bodyFd->locals = fd->locals;
                bodyFd->cachedTarget = fd->locals;
                bodyFd->state = FRAME_STATE_START;
                bodyFd->passStops = 1;

                fd->state = FRAME_STATE_FOR_AFTER_BODY;
                break;
            }

            // Should not reach here
            fd->state = FRAME_STATE_FOR_AFTER_BODY;
            break;
        }

        for_after_body:
        case FRAME_STATE_FOR_AFTER_BODY: {
            // Body has been evaluated
            fd->controlFlow.forInfo.lastResult = fd->result;

            // Check for break
            if (state->stopStatus == MESSAGE_STOP_STATUS_BREAK) {
                IoState_resetStopStatus(state);
                fd->result = state->returnValue ? state->returnValue : state->ioNil;
                fd->state = FRAME_STATE_CONTINUE_CHAIN;
                break;
            }

            // Check for continue
            if (state->stopStatus == MESSAGE_STOP_STATUS_CONTINUE) {
                IoState_resetStopStatus(state);
                // Fall through to increment and continue
            }

            // Check for return (propagate through CONTINUE_CHAIN)
            if (state->stopStatus == MESSAGE_STOP_STATUS_RETURN) {
                fd->state = FRAME_STATE_CONTINUE_CHAIN;
                break;
            }

            // Increment counter
            fd->controlFlow.forInfo.currentValue += fd->controlFlow.forInfo.increment;

            // Fast path: body is a cached literal — run remaining iterations
            // in a tight C loop, bypassing the eval loop overhead entirely.
            // This matches master's tight C for-loop performance.
            if (BODY_IS_CACHED_LITERAL(fd->controlFlow.forInfo.bodyMsg)) {
                IoObject *cachedBody = CACHED_LITERAL_RESULT(fd->controlFlow.forInfo.bodyMsg);
                double incr = fd->controlFlow.forInfo.increment;
                double end = fd->controlFlow.forInfo.endValue;
                IoSymbol *counterName = fd->controlFlow.forInfo.counterName;
                PHash *localSlots = IoObject_slots(fd->locals);

                for (;;) {
                    double i = fd->controlFlow.forInfo.currentValue;
                    if ((incr > 0 && i > end) || (incr < 0 && i < end)) {
                        fd->result = cachedBody;
                        fd->state = FRAME_STATE_CONTINUE_CHAIN;
                        goto for_after_body_done;
                    }
#ifdef COLLECTOR_USE_REFCOUNT
                    IoObject *oldCtr2 = (IoObject *)PHash_at_(localSlots, counterName);
#endif
                    {
                    IoObject *newCtr2 = IoState_numberWithDouble_(state, i);
#ifdef COLLECTOR_USE_REFCOUNT
                    ((CollectorMarker *)newCtr2)->refCount = 1;
#endif
                    PHash_at_put_(localSlots, counterName, newCtr2);
                    }
                    Stack_pop(state->currentIoStack);
#ifdef COLLECTOR_USE_REFCOUNT
                    if (oldCtr2) {
                        Collector_value_removingRefTo_(state->collector, oldCtr2);
                        Collector_rcDrainFreeList_(state->collector);
                    }
#endif
                    fd->controlFlow.forInfo.currentValue += incr;
                }
            }

            {
            double i = fd->controlFlow.forInfo.currentValue;
            double end = fd->controlFlow.forInfo.endValue;
            double incr = fd->controlFlow.forInfo.increment;

            // Check if we should continue
            if ((incr > 0 && i > end) || (incr < 0 && i < end)) {
                // Done - return last result
                fd->result = fd->controlFlow.forInfo.lastResult
                    ? fd->controlFlow.forInfo.lastResult
                    : state->ioNil;
                fd->state = FRAME_STATE_CONTINUE_CHAIN;
                break;
            }

            // Set the counter variable (direct PHash access since
            // slots are guaranteed to exist from for-loop setup)
            IoObject *num = IoState_numberWithDouble_(state, i);
#ifdef COLLECTOR_USE_REFCOUNT
            ((CollectorMarker *)num)->refCount = 1;
            IoObject *oldCtr3 = (IoObject *)PHash_at_(IoObject_slots(fd->locals),
                fd->controlFlow.forInfo.counterName);
#endif
            PHash_at_put_(IoObject_slots(fd->locals),
                fd->controlFlow.forInfo.counterName, num);
            // Counter is now reachable via PHash slot; pop retain stack
            Stack_pop(state->currentIoStack);
#ifdef COLLECTOR_USE_REFCOUNT
            if (oldCtr3) {
                Collector_value_removingRefTo_(state->collector, oldCtr3);
                Collector_rcDrainFreeList_(state->collector);
            }
#endif

            // Push body frame for next iteration
            IoEvalFrame *bodyFrame = IoState_pushFrame_(state);
            IoEvalFrameData *bodyFd = FRAME_DATA(bodyFrame);
            bodyFd->message = fd->controlFlow.forInfo.bodyMsg;
            bodyFd->target = fd->locals;
            bodyFd->locals = fd->locals;
            bodyFd->cachedTarget = fd->locals;
            bodyFd->state = FRAME_STATE_START;
            bodyFd->passStops = 1;
            }

            // Stay in FOR_AFTER_BODY
            for_after_body_done:
            break;
        }

        // ============================================================
        // FOREACH STATE MACHINE (collection iteration)
        // ============================================================

        case FRAME_STATE_FOREACH_EVAL_BODY: {
            int idx = fd->controlFlow.foreachInfo.currentIndex;
            int size = fd->controlFlow.foreachInfo.collectionSize;
            int dir = fd->controlFlow.foreachInfo.direction;

            // Check bounds
            if (dir > 0 ? (idx >= size) : (idx < 0)) {
                // Done iterating
                fd->result = fd->controlFlow.foreachInfo.lastResult
                    ? fd->controlFlow.foreachInfo.lastResult
                    : state->ioNil;
                fd->state = FRAME_STATE_CONTINUE_CHAIN;
                break;
            }

            // Get element from collection
            IoObject *collection = fd->controlFlow.foreachInfo.collection;
            IoObject *mapSource = fd->controlFlow.foreachInfo.mapSource;
            IoObject *element = NULL;

            if (ISLIST(collection)) {
                List *list = IoList_rawList(collection);
                // Re-check size (list may have been mutated)
                int currentSize = (int)List_size(list);
                if (dir > 0 ? (idx >= currentSize) : (idx < 0)) {
                    fd->result = fd->controlFlow.foreachInfo.lastResult
                        ? fd->controlFlow.foreachInfo.lastResult
                        : state->ioNil;
                    fd->state = FRAME_STATE_CONTINUE_CHAIN;
                    break;
                }
                fd->controlFlow.foreachInfo.collectionSize = currentSize;

                if (mapSource) {
                    // Map iteration: keys list, look up value from map
                    IoSymbol *key = (IoSymbol *)List_at_(list, idx);
                    if (fd->controlFlow.foreachInfo.indexName) {
                        IoObject_setSlot_to_(fd->locals,
                            fd->controlFlow.foreachInfo.indexName, key);
                    }
                    element = IoMap_rawAt(mapSource, key);
                    if (!element) element = state->ioNil;
                } else {
                    element = (IoObject *)List_at_(list, idx);
                }
            }

            if (!element) element = state->ioNil;

            // Set slot values
            if (fd->controlFlow.foreachInfo.isEach) {
                // "each" mode: send body message to element as target
            } else if (!mapSource) {
                // List/Seq iteration: set index and value slots
                if (fd->controlFlow.foreachInfo.indexName) {
                    IoObject_setSlot_to_(fd->locals,
                        fd->controlFlow.foreachInfo.indexName,
                        IoState_numberWithDouble_(state, idx));
                    Stack_pop(state->currentIoStack);
                }
                if (fd->controlFlow.foreachInfo.valueName) {
                    IoObject_setSlot_to_(fd->locals,
                        fd->controlFlow.foreachInfo.valueName, element);
                }
            } else {
                // Map: value slot (indexName/key already set above)
                if (fd->controlFlow.foreachInfo.valueName) {
                    IoObject_setSlot_to_(fd->locals,
                        fd->controlFlow.foreachInfo.valueName, element);
                }
            }

            // Fast path: body is a cached literal — run remaining iterations
            // in a tight C loop for forward List iteration (not "each", not map).
            if (BODY_IS_CACHED_LITERAL(fd->controlFlow.foreachInfo.bodyMsg) &&
                ISLIST(collection) && !mapSource &&
                !fd->controlFlow.foreachInfo.isEach && dir > 0) {
                IoObject *cachedBody = CACHED_LITERAL_RESULT(fd->controlFlow.foreachInfo.bodyMsg);
                IoSymbol *indexName = fd->controlFlow.foreachInfo.indexName;
                IoSymbol *valueName = fd->controlFlow.foreachInfo.valueName;
                List *list = IoList_rawList(collection);

                // First iteration slots already set above, advance to next
                idx += dir;
                for (;;) {
                    int currentSize = (int)List_size(list);
                    if (idx >= currentSize) break;
                    IoObject *el = (IoObject *)List_at_(list, idx);
                    if (!el) el = state->ioNil;

                    if (indexName) {
                        IoObject_setSlot_to_(fd->locals, indexName,
                            IoState_numberWithDouble_(state, idx));
                        Stack_pop(state->currentIoStack);
                    }
                    if (valueName) {
                        IoObject_setSlot_to_(fd->locals, valueName, el);
                    }
                    idx++;
                }
                fd->controlFlow.foreachInfo.currentIndex = idx;
                fd->result = cachedBody;
                fd->state = FRAME_STATE_CONTINUE_CHAIN;
                break;
            }

            // Slower cached literal fallback (map, reverse, each)
            if (BODY_IS_CACHED_LITERAL(fd->controlFlow.foreachInfo.bodyMsg)) {
                fd->result = CACHED_LITERAL_RESULT(fd->controlFlow.foreachInfo.bodyMsg);
                fd->state = FRAME_STATE_FOREACH_AFTER_BODY;
                break;
            }

            // Push body frame
            IoEvalFrame *bodyFrame = IoState_pushFrame_(state);
            IoEvalFrameData *bodyFd = FRAME_DATA(bodyFrame);
            bodyFd->message = fd->controlFlow.foreachInfo.bodyMsg;
            if (fd->controlFlow.foreachInfo.isEach) {
                // "each": body runs with element as target
                bodyFd->target = element;
                bodyFd->locals = fd->locals;
                bodyFd->cachedTarget = element;
            } else {
                bodyFd->target = fd->locals;
                bodyFd->locals = fd->locals;
                bodyFd->cachedTarget = fd->locals;
            }
            bodyFd->state = FRAME_STATE_START;
            bodyFd->passStops = 1;

            fd->state = FRAME_STATE_FOREACH_AFTER_BODY;
            break;
        }

        foreach_after_body:
        case FRAME_STATE_FOREACH_AFTER_BODY: {
            fd->controlFlow.foreachInfo.lastResult = fd->result;

            // Check for break
            if (state->stopStatus == MESSAGE_STOP_STATUS_BREAK) {
                IoState_resetStopStatus(state);
                fd->result = state->returnValue ? state->returnValue : state->ioNil;
                fd->state = FRAME_STATE_CONTINUE_CHAIN;
                break;
            }

            // Check for continue
            if (state->stopStatus == MESSAGE_STOP_STATUS_CONTINUE) {
                IoState_resetStopStatus(state);
                // Fall through to increment
            }

            // Check for return (propagate through CONTINUE_CHAIN)
            if (state->stopStatus == MESSAGE_STOP_STATUS_RETURN) {
                fd->state = FRAME_STATE_CONTINUE_CHAIN;
                break;
            }

            // Increment/decrement index
            fd->controlFlow.foreachInfo.currentIndex +=
                fd->controlFlow.foreachInfo.direction;

            // Go back to EVAL_BODY (checks bounds at top)
            fd->state = FRAME_STATE_FOREACH_EVAL_BODY;
            break;
        }

#ifdef IO_CALLCC
        case FRAME_STATE_CALLCC_EVAL_BLOCK: {
            // Block body has finished evaluating
            // The result is in fd->result (set by RETURN handler)
            // This is the "normal return" path - continuation was not invoked

            // Move to continue chain
            fd->state = FRAME_STATE_CONTINUE_CHAIN;
            break;
        }
#endif

        // ============================================================
        // COROUTINE STATES
        // ============================================================

        case FRAME_STATE_CORO_WAIT_CHILD: {
            // Waiting for a child coroutine to complete.
            // When we get here, the child has finished and set fd->result.
            // Just continue the chain.
            fd->state = FRAME_STATE_CONTINUE_CHAIN;
            break;
        }

        case FRAME_STATE_CORO_YIELDED: {
            // Resumed after yield/resume. Continue where we left off.
            fd->state = FRAME_STATE_CONTINUE_CHAIN;
            break;
        }

        // ============================================================
        // DO STRING/MESSAGE/FILE STATE MACHINE (Phase 1: C stack elimination)
        // ============================================================

        case FRAME_STATE_DO_EVAL: {
            // Push frame to evaluate the compiled code
            IoEvalFrame *childFrame = IoState_pushFrame_(state);
            IoEvalFrameData *childFd = FRAME_DATA(childFrame);
            childFd->message = fd->controlFlow.doInfo.codeMessage;
            childFd->target = fd->controlFlow.doInfo.evalTarget;
            childFd->locals = fd->controlFlow.doInfo.evalLocals;
            childFd->cachedTarget = fd->controlFlow.doInfo.evalTarget;
            childFd->state = FRAME_STATE_START;

            fd->state = FRAME_STATE_DO_WAIT;
            break;
        }

        case FRAME_STATE_DO_WAIT: {
            // Child frame has returned, result is in fd->result
            // (set by RETURN handler when child completes)
            // Continue to next message in chain
            fd->state = FRAME_STATE_CONTINUE_CHAIN;
            break;
        }

        case FRAME_STATE_AWAIT_JS: {
            // Suspended waiting for a JS Promise to resolve.
            // Two paths here:
            // 1. First entry (awaitingJsPromise=1): yield to JS host
            // 2. Resume (awaitingJsPromise=0): future resolved, re-activate await
            if (state->awaitingJsPromise) {
                // Yield: break out of eval loop entirely.
                // Frame stack is preserved. JS will call io_resume_eval()
                // after io_resolve_future() delivers the value.
                return state->ioNil;
            }
            // Resumed: the future should now be resolved/rejected.
            // Transition back to ACTIVATE to re-call Future_await,
            // which will now return the value (resolved) or raise (rejected).
            fd->state = FRAME_STATE_ACTIVATE;
            break;
        }

        } // end switch
    } // end while (unreachable due to while(1) and returns)

    return result;
}

// Helper to activate a block without C recursion
static void IoState_activateBlock_(IoState *state, IoEvalFrame *callerFrame) {
    IoEvalFrameData *callerFd = FRAME_DATA(callerFrame);
    IoBlock *block = (IoBlock *)callerFd->slotValue;
    IoBlockData *blockData = (IoBlockData *)IoObject_dataPointer(block);

    // DEBUG
    if (state->showAllMessages) {
        printf("ACTIVATING BLOCK (method call), body message: %s\n",
               blockData->message ? CSTRING(IoMessage_name(blockData->message)) : "NULL");
        if (blockData->message && IOMESSAGEDATA(blockData->message)->next) {
            printf("  ... with next: %s\n", CSTRING(IoMessage_name(IOMESSAGEDATA(blockData->message)->next)));
        }
    }

    // Push retain pool BEFORE creating block locals/call objects.
    // Everything created during block activation and body execution
    // lands above this mark. When the block returns, the pool is
    // popped (keeping only the result), releasing temporaries from
    // the ioStack so GC can collect unreferenced objects.
    uintptr_t retainPoolMark = IoState_pushRetainPool(state);

    // Create or reuse block locals
    IoObject *blockLocals;
    PHash *bslots;
    if (state->blockLocalsPoolSize > 0) {
        // Reuse pooled blockLocals (already has PHash allocated)
        blockLocals = state->blockLocalsPool[--state->blockLocalsPoolSize];
        // Push onto ioStack so GC can reach it. Without this,
        // the blockLocals is unreachable between pool removal and
        // frame attachment — any GC triggered by IoCall_with or
        // argument evaluation would sweep it.
        IoState_stackRetain_(state, blockLocals);
        bslots = IoObject_slots(blockLocals);
        PHash_clean(bslots);
    } else {
        blockLocals = IOCLONE(state->localsProto);
        IoObject_isLocals_(blockLocals, 1);
        IoObject_createSlotsIfNeeded(blockLocals);
        bslots = IoObject_slots(blockLocals);
    }


    // Determine scope
    IoObject *scope =
        blockData->scope ? blockData->scope : callerFd->target;

    // Create or reuse Call object
    IoCall *callObject;
    if (state->callPoolSize > 0) {
        // Reuse pooled Call (already a valid collector object with
        // allocated IoCallData). Just reset the fields.
        callObject = state->callPool[--state->callPoolSize];
        IoState_stackRetain_(state, callObject);
    } else {
        // Allocate new Call via IOCLONE
        callObject = IOCLONE(state->callProto);
    }
    {
        IoCallData *cd = (IoCallData *)IoObject_dataPointer(callObject);
        cd->sender = callerFd->locals;
        cd->target = callerFd->target;
        cd->message = callerFd->message;
        cd->slotContext = callerFd->slotContext;
        cd->activated = block;
        cd->coroutine = state->currentCoroutine;
        cd->stopStatus = MESSAGE_STOP_STATUS_NORMAL;
    }

    PHash_at_put_(bslots, state->callSymbol, callObject);
    PHash_at_put_(bslots, state->selfSymbol, scope);
    PHash_at_put_(bslots, state->updateSlotSymbol,
                  state->localsUpdateSlotCFunc);


    // Bind arguments
    // Named formal parameters are pre-evaluated by the eval loop
    // (stored in callerFd->argValues). This eliminates C stack
    // re-entrancy for the common case.
    List *argNames = blockData->argNames;
    IoMessage *m = callerFd->message;
    int argCount = IoMessage_argCount(m);

    LIST_FOREACH(argNames, i, name,
                 IoObject *arg;
                 if ((int)i < argCount) {
                     // Use pre-evaluated value if available
                     if (callerFd->argValues &&
                         (int)i < callerFd->argCount &&
                         callerFd->argValues[(int)i] != NULL) {
                         arg = callerFd->argValues[(int)i];
                     } else {
                         // Fallback: evaluate in sender's context (recursive)
                         arg = IoMessage_locals_valueArgAt_(
                             m, callerFd->locals, (int)i);
                     }
                 } else {
                     // Unbound params (fewer args than params) default to nil
                     arg = state->ioNil;
                 }
                 IoObject_setSlot_to_(blockLocals, name, arg););

    // Mark these as unreferenced for potential recycling
    IoObject_isReferenced_(blockLocals, 0);
    IoObject_isReferenced_(callObject, 0);

    // Push new frame for block body evaluation
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
    blockFd->retainPoolMark = retainPoolMark;

    // Caller frame will wait for block frame to return
    // Keep callerFrame in ACTIVATE state - when blockFrame returns,
    // the RETURN handler will move callerFrame to CONTINUE_CHAIN
}

// Tail Call Optimization: reuse the current block body frame for a tail call.
// Instead of pushing a new frame on top of the current one, we replace the
// current frame's context with the new block's context. This keeps the frame
// stack flat for recursive calls (e.g., factorial(n-1, n*acc)).
//
// blockFrame is the CURRENT block body frame being reused.
// blockFd->slotValue contains the Block to tail-call.
// blockFd->message, ->target, ->locals, ->slotContext have the call info.
static void IoState_activateBlockTCO_(IoState *state, IoEvalFrame *blockFrame) {
    IoEvalFrameData *blockFd = FRAME_DATA(blockFrame);
    IoBlock *block = (IoBlock *)blockFd->slotValue;
    IoBlockData *blockData = (IoBlockData *)IoObject_dataPointer(block);

    // NOTE: We do NOT return old blockLocals to the pool here.
    // The new block's Call object references the old blockLocals
    // as call sender/target (via blockFd->locals/target passed to
    // IoCall_with below). Pooling the old blockLocals would clean
    // its PHash, destroying slots that call sender still needs
    // (e.g., the ? operator's "m" slot used by relayStopStatus).
    // Only the RETURN handler pools blockLocals, when the block
    // has fully completed and its locals are no longer referenced.

    // Create or reuse block locals for the tail-called block
    IoObject *blockLocals;
    PHash *bslots;
    if (state->blockLocalsPoolSize > 0) {
        blockLocals = state->blockLocalsPool[--state->blockLocalsPoolSize];
        IoState_stackRetain_(state, blockLocals);
        bslots = IoObject_slots(blockLocals);
        PHash_clean(bslots);
    } else {
        blockLocals = IOCLONE(state->localsProto);
        IoObject_isLocals_(blockLocals, 1);
        IoObject_createSlotsIfNeeded(blockLocals);
        bslots = IoObject_slots(blockLocals);
    }

    // Determine scope
    IoObject *scope =
        blockData->scope ? blockData->scope : blockFd->target;

    // Create or reuse Call object
    IoCall *callObject;
    if (state->callPoolSize > 0) {
        callObject = state->callPool[--state->callPoolSize];
        IoState_stackRetain_(state, callObject);
    } else {
        callObject = IOCLONE(state->callProto);
    }
    {
        IoCallData *cd = (IoCallData *)IoObject_dataPointer(callObject);
        cd->sender = blockFd->locals;
        cd->target = blockFd->target;
        cd->message = blockFd->message;
        cd->slotContext = blockFd->slotContext;
        cd->activated = block;
        cd->coroutine = state->currentCoroutine;
        cd->stopStatus = MESSAGE_STOP_STATUS_NORMAL;
    }

    PHash_at_put_(bslots, state->callSymbol, callObject);
    PHash_at_put_(bslots, state->selfSymbol, scope);
    PHash_at_put_(bslots, state->updateSlotSymbol,
                  state->localsUpdateSlotCFunc);

    // Bind arguments (same as activateBlock_)
    List *argNames = blockData->argNames;
    IoMessage *m = blockFd->message;
    int argCount = IoMessage_argCount(m);

    LIST_FOREACH(argNames, i, name,
                 IoObject *arg;
                 if ((int)i < argCount) {
                     if (blockFd->argValues &&
                         (int)i < blockFd->argCount &&
                         blockFd->argValues[(int)i] != NULL) {
                         arg = blockFd->argValues[(int)i];
                     } else {
                         arg = IoMessage_locals_valueArgAt_(
                             m, blockFd->locals, (int)i);
                     }
                 } else {
                     // Unbound params (fewer args than params) default to nil
                     arg = state->ioNil;
                 }
                 IoObject_setSlot_to_(blockLocals, name, arg););

    IoObject_isReferenced_(blockLocals, 0);
    IoObject_isReferenced_(callObject, 0);

    // REUSE the current frame: replace context with the tail-called block
    blockFd->message = blockData->message;
    blockFd->target = blockLocals;
    blockFd->locals = blockLocals;
    blockFd->cachedTarget = blockLocals;
    blockFd->state = FRAME_STATE_START;
    blockFd->call = callObject;
    blockFd->blockLocals = blockLocals;
    blockFd->passStops = blockData->passStops;

    // Free pre-evaluated args from the previous call
    if (blockFd->argValues) {
        if (blockFd->argValues != blockFd->inlineArgs) {
            io_free(blockFd->argValues);
        }
        blockFd->argValues = NULL;
    }
    blockFd->argCount = 0;
    blockFd->currentArgIndex = 0;
    blockFd->result = NULL;
    blockFd->slotValue = NULL;
    blockFd->slotContext = NULL;

    // Signal control flow handling so the eval loop restarts
    // with the updated frame (don't process stale ACTIVATE state)
    state->needsControlFlowHandling = 1;
}

// Entry point for iterative evaluation from C code
// This is called when a CFunction needs to evaluate an argument.
// It pushes a frame and runs the eval loop until that frame returns.
IoObject *IoMessage_locals_performOn_iterative(IoMessage *self,
                                               IoObject *locals,
                                               IoObject *target) {
    IoState *state = IOSTATE;

    // Push initial frame
    IoEvalFrame *frame = IoState_pushFrame_(state);
    IoEvalFrameData *fd = FRAME_DATA(frame);
    fd->message = self;
    fd->target = target;
    fd->locals = locals;
    fd->cachedTarget = target;
    fd->state = FRAME_STATE_START;
    fd->isNestedEvalRoot = 1;  // Mark this frame as a nested eval boundary

    // Track nested eval depth so the eval loop knows to return when
    // hitting an isNestedEvalRoot boundary during stop-status unwinding
    state->nestedEvalDepth++;

    // Run evaluation loop - it will return when this frame (or the coroutine) completes
    IoObject *result = IoState_evalLoop_(state);

    state->nestedEvalDepth--;

    return result;
}
