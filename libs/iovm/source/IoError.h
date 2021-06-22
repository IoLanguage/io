// metadoc Error copyright Rich Collins 2008
// metadoc Error license BSD revised

#ifndef IOERROR_DEFINED
#define IOERROR_DEFINED 1

#ifdef __cplusplus
extern "C" {
#endif

#include "IoObject.h"

#define ISERROR(self)                                                          \
    IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoError_rawClone)

typedef IoObject IoError;

IoError *IoError_rawClone(IoError *self);
IoError *IoError_proto(void *state);
IoError *IoError_new(void *state);

void IoError_free(IoError *self);
void IoError_mark(IoError *self);

IOVM_API IoObject *IoError_newWithMessageFormat_(void *state,
                                                 const char *format, ...);
IOVM_API IoObject *IoError_newWithCStringMessage_(IoState *state,
                                                  char *cString);

#ifdef __cplusplus
}
#endif
#endif
