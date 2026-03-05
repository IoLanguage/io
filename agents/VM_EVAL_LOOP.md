# Io VM Eval Loop Design

## Core Data Structures

### Frame

```c
typedef struct Frame {
    IoMessage *message;      // current message (instruction pointer)
    struct Frame *caller;    // return address (calling frame)
    IoObject *locals;        // context for slot lookup
    IoObject *target;        // current receiver
    IoObject *result;        // evaluation result
    bool tailCall;           // if true, reuse frame instead of push
} Frame;
```

### Sentinel Value

```c
#define SKIP_NORMAL_DISPATCH ((IoObject *)-1)
```

When a primitive returns this, it has already manipulated the frame directly.

## Main Eval Loop

This is the conceptual loop. See "Design Review" section below for identified issues and corrections.

```c
IoObject *IoState_eval(IoState *state) {
    Frame *frame = state->currentFrame;

    while (frame) {
        // === STOP STATUS CHECK ===
        if (state->stopStatus != STOP_STATUS_NORMAL) {
            // Handle break/continue/return/exception - see detailed design below
            IoState_handleStopStatus(state, &frame);
            if (!frame) break;
            continue;
        }

        IoMessage *m = frame->message;

        // === END OF MESSAGE CHAIN ===
        if (!m) {
            IoObject *result = frame->result;
            Frame *caller = frame->caller;
            if (caller) {
                caller->result = result;
                caller->target = result;
                frame = caller;
            } else {
                state->result = result;  // Save final result
                frame = NULL;
            }
            state->currentFrame = frame;
            continue;
        }

        // === SEMICOLON - RESET TARGET ===
        IoSymbol *name = IoMessage_name(m);
        if (name == state->semicolonSymbol) {
            frame->target = frame->originalTarget;
            frame->message = IoMessage_next(m);
            continue;
        }

        // === LITERAL (CACHED RESULT) ===
        if (IoMessage_cachedResult(m)) {
            frame->result = IoMessage_cachedResult(m);
            frame->target = frame->result;
            frame->message = IoMessage_next(m);
            continue;
        }

        // === SLOT LOOKUP ===
        IoObject *context;
        IoObject *slotValue = IoObject_getSlot_context_(frame->target, name, &context);

        // === SLOT NOT FOUND - TRY FORWARD ===
        if (!slotValue) {
            IoObject *fwdContext;
            IoObject *fwdSlot = IoObject_getSlot_context_(frame->target,
                                                          state->forwardSymbol,
                                                          &fwdContext);
            if (fwdSlot && IoObject_isActivatable(fwdSlot)) {
                IoObject *result = IoCFunction_activate(fwdSlot, frame->target,
                                                         frame, m, fwdContext);
                if (result == SKIP_NORMAL_DISPATCH) {
                    continue;
                }
                frame->result = result;
                frame->target = result;
                frame->message = IoMessage_next(m);
                continue;
            } else {
                IoState_raise(state, IoError_new(state, "Slot '%s' not found", CSTRING(name)));
                continue;
            }
        }

        // === ACTIVATE SLOT VALUE ===
        if (IoObject_isActivatable(slotValue)) {

            if (ISCFUNCTION(slotValue)) {
                // See "Argument Evaluation: Complete Solution" for full handling
                IoObject *result = IoCFunction_activate(slotValue, frame->target,
                                                         frame, m, context);
                if (result == SKIP_NORMAL_DISPATCH) {
                    continue;
                }
                frame->result = result;
                frame->target = result;
                frame->message = IoMessage_next(m);
            }
            else if (ISBLOCK(slotValue)) {
                IoBlock *block = slotValue;
                IoObject *newLocals = IoObject_createLocals(block, frame, m);

                // TCO: reuse frame if in tail position and this is a block frame
                if (m->isTailPosition && frame->isBlockActivation) {
                    frame->message = IoBlock_message(block);
                    frame->locals = newLocals;
                    frame->target = newLocals;
                    frame->originalTarget = newLocals;
                } else {
                    // Push new frame
                    Frame *newFrame = Frame_new();
                    newFrame->message = IoBlock_message(block);
                    newFrame->locals = newLocals;
                    newFrame->target = newLocals;
                    newFrame->originalTarget = newLocals;
                    newFrame->caller = frame;
                    newFrame->isBlockActivation = true;
                    newFrame->passStops = IoBlock_passStops(block);
                    frame = newFrame;
                    state->currentFrame = frame;
                }
            }
        }
        else {
            // === DATA SLOT - RETURN VALUE ===
            frame->result = slotValue;
            frame->target = slotValue;
            frame->message = IoMessage_next(m);
        }
    }

    return state->result;
}
```

## Primitive Styles

### Normal Primitives

Receive evaluated args, return a value:

```c
IO_METHOD(IoNumber, add) {
    double a = IoNumber_asDouble(self);
    double b = IoNumber_asDouble(args[0]);
    return IoNumber_new(state, a + b);
}

IO_METHOD(IoList, size) {
    return IoNumber_new(state, List_size(DATA(self)));
}

IO_METHOD(IoString, at) {
    int index = IoNumber_asInt(args[0]);
    return IoString_charAt(self, index);
}
```

### Control Flow Primitives

Receive frame + unevaluated message, manipulate frame directly.

**NOTE:** The simple implementation below is WRONG - see "Issue 1" in Design Review for the corrected version that properly pushes frames.

```c
// WRONG - naive implementation (doesn't handle message chains in args correctly)
// On true object
IO_METHOD(IoTrue, ifTrue) {
    frame->message = IoMessage_argAt(m, 0);  // evaluate arg
    return SKIP_NORMAL_DISPATCH;
}

IO_METHOD(IoTrue, ifFalse) {
    frame->result = self;                     // skip arg
    frame->target = self;
    frame->message = IoMessage_next(m);
    return SKIP_NORMAL_DISPATCH;
}

// On false object
IO_METHOD(IoFalse, ifTrue) {
    frame->result = self;                     // skip arg
    frame->target = self;
    frame->message = IoMessage_next(m);
    return SKIP_NORMAL_DISPATCH;
}

IO_METHOD(IoFalse, ifFalse) {
    frame->message = IoMessage_argAt(m, 0);  // evaluate arg
    return SKIP_NORMAL_DISPATCH;
}
```

