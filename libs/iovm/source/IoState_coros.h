
// metadoc State copyright Steve Dekorte 2002
// metadoc State license BSD revised

#include "IoVMApi.h"

IOVM_API IoObject *IoState_currentCoroutine(IoState *self);
IOVM_API void IoState_setCurrentCoroutine_(IoState *self, IoObject *coroutine);

IOVM_API void IoState_yield(IoState *self);
