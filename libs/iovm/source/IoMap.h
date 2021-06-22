// metadoc Map copyright Steve Dekorte 2002
// metadoc Map license BSD revised

#ifndef IoMap_DEFINED
#define IoMap_DEFINED 1

#include "Common.h"
#include "IoObject.h"
#include "IoList.h"
#include "PHash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISMAP(self)                                                            \
    IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoMap_rawClone)

typedef IoObject IoMap;

IoMap *IoMap_proto(void *state);
IOVM_API IoMap *IoMap_rawClone(IoMap *self);
IOVM_API IoMap *IoMap_new(void *state);
void IoMap_free(IoMap *self);
void IoMap_mark(IoMap *self);
IOVM_API PHash *IoMap_rawHash(IoMap *self);

/*
void IoMap_writeToStream_(IoMap *self, BStream *stream);
void IoMap_readFromStream_(IoMap *self, BStream *stream);
*/

// -----------------------------------------------------------

IO_METHOD(IoMap, empty);

IOVM_API IoObject *IoMap_rawAt(IoMap *self, IoSymbol *k);
IOVM_API IO_METHOD(IoMap, at);

IOVM_API void IoMap_rawAtPut(IoMap *self, IoSymbol *k, IoObject *v);
IO_METHOD(IoMap, atPut);
IO_METHOD(IoMap, atIfAbsentPut);
IO_METHOD(IoMap, removeAt);
IO_METHOD(IoMap, size);

IO_METHOD(IoMap, hasKey);
IO_METHOD(IoMap, hasValue);

IOVM_API IoList *IoMap_rawKeys(IoMap *self);
IO_METHOD(IoMap, keys);

IO_METHOD(IoMap, values);
IO_METHOD(IoMap, foreach);

#ifdef __cplusplus
}
#endif
#endif