### Forward Implementation

```c
// Default forward - raises error
IO_METHOD(IoObject, forward) {
    IoSymbol *name = IoMessage_name(m);
    return IoState_error(state, "Object does not respond to '%s'", CSTRING(name));
}

// Delegating forward - redirects to another object
IO_METHOD(IoProxyObject, forward) {
    IoObject *delegate = IoObject_getSlot_(self, state->delegateSymbol);
    frame->target = delegate;
    // frame->message stays the same - retry on delegate
    return SKIP_NORMAL_DISPATCH;
}
```

## Control Flow in Io

Built on ifTrue/ifFalse + recursion + TCO:

```io
if := method(cond, then, else,
    cond ifTrue(return then)
    else
)

while := method(cond, body,
    cond ifTrue(
        body
        while(cond, body)
    )
)

for := method(counter, start, end, body,
    counter := start
    while(counter <= end,
        body
        counter = counter + 1
    )
)

loop := method(body,
    body
    loop(body)
)
```

## Argument Evaluation

For normal primitives, arguments must be evaluated before calling.

**Option A: Eager evaluation in eval loop**

Before calling CFunction, eval loop evaluates all args:

```c
if (ISCFUNCTION(slotValue)) {
    // Evaluate args first
    int argc = IoMessage_argCount(m);
    IoObject **args = alloca(argc * sizeof(IoObject *));

    for (int i = 0; i < argc; i++) {
        // Push frame to evaluate arg[i]
        // ... this needs thought ...
    }

    IoObject *result = IoCFunction_activate(slotValue, frame->target,
                                             frame, m, context, args, argc);
}
```

**Option B: Lazy evaluation via frame manipulation**

Primitives that need evaluated args set up frames:

```c
IO_METHOD(IoNumber, add) {
    if (!frame->argsEvaluated) {
        // Request arg evaluation
        frame->argsEvaluated = true;
        // Set up frame to evaluate arg 0, then return here
        // ... complex ...
        return SKIP_NORMAL_DISPATCH;
    }
    // Args ready
    double a = IoNumber_asDouble(self);
    double b = IoNumber_asDouble(frame->evaluatedArgs[0]);
    return IoNumber_new(state, a + b);
}
```

**Option C: Synchronous arg evaluation (current Io)**

Args evaluated synchronously, but only for non-control-flow primitives.
This requires careful design to avoid re-entrancy.

## Deep Analysis: Potential Issues

### 1. Argument Evaluation Without Re-entrancy

**Problem:** Normal primitives need evaluated args, but evaluating args requires the eval loop.

**Solution:** Hybrid approach:

```c
IoObject *Frame_evaluateArgSync(Frame *frame, IoMessage *m, int i) {
    IoMessage *argMsg = IoMessage_argAt(m, i);

    // Fast path: literal
    if (IoMessage_cachedResult(argMsg)) {
        return IoMessage_cachedResult(argMsg);
    }

    // Fast path: simple slot lookup (no chain, no args)
    if (!IoMessage_next(argMsg) && IoMessage_argCount(argMsg) == 0) {
        IoObject *slotValue = IoObject_getSlot_(frame->locals, IoMessage_name(argMsg));
        if (slotValue && !IoObject_isActivatable(slotValue)) {
            return slotValue;
        }
    }

    // Slow path: need full frame-based evaluation
    return NEEDS_FRAME_EVAL;
}
```

Most args (literals, variable lookups) hit the fast path. Complex expressions fall back to frame-based evaluation, which requires the primitive to be written as a state machine or for the eval loop to pre-evaluate.

**Alternative:** Pre-evaluate ALL args in eval loop before calling primitives:

```c
if (ISCFUNCTION(slotValue) && !isControlFlowPrimitive(slotValue)) {
    // Push frames to evaluate all args
    // After all args evaluated, call primitive
    // This requires tracking "pending primitive call" state
}
```

### 2. ifTrue/ifFalse Must Push Frames

**Issue discovered:** Simply setting `frame->message` to the arg message doesn't work correctly for return flow.

**Partially corrected implementation** (still has a bug - see Issue 1 in Design Review):

```c
IO_METHOD(IoTrue, ifTrue) {
    // Push frame to evaluate the argument
    Frame *argFrame = Frame_new();
    argFrame->message = IoMessage_argAt(m, 0);
    argFrame->locals = frame->locals;
    argFrame->target = frame->target;   // BUG: should be frame->locals
    argFrame->caller = frame;

    state->currentFrame = argFrame;

    // When argFrame completes, current frame continues to next message
    frame->message = IoMessage_next(m);

    return SKIP_NORMAL_DISPATCH;
}

IO_METHOD(IoFalse, ifTrue) {
    // Don't evaluate arg, just continue
    frame->result = self;
    frame->target = self;
    frame->message = IoMessage_next(m);
    return SKIP_NORMAL_DISPATCH;
}
```

**Trace:** `true ifTrue(x) println`
1. Eval `true` → result=true, message=`ifTrue(x)`
2. Lookup ifTrue on true, call IoTrue_ifTrue
3. Push argFrame for `x`, set frame->message to `println`
4. argFrame evaluates `x`, returns result
5. Pop to frame: result=x_result, target=x_result
6. Eval `println` on x_result

This correctly chains the result. **But** the argFrame->target is wrong - see Issue 1 for the fully corrected version.

### 3. TCO Detection

**Problem:** When is a call in tail position?

**Answer:** A call is in tail position when:
- It's the last message in a chain (no `next`)
- AND we're about to return from the current block

**Detection:**

