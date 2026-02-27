# Continuations Implementation - TODO

## CRITICAL: NO C STACK MANIPULATION

**The entire purpose of this project is to remove C stack dependency.**

This means:
- **NO setjmp/longjmp** - not even for "error handling"
- **NO ucontext** - not even for "lightweight switching"
- **NO fibers** - not even for "performance"
- **NO assembly coroutine code** - this is what we're removing
- **NO Coro_switchTo_** - libcoroutine is deleted

The goal is portable, serializable, network-transmittable execution state.
All execution state must be in heap-allocated frames, not on the C stack.

**Error handling:** `IoState_error_` sets `errorRaised = 1` and returns
normally. Every CFunction that calls a helper which might raise an error
must check `IOSTATE->errorRaised` and return early. The eval loop checks
`errorRaised` after each CFunction returns and unwinds frames.

---

## New VM Architecture

### Core Concept

One operation (SEND) + frame manipulation. Control flow primitives manipulate frames directly.

### Frame Structure

```c
typedef struct Frame {
    IoMessage *message;         // current message (instruction pointer)
    struct Frame *caller;       // return address (calling frame)
    IoObject *locals;           // context for slot lookup
    IoObject *target;           // current receiver
    IoObject *result;           // evaluation result
    IoObject *originalTarget;   // reset point for semicolon
    bool tailCall;              // if true, reuse frame instead of push
    bool isBlockBoundary;       // for return unwinding
    bool isLoopBoundary;        // for break/continue unwinding
} Frame;
```

### Sentinel Value

```c
#define SKIP_NORMAL_DISPATCH ((IoObject *)-1)
```

When a primitive returns this, it has already manipulated the frame directly.

### Two Primitive Styles

**Normal primitives** - receive evaluated args, return value:

```c
IO_METHOD(IoNumber, add) {
    double a = IoNumber_asDouble(self);
    double b = IoNumber_asDouble(args[0]);
    return IoNumber_new(state, a + b);
}
```

**Control flow primitives** - receive frame + unevaluated message, manipulate frame:

```c
IO_METHOD(IoTrue, ifTrue) {
    // Push frame to evaluate the argument
    Frame *argFrame = Frame_new();
    argFrame->message = IoMessage_argAt(m, 0);
    argFrame->locals = frame->locals;
    argFrame->target = frame->target;
    argFrame->caller = frame;

    state->currentFrame = argFrame;
    frame->message = IoMessage_next(m);

    return SKIP_NORMAL_DISPATCH;
}

IO_METHOD(IoFalse, ifTrue) {
    frame->result = self;
    frame->target = self;
    frame->message = IoMessage_next(m);
    return SKIP_NORMAL_DISPATCH;
}
```

### Minimal C Primitives Required

**Must be C (frame manipulation):**

| Primitive | Reason |
|-----------|--------|
| `ifTrue` on true | Conditional - push frame for branch |
| `ifTrue` on false | Conditional - skip branch |
| `ifFalse` on true | Conditional - skip branch |
| `ifFalse` on false | Conditional - push frame for branch |
| `while` | Loop with break/continue support |
| `for` | Loop with break/continue support |
| `loop` | Loop with break/continue support |
| `return` | Unwind to block boundary |
| `break` | Unwind to loop boundary, exit |
| `continue` | Unwind to loop boundary, restart |
| `try`/`catch` | Exception frame handling |

**Should be C (performance):**
- Number arithmetic (+, -, *, /, etc.)
- String operations
- List/Array operations
- Slot operations (setSlot, getSlot, etc.)

**Can be Io:**
- `if` - built on ifTrue/ifFalse
- `forEach`, `map`, `select` - built on while/for
- Most object methods

### Control Flow in Io

```io
// Built on ifTrue/ifFalse
if := method(cond, then, else,
    cond ifTrue(return then)
    else
)

// forEach built on while (which is C)
List forEach := method(indexName, valueName, body,
    i := 0
    while(i < self size,
        call sender setSlot(indexName, i)
        call sender setSlot(valueName, self at(i))
        body doInContext(call sender)
        i = i + 1
    )
)
```

## Eval Loop

See `VM_EVAL_LOOP.md` for detailed implementation.

Core loop handles:
1. End of message chain → return to caller
2. Cached literals → use directly
3. Slot lookup → find value
4. Forward → handle missing slot
5. Activation → CFunction or Block
6. TCO → reuse frame when in tail position
7. Stop status → handle return/break/continue

## Argument Evaluation

**Fast path (synchronous):**
- Literals (cached result)
- Simple slot lookup (single message, no args, non-activatable)

**Slow path (frame-based):**
- Complex expressions
- Method calls
- Message chains

