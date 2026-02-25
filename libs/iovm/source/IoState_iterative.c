
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
#include "IoNumber.h"
#include "IoCoroutine.h"
#include <stdio.h>
#include <string.h>

// Forward declarations
static void IoState_activateBlock_(IoState *state, IoEvalFrame *callerFrame);
static void IoState_handleStopStatus_(IoState *state);

// Debug: Validate that a frame's message field is a Message.
// This should be called after every frame setup where message is assigned.
#define VALIDATE_FRAME(f, location) do { \
    if ((f)->message && !ISMESSAGE((f)->message)) { \
        fprintf(stderr, "FRAME VALIDATION FAILED at %s\n", location); \
        fprintf(stderr, "  frame=%p, message=%p (tag=%s), target=%p, locals=%p\n", \
                (void*)(f), (void*)(f)->message, \
                IoObject_tag((IoObject*)(f)->message) ? IoObject_tag((IoObject*)(f)->message)->name : "NULL", \
                (void*)(f)->target, (void*)(f)->locals); \
        fprintf(stderr, "  parent=%p, state=%d\n", (void*)(f)->parent, (f)->state); \
        fflush(stderr); \
        abort(); \
    } \
} while(0)

// Check for pre-evaluated argument in the current eval frame.
// Called from IoMessage_locals_quickValueArgAt_ (IoState_inline.h).
// Separated into a function to avoid circular IoEvalFrame.h includes.
IoObject *IoState_preEvalArgAt_(IoState *self, IoMessage *msg, int n) {
    IoEvalFrame *frame = self->currentFrame;
    if (frame &&
        frame->argValues &&
        msg == frame->message &&
        n < frame->argCount &&
        frame->argValues[n] != NULL) {
        return frame->argValues[n];
    }
    return NULL;
}

// Push a new frame onto the evaluation stack (with pooling)
IoEvalFrame *IoState_pushFrame_(IoState *state) {
    IoEvalFrame *frame;

    // Try to get from pool first
    if (state->framePoolCount > 0) {
        frame = state->framePool[--state->framePoolCount];
        IoEvalFrame_reset(frame);
    } else {
        frame = IoEvalFrame_new();
    }

    frame->parent = state->currentFrame;
    state->currentFrame = frame;
    state->frameDepth++;

    if (state->frameDepth > state->maxFrameDepth) {
        IoState_error_(state, NULL, "Stack overflow: frame depth exceeded %d",
                      state->maxFrameDepth);
    }

    return frame;
}

// Pop a frame from the evaluation stack (with pooling)
void IoState_popFrame_(IoState *state) {
    IoEvalFrame *frame = state->currentFrame;
    if (frame) {
        state->currentFrame = frame->parent;
        state->frameDepth--;

        // Return to pool if there's space
        if (state->framePoolCount < 256) {
            IoEvalFrame_reset(frame);
            state->framePool[state->framePoolCount++] = frame;
        } else {
            IoEvalFrame_free(frame);
        }
    }
}

