//
// Created by michaelpollind on 3/18/17.
//

#ifndef IOLANGUAGE_IOTUPLE_H
#define IOLANGUAGE_IOTUPLE_H

#include "IoVMApi.h"
#include "Common.h"
#include "IoState.h"
#include "IoObject.h"
#include "List.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISTUPLE(self) \
	IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoTuple_rawClone)

typedef IoObject IoTuple;

IOVM_API IoTuple *IoTuple_proto(void *state);
IOVM_API IoTuple *IoTuple_rawClone(IoTuple *self);
IOVM_API IoTuple *IoTuple_new(void *state);
IOVM_API void IoTuple_free(IoTuple *self);
IOVM_API void IoTuple_mark(IoTuple *self);
IOVM_API int IoTuple_compare(IoTuple *self, IoObject *otherObject);
// immutable

IOVM_API IO_METHOD(IoTuple, set);
IOVM_API IO_METHOD(IoTuple, size);
IOVM_API IO_METHOD(IoTuple,concat);
IOVM_API IO_METHOD(IoTuple,toList);
IOVM_API IO_METHOD(IoTuple, at);
IOVM_API IO_METHOD(IoTuple, slice);
IOVM_API IO_METHOD(IoTuple, empty);


#ifdef __cplusplus
}
#endif
#endif //IOLANGUAGE_IOTUPLE_H