```c
IoObject *Frame_evaluateArgSync(Frame *frame, IoMessage *m, int i) {
    IoMessage *argMsg = IoMessage_argAt(m, i);

    // Literal
    if (IoMessage_cachedResult(argMsg)) {
        return IoMessage_cachedResult(argMsg);
    }

    // Simple slot lookup
    if (!IoMessage_next(argMsg) && IoMessage_argCount(argMsg) == 0) {
        IoObject *slotValue = IoObject_getSlot_(frame->locals, IoMessage_name(argMsg));
        if (slotValue && !IoObject_isActivatable(slotValue)) {
            return slotValue;
        }
    }

    return NEEDS_FRAME_EVAL;
}
```

## Stop Status Handling

```c
typedef enum {
    STOP_NONE,
    STOP_RETURN,
    STOP_BREAK,
    STOP_CONTINUE
} StopStatus;
```

**return:** Unwind to `isBlockBoundary`, set result, continue after block.

**break:** Unwind to `isLoopBoundary`, exit loop entirely.

**continue:** Unwind to `isLoopBoundary`, restart loop iteration.

## Implementation Layers

```
┌─────────────────────────────────────────┐
│  Io Code                                │
│  - if, forEach, map, select, etc.       │
├─────────────────────────────────────────┤
│  Control Flow (C, frame manipulation)   │
│  - ifTrue/ifFalse, while, for, loop     │
│  - return, break, continue              │
├─────────────────────────────────────────┤
│  Core Primitives (C, non-reentrant)     │
│  - Number, String, List, Map            │
├─────────────────────────────────────────┤
│  VM (C)                                 │
│  - eval loop, frames, GC                │
├─────────────────────────────────────────┤
│  Platform Bindings (FFI)                │
│  - JS for WASM, C libs for native       │
└─────────────────────────────────────────┘
```

## Result

- **No re-entrancy** - primitives never call evaluator
- **Portable coroutines** - all state in frames (serializable)
- **Clean break/continue** - loops as C primitives
- **TCO** - recursive calls in tail position reuse frames
- **Minimal C** - ~300 lines eval loop + primitives

## Work Items

### Phase 1: Core VM ✅ COMPLETE
- [x] Define new Frame structure (`IoEvalFrame.h`)
- [x] Implement eval loop (~800 lines in `IoState_iterative.c`)
- [x] Implement frame push/pop with pooling
- [ ] Implement TCO detection (deferred)
- [x] Implement stop status handling for loops (break/continue)
- [x] Add GC marking for frame stack (`IoEvalFrame_mark`)

### Phase 2: Control Flow Primitives ✅ COMPLETE
- [x] Implement `if` with lazy branch evaluation (frame state machine)
- [x] Implement `while` (loop with break/continue)
- [x] Implement `for` (loop with break/continue)
- [x] Implement `loop` (infinite loop with break)
- [x] `return`, `break`, `continue` work with existing stop status mechanism

**Note:** Control flow uses a hybrid approach:
- Control flow primitives (`if`, `while`, `for`, `loop`) use the iterative
  frame state machine - NO C stack re-entry for control flow
