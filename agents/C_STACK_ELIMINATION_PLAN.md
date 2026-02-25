# C Stack Elimination Plan

## Problem Statement

For continuations to work correctly, ALL execution state must be in heap-allocated frames.
Currently, C stack is used in many places during evaluation:

1. **Recursive Evaluator** (`IoMessage_locals_performOn_`) - uses C stack for call chain
2. **Argument Evaluation** (`IoMessage_locals_valueArgAt_`) - calls recursive evaluator
3. **Collection Iteration** (`foreach`, `each`, etc.) - loops call evaluator per-element
4. **`doString`/`doMessage`** - explicitly call recursive evaluator

When a continuation is captured while C code is on the stack:
- The C stack frames are NOT captured
- Invoking the continuation restores heap frames but C stack is gone
- Results in broken/incorrect execution

## Current State

### What Works (Iterative)
- Control flow primitives (`if`, `while`, `for`, `loop`) have iterative paths
- Block activation via `IoState_activateBlock_` pushes frames
- The `inRecursiveEval` flag prevents control flow from corrupting frames

### What's Broken (Uses C Stack)
- `doString`/`doMessage`/`doFile` call recursive evaluator
- All CFunction argument evaluation via `IoMessage_locals_valueArgAt_`
- Collection iteration methods (`foreach`, `each`, `map`, etc.)
- ~100+ CFunctions that evaluate arguments

## Solution Architecture

### Core Insight

The fundamental issue is that CFunctions expect **synchronous** argument evaluation,
but the iterative evaluator is **asynchronous** (pushes frames, returns to eval loop).

**Solution**: Pre-evaluate arguments in the eval loop before calling CFunctions.

### Key Changes

1. **Argument Pre-evaluation**: Eval loop evaluates CFunction arguments before calling
2. **doString Pattern**: Convert evaluation-calling CFunctions to frame state machines
3. **Collection Iteration**: Convert foreach/each/etc to frame state machines
4. **Remove Fallbacks**: Eventually remove recursive evaluator calls entirely

---

## Phase 1: Convert doString/doMessage/doFile

**Goal**: Make code evaluation non-reentrant.

### New Frame States
```c
FRAME_STATE_DO_COMPILE,      // Compiling string to message (if needed)
FRAME_STATE_DO_EVAL,         // Pushing frame to evaluate code
FRAME_STATE_DO_WAIT,         // Waiting for evaluation result
```

### New Control Flow Info
```c
struct {
    IoMessage *codeMessage;   // The parsed/compiled message to evaluate
    IoObject *evalTarget;     // Target for evaluation
    IoObject *evalLocals;     // Locals for evaluation
} doInfo;
```

### Implementation Pattern

**doString CFunction:**
```c
IO_METHOD(IoObject, doString) {
    IoState *state = IOSTATE;

    if (state->currentFrame != NULL && !state->inRecursiveEval) {
        // Iterative path
        IoEvalFrame *frame = state->currentFrame;

        // Get the string argument (synchronously - it's usually a literal)
        IoSymbol *string = IoMessage_locals_seqArgAt_(m, locals, 0);
        IoSymbol *label = (IoMessage_argCount(m) > 1)
            ? IoMessage_locals_symbolArgAt_(m, locals, 1)
            : IOSYMBOL("doString");

        // Compile string to message
        IoMessage *codeMsg = IoMessage_newFromText_label_(state, CSTRING(string), CSTRING(label));

        // Set up frame for evaluation
        frame->controlFlow.doInfo.codeMessage = codeMsg;
        frame->controlFlow.doInfo.evalTarget = self;
        frame->controlFlow.doInfo.evalLocals = self;
        frame->state = FRAME_STATE_DO_EVAL;

        state->needsControlFlowHandling = 1;
        return state->ioNil;  // Placeholder, real result comes from frame
    } else {
        // Recursive fallback (existing code)
        return IoObject_rawDoString_label_(self, string, label);
    }
}
```

**Eval Loop Handlers:**
```c
case FRAME_STATE_DO_EVAL: {
    // Push frame to evaluate the code
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
    // Continue to next message in chain
    frame->state = FRAME_STATE_CONTINUE_CHAIN;
    break;
}
```

### Files to Modify
- `IoEvalFrame.h` - Add new states and doInfo union member
- `IoState_iterative.c` - Add case handlers
- `IoObject.c` - Modify doString, doMessage, doFile

### Testing
- `doString("1+1")` should return 2
- Continuation captured in doString code should work correctly
- `doFile` should work for loading scripts

---

## Phase 2: Pre-evaluate CFunction Arguments

**Goal**: Eliminate `IoMessage_locals_valueArgAt_` re-entrancy.

