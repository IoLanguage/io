
// metadoc Continuation copyright Steve Dekorte 2002, 2025
// metadoc Continuation license BSD revised
// metadoc Continuation category Core
/*metadoc Continuation description
A first-class continuation that captures the execution state
(frame stack) at a point in time. When invoked, it restores
that state and continues execution from where it was captured.

Continuations are one-shot by default. The escape pattern
(invoking from within the callcc block) works without copying.
For multi-shot or delayed invocation, use the copy method to
create an independent deep copy of the continuation first.
*/

#ifndef IoContinuation_DEFINED
#define IoContinuation_DEFINED 1

#include "IoVMApi.h"
#include "IoObject.h"
#include "IoEvalFrame.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISCONTINUATION(self) \
    IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoContinuation_rawClone)

typedef IoObject IoContinuation;

typedef struct {
    IoEvalFrame *capturedFrame;  // Captured frame stack (GC-managed, not deep copied)
    IoObject *capturedLocals;    // The locals where callcc was invoked
    int invoked;                 // Has this continuation been invoked?
} IoContinuationData;

// Proto and lifecycle
IOVM_API IoContinuation *IoContinuation_proto(void *state);
IOVM_API IoContinuation *IoContinuation_rawClone(IoContinuation *self);
IOVM_API IoContinuation *IoContinuation_new(void *state);
IOVM_API void IoContinuation_free(IoContinuation *self);
IOVM_API void IoContinuation_mark(IoContinuation *self);

// Internal
IOVM_API void IoContinuation_captureFrameStack_(IoContinuation *self,
                                                 IoEvalFrame *frame,
                                                 IoObject *locals);

// Methods
IOVM_API IO_METHOD(IoContinuation, invoke);
IOVM_API IO_METHOD(IoContinuation, isInvoked);
IOVM_API IO_METHOD(IoContinuation, copy);
IOVM_API IO_METHOD(IoContinuation, frameCount);
IOVM_API IO_METHOD(IoContinuation, frameStates);
IOVM_API IO_METHOD(IoContinuation, frameMessages);
IOVM_API IO_METHOD(IoContinuation, asMap);
IOVM_API IO_METHOD(IoContinuation, fromMap);

// callcc primitive (on Object)
IOVM_API IO_METHOD(IoObject, callcc);

#ifdef __cplusplus
}
#endif
#endif