```c
bool isTailPosition(Frame *frame, IoMessage *m) {
    return IoMessage_next(m) == NULL &&
           frame->message == m;  // not in middle of evaluating something
}
```

**When to apply:**
- Block activation when in tail position → reuse frame
- Must be careful with control flow (ifTrue pushes frames, not TCO)

### 4. Return Statement

**Problem:** `return x` needs to exit current block and return x.

**Solution:** Set a flag and unwind:

```c
typedef struct Frame {
    // ... existing fields ...
    bool isBlockBoundary;  // true for block/method activations
} Frame;

IO_METHOD(IoObject, return) {
    // Evaluate the return value (need frame for this)
    // Then set stop status
    state->stopStatus = STOP_RETURN;
    state->returnValue = evaluatedArg;

    return SKIP_NORMAL_DISPATCH;
}
```

In eval loop:
```c
// At start of loop iteration
if (state->stopStatus == STOP_RETURN) {
    // Unwind to block boundary
    while (frame && !frame->isBlockBoundary) {
        frame = frame->caller;
    }
    if (frame) {
        frame->result = state->returnValue;
        frame->message = NULL;  // signal return
    }
    state->stopStatus = STOP_NONE;
    state->currentFrame = frame;
    continue;
}
```

### 5. Break/Continue in Loops

**Problem:** With `while` as recursive Io code, how do break/continue work?

**Option A:** Use stop status flags

```c
IO_METHOD(IoObject, break) {
    state->stopStatus = STOP_BREAK;
    return SKIP_NORMAL_DISPATCH;
}
```

In while (Io code), check stop status:
```io
while := method(cond, body,
    cond ifTrue(
        body
        _checkBreak ifFalse(
            while(cond, body)
        )
    )
)
```

But `_checkBreak` would need to be a primitive...

**Option B:** Implement while/for/loop as C primitives

These control structures manipulate frames directly and handle break/continue:

```c
IO_METHOD(IoObject, while) {
    // Mark frame as loop boundary
    frame->isLoopBoundary = true;
    frame->loopCondition = IoMessage_argAt(m, 0);
    frame->loopBody = IoMessage_argAt(m, 1);

    // Set up to evaluate condition
    Frame *condFrame = Frame_new();
    condFrame->message = frame->loopCondition;
    // ... set up frame ...

    // After condition, check result and either:
    // - evaluate body then loop back
    // - or exit

    return SKIP_NORMAL_DISPATCH;
}
```

**Recommendation:** Option B is cleaner. while/for/loop as C primitives that manage their own looping via frame manipulation. Only `ifTrue`/`ifFalse` need to be minimal.

### 6. Semicolon / Message Chain Boundaries

**Problem:** In `a; b`, semicolon resets target to original.

**Solution:** Track original target in frame:

```c
typedef struct Frame {
    // ... existing fields ...
    IoObject *originalTarget;  // reset point for semicolon
} Frame;
```

When evaluating semicolon message:
```c
if (name == state->semicolonSymbol) {
    frame->target = frame->originalTarget;
    frame->message = IoMessage_next(m);
    continue;
}
```

### 7. The `call` Object

**Problem:** Io's `call` provides introspection: sender, message, target, etc.

**Solution:** Create Call object during block activation:

```c
// In block activation
IoCall *callObj = IoCall_new(state);
IoCall_setSender_(callObj, frame->caller->locals);
IoCall_setMessage_(callObj, m);
IoCall_setTarget_(callObj, frame->target);
IoCall_setActivated_(callObj, block);

// Add to new locals
IoObject_setSlot_(newLocals, state->callSymbol, callObj);
```

This is already how current Io works. Just need to preserve it.

### 8. Garbage Collection

**Problem:** Frames reference IoObjects. GC must mark them.

**Solution:** Walk frame stack during GC mark phase:

```c
void IoState_markFrames(IoState *state) {
    Frame *frame = state->currentFrame;
    while (frame) {
        IoObject_mark(frame->locals);
        IoObject_mark(frame->target);
        IoObject_mark(frame->result);
        // Mark messages if they're IoObjects
        frame = frame->caller;
    }
}
```

### 9. Coroutine Capture/Restore

**Capture:**
```c
IoCoroutine *IoCoroutine_capture(IoState *state) {
    IoCoroutine *coro = IoCoroutine_new(state);

    // Deep copy frame stack
    coro->frameStack = copyFrameStack(state->currentFrame);

    return coro;
}
```

**Restore:**
```c
void IoCoroutine_resume(IoCoroutine *coro, IoState *state) {
    // Save current frame stack (for the calling coroutine)
    state->currentCoroutine->frameStack = state->currentFrame;

    // Restore this coroutine's frame stack
    state->currentFrame = coro->frameStack;
    state->currentCoroutine = coro;
}
```

Locals objects are heap-allocated, so they survive capture automatically.

### 10. Slot Assignment (:= and =)

**Problem:** `x := 1` and `x = 1` are special forms.

**Solution:** Handled by parser or as primitives:

```c
IO_METHOD(IoObject, setSlot) {
    // args: name (symbol), value
    IoSymbol *name = args[0];  // or get from message
    IoObject *value = args[1];
    IoObject_setSlot_(frame->target, name, value);
    return value;
}
```

The parser transforms `x := 1` into `setSlot("x", 1)`.

### 11. Lazy Argument Evaluation

**Current Io behavior:** Args only evaluated when accessed.

**Question:** Do we preserve this?

**Options:**
- A: Pre-evaluate all args (loses lazy semantics)
- B: Keep lazy, primitives request evaluation (complex)
- C: Lazy for control flow, eager for normal primitives (pragmatic)

**Recommendation:** Option C. Control flow primitives receive unevaluated messages. Normal primitives get evaluated args. This matches user expectations - side effects in args happen in predictable order.

## Revised Design Decisions

