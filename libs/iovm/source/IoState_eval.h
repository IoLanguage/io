
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

// iterative (non-recursive) evaluation

IOVM_API IoObject *IoMessage_locals_performOn_iterative(IoMessage *self,
                                                        IoObject *locals,
                                                        IoObject *target);
IOVM_API IoObject *IoMessage_locals_performOn_fast(IoMessage *self,
                                                   IoObject *locals,
                                                   IoObject *target);
IOVM_API IoObject *IoState_evalLoop_(IoState *self);
IOVM_API struct IoEvalFrame *IoState_pushFrame_(IoState *self);
IOVM_API void IoState_popFrame_(IoState *self);

// sandbox

IOVM_API void IoState_zeroSandboxCounts(IoState *self);
IOVM_API void IoState_resetSandboxCounts(IoState *self);
IOVM_API IoObject *IoState_doSandboxCString_(IoState *self, const char *s);
IOVM_API double IoState_endTime(IoState *self);

// file

IOVM_API IoObject *IoState_doFile_(IoState *self, const char *path);