### Strategy

After slot lookup, if the slot is a CFunction (not a block/method):
1. Count arguments
2. Push frames to evaluate each argument
3. Store results in `frame->argValues`
4. Then call the CFunction

### Lazy vs Eager Arguments

Some methods need unevaluated arguments:
- `and(a, b)` - don't evaluate `b` if `a` is false
- `or(a, b)` - don't evaluate `b` if `a` is true
- Macro-style methods

**Solution**: Check if the CFunction has an "eager" tag. Default to eager.
Lazy methods (and/or/etc.) are marked specially.

### Implementation

**Modified LOOKUP_SLOT → ACTIVATE transition:**
```c
case FRAME_STATE_LOOKUP_SLOT: {
    // ... existing slot lookup code ...

    if (slotValue) {
        frame->slotValue = slotValue;
        frame->slotContext = slotContext;

        // Check if we need to pre-evaluate arguments
        if (ISCFUNCTION(slotValue) && !IoCFunction_isLazy(slotValue)) {
            int argCount = IoMessage_argCount(frame->message);
            if (argCount > 0 && !state->inRecursiveEval) {
                // Set up argument evaluation
                frame->argCount = argCount;
                frame->argValues = io_calloc(argCount, sizeof(IoObject*));
                frame->currentArgIndex = 0;
                frame->state = FRAME_STATE_EVAL_ARGS;
                break;
            }
        }

        frame->state = FRAME_STATE_ACTIVATE;
    }
    // ...
}
```

**EVAL_ARGS state (already partially exists):**
```c
case FRAME_STATE_EVAL_ARGS: {
    if (frame->currentArgIndex >= frame->argCount) {
        // All arguments evaluated
        frame->state = FRAME_STATE_ACTIVATE;
        break;
    }

    IoMessage *argMsg = IoMessage_rawArgAt_(frame->message, frame->currentArgIndex);

    // Check for cached result (literal)
    if (IOMESSAGEDATA(argMsg)->cachedResult && !IOMESSAGEDATA(argMsg)->next) {
        frame->argValues[frame->currentArgIndex] = IOMESSAGEDATA(argMsg)->cachedResult;
        frame->currentArgIndex++;
        break;
    }

    // Push frame to evaluate argument
    IoEvalFrame *argFrame = IoState_pushFrame_(state);
    argFrame->message = argMsg;
    argFrame->target = frame->locals;
    argFrame->locals = frame->locals;
    argFrame->cachedTarget = frame->locals;
    argFrame->state = FRAME_STATE_START;

    // Result will be captured when argFrame returns
    break;
}
```

**Modified RETURN to capture arg results:**
```c
case FRAME_STATE_RETURN: {
    // ... existing code ...

    if (parent && parent->state == FRAME_STATE_EVAL_ARGS) {
        parent->argValues[parent->currentArgIndex] = result;
        parent->currentArgIndex++;
    }
    // ...
}
```

**Modified IoMessage_locals_valueArgAt_:**
```c
IoObject *IoMessage_locals_valueArgAt_(IoMessage *m, IoObject *locals, int n) {
    IoState *state = IOSTATE;

    // Check for pre-evaluated arguments
    if (state->currentFrame && state->currentFrame->argValues) {
        if (n < state->currentFrame->argCount) {
            IoObject *preEval = state->currentFrame->argValues[n];
            if (preEval) return preEval;
        }
    }

    // Fallback to recursive evaluation
    IoMessage *argMessage = IoMessage_rawArgAt_(m, n);
    return argMessage ? IoMessage_locals_performOn_(argMessage, locals, locals)
                      : state->ioNil;
}
```

### Files to Modify
- `IoState_iterative.c` - LOOKUP_SLOT, EVAL_ARGS, RETURN cases
- `IoMessage.c` - IoMessage_locals_valueArgAt_ and variants
- `IoCFunction.h/c` - Add isLazy flag (optional, for and/or)

### Testing
- All existing tests should pass
- CFunction calls should work correctly
- Continuations captured during CFunction calls should work

---

## Phase 3: Convert Collection Iteration

**Goal**: Make foreach/each/map/etc non-reentrant.

### New Frame States
```c
FRAME_STATE_FOREACH_INIT,       // Initialize iteration
FRAME_STATE_FOREACH_EVAL_BODY,  // Evaluating body expression
FRAME_STATE_FOREACH_NEXT,       // Moving to next element
```