1. **ifTrue/ifFalse** push frames (not just pointer manipulation)
2. **while/for/loop** implemented as C primitives for clean break/continue
3. **Normal primitives** receive evaluated args (eager)
4. **Control flow primitives** receive frame + message (lazy)
5. **break/continue/return** use stop status flags + frame unwinding
6. **TCO** applied at block activation when in tail position
7. **Frame has** originalTarget for semicolon reset
8. **call object** created during block activation

## Minimal Primitive Set

After analysis, the true minimal set of C primitives:

**Must be C (frame manipulation):**
- `ifTrue` on true/false
- `ifFalse` on true/false
- `while` (for break/continue)
- `for` (for break/continue)
- `loop` (for break/continue)
- `return`
- `break`
- `continue`
- `try`/`catch` (exception handling)

**Should be C (performance):**
- Number arithmetic
- String operations
- List/Array operations
- Slot operations (setSlot, getSlot, etc.)

**Can be Io:**
- `if` (built on ifTrue/ifFalse)
- Higher-level iteration (forEach, map, select - built on while/for)
- Most object methods

---

# Design Review: Identified Issues and Solutions

## Issue 1: ifTrue argFrame has wrong initial target

**Problem:** In section 2, the corrected ifTrue pushes an argFrame with:
```c
argFrame->target = frame->target;  // BUG: target is `true`, not locals
```

When evaluating `true ifTrue(x)`, the argument `x` should be looked up starting from **locals**, not from `true`. Io's slot lookup searches target → target's protos, and locals are NOT automatically searched unless target IS locals.

**Solution:**
```c
IO_METHOD(IoTrue, ifTrue) {
    Frame *argFrame = Frame_new();
    argFrame->message = IoMessage_argAt(m, 0);
    argFrame->locals = frame->locals;
    argFrame->target = frame->locals;      // FIXED: start lookup from locals
    argFrame->originalTarget = frame->locals;
    argFrame->caller = frame;

    state->currentFrame = argFrame;
    frame->message = IoMessage_next(m);

    return SKIP_NORMAL_DISPATCH;
}
```

## Issue 2: Forward passes undefined context

**Problem:** In the main eval loop, when a slot isn't found, `context` is undefined:
```c
IoObject *slotValue = IoObject_getSlot_context_(frame->target, name, &context);
if (!slotValue) {
    IoObject *forwardSlot = IoObject_getSlot_(frame->target, state->forwardSymbol);
    // ...
    IoCFunction_activate(forwardSlot, frame->target, frame, m, context);  // STALE
```

**Solution:** Get context when looking up forward:
```c
if (!slotValue) {
    IoObject *forwardContext;
    IoObject *forwardSlot = IoObject_getSlot_context_(frame->target,
                                                       state->forwardSymbol,
                                                       &forwardContext);
    if (forwardSlot && IoObject_isActivatable(forwardSlot)) {
        IoObject *result = IoCFunction_activate(forwardSlot, frame->target,
                                                 frame, m, forwardContext);
        // ...
    }
}
```

## Issue 3: Final result not saved to state

**Problem:** At eval loop exit, `state->result` is never set. When the last frame pops, there's no caller to receive the result.

**Solution:** Save result when popping the last frame:
```c
if (!m) {
    IoObject *result = frame->result;
    Frame *caller = frame->caller;
    if (caller) {
        caller->result = result;
        caller->target = result;
        frame = caller;
    } else {
        state->result = result;  // SAVE FINAL RESULT
        frame = NULL;
    }
    state->currentFrame = frame;
    continue;
}
```

## Issue 4: originalTarget never initialized

**Problem:** The Frame has `originalTarget` for semicolon reset, but block activation never sets it.

**Solution:** Initialize in block activation:
```c
// Push new frame for block
Frame *newFrame = Frame_new();
newFrame->message = IoBlock_message(block);
newFrame->locals = newLocals;
newFrame->target = newLocals;
newFrame->originalTarget = newLocals;  // ADD THIS
newFrame->caller = frame;
```

---

# Break/Continue: The passStops Approach

After reviewing current Io's implementation, the `passStops` mechanism is cleaner than `isLoopBoundary`. Here's how it works:

## Stop Status Constants

```c
#define STOP_STATUS_NORMAL   0
#define STOP_STATUS_BREAK    1
#define STOP_STATUS_CONTINUE 2
#define STOP_STATUS_RETURN   4
```

## Block passStops Flag

Blocks have a `passStops` flag (default false). When false, the block **catches** break/continue/return. When true, they propagate through.

```c
typedef struct Frame {
    IoMessage *message;
    struct Frame *caller;
    IoObject *locals;
    IoObject *target;
    IoObject *result;
    IoObject *originalTarget;

    // Control flow
    bool isBlockActivation;  // true for block/method frames
    bool passStops;          // if false, catches break/continue/return
} Frame;
```

## Break and Continue Primitives

```c
IO_METHOD(IoObject, break) {
    IoObject *value = IONIL(self);

    // Break can take an optional return value
    if (IoMessage_argCount(m) > 0) {
        // Must evaluate arg before setting stop status
        // Use fast path if possible, otherwise push frame
        IoMessage *argMsg = IoMessage_argAt(m, 0);

        if (IoMessage_cachedResult(argMsg)) {
            value = IoMessage_cachedResult(argMsg);
        } else if (!IoMessage_next(argMsg) && IoMessage_argCount(argMsg) == 0) {
            // Simple slot lookup
            IoObject *v = IoObject_getSlot_(frame->locals, IoMessage_name(argMsg));
            if (v && !IoObject_isActivatable(v)) {
                value = v;
            } else {
                // Need frame-based eval - push special frame
                Frame *argFrame = Frame_new();
                argFrame->message = argMsg;
                argFrame->locals = frame->locals;
                argFrame->target = frame->locals;
                argFrame->caller = frame;
                argFrame->isBreakArg = true;  // Special marker
                state->currentFrame = argFrame;
                frame->message = NULL;
                return SKIP_NORMAL_DISPATCH;
            }
        }
    }

    state->stopStatus = STOP_STATUS_BREAK;
    state->returnValue = value;
    return SKIP_NORMAL_DISPATCH;
}

IO_METHOD(IoObject, continue) {
    state->stopStatus = STOP_STATUS_CONTINUE;
    return SKIP_NORMAL_DISPATCH;
}
```

