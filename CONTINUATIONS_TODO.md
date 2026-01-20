# Continuations Implementation - TODO

## Current Status

The iterative evaluator has been implemented in `IoState_iterative.c` with the following features:

- ✅ Explicit frame stack (`IoEvalFrame`) instead of C stack
- ✅ State machine evaluation loop (6 states)
- ✅ Frame pooling for performance (1.8x overhead vs recursive)
- ✅ Stack overflow protection via `maxFrameDepth`
- ✅ Basic test suite (9/10 tests passing)

## Critical Blocker: Re-entrant Primitives

**Problem**: All control flow primitives currently re-enter the recursive evaluator, which breaks continuation support.

### Why This Breaks Continuations

When primitives call back into the evaluator, execution state is split between:
1. **IoEvalFrame stack** - our explicit frame stack
2. **C call stack** - primitive activation records we cannot capture

This means we cannot:
- Serialize/restore full execution state
- Implement first-class continuations
- Eliminate platform-specific coroutine code (libcoroutine)

### Examples of Re-entrant Primitives

In `IoObject_flow.c`, the `if` primitive (lines 216-222):

```c
IO_METHOD(IoObject, if) {
    IoObject *r = IoMessage_locals_valueArgAt_(m, locals, 0);  // RE-ENTERS!
    const int condition =
        ISTRUE(IoMessage_locals_performOn_(IOSTATE->asBooleanMessage, r, r));  // RE-ENTERS!

    if (index < IoMessage_argCount(m))
        return IoMessage_locals_valueArgAt_(m, locals, index);  // RE-ENTERS!

    return IOBOOL(self, condition);
}
```

Similar problems in:
- `while` - evaluates condition and body
- `loop` - evaluates body repeatedly
- `for` - evaluates range and body
- `break` / `continue` / `return` - may evaluate arguments
- Any primitive that calls `IoMessage_locals_valueArgAt_()` or `IoMessage_locals_performOn_()`

## Solution Architecture

Primitives must use **frame-based trampolining** instead of direct evaluation:

```
IoMessage_locals_performOn_iterative()
  └─> Main eval loop
      ├─> Primitive call (NO re-entry)
      │   └─> Sets up frames for args/branches
      │       └─> RETURNS to main loop (with continuation info)
      └─> Main loop continues evaluation from frames
```

### Implementation Strategy

1. **Add new frame states** for control flow:
   - `FRAME_STATE_IF_EVAL_CONDITION`
   - `FRAME_STATE_IF_EVAL_BRANCH`
   - `FRAME_STATE_WHILE_EVAL_CONDITION`
   - `FRAME_STATE_WHILE_EVAL_BODY`
   - etc.

2. **Modify primitives** to:
   - Push frames with appropriate state
   - Store continuation info in frame
   - Return control to main loop

3. **Extend main eval loop** to handle new states

### Example: Refactored `if` Primitive

```c
IO_METHOD(IoObject, if) {
    // Instead of evaluating directly, push a frame to evaluate condition
    IoEvalFrame *frame = IoState_pushFrame_(IOSTATE);
    frame->message = IoMessage_rawArgAt_(m, 0);  // condition message
    frame->target = locals;
    frame->locals = locals;
    frame->cachedTarget = locals;
    frame->state = FRAME_STATE_IF_EVAL_CONDITION;

    // Store branch info for later
    frame->ifTrueBranch = IoMessage_rawArgAt_(m, 1);
    frame->ifFalseBranch = IoMessage_rawArgAt_(m, 2);

    // Return to main loop - it will evaluate the condition
    return NULL;  // Or special sentinel value
}
```

Then in main loop, add:

```c
case FRAME_STATE_IF_EVAL_CONDITION: {
    // Condition has been evaluated, result is in frame->result
    int condition = ISTRUE(frame->result);

    // Set up frame to evaluate the appropriate branch
    IoMessage *branch = condition ? frame->ifTrueBranch : frame->ifFalseBranch;
    if (branch) {
        frame->message = branch;
        frame->state = FRAME_STATE_START;  // Evaluate branch
    } else {
        frame->result = IOBOOL(self, condition);
        frame->state = FRAME_STATE_RETURN;
    }
    break;
}
```

## Work Items

### Phase 1: Core Control Flow (Priority)
- [ ] Refactor `if` primitive (template for others)
- [ ] Refactor `while` primitive
- [ ] Refactor `loop` primitive
- [ ] Refactor `for` primitive
- [ ] Refactor `break`, `continue`, `return`

### Phase 2: Other Re-entrant Primitives
- [ ] Audit all primitives for `IoMessage_locals_valueArgAt_()` usage
- [ ] Audit all primitives for `IoMessage_locals_performOn_()` usage
- [ ] Refactor identified primitives

### Phase 3: Testing & Validation
- [ ] Test lazy argument evaluation works correctly
- [ ] Test nested control flow
- [ ] Test break/continue/return in various contexts
- [ ] Verify no C stack dependence remains

### Phase 4: Continuation API
- [ ] Design continuation capture API
- [ ] Implement `callcc` primitive
- [ ] Implement continuation invocation
- [ ] Test serialization/deserialization

### Phase 5: Coroutine Replacement
- [ ] Replace libcoroutine with continuation-based coroutines
- [ ] Remove platform-specific assembly code
- [ ] Test across platforms

## Performance Notes

Current overhead with frame pooling: **1.8x vs recursive evaluator**

To reach 10x faster (Ruby performance goal), would need:
- Inline caching for slot lookup
- Method dispatch caching
- Possibly bytecode compilation
- Or JIT compilation

However, tree-walking interpreter is fundamentally limited. Focus on **correctness first**, then optimize hot paths.

## Files Modified

- `libs/iovm/source/IoEvalFrame.h` - Frame structure
- `libs/iovm/source/IoEvalFrame.c` - Frame management
- `libs/iovm/source/IoState_iterative.c` - Main eval loop
- `libs/iovm/source/IoState.h` - Added frame stack fields
- `libs/iovm/source/IoState.c` - Initialize frame stack
- `libs/iovm/source/IoState_eval.h` - API declarations
- `libs/iovm/tests/test_iterative_eval.c` - Test suite
- `libs/iovm/tests/benchmark_iterative.c` - Benchmarks

## Next Immediate Step

Start with refactoring the `if` primitive in `IoObject_flow.c` as a template for other control flow primitives.