### Control Flow Info
```c
struct {
    IoObject *collection;      // List/Map/Sequence being iterated
    IoMessage *bodyMessage;    // Body to evaluate each iteration
    IoSymbol *indexName;       // Index variable name (optional)
    IoSymbol *valueName;       // Value variable name
    int currentIndex;          // Current iteration index
    int collectionSize;        // Total size
    IoObject *lastResult;      // Result of last body evaluation
} foreachInfo;
```

### Implementation Pattern

This is similar to how `for` loop already works, but iterates over a collection.

**List foreach CFunction:**
```c
IO_METHOD(IoList, foreach) {
    IoState *state = IOSTATE;

    if (state->currentFrame != NULL && !state->inRecursiveEval) {
        IoEvalFrame *frame = state->currentFrame;

        // Parse arguments (variable names, body message)
        IoSymbol *indexName = NULL, *valueName = NULL;
        IoMessage *bodyMsg = NULL;
        // ... parse based on arg count ...

        frame->controlFlow.foreachInfo.collection = self;
        frame->controlFlow.foreachInfo.bodyMessage = bodyMsg;
        frame->controlFlow.foreachInfo.indexName = indexName;
        frame->controlFlow.foreachInfo.valueName = valueName;
        frame->controlFlow.foreachInfo.currentIndex = 0;
        frame->controlFlow.foreachInfo.collectionSize = IoList_rawSize(self);
        frame->controlFlow.foreachInfo.lastResult = NULL;

        frame->state = FRAME_STATE_FOREACH_EVAL_BODY;
        state->needsControlFlowHandling = 1;
        return state->ioNil;
    } else {
        // Recursive fallback (existing code)
        // ...
    }
}
```

### Files to Modify
- `IoEvalFrame.h` - Add new states and foreachInfo
- `IoState_iterative.c` - Add iteration state handlers
- `IoList.c` - foreach, each, reverseForeach
- `IoMap.c` - foreach, each
- `IoSeq_immutable.c` - foreach, each

### Testing
- `list(1,2,3) foreach(i, v, v println)` should print 1, 2, 3
- Continuation captured during iteration should work

---

## Phase 4: Remove Recursive Fallbacks

**Goal**: Eliminate the `inRecursiveEval` flag and recursive code paths.

### Steps

1. Ensure all control flow works without fallback
2. Ensure doString/doMessage work without fallback
3. Ensure collection iteration works without fallback
4. Remove `inRecursiveEval` flag from IoState
5. Remove recursive code paths from control flow primitives
6. Remove `IoMessage_locals_performOn_` or keep only for legacy/bootstrap

### Testing
- All tests pass without recursive fallback
- Continuations work in all contexts

---

## Phase 5: Audit Remaining Re-entrancy

### Known Remaining Issues

1. **Block activation from recursive context** - Blocks called via `IoBlock_activate`
   when not in iterative context still use recursive eval

2. **Dynamic method calls** - `perform`, `resend`, `super` may need conversion

3. **Bootstrap** - Initial loading of Io files uses recursive eval
   - This is acceptable since continuations aren't used during bootstrap

### Action Items

- Audit all calls to `IoMessage_locals_performOn_`
- Convert or mark as "bootstrap only"
- Document any remaining limitations

---

## Implementation Order

| Phase | Priority | Effort | Impact |
|-------|----------|--------|--------|
| Phase 1: doString | HIGH | Medium | Enables REPL continuation capture |
| Phase 2: Arg Pre-eval | HIGH | High | Enables most CFunctions to be safe |
| Phase 3: Collection | MEDIUM | Medium | Enables foreach/map continuation capture |
| Phase 4: Remove Fallbacks | LOW | Low | Cleanup, not functional |
| Phase 5: Audit | LOW | Low | Documentation |

---

## Success Criteria

1. **Continuations work in REPL**: Can capture continuation in doString code
2. **Continuations work in loops**: Can capture continuation inside foreach
3. **No C stack during normal execution**: All evaluation uses heap frames
4. **All tests pass**: 13/13 iterative evaluator tests
5. **Performance acceptable**: Not more than 2-3x slower than current

---

## Files Summary

### Must Modify
- `IoEvalFrame.h` - New states, control flow info
- `IoState_iterative.c` - New state handlers
- `IoObject.c` - doString, doMessage, doFile
- `IoMessage.c` - IoMessage_locals_valueArgAt_
- `IoList.c` - foreach, each
- `IoMap.c` - foreach
- `IoObject_flow.c` - Remove recursive fallbacks (Phase 4)

### May Modify
- `IoSeq_immutable.c` - foreach, each
- `IoFile.c` - foreachLine, foreach
- `IoNumber.c` - repeat
- `IoCFunction.h/c` - isLazy flag

### Delete Eventually
- Recursive fallback code in control flow primitives
- Possibly `inRecursiveEval` flag
