
// metadoc List copyright Steve Dekorte 2002
// metadoc List license BSD revised

#ifndef IOLIST_DEFINED
#define IOLIST_DEFINED 1

#include "IoVMApi.h"
#include "Common.h"
#include "IoState.h"
#include "IoObject.h"
#include "List.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISLIST(self)                                                           \
    IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoList_rawClone)

typedef IoObject IoList;

IOVM_API IoList *IoList_proto(void *state);
IOVM_API IoList *IoList_rawClone(IoList *self);
IOVM_API IoList *IoList_new(void *state);
IOVM_API IoList *IoList_newWithList_(void *state, List *list);
IOVM_API void IoList_free(IoList *self);
IOVM_API void IoList_mark(IoList *self);
IOVM_API int IoList_compare(IoList *self, IoList *otherList);

// IOVM_API void IoList_writeToStream_(IoList *self, BStream *stream);
// IOVM_API void IoList_readFromStream_(IoList *self, BStream *stream);

IOVM_API List *IoList_rawList(IoList *self);
IOVM_API IoObject *IoList_rawAt_(IoList *self, int i);
IOVM_API void IoList_rawAt_put_(IoList *self, int i, IoObject *v);
IOVM_API void IoList_rawAppend_(IoList *self, IoObject *v);
IOVM_API void IoList_rawRemove_(IoList *self, IoObject *v);
IOVM_API void IoList_rawAddIoList_(IoList *self, IoList *other);
IOVM_API void IoList_rawAddBaseList_(IoList *self, List *other);
IOVM_API size_t IoList_rawSize(IoList *self);

// immutable

IOVM_API IO_METHOD(IoList, with);
IOVM_API IO_METHOD(IoList, indexOf);
IOVM_API IO_METHOD(IoList, contains);
IOVM_API IO_METHOD(IoList, containsIdenticalTo);
IOVM_API IO_METHOD(IoList, capacity);
IOVM_API IO_METHOD(IoList, size);
IOVM_API IO_METHOD(IoList, at);
IOVM_API IO_METHOD(IoList, first);
IOVM_API IO_METHOD(IoList, last);

IOVM_API IO_METHOD(IoList, slice);
IOVM_API IO_METHOD(IoList, sliceInPlace);

IOVM_API IO_METHOD(IoList, mapInPlace);
IOVM_API IO_METHOD(IoList, map);

IOVM_API IO_METHOD(IoList, select);
IOVM_API IO_METHOD(IoList, detect);

IOVM_API IO_METHOD(IoList, foreach);
IOVM_API IO_METHOD(IoList, reverseForeach);

// mutable

IOVM_API IO_METHOD(IoList, preallocateToSize);
IOVM_API IO_METHOD(IoList, append);
IOVM_API IO_METHOD(IoList, prepend);
IOVM_API IO_METHOD(IoList, appendIfAbsent);
IOVM_API IO_METHOD(IoList, appendSeq);
IOVM_API IO_METHOD(IoList, remove);
IOVM_API IO_METHOD(IoList, push);
IOVM_API IO_METHOD(IoList, pop);

IO_METHOD(IoList, setSize);
IOVM_API IO_METHOD(IoList, removeAll);
IOVM_API IO_METHOD(IoList, atInsert);
IOVM_API IO_METHOD(IoList, removeAt);
IOVM_API IO_METHOD(IoList, atPut);
IOVM_API IO_METHOD(IoList, removeAll);
IOVM_API IO_METHOD(IoList, swapIndices);
IOVM_API IO_METHOD(IoList, reverseInPlace);
IOVM_API IO_METHOD(IoList, sortInPlace);
IOVM_API IO_METHOD(IoList, sortInPlaceBy);
IOVM_API IO_METHOD(IoList, selectInPlace);

IOVM_API IO_METHOD(IoList, asEncodedList);
IOVM_API IO_METHOD(IoList, fromEncodedList);
IOVM_API IO_METHOD(IoList, join);

#ifdef __cplusplus
}
#endif
#endif
