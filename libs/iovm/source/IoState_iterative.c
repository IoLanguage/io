
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

// Forward declarations
static void IoState_activateBlock_(IoState *state, IoEvalFrame *callerFrame);
static void IoState_handleStopStatus_(IoState *state);

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

    // If we get here, the stop status wasn't caught
    // This is OK - it will propagate out of the eval loop
}

// Main iterative evaluation loop
IoObject *IoState_evalLoop_(IoState *state) {
    IoEvalFrame *frame;
    IoObject *result = state->ioNil;

    while ((frame = state->currentFrame) != NULL) {
        IoMessage *m;
        IoMessageData *md;

        // Check for signals (Ctrl-C, etc.)
        if (state->receivedSignal) {
            IoState_callUserInterruptHandler(state);
        }

        // Check stop status (break/continue/return)
        if (state->stopStatus != MESSAGE_STOP_STATUS_NORMAL) {
            IoState_handleStopStatus_(state);
            continue;
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

            md = IOMESSAGEDATA(m);

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

            // Need to evaluate this message - start with arguments
            frame->argCount = IoMessage_argCount(m);
            frame->currentArgIndex = 0;

            if (frame->argCount > 0) {
                // Allocate array for argument values
                frame->argValues = io_calloc(frame->argCount, sizeof(IoObject *));
                frame->state = FRAME_STATE_EVAL_ARGS;
            } else {
                // No args, go straight to slot lookup
                frame->state = FRAME_STATE_LOOKUP_SLOT;
            }
            break;
        }

        case FRAME_STATE_EVAL_ARGS: {
            // Evaluate arguments one at a time (lazy evaluation)
            IoMessage *argMsg;

            if (frame->currentArgIndex >= frame->argCount) {
                // All arguments evaluated, proceed to slot lookup
                frame->state = FRAME_STATE_LOOKUP_SLOT;
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
                // It's a Block or CFunction - need to activate
                if (ISBLOCK(slotValue)) {
                    // Block activation - push new frame for block body
                    IoState_activateBlock_(state, frame);
                    // Frame state has been updated by activateBlock
                } else {
                    // CFunction - call directly
                    // NOTE: CFunctions must not recursively call the evaluator!
                    // This is a critical requirement for the iterative model.
                    IoTagActivateFunc *activateFunc =
                        IoObject_tag(slotValue)->activateFunc;

                    IoState_pushRetainPool(state);
                    frame->result =
                        activateFunc(slotValue, frame->target, frame->locals,
                                    frame->message, frame->slotContext);
                    IoState_popRetainPoolExceptFor_(state, frame->result);

                    frame->state = FRAME_STATE_CONTINUE_CHAIN;
                }
            } else {
                // Not activatable - just return the value
                frame->result = slotValue;
                frame->state = FRAME_STATE_CONTINUE_CHAIN;
            }
            break;
        }

        case FRAME_STATE_CONTINUE_CHAIN: {
            // Move to next message in chain
            IoMessage *next = IOMESSAGEDATA(frame->message)->next;

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

            if (parent) {
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
            }

            IoState_popFrame_(state);
            break;
        }

        } // end switch
    } // end while

    return result;
}

// Helper to activate a block without C recursion
static void IoState_activateBlock_(IoState *state, IoEvalFrame *callerFrame) {
    IoBlock *block = (IoBlock *)callerFrame->slotValue;
    IoBlockData *blockData = (IoBlockData *)IoObject_dataPointer(block);

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

    // Bind arguments (they're already evaluated in callerFrame->argValues)
    List *argNames = blockData->argNames;
    if (callerFrame->argValues) {
        LIST_FOREACH(argNames, i, name,
                     if ((int)i < callerFrame->argCount) {
                         IoObject *arg = callerFrame->argValues[i];
                         IoObject_setSlot_to_(blockLocals, name, arg);
                     });
    }

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

// Entry point for iterative evaluation
// This replaces the recursive IoMessage_locals_performOn_
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

    // Run evaluation loop
    IoObject *result = IoState_evalLoop_(state);

    return result;
}
