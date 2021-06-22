
// metadoc Coroutine copyright Steve Dekorte 2002
// metadoc Coroutine license BSD revised

#ifndef IoCoroutine_DEFINED
#define IoCoroutine_DEFINED 1
#include "IoVMApi.h"

#include "IoState.h"

#include "Common.h"
#include "Coro.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISCOROUTINE(self)                                                      \
    IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCoroutine_rawClone)

typedef IoObject IoCoroutine;

typedef struct {
    Coro *cid;
    Stack *ioStack;
    int debuggingOn;
} IoCoroutineData;

IOVM_API IoCoroutine *IoCoroutine_proto(void *state);
IOVM_API void IoCoroutine_protoFinish(IoCoroutine *self);
IOVM_API IoCoroutine *IoCoroutine_rawClone(IoCoroutine *self);
IOVM_API IoCoroutine *IoCoroutine_new(void *state);

IOVM_API void IoCoroutine_free(IoCoroutine *self);
IOVM_API void IoCoroutine_mark(IoCoroutine *self);
Stack *IoCoroutine_rawIoStack(IoCoroutine *self);
IOVM_API void IoCoroutine_rawShow(IoCoroutine *self);

IOVM_API IO_METHOD(IoCoroutine, main);
IOVM_API IO_METHOD(IoCoroutine, freeStack);

IOVM_API void *IoCoroutine_cid(IoCoroutine *self);

// label

// void IoCoroutine_rawSetLabel_(IoCoroutine *self, IoSymbol *s);
// IoObject *IoCoroutine_rawLabel(IoCoroutine *self);

IOVM_API IO_METHOD(IoCoroutine, setLabel);
IOVM_API IO_METHOD(IoCoroutine, label);

// runTarget

IOVM_API void IoCoroutine_rawSetRunTarget_(IoCoroutine *self, IoObject *v);
IOVM_API IoObject *IoCoroutine_rawRunTarget(IoCoroutine *self);

// runMessage

IOVM_API void IoCoroutine_rawSetRunMessage_(IoCoroutine *self, IoObject *v);
IOVM_API IoObject *IoCoroutine_rawRunMessage(IoCoroutine *self);

// runLocals

IOVM_API void IoCoroutine_rawSetRunLocals_(IoCoroutine *self, IoObject *v);
IOVM_API IoObject *IoCoroutine_rawRunLocals(IoCoroutine *self);

// parent

IOVM_API void IoCoroutine_rawSetParentCoroutine_(IoCoroutine *self,
                                                 IoObject *v);
IOVM_API IoObject *IoCoroutine_rawParentCoroutine(IoCoroutine *self);
// exception

IOVM_API void IoCoroutine_rawSetResult_(IoCoroutine *self, IoObject *v);
IOVM_API IoObject *IoCoroutine_rawResult(IoCoroutine *self);

// exception

IOVM_API void IoCoroutine_rawRemoveException(IoCoroutine *self);
IOVM_API void IoCoroutine_rawSetException_(IoCoroutine *self, IoObject *v);
IOVM_API IoObject *IoCoroutine_rawException(IoCoroutine *self);

// ioStack

IOVM_API IO_METHOD(IoCoroutine, ioStack);
IOVM_API int IoCoroutine_rawIoStackSize(IoCoroutine *self);

// raw

IOVM_API void IoCoroutine_rawRun(IoCoroutine *self);
IOVM_API void IoCoroutine_clearStack(IoCoroutine *self);
IOVM_API void IoCoroutine_try(IoCoroutine *self, IoObject *target,
                              IoObject *locals, IoMessage *message);
IOVM_API IoCoroutine *IoCoroutine_newWithTry(void *state, IoObject *target,
                                             IoObject *locals,
                                             IoMessage *message);
IOVM_API void IoCoroutine_raiseError(IoCoroutine *self, IoSymbol *description,
                                     IoMessage *m);

// methods

IOVM_API IO_METHOD(IoCoroutine, implementation);
IOVM_API IO_METHOD(IoCoroutine, run);
IOVM_API IO_METHOD(IoCoroutine, callStack);
IOVM_API IO_METHOD(IoCoroutine, isCurrent);

// runTarget

IOVM_API void IoCoroutine_rawSetRunTarget_(IoCoroutine *self, IoObject *v);
IOVM_API IoObject *IoCoroutine_rawRunTarget(IoCoroutine *self);

// message

IOVM_API void IoCoroutine_rawSetRunMessage_(IoCoroutine *self, IoObject *v);
IOVM_API IoObject *IoCoroutine_rawRunMessage(IoCoroutine *self);

// parent

IOVM_API void IoCoroutine_rawSetParentCoroutine_(IoCoroutine *self,
                                                 IoObject *v);
IOVM_API IoObject *IoCoroutine_rawParentCoroutine(IoCoroutine *self);

// recentInChain

IOVM_API void IoCoroutine_rawSetRecentInChain_(IoCoroutine *self, IoObject *v);
IOVM_API IO_METHOD(IoCoroutine, setRecentInChain);
IOVM_API IoObject *IoCoroutine_rawRecentInChain(IoCoroutine *self);

// try

IOVM_API IO_METHOD(IoCoroutine, start);
IOVM_API IoObject *IoCoroutine_rawResume(IoCoroutine *self);
IOVM_API IO_METHOD(IoCoroutine, resume);
IOVM_API IO_METHOD(IoCoroutine, isCurrent);
IOVM_API IO_METHOD(IoCoroutine, currentCoroutine);

// stack trace

IOVM_API IoObject *IoObject_callStack(IoCoroutine *self);
IOVM_API void IoObject_appendStackEntryDescription(IoCoroutine *self,
                                                   UArray *ba);
IOVM_API void IoCoroutine_rawPrint(IoCoroutine *self);

// debugging

IOVM_API int IoCoroutine_rawDebuggingOn(IoCoroutine *self);
IOVM_API IO_METHOD(IoCoroutine, setMessageDebugging);
IOVM_API IoObject *IoObject_performWithDebugger(IoCoroutine *self,
                                                IoObject *locals, IoMessage *m);
IOVM_API IO_METHOD(IoCoroutine, callStack);
IOVM_API void IoCoroutine_rawPrintBackTrace(IoCoroutine *self);

#ifdef __cplusplus
}
#endif
#endif
