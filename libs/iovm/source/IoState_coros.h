/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/
#include "IoVMApi.h"

IOVM_API IoObject *IoState_currentCoroutine(IoState *self);
IOVM_API void IoState_setCurrentCoroutine_(IoState *self, IoObject *coroutine);

IOVM_API void IoState_yield(IoState *self);