## Stop Status Handling in Eval Loop

```c
// After popping a frame that was evaluating break's arg
if (frame && frame->isBreakArg) {
    state->stopStatus = STOP_STATUS_BREAK;
    state->returnValue = frame->result;
    frame = frame->caller;
    state->currentFrame = frame;
}

// Handle stop status
if (state->stopStatus != STOP_STATUS_NORMAL) {
    // Unwind to a frame that catches this stop
    while (frame) {
        // Block activations without passStops catch the stop
        if (frame->isBlockActivation && !frame->passStops) {
            frame->result = state->returnValue;
            frame->message = NULL;  // Signal return from this frame
            state->stopStatus = STOP_STATUS_NORMAL;
            break;
        }
        frame = frame->caller;
    }
    state->currentFrame = frame;
    continue;
}
```

## C-Level Loops Check Status After Body

```c
IO_METHOD(IoObject, while) {
    IoState_resetStopStatus(state);

    for (;;) {
        // Evaluate condition
        IoObject *cond = IoMessage_locals_valueArgAt_(m, locals, 0);
        if (!ISTRUE(cond)) break;

        // Evaluate body
        IoObject *result = IoMessage_locals_valueArgAt_(m, locals, 1);

        // Check for break/continue
        int status = state->stopStatus;
        if (status == STOP_STATUS_BREAK) {
            IoState_resetStopStatus(state);
            return state->returnValue;  // break(value) returns this
        }
        if (status == STOP_STATUS_CONTINUE) {
            IoState_resetStopStatus(state);
            // Continue to next iteration
        }
        if (status == STOP_STATUS_RETURN) {
            // Don't reset - let it propagate
            return result;
        }
    }
    return result;
}
```

## Io-Level Iteration with passStops

For Io code that implements iteration, the body block needs `passStops = true`:

```io
List myForEach := method(body,
    body setPassStops(true)  // Let break/continue propagate to caller's loop
    i := 0
    while(i < self size,
        body call(self at(i))
        i = i + 1
    )
)

// Usage - break works correctly
list(1, 2, 3, 4, 5) myForEach(x,
    if(x == 3, break)
    x println
)
```

Without `setPassStops(true)`, the break would be caught by the body block and not propagate to the while loop.

## Frame-Based While (Iterative, No Re-entrancy)

For the fully iterative eval loop, while needs to be a state machine:

```c
typedef enum {
    WHILE_EVAL_COND,
    WHILE_CHECK_COND,
    WHILE_EVAL_BODY,
    WHILE_DONE
} WhileState;

typedef struct WhileFrame {
    Frame base;
    WhileState state;
    IoMessage *condMsg;
    IoMessage *bodyMsg;
    IoObject *lastResult;
} WhileFrame;

// In eval loop, handle WhileFrame specially:
if (frame->type == FRAME_WHILE) {
    WhileFrame *wf = (WhileFrame *)frame;

    switch (wf->state) {
    case WHILE_EVAL_COND:
        // Push frame to evaluate condition
        {
            Frame *condFrame = Frame_new();
            condFrame->message = wf->condMsg;
            condFrame->locals = frame->locals;
            condFrame->target = frame->locals;
            condFrame->caller = frame;
            state->currentFrame = condFrame;
            wf->state = WHILE_CHECK_COND;
        }
        continue;

    case WHILE_CHECK_COND:
        // Condition result is in frame->result (from popped condFrame)
        if (!ISTRUE(frame->result)) {
            wf->state = WHILE_DONE;
            frame->result = wf->lastResult;
            frame->message = NULL;  // Signal done
        } else {
            // Push frame to evaluate body
            Frame *bodyFrame = Frame_new();
            bodyFrame->message = wf->bodyMsg;
            bodyFrame->locals = frame->locals;
            bodyFrame->target = frame->locals;
            bodyFrame->caller = frame;
            bodyFrame->passStops = true;  // Let break/continue come back to us
            state->currentFrame = bodyFrame;
            wf->state = WHILE_EVAL_BODY;
        }
        continue;

    case WHILE_EVAL_BODY:
        // Body result is in frame->result
        wf->lastResult = frame->result;

        // Check for break/continue
        if (state->stopStatus == STOP_STATUS_BREAK) {
            state->stopStatus = STOP_STATUS_NORMAL;
            frame->result = state->returnValue;
            frame->message = NULL;
            continue;
        }
        if (state->stopStatus == STOP_STATUS_CONTINUE) {
            state->stopStatus = STOP_STATUS_NORMAL;
            // Fall through to re-evaluate condition
        }
        if (state->stopStatus == STOP_STATUS_RETURN) {
            // Don't catch return - let it propagate
            frame->message = NULL;
            continue;
        }

        // Loop back to condition
        wf->state = WHILE_EVAL_COND;
        continue;

    case WHILE_DONE:
        // Handled by normal frame pop
        break;
    }
}
```

---

# Argument Evaluation: Complete Solution

## The Problem

Normal primitives need evaluated args, but we can't recursively call eval.

## Solution: ArgEval Frames

When a CFunction needs evaluated args, push an ArgEvalFrame:

```c
typedef struct ArgEvalFrame {
    Frame base;
    IoCFunction *pendingPrimitive;
    IoObject *receiver;
    IoObject *context;
    IoMessage *originalMessage;
    int argCount;
    int nextArg;
    IoObject **evaluatedArgs;
} ArgEvalFrame;
```

## Eval Loop Integration

