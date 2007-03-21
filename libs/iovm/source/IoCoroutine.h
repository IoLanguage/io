/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IoCoroutine_DEFINED
#define IoCoroutine_DEFINED 1
#include "IoVMApi.h"

#include "IoState.h"

#include "Common.h"
#include "Coro.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISCOROUTINE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCoroutine_rawClone)

typedef IoObject IoCoroutine;

typedef struct
{
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

IOVM_API IoObject *IoCoroutine_main(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoCoroutine_freeStack(IoObject *self, IoObject *locals, IoMessage *m);

IOVM_API void *IoCoroutine_cid(IoObject *self);

// label

//void IoCoroutine_rawSetLabel_(IoCoroutine *self, IoSymbol *s);
//IoObject *IoCoroutine_rawLabel(IoCoroutine *self);

IOVM_API IoObject *IoCoroutine_setLabel(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoCoroutine_label(IoObject *self, IoObject *locals, IoMessage *m);

// runTarget

IOVM_API void IoCoroutine_rawSetRunTarget_(IoObject *self, IoObject *v);
IOVM_API IoObject *IoCoroutine_rawRunTarget(IoObject *self);

// runMessage

IOVM_API void IoCoroutine_rawSetRunMessage_(IoObject *self, IoObject *v);
IOVM_API IoObject *IoCoroutine_rawRunMessage(IoObject *self);

// runLocals

IOVM_API void IoCoroutine_rawSetRunLocals_(IoObject *self, IoObject *v);
IOVM_API IoObject *IoCoroutine_rawRunLocals(IoObject *self);

// parent

IOVM_API void IoCoroutine_rawSetParentCoroutine_(IoObject *self, IoObject *v);
IOVM_API IoObject *IoCoroutine_rawParentCoroutine(IoObject *self);
// exception

IOVM_API void IoCoroutine_rawSetResult_(IoObject *self, IoObject *v);
IOVM_API IoObject *IoCoroutine_rawResult(IoObject *self);

// exception

IOVM_API void IoCoroutine_rawRemoveException(IoObject *self);
IOVM_API void IoCoroutine_rawSetException_(IoObject *self, IoObject *v);
IOVM_API IoObject *IoCoroutine_rawException(IoObject *self);

// ioStack

IOVM_API IoObject *IoCoroutine_ioStack(IoCoroutine *self, IoObject *locals, IoMessage *m);
IOVM_API int IoCoroutine_rawIoStackSize(IoObject *self);

// raw

IOVM_API void IoCoroutine_rawRun(IoObject *self);

IOVM_API void IoCoroutine_clearStack(IoObject *self);

IOVM_API void IoCoroutine_try(IoObject *self, IoObject *target, IoObject *locals, IoMessage *message);

IOVM_API IoCoroutine *IoCoroutine_newWithTry(void *state, 
							 IoObject *target, 
							 IoObject *locals, 
							 IoMessage *message);

IOVM_API void IoCoroutine_raiseError(IoCoroutine *self, IoSymbol *description, IoMessage *m);

// methods

IOVM_API IoObject *IoCoroutine_implementation(IoObject *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoCoroutine_run(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoCoroutine_callStack(IoObject *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoCoroutine_isCurrent(IoObject *self, IoObject *locals, IoMessage *m);

// runTarget

IOVM_API void IoCoroutine_rawSetRunTarget_(IoObject *self, IoObject *v);
IOVM_API IoObject *IoCoroutine_rawRunTarget(IoObject *self);

// message

IOVM_API void IoCoroutine_rawSetRunMessage_(IoObject *self, IoObject *v);
IOVM_API IoObject *IoCoroutine_rawRunMessage(IoObject *self);

// parent

IOVM_API void IoCoroutine_rawSetParentCoroutine_(IoObject *self, IoObject *v);
IOVM_API IoObject *IoCoroutine_rawParentCoroutine(IoObject *self);

// try

IOVM_API IoObject *IoCoroutine_start(IoObject *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoCoroutine_rawResume(IoObject *self);
IOVM_API IoObject *IoCoroutine_resume(IoObject *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoCoroutine_isCurrent(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoCoroutine_currentCoroutine(IoObject *self, IoObject *locals, IoMessage *m);

// stack trace

IOVM_API IoObject *IoObject_callStack(IoObject *self);

IOVM_API void IoObject_appendStackEntryDescription(IoObject *self, UArray *ba);

IOVM_API void IoCoroutine_rawPrint(IoObject *self);

// debugging

IOVM_API int IoCoroutine_rawDebuggingOn(IoObject *self);

IOVM_API IoObject *IoCoroutine_setMessageDebugging(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_performWithDebugger(IoObject *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoCoroutine_callStack(IoObject *self, IoObject *locals, IoMessage *m);

IOVM_API void IoCoroutine_rawPrintBackTrace(IoObject *self);

#ifdef __cplusplus
}
#endif
#endif
