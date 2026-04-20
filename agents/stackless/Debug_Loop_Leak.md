# Debug: Loop Retain Stack Memory Leak

**Project**: `/Users/steve/_projects/Inactive/Io sites/io/` (branch: `stackless`)
**Build**: `cd build && cmake --build .`
**Binary**: `build/_build/binaries/io`
**C tests**: `build/_build/binaries/test_iterative_eval`

## Problem

Tight loops leak memory — the retain stack (`currentIoStack`) grows without bound because loop body temporaries are never cleaned up between iterations. A `for(i, 1, 5000000, i + 1)` grew from 218MB to 572MB in 7 seconds.

**Root cause**: Each loop iteration's CFunction calls (line 789-848 of `IoState_iterative.c`) push/pop retain pools per call, but `popRetainPoolExceptFor_` preserves the result on the stack. These preserved results accumulate in the loop frame's retain scope, which has no pool mark to clean up between iterations. Over millions of iterations, this prevents GC from collecting dead Number objects.

## Current fix attempt (in working tree on stackless branch, crashes)

Added retain pool push on first loop entry and pop+re-push between iterations for all loop types (FOR, WHILE, LOOP, FOREACH). See `git diff HEAD -- libs/iovm/source/IoState_iterative.c` on the stackless branch for the full diff.

The approach:
1. On first entry to a loop state machine, push a retain pool: `fd->retainPoolMark = IoState_pushRetainPool(state);`
2. At the start of each after-body handler, pop the old pool and push a fresh one: `IoState_popRetainPoolExceptFor_(state, fd->result); fd->retainPoolMark = IoState_pushRetainPool(state);`
3. On frame RETURN (line ~1042), the existing `if (fd->retainPoolMark)` check pops the final pool.

**The crash**: `EXC_BAD_ACCESS` / SIGBUS (exit code 138) even on `./build/_build/binaries/io -e '"hello" println'`. Since `"hello" println` has no loops, the crash must be happening during .io bootstrap file loading (the bootstrap DOES use loops — `foreach`, `for`, etc. in the standard library .io files).

## Key files to read

- `libs/iovm/source/IoState_iterative.c` — The iterative eval loop with the frame state machine. The loop state handlers (WHILE_*, LOOP_*, FOR_*, FOREACH_*) have the retain pool changes.
- `libs/iovm/source/IoState_inline.h` — `IoState_pushRetainPool`, `IoState_popRetainPool`, `IoState_popRetainPoolExceptFor_`
- `libs/basekit/source/Stack_inline.h` — Stack mark mechanism: `Stack_pushMark`, `Stack_popMark`. Marks form a linked list within the stack array via `lastMark`.
- `libs/iovm/source/IoEvalFrame.h` — Frame structure. `retainPoolMark` field at line 105 (also used for block activation retain pools).
- `libs/iovm/source/IoObject_flow.c` — Control flow CFunctions (if, while, for, loop). These set `needsControlFlowHandling = 1` and return. The CFunction retain pool bracket (lines 789/848 of iterative.c) pops BEFORE the needsControlFlowHandling check at line 852.

## What to investigate

1. **Why does the crash happen during bootstrap?** The .io bootstrap files use `foreach` and `for` loops. The retain pool push in `FOREACH_EVAL_BODY` (`if (!fd->retainPoolMark)`) and pop+re-push in `FOREACH_AFTER_BODY` may be interfering with mark nesting from outer contexts.

2. **Mark nesting correctness**: The RETURN handler at line ~1042 does `IoState_popRetainPoolExceptFor_(state, result)` when `fd->retainPoolMark` is set. But the after-body handlers ALSO do pop+re-push on every iteration. When a loop exits (break/return/condition-false), the after-body handler has already popped the old mark and pushed a new one. Then RETURN pops that new mark. Verify this double-lifecycle is correct.

3. **Interaction with block activation retain pools**: Block frames also use `fd->retainPoolMark` (set during `IoState_activateBlock_`). If a loop runs inside a block, both the block frame and the loop frame have retain pool marks. The loop frame is a child of the block frame's body evaluation. When the loop's after-body handler does `IoState_popRetainPoolExceptFor_`, it must pop ONLY the loop's mark, not the block's. Verify mark nesting is correct here.

4. **The `goto` fast paths**: RETURN has `goto for_after_body`, `goto foreach_after_body`, `goto loop_after_body`, `goto while_eval_body` fast paths that skip the normal loop restart. These gotos jump directly into the after-body handlers which now do pop+re-push. Make sure the marks are in the right state when entering via goto.

## Task

Debug and fix the retain pool crash. Build with `cd build && cmake --build .` and test with `./build/_build/binaries/io -e '"hello" println'`. A working fix should:
- Not crash on simple expressions or bootstrap
- Keep memory stable for `for(i, 1, 5000000, i + 1)` (test with `top` or similar)
- Pass `./build/_build/binaries/test_iterative_eval`
- Pass `./build/_build/binaries/io -e 'if(true, "yes") println; for(i,1,3, i println); list(1,2,3) foreach(v, v println); e := try(1 unknownMethod); e error println; "done" println'`