```c
// When activating a CFunction
if (ISCFUNCTION(slotValue)) {
    IoCFunction *cf = (IoCFunction *)slotValue;

    // Control flow primitives handle their own args
    if (cf->isControlFlow) {
        IoObject *result = IoCFunction_activate(cf, frame->target, frame, m, context);
        if (result == SKIP_NORMAL_DISPATCH) continue;
        frame->result = result;
        frame->target = result;
        frame->message = IoMessage_next(m);
        continue;
    }

    // Normal primitives need evaluated args
    int argc = IoMessage_argCount(m);
    if (argc == 0) {
        // No args - call directly
        IoObject *result = IoCFunction_activate(cf, frame->target, frame, m, context);
        frame->result = result;
        frame->target = result;
        frame->message = IoMessage_next(m);
        continue;
    }

    // Try fast path for all args
    IoObject **args = alloca(argc * sizeof(IoObject *));
    bool allFast = true;

    for (int i = 0; i < argc && allFast; i++) {
        IoMessage *argMsg = IoMessage_argAt(m, i);

        if (IoMessage_cachedResult(argMsg)) {
            args[i] = IoMessage_cachedResult(argMsg);
        } else if (!IoMessage_next(argMsg) && IoMessage_argCount(argMsg) == 0) {
            IoObject *v = IoObject_getSlot_(frame->locals, IoMessage_name(argMsg));
            if (v && !IoObject_isActivatable(v)) {
                args[i] = v;
            } else {
                allFast = false;
            }
        } else {
            allFast = false;
        }
    }

    if (allFast) {
        // All args evaluated via fast path - call primitive
        IoObject *result = IoCFunction_activateWithArgs(cf, frame->target,
                                                         frame, m, context, args, argc);
        frame->result = result;
        frame->target = result;
        frame->message = IoMessage_next(m);
        continue;
    }

    // Slow path: push ArgEvalFrame
    ArgEvalFrame *aef = ArgEvalFrame_new();
    aef->base.type = FRAME_ARG_EVAL;
    aef->base.caller = frame;
    aef->base.locals = frame->locals;
    aef->pendingPrimitive = cf;
    aef->receiver = frame->target;
    aef->context = context;
    aef->originalMessage = m;
    aef->argCount = argc;
    aef->nextArg = 0;
    aef->evaluatedArgs = IoState_alloc(state, argc * sizeof(IoObject *));

    // Copy any args we already evaluated
    for (int i = 0; i < argc; i++) {
        if (args[i]) aef->evaluatedArgs[i] = args[i];
    }

    // Find first arg that needs frame eval
    while (aef->nextArg < argc && aef->evaluatedArgs[aef->nextArg]) {
        aef->nextArg++;
    }

    // Push frame to evaluate that arg
    Frame *argFrame = Frame_new();
    argFrame->message = IoMessage_argAt(m, aef->nextArg);
    argFrame->locals = frame->locals;
    argFrame->target = frame->locals;
    argFrame->caller = (Frame *)aef;

    frame->message = IoMessage_next(m);  // After primitive, continue here
    state->currentFrame = argFrame;
    continue;
}

// Handle ArgEvalFrame receiving a result
if (frame->type == FRAME_ARG_EVAL) {
    ArgEvalFrame *aef = (ArgEvalFrame *)frame;

    // Store the just-evaluated arg
    aef->evaluatedArgs[aef->nextArg] = aef->base.result;
    aef->nextArg++;

    // Find next arg needing eval
    while (aef->nextArg < aef->argCount && aef->evaluatedArgs[aef->nextArg]) {
        aef->nextArg++;
    }

    if (aef->nextArg < aef->argCount) {
        // More args to evaluate
        Frame *argFrame = Frame_new();
        argFrame->message = IoMessage_argAt(aef->originalMessage, aef->nextArg);
        argFrame->locals = aef->base.locals;
        argFrame->target = aef->base.locals;
        argFrame->caller = (Frame *)aef;
        state->currentFrame = argFrame;
        continue;
    }

    // All args ready - call the primitive
    IoObject *result = IoCFunction_activateWithArgs(
        aef->pendingPrimitive, aef->receiver,
        (Frame *)aef, aef->originalMessage, aef->context,
        aef->evaluatedArgs, aef->argCount);

    // Pop ArgEvalFrame, store result in caller
    Frame *caller = aef->base.caller;
    caller->result = result;
    caller->target = result;
    state->currentFrame = caller;

    // Free the ArgEvalFrame
    ArgEvalFrame_free(aef);
    continue;
}
```

---

# Exception Handling: try/catch

## Frame Structure

```c
typedef struct TryFrame {
    Frame base;
    IoMessage *catchMsg;
    IoSymbol *exceptionSlotName;
    bool inCatch;
} TryFrame;
```

## try Primitive

```c
IO_METHOD(IoObject, try) {
    // try(tryBody, catch(e, catchBody))
    // or: try(tryBody) catch(e, catchBody)

    TryFrame *tf = TryFrame_new();
    tf->base.type = FRAME_TRY;
    tf->base.caller = frame;
    tf->base.locals = frame->locals;
    tf->catchMsg = IoMessage_argAt(m, 1);  // The catch clause
    tf->exceptionSlotName = /* extract from catch */;
    tf->inCatch = false;

    // Push frame to evaluate try body
    Frame *tryFrame = Frame_new();
    tryFrame->message = IoMessage_argAt(m, 0);
    tryFrame->locals = frame->locals;
    tryFrame->target = frame->locals;
    tryFrame->caller = (Frame *)tf;
    tryFrame->passStops = true;  // Let exceptions propagate to TryFrame

    state->currentFrame = tryFrame;
    frame->message = IoMessage_next(m);

    return SKIP_NORMAL_DISPATCH;
}
```

## Exception Handling in Eval Loop