// Handle break/continue/return flow control
static void IoState_handleStopStatus_(IoState *state) {
    IoEvalFrame *frame = state->currentFrame;

    while (frame) {
        // If this frame is a block activation and it doesn't pass stops,
        // then it should catch the stop status
        if (frame->blockLocals && !frame->passStops) {
            // Caught the stop - return the returnValue and reset status
            frame->result = state->returnValue;
            frame->state = FRAME_STATE_RETURN;
            state->stopStatus = MESSAGE_STOP_STATUS_NORMAL;
            return;
        }

        // Keep unwinding
        frame = frame->parent;
    }

    // If we get here, no frame caught the stop status.
    // We need to unwind all frames and exit the eval loop.
    // Set all frames to RETURN state so they'll pop in sequence.
    frame = state->currentFrame;
    if (frame) {
        frame->result = state->returnValue ? state->returnValue : state->ioNil;
        frame->state = FRAME_STATE_RETURN;
    }
    state->stopStatus = MESSAGE_STOP_STATUS_NORMAL;
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
    IoObject *result = state->ioNil;

#ifdef DEBUG_EVAL_LOOP
    static int loopIter = 0;
#endif

    while (1) {
        frame = state->currentFrame;

#ifdef DEBUG_EVAL_LOOP
        loopIter++;
        // Always print to trace coro issues
        fprintf(stderr, "evalLoop iter %d: frame=%p, coro=%p, msg=%s, state=%d, nestedDepth=%d\n",
                loopIter, (void*)frame, (void*)state->currentCoroutine,
                (frame && frame->message) ? CSTRING(IoMessage_name(frame->message)) : "NULL",
                frame ? frame->state : -1,
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
            // CORO_WAIT_CHILD frame with us as the child. We must check
            // this BEFORE the nestedEvalDepth check, because a coro swap
            // child can finish within a nested eval loop.
            if (parent && ISCOROUTINE(parent)) {
                IoEvalFrame *parentTopFrame = ((IoCoroutineData *)IoObject_dataPointer(parent))->frameStack;
                if (parentTopFrame &&
                    parentTopFrame->state == FRAME_STATE_CORO_WAIT_CHILD &&
                    parentTopFrame->controlFlow.coroInfo.childCoroutine == (IoObject *)current) {
#ifdef DEBUG_EVAL_LOOP
                    fprintf(stderr, "evalLoop: coro swap child finished, returning to parent coro\n");
                    fflush(stderr);
#endif
                    // Coro swap child finished - restore parent
                    IoCoroutine_rawSetResult_(current, result);
                    IoCoroutine_saveState_(current, state);

                    IoCoroutine_restoreState_(parent, state);
                    IoState_setCurrentCoroutine_(state, parent);

                    // Parent's top frame should be waiting for us
                    frame = state->currentFrame;
                    if (frame && frame->state == FRAME_STATE_CORO_WAIT_CHILD) {
                        frame->result = result;
                        frame->state = FRAME_STATE_CONTINUE_CHAIN;
                    }
                    continue;  // Continue with parent's frames
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
#ifdef DEBUG_EVAL_LOOP
                fprintf(stderr, "evalLoop: parent frame=%p, state=%d\n",
                        (void*)frame, frame ? frame->state : -1);
                fflush(stderr);
#endif

                if (frame && frame->state == FRAME_STATE_CORO_WAIT_CHILD) {
                    frame->result = result;
                    frame->state = FRAME_STATE_CONTINUE_CHAIN;
                }
                continue;  // Continue with parent's frames
            }

#ifdef DEBUG_EVAL_LOOP
            fprintf(stderr, "evalLoop: exiting (no parent)\n");
            fflush(stderr);
#endif
            // Main coroutine done - exit the loop
            return result;
        }

        IoMessage *m;
        IoMessageData *md;

        // Validate frame integrity
        if (frame->magic != IOEVAL_FRAME_MAGIC) {
            fprintf(stderr, "CORRUPTION: frame magic invalid! frame=%p, magic=0x%08x (expected 0x%08x)\n",
                    (void*)frame, frame->magic, IOEVAL_FRAME_MAGIC);
            fprintf(stderr, "  This means state->currentFrame is NOT pointing to a valid IoEvalFrame!\n");
            fprintf(stderr, "  Dumping raw bytes at frame address:\n  ");
            unsigned char *bytes = (unsigned char *)frame;
            for (int i = 0; i < 64; i++) {
                fprintf(stderr, "%02x ", bytes[i]);
                if ((i+1) % 16 == 0) fprintf(stderr, "\n  ");
            }
            fprintf(stderr, "\n");
            fflush(stderr);
            abort();
        }

        if (frame->message && !ISMESSAGE(frame->message)) {
            fprintf(stderr, "CORRUPTION: frame->message is not a Message!\n");
            fprintf(stderr, "  frame=%p (magic OK), state=%d, message=%p, tag=%s\n",
                    (void*)frame, frame->state,
                    (void*)frame->message,
                    IoObject_tag(frame->message) ? IoObject_tag(frame->message)->name : "NULL");
            fprintf(stderr, "  target=%p, locals=%p, result=%p, cachedTarget=%p\n",
                    (void*)frame->target, (void*)frame->locals,
                    (void*)frame->result, (void*)frame->cachedTarget);
            fprintf(stderr, "  slotValue=%p, slotContext=%p, blockLocals=%p\n",
                    (void*)frame->slotValue, (void*)frame->slotContext,
                    (void*)frame->blockLocals);
            fprintf(stderr, "  argValues=%p, argCount=%d, currentArgIndex=%d\n",
                    (void*)frame->argValues, frame->argCount, frame->currentArgIndex);
            fprintf(stderr, "  parent=%p\n", (void*)frame->parent);
            // Walk parent chain
            IoEvalFrame *p = frame->parent;
            int depth = 0;
            while (p && depth < 10) {
                fprintf(stderr, "  parent[%d]: frame=%p, magic=0x%08x, state=%d, msg=%p (%s)\n",
                        depth, (void*)p, p->magic, p->state, (void*)p->message,
                        (p->message && p->magic == IOEVAL_FRAME_MAGIC && ISMESSAGE(p->message))
                            ? CSTRING(IoMessage_name(p->message)) : "INVALID/UNKNOWN");
                fprintf(stderr, "    target=%p, locals=%p, blockLocals=%p\n",
                        (void*)p->target, (void*)p->locals, (void*)p->blockLocals);
                p = p->parent;
                depth++;
            }
            fflush(stderr);
            abort();
        }

        // Check for signals (Ctrl-C, etc.)
        if (state->receivedSignal) {
            IoState_callUserInterruptHandler(state);
        }

        // Check stop status: only intercept 'return' at the top of the loop.
        // 'return' needs to unwind to the nearest block frame that catches stops.
        // 'break' and 'continue' propagate naturally through CONTINUE_CHAIN → RETURN
        // until reaching a loop state handler (LOOP_AFTER_BODY, WHILE_EVAL_BODY, etc.)
        // which already checks for them.
        if (state->stopStatus == MESSAGE_STOP_STATUS_RETURN) {
            IoState_handleStopStatus_(state);
            continue;
        }

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
            while (state->currentFrame) {
                IoState_popFrame_(state);
            }
            continue;  // frame=NULL handler takes over
        }

        // Show message if debugging
        if (state->showAllMessages && frame->message) {
            printf("M:%s:%s:%i\n", CSTRING(IoMessage_name(frame->message)),
                   CSTRING(IoMessage_rawLabel(frame->message)),
                   IoMessage_rawLineNumber(frame->message));
        }

        switch (frame->state) {

        case FRAME_STATE_START: {
            // Starting evaluation of a message
            m = frame->message;
            if (!m) {
                // No message to evaluate, return nil
                frame->result = state->ioNil;
                frame->state = FRAME_STATE_RETURN;
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
                        (void*)m, (void*)frame, frame->state);
                fprintf(stderr, "  frame->target=%p, frame->locals=%p\n",
                        (void*)frame->target, (void*)frame->locals);
                fprintf(stderr, "  frame->argValues=%p, frame->argCount=%d, frame->currentArgIndex=%d\n",
                        (void*)frame->argValues, frame->argCount, frame->currentArgIndex);
                fprintf(stderr, "  IoObject_tag(m)->name=%s\n",
                        IoObject_tag(m) ? IoObject_tag(m)->name : "NULL");
                IoEvalFrame *p = frame->parent;
                int depth = 0;
                while (p && depth < 10) {
                    fprintf(stderr, "  parent[%d]: state=%d, msg=%p", depth,
                            p->state, (void*)p->message);
                    if (p->message) {
                        IoMessageData *pmd = IOMESSAGEDATA(p->message);
                        if (pmd) {
                            fprintf(stderr, " name=%s", CSTRING(IoMessage_name(p->message)));
                        } else {
                            fprintf(stderr, " (no msg data, tag=%s)",
                                    IoObject_tag(p->message) ? IoObject_tag(p->message)->name : "NULL");
                        }
                    }
                    fprintf(stderr, " argValues=%p argCount=%d\n",
                            (void*)p->argValues, p->argCount);
                    p = p->parent;
                    depth++;
                }
                fprintf(stderr, "  (total parent depth: searched %d)\n", depth);
                fflush(stderr);
                frame->result = state->ioNil;
                frame->state = FRAME_STATE_RETURN;
                break;
            }

            // Check if this is a semicolon (resets target)
            if (md->name == state->semicolonSymbol) {
                frame->target = frame->cachedTarget;
                frame->message = md->next;
                if (md->next) {
                    // Continue with next message
                    frame->state = FRAME_STATE_START;
                } else {
                    // End of chain after semicolon
                    frame->result = frame->target;
                    frame->state = FRAME_STATE_RETURN;
                }
                break;
            }

            // Check if message has a cached result (literal)
            if (md->cachedResult) {
                frame->result = md->cachedResult;
                // If there's no next message, we can return immediately
                if (!md->next) {
                    frame->state = FRAME_STATE_RETURN;
                } else {
                    frame->state = FRAME_STATE_CONTINUE_CHAIN;
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
                                 messageName == state->foreachLineSymbol);

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
            frame->argCount = 0;
            frame->currentArgIndex = 0;
            frame->argValues = NULL;
            frame->state = FRAME_STATE_LOOKUP_SLOT;
            break;
        }

        case FRAME_STATE_EVAL_ARGS: {
            // Evaluate arguments one at a time iteratively (no C recursion).
            // Entered from FRAME_STATE_ACTIVATE when args need pre-evaluation.
            IoMessage *argMsg;

            if (frame->currentArgIndex >= frame->argCount) {
                // All arguments evaluated, return to ACTIVATE to call the function
                frame->state = FRAME_STATE_ACTIVATE;
                break;
            }

            // Get the next argument message
            argMsg = IoMessage_rawArgAt_(frame->message, frame->currentArgIndex);

            if (!argMsg) {
                // Shouldn't happen, but handle gracefully
                frame->argValues[frame->currentArgIndex] = state->ioNil;
                frame->currentArgIndex++;
                break;
            }

            // Check if argument has a cached result
            if (IOMESSAGEDATA(argMsg)->cachedResult &&
                !IOMESSAGEDATA(argMsg)->next) {
                // Use cached value, no need to push frame
                frame->argValues[frame->currentArgIndex] =
                    IOMESSAGEDATA(argMsg)->cachedResult;
                frame->currentArgIndex++;
                break;
            }

            // Need to evaluate this argument - push a new frame
            IoEvalFrame *argFrame = IoState_pushFrame_(state);
            argFrame->message = argMsg;
            argFrame->target = frame->locals; // Args eval in sender context
            argFrame->locals = frame->locals;
            argFrame->cachedTarget = frame->locals;
            argFrame->state = FRAME_STATE_START;

            // When argFrame returns, we'll resume at EVAL_ARGS state
            // and currentArgIndex will be incremented
            break;
        }

        case FRAME_STATE_LOOKUP_SLOT: {
            // Perform slot lookup on target
            IoSymbol *messageName = IoMessage_name(frame->message);
            IoObject *slotValue;
            IoObject *slotContext;

            slotValue = IoObject_rawGetSlot_context_(frame->target, messageName,
                                                    &slotContext);

            if (slotValue) {
                frame->slotValue = slotValue;
                frame->slotContext = slotContext;
                frame->state = FRAME_STATE_ACTIVATE;
            } else {
                // Slot not found - handle forward
                if (IoObject_isLocals(frame->target)) {
                    frame->result = IoObject_localsForward(
                        frame->target, frame->locals, frame->message);
                } else {
                    frame->result = IoObject_forward(frame->target, frame->locals,
                                                     frame->message);
                }
                frame->state = FRAME_STATE_CONTINUE_CHAIN;
            }
            break;
        }

        case FRAME_STATE_ACTIVATE: {
            // Activate the slot value (if activatable)
            IoObject *slotValue = frame->slotValue;

            if (IoObject_isActivatable(slotValue)) {
                // ============================================================
                // ARG PRE-EVALUATION
                // Before calling any Block or CFunction, pre-evaluate all
                // arguments iteratively. This eliminates C stack re-entrancy
                // for argument evaluation (~95% of all re-entrant call sites).
                //
                // Skip pre-eval for special forms that handle their own args:
                // - Control flow: if, while, for, loop, callcc
                // - Block construction: method, block
                // - Iteration: foreach, reverseForeach, foreachLine
                // ============================================================
                IoSymbol *msgName = IoMessage_name(frame->message);
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
                     msgName == state->foreachLineSymbol);

                if (!isSpecialForm && !frame->argValues && !ISBLOCK(slotValue)) {
                    // Need to pre-evaluate arguments (CFunctions only).
                    // Blocks handle their own args: formal params are evaluated
                    // in IoState_activateBlock_, and extra args are accessed as
                    // raw messages via call argAt(). Pre-evaluating would wrongly
                    // evaluate lazy arguments like map(asUTF8).
                    int msgArgCount = IoMessage_argCount(frame->message);
                    if (msgArgCount > 0) {
                        frame->argCount = msgArgCount;
                        frame->currentArgIndex = 0;
                        frame->argValues = (IoObject **)io_calloc(msgArgCount, sizeof(IoObject *));

                        // Fast path: check if ALL args are simple cached literals
                        int allCached = 1;
                        int i;
                        for (i = 0; i < msgArgCount; i++) {
                            IoMessage *argMsg = IoMessage_rawArgAt_(frame->message, i);
                            if (argMsg) {
                                IoMessageData *argMd = IOMESSAGEDATA(argMsg);
                                if (argMd->cachedResult && !argMd->next) {
                                    frame->argValues[i] = argMd->cachedResult;
                                } else {
                                    allCached = 0;
                                    break;
                                }
                            } else {
                                frame->argValues[i] = state->ioNil;
                            }
                        }

                        if (allCached) {
                            // All args were cached literals - done
                            frame->currentArgIndex = msgArgCount;
                        } else {
                            // Slow path: evaluate args iteratively via EVAL_ARGS
                            // Reset to evaluate from the first non-cached arg
                            frame->currentArgIndex = 0;
                            // Clear fast-path partial results
                            memset(frame->argValues, 0, msgArgCount * sizeof(IoObject *));
                            frame->state = FRAME_STATE_EVAL_ARGS;
                            break;
                        }
                    }
                }

                // ============================================================
                // ACTIVATION (args are pre-evaluated or not needed)
                // ============================================================
                if (ISBLOCK(slotValue)) {
                    // Block activation - push new frame for block body
                    IoState_activateBlock_(state, frame);
                    // Frame state has been updated by activateBlock
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
                    frame->result =
                        activateFunc(slotValue, frame->target, frame->locals,
                                    frame->message, frame->slotContext);

#ifdef DEBUG_EVAL_LOOP
                    fprintf(stderr, "evalLoop ACTIVATE: CFunction %s returned, result=%p, frame=%p\n",
                            CSTRING(IoMessage_name(frame->message)),
                            (void*)frame->result, (void*)frame);
                    fflush(stderr);
#endif

                    // Check if an error was raised during CFunction execution.
                    // IoState_error_ is now lightweight — it creates the Exception
                    // and sets errorRaised but does NOT unwind frames or swap coros.
                    // We handle all unwinding here in the eval loop.
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

                        // Unwind all frames — the frame=NULL handler at top of
                        // loop handles coro switching (back to parent, nested
                        // eval exit, etc.)
                        while (state->currentFrame) {
                            IoState_popFrame_(state);
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
                        IoState_popRetainPoolExceptFor_(state, frame->result);
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
                    frame->state = FRAME_STATE_CONTINUE_CHAIN;
                }
            } else {
                // Not activatable - just return the value
                frame->result = slotValue;
                frame->state = FRAME_STATE_CONTINUE_CHAIN;
            }
#ifdef DEBUG_EVAL_LOOP
            fprintf(stderr, "evalLoop ACTIVATE: case done, frame=%p, state=%d\n",
                    (void*)frame, frame->state);
            fflush(stderr);
#endif
            break;
        }

        case FRAME_STATE_CONTINUE_CHAIN: {
            // Check for non-normal stop status (break, continue, return).
            // Stop status can be set by CFunctions like break/continue/return
            // that were called in a child frame (e.g., break inside if inside loop).
            // We must propagate it upward immediately instead of continuing the chain.
            if (state->stopStatus != MESSAGE_STOP_STATUS_NORMAL) {
                frame->state = FRAME_STATE_RETURN;
                break;
            }

            // Move to next message in chain
            IoMessage *next = IOMESSAGEDATA(frame->message)->next;

#ifdef DEBUG_EVAL_LOOP
            fprintf(stderr, "  CONTINUE_CHAIN: current=%s, next=%s\n",
                    CSTRING(IoMessage_name(frame->message)),
                    next ? CSTRING(IoMessage_name(next)) : "(null)");
            fflush(stderr);
#endif

            if (next) {
                // Update target to be the result
                frame->target = frame->result;
                frame->message = next;
                frame->state = FRAME_STATE_START;

                // Reset arg state
                if (frame->argValues) {
                    io_free(frame->argValues);
                    frame->argValues = NULL;
                }
                frame->argCount = 0;
                frame->currentArgIndex = 0;
            } else {
                // End of chain - return
                frame->state = FRAME_STATE_RETURN;
            }
            break;
        }

        case FRAME_STATE_RETURN: {
            // Pop this frame and return result to parent
            result = frame->result;
            IoEvalFrame *parent = frame->parent;
            int isNestedRoot = frame->isNestedEvalRoot;

#ifdef DEBUG_EVAL_LOOP
            fprintf(stderr, "  RETURN: msg=%s, parent=%p, isNestedRoot=%d\n",
                    frame->message ? CSTRING(IoMessage_name(frame->message)) : "(null)",
                    (void*)parent, isNestedRoot);
            if (parent) {
                fprintf(stderr, "  RETURN: parent msg=%s, parent state=%d\n",
                        parent->message ? CSTRING(IoMessage_name(parent->message)) : "(null)",
                        parent->state);
            }
            fflush(stderr);
#endif

            if (parent && !isNestedRoot) {
                // Store result in parent's current arg slot
                if (parent->state == FRAME_STATE_EVAL_ARGS) {
                    parent->argValues[parent->currentArgIndex] = result;
                    parent->currentArgIndex++;
                }
                // If parent is waiting for a block to return, store result
                else if (parent->state == FRAME_STATE_ACTIVATE) {
                    parent->result = result;
                    parent->state = FRAME_STATE_CONTINUE_CHAIN;
                }
                // If parent is in a control flow state or waiting for result, store it
                else if (parent->state == FRAME_STATE_IF_CONVERT_BOOLEAN ||
                         parent->state == FRAME_STATE_IF_EVAL_BRANCH ||
                         parent->state == FRAME_STATE_WHILE_EVAL_CONDITION ||
                         parent->state == FRAME_STATE_WHILE_CHECK_CONDITION ||
                         parent->state == FRAME_STATE_WHILE_DECIDE ||
                         parent->state == FRAME_STATE_WHILE_EVAL_BODY ||
                         parent->state == FRAME_STATE_LOOP_EVAL_BODY ||
                         parent->state == FRAME_STATE_LOOP_AFTER_BODY ||
                         parent->state == FRAME_STATE_FOR_EVAL_SETUP ||
                         parent->state == FRAME_STATE_FOR_EVAL_BODY ||
                         parent->state == FRAME_STATE_FOR_AFTER_BODY ||
                         parent->state == FRAME_STATE_CALLCC_EVAL_BLOCK ||
                         parent->state == FRAME_STATE_DO_WAIT ||
                         parent->state == FRAME_STATE_CONTINUE_CHAIN) {
                    parent->result = result;
                    // State already set by parent before pushing child, don't change it
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
            condFrame->message = frame->controlFlow.ifInfo.conditionMsg;
            condFrame->target = frame->locals;
            condFrame->locals = frame->locals;
            condFrame->cachedTarget = frame->locals;
            condFrame->state = FRAME_STATE_START;

            // When condition returns, convert it to boolean
            frame->state = FRAME_STATE_IF_CONVERT_BOOLEAN;
            break;
        }

        case FRAME_STATE_IF_CONVERT_BOOLEAN: {
            // Condition has been evaluated, now convert to boolean
            IoObject *condResult = frame->result;

            // Push frame to send 'asBoolean' message to result
            IoEvalFrame *boolFrame = IoState_pushFrame_(state);
            boolFrame->message = state->asBooleanMessage;
            boolFrame->target = condResult;
            boolFrame->locals = condResult;
            boolFrame->cachedTarget = condResult;
            boolFrame->state = FRAME_STATE_START;

            // When boolean conversion returns, evaluate branch
            frame->state = FRAME_STATE_IF_EVAL_BRANCH;
            break;
        }

        case FRAME_STATE_IF_EVAL_BRANCH: {
            // Boolean result is in frame->result
            int condition = ISTRUE(frame->result);

            // Store the condition result for potential use
            frame->controlFlow.ifInfo.conditionResult = condition;

            // DEBUG
            if (state->showAllMessages) {
                IoMessage *trueBr = frame->controlFlow.ifInfo.trueBranch;
                IoMessage *falseBr = frame->controlFlow.ifInfo.falseBranch;
                printf("IF_EVAL_BRANCH: condition=%d, evaluating %s branch\n",
                       condition,
                       condition ? "TRUE" : "FALSE");
                printf("  trueBranch=%s, falseBranch=%s\n",
                       trueBr ? CSTRING(IoMessage_name(trueBr)) : "NULL",
                       falseBr ? CSTRING(IoMessage_name(falseBr)) : "NULL");
            }

            // Determine which branch to take
            IoMessage *branch = condition ? frame->controlFlow.ifInfo.trueBranch
                                           : frame->controlFlow.ifInfo.falseBranch;

            if (branch) {
                // Evaluate the branch
                IoEvalFrame *branchFrame = IoState_pushFrame_(state);
                branchFrame->message = branch;
                branchFrame->target = frame->locals;
                branchFrame->locals = frame->locals;
                branchFrame->cachedTarget = frame->locals;
                branchFrame->state = FRAME_STATE_START;

                // When branch returns, continue with the chain
                frame->state = FRAME_STATE_CONTINUE_CHAIN;
            } else {
                // No branch for this condition, return boolean
                frame->result = IOBOOL(frame->target, condition);
                frame->state = FRAME_STATE_CONTINUE_CHAIN;
            }
            break;
        }

        // ============================================================
        // WHILE LOOP STATE MACHINE
        // ============================================================

        case FRAME_STATE_WHILE_EVAL_CONDITION: {
            // Push a frame to evaluate the condition
            IoEvalFrame *condFrame = IoState_pushFrame_(state);
            condFrame->message = frame->controlFlow.whileInfo.conditionMsg;
            condFrame->target = frame->locals;
            condFrame->locals = frame->locals;
            condFrame->cachedTarget = frame->locals;
            condFrame->state = FRAME_STATE_START;

            // When condition returns, we need to convert it to boolean
            frame->state = FRAME_STATE_WHILE_CHECK_CONDITION;
            break;
        }

        case FRAME_STATE_WHILE_CHECK_CONDITION: {
            // Condition has been evaluated, result is in frame->result
            // Now convert to boolean by sending asBoolean message
            IoObject *condResult = frame->result;

            IoEvalFrame *boolFrame = IoState_pushFrame_(state);
            boolFrame->message = state->asBooleanMessage;
            boolFrame->target = condResult;
            boolFrame->locals = condResult;
            boolFrame->cachedTarget = condResult;
            boolFrame->state = FRAME_STATE_START;

            // When boolean conversion returns, check result and maybe eval body
            frame->state = FRAME_STATE_WHILE_DECIDE;
            break;
        }

        case FRAME_STATE_WHILE_DECIDE: {
            // Boolean result is in frame->result
            int condition = ISTRUE(frame->result);

            if (state->showAllMessages) {
                printf("WHILE_DECIDE: condition=%d\n", condition);
            }

            if (!condition) {
                // Condition is false - exit loop
                // Return the last body result (stored in whileInfo) or nil
                frame->result = frame->controlFlow.whileInfo.lastResult
                    ? frame->controlFlow.whileInfo.lastResult
                    : state->ioNil;
                frame->state = FRAME_STATE_CONTINUE_CHAIN;
            } else {
                // Condition is true - evaluate body
                IoEvalFrame *bodyFrame = IoState_pushFrame_(state);
                bodyFrame->message = frame->controlFlow.whileInfo.bodyMsg;
                bodyFrame->target = frame->locals;
                bodyFrame->locals = frame->locals;
                bodyFrame->cachedTarget = frame->locals;
                bodyFrame->state = FRAME_STATE_START;
                bodyFrame->passStops = 1;  // Let break/continue propagate to us

                frame->state = FRAME_STATE_WHILE_EVAL_BODY;
            }
            break;
        }

        case FRAME_STATE_WHILE_EVAL_BODY: {
            // Body has been evaluated, result is in frame->result
            // Store the result for potential return
            frame->controlFlow.whileInfo.lastResult = frame->result;

            // Check for break/continue
            if (state->stopStatus == MESSAGE_STOP_STATUS_BREAK) {
                // Break - exit loop with break value
                IoState_resetStopStatus(state);
                frame->result = state->returnValue ? state->returnValue : state->ioNil;
                frame->state = FRAME_STATE_CONTINUE_CHAIN;
                break;
            }

            if (state->stopStatus == MESSAGE_STOP_STATUS_CONTINUE) {
                // Continue - go back to condition
                IoState_resetStopStatus(state);
                frame->state = FRAME_STATE_WHILE_EVAL_CONDITION;
                break;
            }

            if (state->stopStatus == MESSAGE_STOP_STATUS_RETURN) {
                // Return - don't catch, let it propagate
                frame->state = FRAME_STATE_RETURN;
                break;
            }

            // Normal - loop back to condition
            frame->state = FRAME_STATE_WHILE_EVAL_CONDITION;
            break;
        }

        // ============================================================
        // LOOP (infinite) STATE MACHINE
        // ============================================================

        case FRAME_STATE_LOOP_EVAL_BODY: {
            // For first iteration, just push body frame
            // For subsequent iterations, we arrive here after body returns

            // Check if this is first iteration (lastResult is NULL)
            if (frame->controlFlow.loopInfo.lastResult == NULL) {
                // First iteration - push body frame
                IoEvalFrame *bodyFrame = IoState_pushFrame_(state);
                bodyFrame->message = frame->controlFlow.loopInfo.bodyMsg;
                bodyFrame->target = frame->locals;
                bodyFrame->locals = frame->locals;
                bodyFrame->cachedTarget = frame->locals;
                bodyFrame->state = FRAME_STATE_START;
                bodyFrame->passStops = 1;

                // Mark that we've started (use a non-null value)
                frame->controlFlow.loopInfo.lastResult = state->ioNil;
                frame->state = FRAME_STATE_LOOP_AFTER_BODY;
                break;
            }

            // Should not reach here - use LOOP_AFTER_BODY instead
            frame->state = FRAME_STATE_LOOP_AFTER_BODY;
            break;
        }

        case FRAME_STATE_LOOP_AFTER_BODY: {
            // Body has been evaluated
            frame->controlFlow.loopInfo.lastResult = frame->result;

            // Check for break
            if (state->stopStatus == MESSAGE_STOP_STATUS_BREAK) {
                IoState_resetStopStatus(state);
                frame->result = state->returnValue ? state->returnValue : state->ioNil;
                frame->state = FRAME_STATE_CONTINUE_CHAIN;
                break;
            }

            // Check for continue (just loop again)
            if (state->stopStatus == MESSAGE_STOP_STATUS_CONTINUE) {
                IoState_resetStopStatus(state);
            }

            // Check for return (propagate)
            if (state->stopStatus == MESSAGE_STOP_STATUS_RETURN) {
                frame->state = FRAME_STATE_RETURN;
                break;
            }

            // Loop again - push another body frame
            IoEvalFrame *bodyFrame = IoState_pushFrame_(state);
            bodyFrame->message = frame->controlFlow.loopInfo.bodyMsg;
            bodyFrame->target = frame->locals;
            bodyFrame->locals = frame->locals;
            bodyFrame->cachedTarget = frame->locals;
            bodyFrame->state = FRAME_STATE_START;
            bodyFrame->passStops = 1;

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
            frame->state = FRAME_STATE_FOR_EVAL_BODY;
            break;

        case FRAME_STATE_FOR_EVAL_BODY: {
            // Check if we need to initialize or continue
            if (!frame->controlFlow.forInfo.initialized) {
                // First time - just evaluate body for first iteration
                frame->controlFlow.forInfo.initialized = 1;

                // Check if we should even start
                double i = frame->controlFlow.forInfo.currentValue;
                double end = frame->controlFlow.forInfo.endValue;
                double incr = frame->controlFlow.forInfo.increment;

                if ((incr > 0 && i > end) || (incr < 0 && i < end)) {
                    // Range is empty, return nil
                    frame->result = state->ioNil;
                    frame->state = FRAME_STATE_CONTINUE_CHAIN;
                    break;
                }

                // Set the counter variable
                IoObject *num = IoState_numberWithDouble_(state, i);
                IoObject_setSlot_to_(frame->locals,
                    frame->controlFlow.forInfo.counterName, num);

                // Push body frame
                IoEvalFrame *bodyFrame = IoState_pushFrame_(state);
                bodyFrame->message = frame->controlFlow.forInfo.bodyMsg;
                bodyFrame->target = frame->locals;
                bodyFrame->locals = frame->locals;
                bodyFrame->cachedTarget = frame->locals;
                bodyFrame->state = FRAME_STATE_START;
                bodyFrame->passStops = 1;

                frame->state = FRAME_STATE_FOR_AFTER_BODY;
                break;
            }

            // Should not reach here
            frame->state = FRAME_STATE_FOR_AFTER_BODY;
            break;
        }

        case FRAME_STATE_FOR_AFTER_BODY: {
            // Body has been evaluated
            frame->controlFlow.forInfo.lastResult = frame->result;

            // Check for break
            if (state->stopStatus == MESSAGE_STOP_STATUS_BREAK) {
                IoState_resetStopStatus(state);
                frame->result = state->returnValue ? state->returnValue : state->ioNil;
                frame->state = FRAME_STATE_CONTINUE_CHAIN;
                break;
            }

            // Check for continue
            if (state->stopStatus == MESSAGE_STOP_STATUS_CONTINUE) {
                IoState_resetStopStatus(state);
                // Fall through to increment and continue
            }

            // Check for return (propagate)
            if (state->stopStatus == MESSAGE_STOP_STATUS_RETURN) {
                frame->state = FRAME_STATE_RETURN;
                break;
            }

            // Increment counter
            frame->controlFlow.forInfo.currentValue += frame->controlFlow.forInfo.increment;
            double i = frame->controlFlow.forInfo.currentValue;
            double end = frame->controlFlow.forInfo.endValue;
            double incr = frame->controlFlow.forInfo.increment;

            // Check if we should continue
            if ((incr > 0 && i > end) || (incr < 0 && i < end)) {
                // Done - return last result
                frame->result = frame->controlFlow.forInfo.lastResult
                    ? frame->controlFlow.forInfo.lastResult
                    : state->ioNil;
                frame->state = FRAME_STATE_CONTINUE_CHAIN;
                break;
            }

            // Set the counter variable
            IoObject *num = IoState_numberWithDouble_(state, i);
            IoObject_setSlot_to_(frame->locals,
                frame->controlFlow.forInfo.counterName, num);

            // Push body frame for next iteration
            IoEvalFrame *bodyFrame = IoState_pushFrame_(state);
            bodyFrame->message = frame->controlFlow.forInfo.bodyMsg;
            bodyFrame->target = frame->locals;
            bodyFrame->locals = frame->locals;
            bodyFrame->cachedTarget = frame->locals;
            bodyFrame->state = FRAME_STATE_START;
            bodyFrame->passStops = 1;

            // Stay in FOR_AFTER_BODY
            break;
        }

        case FRAME_STATE_CALLCC_EVAL_BLOCK: {
            // Block body has finished evaluating
            // The result is in frame->result (set by RETURN handler)
            // This is the "normal return" path - continuation was not invoked

            // Move to continue chain
            frame->state = FRAME_STATE_CONTINUE_CHAIN;
            break;
        }

        // ============================================================
        // COROUTINE STATES
        // ============================================================

        case FRAME_STATE_CORO_WAIT_CHILD: {
            // Waiting for a child coroutine to complete.
            // When we get here, the child has finished and set frame->result.
            // Just continue the chain.
            frame->state = FRAME_STATE_CONTINUE_CHAIN;
            break;
        }

        case FRAME_STATE_CORO_YIELDED: {
            // Resumed after yield/resume. Continue where we left off.
            frame->state = FRAME_STATE_CONTINUE_CHAIN;
            break;
        }

        // ============================================================
        // DO STRING/MESSAGE/FILE STATE MACHINE (Phase 1: C stack elimination)
        // ============================================================

        case FRAME_STATE_DO_EVAL: {
            // Push frame to evaluate the compiled code
            IoEvalFrame *childFrame = IoState_pushFrame_(state);
            childFrame->message = frame->controlFlow.doInfo.codeMessage;
            childFrame->target = frame->controlFlow.doInfo.evalTarget;
            childFrame->locals = frame->controlFlow.doInfo.evalLocals;
            childFrame->cachedTarget = frame->controlFlow.doInfo.evalTarget;
            childFrame->state = FRAME_STATE_START;

            frame->state = FRAME_STATE_DO_WAIT;
            break;
        }

        case FRAME_STATE_DO_WAIT: {
            // Child frame has returned, result is in frame->result
            // (set by RETURN handler when child completes)
            // Continue to next message in chain
            frame->state = FRAME_STATE_CONTINUE_CHAIN;
            break;
        }

        } // end switch
    } // end while (unreachable due to while(1) and returns)

    return result;
}

// Helper to activate a block without C recursion
static void IoState_activateBlock_(IoState *state, IoEvalFrame *callerFrame) {
    IoBlock *block = (IoBlock *)callerFrame->slotValue;
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
        blockData->scope ? blockData->scope : callerFrame->target;

    // Create Call object
    IoCall *callObject = IoCall_with(
        state, callerFrame->locals, // sender
        callerFrame->target,        // target
        callerFrame->message,       // message
        callerFrame->slotContext,   // slotContext
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
    // Since we're not pre-evaluating args in the iterative loop,
    // we need to evaluate them here using the recursive evaluator.
    // This introduces some re-entrancy, but control flow primitives
    // (if, while, etc.) avoid re-entrancy by using the frame state machine.
    List *argNames = blockData->argNames;
    IoMessage *m = callerFrame->message;
    int argCount = IoMessage_argCount(m);

    LIST_FOREACH(argNames, i, name,
                 if ((int)i < argCount) {
                     // Evaluate argument in sender's context
                     // Note: This uses the recursive evaluator (re-entrant)
                     IoObject *arg = IoMessage_locals_valueArgAt_(
                         m, callerFrame->locals, (int)i);
                     IoObject_setSlot_to_(blockLocals, name, arg);
                 });

    // Mark these as unreferenced for potential recycling
    IoObject_isReferenced_(blockLocals, 0);
    IoObject_isReferenced_(callObject, 0);

    // Push new frame for block body evaluation
    IoEvalFrame *blockFrame = IoState_pushFrame_(state);
    blockFrame->message = blockData->message;
    blockFrame->target = blockLocals;
    blockFrame->locals = blockLocals;
    blockFrame->cachedTarget = blockLocals;
    blockFrame->state = FRAME_STATE_START;
    blockFrame->call = callObject;
    blockFrame->blockLocals = blockLocals;
    blockFrame->passStops = blockData->passStops;

    // Caller frame will wait for block frame to return
    // Keep callerFrame in ACTIVATE state - when blockFrame returns,
    // the RETURN handler will move callerFrame to CONTINUE_CHAIN
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
    frame->message = self;
    frame->target = target;
    frame->locals = locals;
    frame->cachedTarget = target;
    frame->state = FRAME_STATE_START;
    frame->isNestedEvalRoot = 1;  // Mark this frame as a nested eval boundary

    // Run evaluation loop - it will return when this frame (or the coroutine) completes
    IoObject *result = IoState_evalLoop_(state);

    return result;
}
