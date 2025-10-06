#if defined(__arm64__) || defined(__aarch64__)
#include "Base.h"
#include "Coro.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "taskimpl.h"

// External functions implemented in asm.S
extern int coro_arm64_getcontext(void *context) __asm("coro_arm64_getcontext");
extern int coro_arm64_setcontext(void *context) __asm("coro_arm64_setcontext");

#ifdef USE_VALGRIND
#include <valgrind/valgrind.h>
#define STACK_REGISTER(coro)                                                   \
    {                                                                          \
        Coro *c = (coro);                                                      \
        c->valgrindStackId = VALGRIND_STACK_REGISTER(                          \
            c->stack, (char *)c->stack + c->requestedStackSize);               \
    }

#define STACK_DEREGISTER(coro)                                                 \
    VALGRIND_STACK_DEREGISTER((coro)->valgrindStackId)
#else
#define STACK_REGISTER(coro)
#define STACK_DEREGISTER(coro)
#endif

typedef struct CallbackBlock {
    void *context;
    CoroStartCallback *func;
} CallbackBlock;

static CallbackBlock globalCallbackBlock;

static void Coro_StartWithArg(void) {
    CallbackBlock *block = &globalCallbackBlock;
    block->func(block->context);
    fprintf(stderr, "Scheduler error: returned from coro start function\n");
    exit(-1);
}

Coro *Coro_new(void) {
    Coro *self = (Coro *)io_calloc(1, sizeof(Coro));
    if (self) {
        self->requestedStackSize = CORO_DEFAULT_STACK_SIZE;
        self->allocatedStackSize = 0;
        self->stack = NULL;
    }
    return self;
}

static void Coro_disposeStack(Coro *self) {
    if (!self->stack) {
        return;
    }

    STACK_DEREGISTER(self);
    io_free(self->stack);
    self->stack = NULL;
    self->allocatedStackSize = 0;
}

static void Coro_allocStackIfNeeded(Coro *self) {
    if (self->stack && self->requestedStackSize < self->allocatedStackSize) {
        Coro_disposeStack(self);
    }

    if (!self->stack) {
        self->stack = io_calloc(1, self->requestedStackSize + 16);
        self->allocatedStackSize = self->requestedStackSize;
        STACK_REGISTER(self);
    }
}

void Coro_free(Coro *self) {
    Coro_disposeStack(self);
    io_free(self);
}

void *Coro_stack(Coro *self) { return self->stack; }

size_t Coro_stackSize(Coro *self) { return self->requestedStackSize; }

void Coro_setStackSize_(Coro *self, size_t size) { self->requestedStackSize = size; }

#if __GNUC__ >= 4
static ptrdiff_t *Coro_CurrentStackPointer(void) __attribute__((noinline));
#endif

static ptrdiff_t *Coro_CurrentStackPointer(void) {
    ptrdiff_t marker;
    ptrdiff_t *markerPtr = &marker;
    return markerPtr;
}

size_t Coro_bytesLeftOnStack(Coro *self) {
    unsigned char dummy;
    ptrdiff_t stackPos = (ptrdiff_t)&dummy;
    ptrdiff_t current = (ptrdiff_t)Coro_CurrentStackPointer();
    int stackMovesUp = current > stackPos;
    ptrdiff_t start = (ptrdiff_t)self->stack;
    ptrdiff_t end = start + self->requestedStackSize;

    if (stackMovesUp) {
        return (size_t)(end - stackPos);
    }

    return (size_t)(stackPos - start);
}

int Coro_stackSpaceAlmostGone(Coro *self) {
    return Coro_bytesLeftOnStack(self) < CORO_STACK_SIZE_MIN;
}

void Coro_initializeMainCoro(Coro *self) { self->isMain = 1; }

// This function initializes the context with a new stack and entry point
void Coro_setup(Coro *self, void *arg) {
    if (arg) {
        globalCallbackBlock = *(CallbackBlock *)arg;
    }
    IoCoroARM64Context *context = &self->env;
    memset(context, 0, sizeof(*context));

    Coro_allocStackIfNeeded(self);

    uintptr_t sp = (uintptr_t)self->stack + self->allocatedStackSize;
    sp = (sp - 16) & ~((uintptr_t)15); // 16-byte alignment as per ABI

    context->sp = sp;
    context->fp_lr[1] = (uint64_t)Coro_StartWithArg;
}

void Coro_startCoro_(Coro *self, Coro *other, void *context,
                     CoroStartCallback *callback) {
    CallbackBlock block = {
        .context = context,
        .func = callback,
    };
    Coro_setup(other, &block);
    Coro_switchTo_(self, other);
}

void Coro_switchTo_(Coro *self, Coro *next) {
    IoCoroARM64Context *from_context = &self->env;
    IoCoroARM64Context *to_context = &next->env;

    if (coro_arm64_getcontext((void *)from_context) == 0) {
        coro_arm64_setcontext((void *)to_context);
    }
}

#endif