```c
#define STOP_STATUS_EXCEPTION 8

// Raising an exception
void IoState_raise(IoState *state, IoObject *exception) {
    state->stopStatus = STOP_STATUS_EXCEPTION;
    state->exception = exception;
}

// In eval loop, handle exceptions
if (state->stopStatus == STOP_STATUS_EXCEPTION) {
    // Unwind looking for TryFrame
    while (frame) {
        if (frame->type == FRAME_TRY) {
            TryFrame *tf = (TryFrame *)frame;
            if (!tf->inCatch && tf->catchMsg) {
                // Found a try block - execute catch
                tf->inCatch = true;

                // Bind exception to slot name
                IoObject_setSlot_(frame->locals, tf->exceptionSlotName, state->exception);
                state->stopStatus = STOP_STATUS_NORMAL;
                state->exception = NULL;

                // Push frame for catch body
                Frame *catchFrame = Frame_new();
                catchFrame->message = IoMessage_argAt(tf->catchMsg, 1);
                catchFrame->locals = frame->locals;
                catchFrame->target = frame->locals;
                catchFrame->caller = frame;
                state->currentFrame = catchFrame;
                break;
            }
        }
        frame = frame->caller;
    }

    if (!frame) {
        // Uncaught exception - exit eval loop with error
        state->currentFrame = NULL;
        return;  // Or handle top-level exception
    }
    continue;
}
```

---

# TCO: Refined Detection

## The Challenge

TCO should apply when a block call is in tail position. But "tail position" is subtle:

| Code | Tail position? |
|------|---------------|
| `method(foo)` | foo: yes |
| `method(foo bar)` | foo: no, bar: yes |
| `method(if(c, a, b))` | a and b: yes (inside if) |
| `method(x := foo)` | foo: no (assignment after) |
| `method(a; foo)` | a: no, foo: yes |
| `method(return foo)` | foo: no (return handles it) |

## Solution: Mark at Parse Time

The parser can mark messages that are in tail position:

```c
typedef struct IoMessage {
    // ... existing fields ...
    bool isTailPosition;
} IoMessage;

// Parser sets this when:
// - Message is last in chain (no next)
// - AND not inside an assignment RHS
// - AND not inside return's argument
```

## TCO in Block Activation

```c
if (ISBLOCK(slotValue)) {
    IoBlock *block = slotValue;
    IoObject *newLocals = IoObject_createLocals(block, frame, m);

    // TCO: if in tail position AND current frame is a block activation
    if (m->isTailPosition && frame->isBlockActivation) {
        // Reuse frame
        frame->message = IoBlock_message(block);
        frame->locals = newLocals;
        frame->target = newLocals;
        frame->originalTarget = newLocals;
        // Keep frame->caller, passStops, etc.
    } else {
        // Push new frame
        Frame *newFrame = Frame_new();
        newFrame->message = IoBlock_message(block);
        newFrame->locals = newLocals;
        newFrame->target = newLocals;
        newFrame->originalTarget = newLocals;
        newFrame->caller = frame;
        newFrame->isBlockActivation = true;
        newFrame->passStops = IoBlock_passStops(block);
        frame = newFrame;
        state->currentFrame = frame;
    }
}
```

## TCO for ifTrue/ifFalse Branches

When ifTrue/ifFalse is itself in tail position, the branch can inherit that:

```c
IO_METHOD(IoTrue, ifTrue) {
    Frame *argFrame = Frame_new();
    argFrame->message = IoMessage_argAt(m, 0);
    argFrame->locals = frame->locals;
    argFrame->target = frame->locals;
    argFrame->originalTarget = frame->locals;
    argFrame->caller = frame;
    argFrame->passStops = true;

    // If ifTrue is in tail position, mark the arg frame for TCO eligibility
    argFrame->inheritTailPosition = m->isTailPosition;

    state->currentFrame = argFrame;
    frame->message = IoMessage_next(m);

    return SKIP_NORMAL_DISPATCH;
}
```

---

# Updated Frame Structure

```c
typedef enum {
    FRAME_NORMAL,
    FRAME_WHILE,
    FRAME_FOR,
    FRAME_ARG_EVAL,
    FRAME_TRY
} FrameType;

typedef struct Frame {
    FrameType type;
    IoMessage *message;
    struct Frame *caller;
    IoObject *locals;
    IoObject *target;
    IoObject *result;
    IoObject *originalTarget;

    // Block activation info
    bool isBlockActivation;
    bool passStops;

    // TCO
    bool inheritTailPosition;

    // Special markers
    bool isBreakArg;
} Frame;
```

---

# Updated Minimal Primitive Set

**Must be C (frame/state manipulation):**
- `ifTrue` / `ifFalse` on true/false objects
- `while` / `for` / `loop` (state machine frames)
- `return` / `break` / `continue` (stop status)
- `try` / `catch` / `raise` (exception handling)
- `setPassStops` on Block

**Should be C (performance + arg eval):**
- Number arithmetic
- String operations
- List/Map operations
- Slot operations

**Can be Io:**
- `if` (built on ifTrue/ifFalse)
- `forEach` / `map` / `select` (use setPassStops for break/continue support)
- Most object methods

---

# Remaining Edge Cases

## 1. Break/Continue Outside Loop

What if `break` is called with no enclosing loop?

```io
break  // No loop!
```

The stop status propagates up. If no frame catches it (no block without passStops, or all blocks have passStops=true), it reaches the top of the frame stack.

**Solution:** Check at top level:

```c
// After unwinding completes
if (!frame && state->stopStatus != STOP_STATUS_NORMAL) {
    if (state->stopStatus == STOP_STATUS_BREAK) {
        IoState_error(state, "break called outside of loop");
    } else if (state->stopStatus == STOP_STATUS_CONTINUE) {
        IoState_error(state, "continue called outside of loop");
    }
    state->stopStatus = STOP_STATUS_NORMAL;
}
```

## 2. Return from Top Level

```io
return 42  // Outside any method
```

**Behavior:** Return at top level should probably just end evaluation with that value:

```c
// In stop status handling
if (state->stopStatus == STOP_STATUS_RETURN) {
    while (frame && frame->passStops) {
        frame = frame->caller;
    }
    if (frame) {
        frame->result = state->returnValue;
        frame->message = NULL;
        state->stopStatus = STOP_STATUS_NORMAL;
    } else {
        // Top-level return - end evaluation
        state->result = state->returnValue;
        state->stopStatus = STOP_STATUS_NORMAL;
        state->currentFrame = NULL;
    }
}
```

