
// metadoc State copyright Steve Dekorte 2002
// metadoc State license BSD revised

#include "IoVMApi.h"

IOVM_API IoObject *IoState_tryToPerform(IoState *self, IoObject *target,
                                        IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoState_rawOn_doCString_withLabel_(IoState *self,
                                                      IoObject *target,
                                                      const char *s,
                                                      const char *label);

IOVM_API IoObject *IoState_doCString_(IoState *self, const char *s);

IOVM_API IoObject *IoState_on_doCString_withLabel_(IoState *self,
                                                   IoObject *target,
                                                   const char *s,
                                                   const char *label);

// As above, and sets *exception to the uncaught exception, or to nil
IOVM_API IoObject *IoState_on_doCString_withLabel_exception_(
    IoState *self, IoObject *target, const char *s, const char *label,
    IoObject **exception);

// iterative (non-recursive) evaluation

IOVM_API IoObject *IoMessage_locals_performOn_iterative(IoMessage *self,
                                                        IoObject *locals,
                                                        IoObject *target);
// Hands code to the eval loop as a frame instead of running it in a nested
// C eval loop. Returns 1 if it did: the caller must then return at once, and
// the calling message's result becomes the code's result (or fixedResult,
// if that isn't NULL). Returns 0 if the caller has to evaluate it itself.
IOVM_API int IoState_isEvalLoopRunning_(IoState *self, IoMessage *m);
IOVM_API int IoState_evalInFrame_removingProto_(IoState *self, IoMessage *m,
                                                IoMessage *code,
                                                IoObject *target,
                                                IoObject *locals,
                                                IoObject *fixedResult,
                                                IoObject *proto);
IOVM_API int IoState_evalInFrame_(IoState *self, IoMessage *m, IoMessage *code,
                                  IoObject *target, IoObject *locals,
                                  IoObject *fixedResult);
IOVM_API IoObject *IoMessage_locals_performOn_fast(IoMessage *self,
                                                   IoObject *locals,
                                                   IoObject *target);
IOVM_API IoObject *IoState_evalLoop_(IoState *self);
IOVM_API IoObject *IoState_pushFrame_(IoState *self);
IOVM_API void IoState_popFrame_(IoState *self);

// sandbox

IOVM_API void IoState_zeroSandboxCounts(IoState *self);
IOVM_API void IoState_resetSandboxCounts(IoState *self);
IOVM_API IoObject *IoState_doSandboxCString_(IoState *self, const char *s);
IOVM_API double IoState_endTime(IoState *self);

// file

IOVM_API IoObject *IoState_doFile_(IoState *self, const char *path);
