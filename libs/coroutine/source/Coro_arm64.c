#if defined(__arm64__) || defined(__aarch64__)
#include "Coro.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// External functions implemented in asm.S
extern int coro_arm64_getcontext(void *context) __asm("coro_arm64_getcontext");
extern int coro_arm64_setcontext(void *context) __asm("coro_arm64_setcontext");

// Custom context implementation for ARM64
typedef struct {
    unsigned long x19_x20[2];  // x19, x20
    unsigned long x21_x22[2];  // x21, x22
    unsigned long x23_x24[2];  // x23, x24
    unsigned long x25_x26[2];  // x25, x26
    unsigned long x27_x28[2];  // x27, x28
    unsigned long fp_lr[2];    // x29 (fp), x30 (lr)
    unsigned long sp;          // stack pointer
} arm64_context_t;

typedef struct CallbackBlock {
    void *context;
    CoroStartCallback *func;
} CallbackBlock;

static CallbackBlock globalCallbackBlock;

static void Coro_StartWithArg(void) {
    //fprintf(stderr, "Coro_StartWithArg called\n");
    CallbackBlock *block = &globalCallbackBlock;
    //fprintf(stderr, "Function pointer: %p\n", block->func);
    block->func(block->context);
    fprintf(stderr, "Scheduler error: returned from coro start function\n");
    exit(-1);
}

void Coro_free(Coro *self) {
    if (self->stack) {
        free(self->stack);
    }
    free(self);
}

void Coro_initializeMainCoro(Coro *self) {
    self->isMain = 1;
}

Coro *Coro_new(void) {
    Coro *c = (Coro *)calloc(1, sizeof(Coro));
    if (c) {
        c->requestedStackSize = CORO_DEFAULT_STACK_SIZE;
        c->allocatedStackSize = 0;
        c->stack = NULL;
    }
    return c;
}

void Coro_setStackSize_(Coro *self, size_t size) {
    self->requestedStackSize = size;
}

static void Coro_allocStackIfNeeded(Coro *self) {
    if (self->stack && self->requestedStackSize < self->allocatedStackSize) {
        free(self->stack);
        self->stack = NULL;
        self->allocatedStackSize = 0;
    }
    if (!self->stack) {
        // Make sure stack is 16-byte aligned for ARM64
        self->stack = calloc(1, self->requestedStackSize + 16);
        self->allocatedStackSize = self->requestedStackSize;
    }
}

// This function initializes the context with a new stack and entry point
void Coro_setup(Coro *self, void *arg) {
    arm64_context_t *context = (arm64_context_t *)&self->env;
    memset(context, 0, sizeof(*context));
    
    Coro_allocStackIfNeeded(self);
    
    // Initialize stack pointer to top of stack (ARM64 full descending stack)
    unsigned long sp = (unsigned long)self->stack + self->allocatedStackSize - 16;
    // Ensure 16-byte alignment
    sp &= ~15UL;
    
    // Store stack pointer in context
    context->sp = sp;
    
    // Store entry point in link register (x30)
    context->fp_lr[1] = (unsigned long)Coro_StartWithArg;
}

int Coro_stackSpaceAlmostGone(Coro *self) {
    arm64_context_t *context = (arm64_context_t *)&self->env;
    unsigned long sp = context->sp;
    unsigned long stack_base = (unsigned long)self->stack;
    
    // Check if we have less than 1KB of stack space left
    return (sp - stack_base) < 1024;
}

size_t Coro_bytesLeftOnStack(Coro *self) {
    arm64_context_t *context = (arm64_context_t *)&self->env;
    unsigned long sp = context->sp;
    unsigned long stack_base = (unsigned long)self->stack;
    
    // Return number of bytes between stack pointer and stack base
    if (sp > stack_base) {
        return sp - stack_base;
    }
    
    // Fallback if stack info not available
    return 1024 * 1024;
}

void Coro_startCoro_(Coro *self, Coro *other, void *context, CoroStartCallback *callback) {
    globalCallbackBlock.context = context;
    globalCallbackBlock.func = callback;
    Coro_setup(other, &globalCallbackBlock);
    Coro_switchTo_(self, other);
}

void Coro_switchTo_(Coro *self, Coro *next) {
    // Get the context pointers
    arm64_context_t *from_context = (arm64_context_t *)&self->env;
    arm64_context_t *to_context = (arm64_context_t *)&next->env;
    
    // Save current context, if successful (returns 0), then restore the next context
    if (coro_arm64_getcontext((void*)from_context) == 0) {
        coro_arm64_setcontext((void*)to_context);
    }
    
    // No return statement needed - when resuming this coroutine later,
    // execution will continue from this point
    
    /* *********************************************************************************
    arm64_context_t *from_context = (arm64_context_t *)&self->env;
    arm64_context_t *to_context = (arm64_context_t *)&next->env;
    
    // Combined save/restore in a single inline assembly block to ensure proper flow
    asm volatile(
        // Save current context
        "stp x19, x20, [%[from], #0]\n"   // Save x19, x20
        "stp x21, x22, [%[from], #16]\n"  // Save x21, x22
        "stp x23, x24, [%[from], #32]\n"  // Save x23, x24
        "stp x25, x26, [%[from], #48]\n"  // Save x25, x26
        "stp x27, x28, [%[from], #64]\n"  // Save x27, x28
        "stp x29, x30, [%[from], #80]\n"  // Save fp (x29), lr (x30)
        "mov x16, sp\n"                   // Get current stack pointer
        "str x16, [%[from], #96]\n"       // Save stack pointer
        
        // Restore next context
        "ldp x19, x20, [%[to], #0]\n"     // Restore x19, x20
        "ldp x21, x22, [%[to], #16]\n"    // Restore x21, x22
        "ldp x23, x24, [%[to], #32]\n"    // Restore x23, x24
        "ldp x25, x26, [%[to], #48]\n"    // Restore x25, x26
        "ldp x27, x28, [%[to], #64]\n"    // Restore x27, x28
        "ldp x29, x30, [%[to], #80]\n"    // Restore fp (x29), lr (x30)
        "ldr x16, [%[to], #96]\n"         // Get new stack pointer
        "mov sp, x16\n"                   // Restore stack pointer
        
        : // No outputs
        : [from] "r" (from_context), [to] "r" (to_context) // Inputs
        : "x16", "memory" // Clobbers
    );
    
    // No return statement needed - when resuming this coroutine later,
    // execution will continue from this point

    */
}
#endif
