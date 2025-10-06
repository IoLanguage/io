// arm64-ucontext.h: Apple Silicon (ARM64) coroutine context representation
#ifndef ARM64_UCONTEXT_H
#define ARM64_UCONTEXT_H

#include <stdint.h>

// Minimal register snapshot that matches the layout expected by
// `coro_arm64_getcontext`/`coro_arm64_setcontext` in asm.S. We only need to
// preserve the callee-saved registers along with the stack pointer.
typedef struct IoCoroARM64Context {
    uint64_t x19_x20[2];
    uint64_t x21_x22[2];
    uint64_t x23_x24[2];
    uint64_t x25_x26[2];
    uint64_t x27_x28[2];
    uint64_t fp_lr[2];
    uint64_t sp;
} IoCoroARM64Context;

typedef IoCoroARM64Context arm64_ucontext_t;

#endif // ARM64_UCONTEXT_H