## 3. Exception in Catch Block

```io
try(
    raise("first")
) catch(e,
    raise("second")  // Exception while handling exception
)
```

When `raise("second")` executes, we're in the TryFrame's catch phase (`inCatch = true`). The exception should propagate to an outer try block, not be caught by the same try.

The current design handles this because we check `!tf->inCatch`:

```c
if (frame->type == FRAME_TRY) {
    TryFrame *tf = (TryFrame *)frame;
    if (!tf->inCatch && tf->catchMsg) {  // Won't catch if already in catch
        // ...
    }
}
```

## 4. GC and Special Frames

ArgEvalFrame, WhileFrame, TryFrame have extra heap-allocated data. GC must mark them:

```c
void IoState_markFrames(IoState *state) {
    Frame *frame = state->currentFrame;
    while (frame) {
        // Mark common fields
        IoObject_mark(frame->locals);
        IoObject_mark(frame->target);
        IoObject_mark(frame->result);
        IoObject_mark(frame->originalTarget);

        // Mark type-specific fields
        switch (frame->type) {
        case FRAME_ARG_EVAL: {
            ArgEvalFrame *aef = (ArgEvalFrame *)frame;
            IoObject_mark(aef->receiver);
            for (int i = 0; i < aef->argCount; i++) {
                if (aef->evaluatedArgs[i]) {
                    IoObject_mark(aef->evaluatedArgs[i]);
                }
            }
            break;
        }
        case FRAME_WHILE: {
            WhileFrame *wf = (WhileFrame *)frame;
            IoObject_mark(wf->lastResult);
            break;
        }
        case FRAME_TRY: {
            TryFrame *tf = (TryFrame *)frame;
            // catchMsg is IoMessage, mark if needed
            break;
        }
        default:
            break;
        }

        frame = frame->caller;
    }
}
```

## 5. Coroutine Capture with Special Frames

When capturing a coroutine, we shallow-copy frame structs. Special frames need their extra data copied too:

```c
Frame *copyFrameStack(Frame *frame) {
    if (!frame) return NULL;

    Frame *copy;
    switch (frame->type) {
    case FRAME_ARG_EVAL:
        copy = (Frame *)ArgEvalFrame_clone((ArgEvalFrame *)frame);
        break;
    case FRAME_WHILE:
        copy = (Frame *)WhileFrame_clone((WhileFrame *)frame);
        break;
    case FRAME_TRY:
        copy = (Frame *)TryFrame_clone((TryFrame *)frame);
        break;
    default:
        copy = Frame_clone(frame);
        break;
    }

    copy->caller = copyFrameStack(frame->caller);
    return copy;
}
```

**Note:** IoObjects are NOT copied - they're shared between coroutine stacks. This is intentional for coroutines (shared mutable state). For full call/cc semantics, this would need consideration.

## 6. Stop Status and Coroutines

Each coroutine must have its own stopStatus:

```c
typedef struct IoCoroutine {
    IoObject base;
    Frame *frameStack;
    int stopStatus;           // Per-coroutine
    IoObject *returnValue;    // Per-coroutine
    IoObject *exception;      // Per-coroutine
} IoCoroutine;
```

When switching coroutines:

```c
void IoCoroutine_switch(IoState *state, IoCoroutine *from, IoCoroutine *to) {
    // Save current state
    from->frameStack = state->currentFrame;
    from->stopStatus = state->stopStatus;
    from->returnValue = state->returnValue;
    from->exception = state->exception;

    // Restore target state
    state->currentFrame = to->frameStack;
    state->stopStatus = to->stopStatus;
    state->returnValue = to->returnValue;
    state->exception = to->exception;

    state->currentCoroutine = to;
}
```

This ensures break/continue/return/exceptions don't leak between coroutines.

## 7. Deeply Nested Arg Evaluation

What if an arg itself calls a function that needs arg evaluation?

```io
foo(bar(baz(x)))
```

This creates nested ArgEvalFrames:
```
[original frame]
  [ArgEvalFrame for foo]
    [arg frame evaluating bar(baz(x))]
      [ArgEvalFrame for bar]
        [arg frame evaluating baz(x)]
          [ArgEvalFrame for baz]
            [arg frame evaluating x]
```

This works correctly because each ArgEvalFrame maintains its own state and they nest naturally through the caller chain.

## 8. Primitive Calling Another Primitive

What if a C primitive needs to call Io code internally?

```c
IO_METHOD(IoList, sort) {
    // Need to call user-provided comparison block
    // But we're in a primitive - can't re-enter eval!
}
```

**Solution:** The primitive must set up frames and return SKIP_NORMAL_DISPATCH. It becomes a state machine:

```c
typedef struct SortFrame {
    Frame base;
    IoList *list;
    IoBlock *compareBlock;
    int sortState;
    // ... sorting algorithm state ...
} SortFrame;
```

This is complex. Alternative: mark such primitives as "yields" and have them push continuation frames. Or: for complex operations like sort, implement in Io using lower-level primitives.

---

# Summary: Complete Frame Lifecycle

```
1. Eval loop running
2. Message lookup finds a block or CFunction
3. For block:
   - If tail position + block frame: reuse frame (TCO)
   - Otherwise: push new Frame with isBlockActivation=true
4. For CFunction:
   - If control flow: call directly, may manipulate frames
   - If no args: call directly
   - If all args fast-path: evaluate inline, call
   - Otherwise: push ArgEvalFrame, start evaluating args
5. ArgEvalFrame evaluates args one by one
6. When all args ready: call primitive, pop ArgEvalFrame
7. When frame->message is NULL: pop frame, pass result to caller
8. Stop status checked after each iteration:
   - break/continue: unwind to catching block
   - return: unwind to block boundary
   - exception: unwind to try block
9. When frame stack empty: eval complete, return state->result
```