- Normal CFunctions and block argument evaluation still use the recursive
  evaluator (acceptable re-entrancy that doesn't block continuations)

### Phase 3: Argument Evaluation - DEFERRED
Current approach: Let CFunctions handle their own argument evaluation.
Control flow primitives receive unevaluated messages.

Future work could pre-evaluate args in the loop for full iterative evaluation,
but this requires modifying all CFunctions.

### Phase 4: Io Standard Library
- [x] `if` works as C primitive (not needed in Io)
- [ ] Implement `forEach` on List in Io
- [ ] Implement `map`, `select` in Io
- [x] Test control flow constructs (21/21 tests pass including callcc, exceptions, coros)

### Phase 5: Continuation API ✅ COMPLETE
- [x] Design continuation capture API (`IoContinuation.h`)
- [x] Implement frame stack copy (`IoContinuation_copyFrameStack_`)
- [x] Implement `callcc` primitive with frame state machine
- [x] Implement `Continuation invoke` method (frame stack replacement)
- [x] Test continuation invoke (13/13 tests pass)
- [ ] Test serialization/deserialization

**Implementation Notes:**
- `Continuation` object type created with capture and invoke methods
- `callcc(block)` captures frame stack before evaluating block
- Frame captured in `CALLCC_EVAL_BLOCK` state so restoration continues correctly
- `cont invoke(value)` replaces frame stack, sets `continuationInvoked` flag
- Eval loop checks flag after CFunction returns, restarts with new frame stack
- No setjmp/longjmp needed - frame state machine handles unwinding

### Phase 6: Coroutine Replacement ✅ COMPLETE
- [x] Implement coroutine as frame stack wrapper (IoCoroutineData with frameStack)
- [x] Implement save/restore state functions
- [x] Rewrite rawRun for frame-based operation
- [x] Rewrite rawReturnToParent for frame-based operation
- [x] Fix error handling without longjmp (helper functions return early)
- [x] REPL starts successfully!
- [x] Fix recursive/iterative evaluator mixing (added `state->inRecursiveEval` flag)
- [x] Make IoState_error_ lightweight (no coro swap from C stack)
- [x] Integrate Io-level exceptions with eval loop (Exception raise/pass)
- [x] Replace libcoroutine build dependency (deleted `libs/coroutine/`, cleaned CMakeLists)
- [x] Remove platform-specific assembly (all in deleted `libs/coroutine/`)
- [x] Test yield/resume (C tests: resume, yield, @@; CLI smoke tests all pass)
- [x] Test portable coroutines (21/21 C tests, 23/23 Io correctness test files pass)

**Error Safety (return-and-check pattern):**
On master, `IoState_error_` did a longjmp (never returned). On this branch it returns
normally after setting `errorRaised = 1`. Every C function that calls a helper which
might raise an error must check `IOSTATE->errorRaised` and return early. Fixed sites:
- `IOASSERT` macro: added `return IONIL(self)` after `IoState_error_`
- `IoObject_rawClonePrimitive`: must set `isActivatable` from proto (prevents stale flags on recycled markers)
- `IoList_checkIndex`: changed to return int (0=ok, 1=error), callers check
- `IoList_sortInPlaceBy`: check `errorRaised` after `blockArgAt_`
- `IoFile_assertOpen/assertWrite`: return IONIL on error, all callers check
- `IoDirectory` opendir: `return IONIL(self)` after error (3 sites)
- `IoObject_self` (thisContext): skip arg pre-evaluation (prevents `ifNil` body executing on non-nil)
- `IoMessage_assertArgCount_receiver_`: all callers check `errorRaised` after
- `IoSeq_mutable.c IO_ASSERT_NOT_SYMBOL`: macro checks `errorRaised` after
- `IoCFunction_activate`: return after type mismatch error
- All `listArgAt_`/`mapArgAt_`/`blockArgAt_` callers: check `errorRaised`
- Various: `IoSeq_asJSON`, `IoSeq_findAnyOf`, `IoSeq_translate`, `IoBlock_code_`, etc.
- Nested eval loops: clear `inRecursiveEval` on entry (for correct control flow path)

**IoState_error_ Made Lightweight:**
`IoState_error_` no longer calls `IoCoroutine_raiseError` → `rawReturnToParent`.
Instead it creates the Exception inline and sets `errorRaised = 1`. The eval loop
handles all frame unwinding: (1) the ACTIVATE handler catches errors during CFunction
calls and pops the retain pool, (2) a top-of-loop generic handler catches errors from
other contexts (e.g., `forward` in LOOKUP_SLOT). After unwinding, the `frame=NULL`
handler takes over for coro switching / nested eval exit. This eliminates the SIGSEGV
that occurred when `rawReturnToParent` tried to unwind frames from deep inside a
CFunction's C call stack.

**Io-Level Exception Integration (FIXED):**
Io-level exceptions now work via `rawSignalException` which sets `errorRaised = 1`,
bridging Io-level exceptions to the eval loop's error handling. `Exception raise` and
`Exception raiseFrom` use `raiseException` which calls `rawSignalException` on the
current coro when there's no parent to resume. The eval loop then unwinds frames just
like C-level errors. `try()` catches both C-level and Io-level exceptions.

**Critical Bug Fixed (recursive/iterative mixing):**
When CFunctions (like `doString`) call `IoMessage_locals_performOn_` (recursive evaluator)
while the iterative eval loop is running, control flow primitives (if, while, for, loop)
would see `state->currentFrame != NULL` and incorrectly try to use the iterative approach,
modifying the wrong frame. Fixed by adding `state->inRecursiveEval` flag that's set when
entering the recursive evaluator, allowing control flow primitives to use the correct path.

## Files

### Implementation
- `libs/iovm/source/IoEvalFrame.h` - Frame structure and state machine enums
- `libs/iovm/source/IoEvalFrame.c` - Frame management (new, free, mark, reset)
- `libs/iovm/source/IoState_iterative.c` - Iterative eval loop (~800 lines)
- `libs/iovm/source/IoObject_flow.c` - Control flow primitives (if, while, for, loop)
- `libs/iovm/source/IoContinuation.h` - Continuation object header
- `libs/iovm/source/IoContinuation.c` - Continuation capture, invoke, and callcc

### Documentation
- `VM_EVAL_LOOP.md` - Detailed eval loop design
- `CONTINUATIONS_TODO.md` - This file

### Tests
- `libs/iovm/tests/test_iterative_eval.c` - 21 tests for iterative evaluator (including coro yield/resume/@@)
- `libs/iovm/tests/debug_if.c` - Debug test for if primitive

## Performance Notes

**Expected overhead:** Tight loops ~2-5x slower than current C implementation.

**Why acceptable:**
1. Current C implementation broken (re-entrancy prevents continuations)
2. Io not designed for tight numerical loops
3. C primitives still fast for actual computation

**Future optimizations:**
- Inline caching for slot lookup
- Bytecode compilation
- JIT (if really needed)
