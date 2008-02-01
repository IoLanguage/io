
//metadoc State copyright Steve Dekorte 2002
//metadoc State license BSD revised

#include "IoVMApi.h"

IOVM_API void IoState_fatalError_(IoState *self, char *error);

IOVM_API void IoState_error_(IoState *self, IoMessage *m, const char *format, ...);

IOVM_API IoObject *IoState_setErrorDescription_(IoState *self, const char *format, ...);