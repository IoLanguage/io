
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
        frame = state->framePool[--state->framePoolCount];
        fd = FRAME_DATA(frame);
        // Reset data fields (argValues already freed on pool return)
        memset(fd, 0, sizeof(IoEvalFrameData));
    } else {
        frame = IoEvalFrame_newWithState(state);
        fd = FRAME_DATA(frame);
    }

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
        fd->parent = NULL;

        // Eagerly free argValues since they can be large
        if (fd->argValues) {
            io_free(fd->argValues);
            fd->argValues = NULL;
        }

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
        int isRoot = FRAME_DATA(state->currentFrame)->isNestedEvalRoot;
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

        // Validate frame integrity
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
                } else {
                    fd->state = FRAME_STATE_CONTINUE_CHAIN;
                }
                break;
            }

            // Check if this is a special form that needs lazy argument evaluation
            IoSymbol *messageName = IoMessage_name(m);
            int isSpecialForm = (messageName == state->ifSymbol ||
                                 messageName == state->whileSymbol ||
                                 messageName == state->loopSymbol ||
                                 messageName == state->forSymbol ||
                                 messageName == state->callccSymbol ||
                                 messageName == state->foreachSymbol ||
                                 messageName == state->reverseForeachSymbol ||
                                 messageName == state->foreachLineSymbol ||
                                 messageName == state->messageSymbol ||
                                 messageName == state->repeatSymbol ||
                                 messageName == state->doSymbol ||
                                 messageName == state->lexicalDoSymbol ||
                                 messageName == state->foreachSlotSymbol ||
                                 messageName == state->cpuSecondsToRunSymbol ||
                                 messageName == state->sortInPlaceSymbol ||
                                 messageName == state->orSymbol ||
                                 messageName == state->andSymbol);

            // DEBUG
            if (state->showAllMessages && messageName == state->ifSymbol) {
                printf("SPECIAL FORM DETECTED: %s (skipping arg eval)\n", CSTRING(messageName));
            }

            // For ALL messages, skip to slot lookup and let the
            // CFunction/Block handle its own argument evaluation.
            // This preserves compatibility with existing CFunctions
            // which use IoMessage_locals_valueArgAt_ to evaluate args.
            //
            // Control flow primitives (if, while, etc.) will receive
            // unevaluated messages and handle them via the frame state
            // machine, avoiding C stack re-entry.
            //
            // TODO: In the future, we could pre-evaluate args for
            // non-control-flow CFunctions to enable full iterative
            // evaluation, but that requires modifying all CFunctions.
            (void)isSpecialForm; // Used for debug output only now
            fd->argCount = 0;
            fd->currentArgIndex = 0;
            fd->argValues = NULL;
            fd->state = FRAME_STATE_LOOKUP_SLOT;
            break;
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

        case FRAME_STATE_LOOKUP_SLOT: {
            // Perform slot lookup on target
            IoSymbol *messageName = IoMessage_name(fd->message);
            IoObject *slotValue;
            IoObject *slotContext;

            slotValue = IoObject_rawGetSlot_context_(fd->target, messageName,
                                                    &slotContext);

            if (slotValue) {
                fd->slotValue = slotValue;
                fd->slotContext = slotContext;
                fd->state = FRAME_STATE_ACTIVATE;
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
                // ============================================================
                // ARG PRE-EVALUATION
                // Before calling any Block or CFunction, pre-evaluate all
                // arguments iteratively. This eliminates C stack re-entrancy
                // for argument evaluation (~95% of all re-entrant call sites).
                //
                // Skip pre-eval for special forms that handle their own args:
                // - Control flow: if, while, for, loop, callcc
                // - Block construction: method, block
                // - Iteration: foreach, reverseForeach, foreachLine, foreachSlot
                // - Unevaluated body: repeat, do, lexicalDo, cpuSecondsToRun
                // - Sort with expression: sortInPlace
                // - Short-circuit: or, and
                // - Message reification: message
                // ============================================================
                IoSymbol *msgName = IoMessage_name(fd->message);
                int isSpecialForm =
                    (msgName == state->ifSymbol ||
                     msgName == state->whileSymbol ||
                     msgName == state->forSymbol ||
                     msgName == state->loopSymbol ||
                     msgName == state->callccSymbol ||
                     msgName == state->methodSymbol ||
                     msgName == state->blockSymbol ||
                     msgName == state->foreachSymbol ||
                     msgName == state->reverseForeachSymbol ||
                     msgName == state->foreachLineSymbol ||
                     msgName == state->messageSymbol ||
                     msgName == state->repeatSymbol ||
                     msgName == state->doSymbol ||
                     msgName == state->lexicalDoSymbol ||
                     msgName == state->foreachSlotSymbol ||
                     msgName == state->cpuSecondsToRunSymbol ||
                     msgName == state->sortInPlaceSymbol ||
                     msgName == state->orSymbol ||
                     msgName == state->andSymbol);

                // Also skip pre-evaluation for CFunctions that ignore
                // their args (e.g., IoObject_self used for thisContext,
                // ifNil, etc.)
                if (!isSpecialForm && ISCFUNCTION(slotValue)) {
                    IoCFunctionData *cfData =
                        (IoCFunctionData *)IoObject_dataPointer(slotValue);
                    if (cfData->func == (IoUserFunction *)IoObject_self) {
                        isSpecialForm = 1;
                    }
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
                        fd->argValues = (IoObject **)io_calloc(preEvalCount, sizeof(IoObject *));

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

                    // Check if a continuation was invoked (frame stack replaced)
                    if (state->continuationInvoked) {
#ifdef DEBUG_EVAL_LOOP
                        fprintf(stderr, "evalLoop ACTIVATE: continuationInvoked, breaking\n");
                        fflush(stderr);
#endif
                        state->continuationInvoked = 0;
                        break;
                    }

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
                }
            } else {
                // Not activatable - just return the value
                fd->result = slotValue;
                fd->state = FRAME_STATE_CONTINUE_CHAIN;
            }
#ifdef DEBUG_EVAL_LOOP
            fprintf(stderr, "evalLoop ACTIVATE: case done, frame=%p, state=%d\n",
                    (void*)frame, fd->state);
            fflush(stderr);
#endif
            break;
        }

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
                    io_free(fd->argValues);
                    fd->argValues = NULL;
                }
                fd->argCount = 0;
                fd->currentArgIndex = 0;
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
                         pd->state == FRAME_STATE_CALLCC_EVAL_BLOCK ||
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

            IoState_popFrame_(state);

            // If this was a nested eval root, exit the eval loop
            if (isNestedRoot) {
                return result;
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
                        io_free(fd->argValues);
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
                // First iteration - push body frame
                IoEvalFrame *bodyFrame = IoState_pushFrame_(state);
                IoEvalFrameData *bodyFd = FRAME_DATA(bodyFrame);
                bodyFd->message = fd->controlFlow.loopInfo.bodyMsg;
                bodyFd->target = fd->locals;
                bodyFd->locals = fd->locals;
                bodyFd->cachedTarget = fd->locals;
                bodyFd->state = FRAME_STATE_START;
                bodyFd->passStops = 1;

                // Mark that we've started (use a non-null value)
                fd->controlFlow.loopInfo.lastResult = state->ioNil;
                fd->state = FRAME_STATE_LOOP_AFTER_BODY;
                break;
            }

            // Should not reach here - use LOOP_AFTER_BODY instead
            fd->state = FRAME_STATE_LOOP_AFTER_BODY;
            break;
        }

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
                IoObject_setSlot_to_(fd->locals,
                    fd->controlFlow.forInfo.counterName, num);

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

            // Set the counter variable
            IoObject *num = IoState_numberWithDouble_(state, i);
            IoObject_setSlot_to_(fd->locals,
                fd->controlFlow.forInfo.counterName, num);

            // Push body frame for next iteration
            IoEvalFrame *bodyFrame = IoState_pushFrame_(state);
            IoEvalFrameData *bodyFd = FRAME_DATA(bodyFrame);
            bodyFd->message = fd->controlFlow.forInfo.bodyMsg;
            bodyFd->target = fd->locals;
            bodyFd->locals = fd->locals;
            bodyFd->cachedTarget = fd->locals;
            bodyFd->state = FRAME_STATE_START;
            bodyFd->passStops = 1;

            // Stay in FOR_AFTER_BODY
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

        case FRAME_STATE_CALLCC_EVAL_BLOCK: {
            // Block body has finished evaluating
            // The result is in fd->result (set by RETURN handler)
            // This is the "normal return" path - continuation was not invoked

            // Move to continue chain
            fd->state = FRAME_STATE_CONTINUE_CHAIN;
            break;
        }

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

    // Create block locals
    IoObject *blockLocals = IOCLONE(state->localsProto);
    IoObject_isLocals_(blockLocals, 1);

    // Determine scope
    IoObject *scope =
        blockData->scope ? blockData->scope : callerFd->target;

    // Create Call object
    IoCall *callObject = IoCall_with(
        state, callerFd->locals, // sender
        callerFd->target,        // target
        callerFd->message,       // message
        callerFd->slotContext,   // slotContext
        block,                      // activated
        state->currentCoroutine     // coroutine
    );

    // Set up block locals slots
    IoObject_createSlotsIfNeeded(blockLocals);
    PHash *bslots = IoObject_slots(blockLocals);
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

    // Create new block locals for the tail-called block
    IoObject *blockLocals = IOCLONE(state->localsProto);
    IoObject_isLocals_(blockLocals, 1);

    // Determine scope
    IoObject *scope =
        blockData->scope ? blockData->scope : blockFd->target;

    // Create Call object
    IoCall *callObject = IoCall_with(
        state, blockFd->locals,     // sender (current block's locals)
        blockFd->target,            // target
        blockFd->message,           // message
        blockFd->slotContext,       // slotContext
        block,                         // activated
        state->currentCoroutine        // coroutine
    );

    // Set up block locals slots
    IoObject_createSlotsIfNeeded(blockLocals);
    PHash *bslots = IoObject_slots(blockLocals);
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
        io_free(blockFd->argValues);
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
