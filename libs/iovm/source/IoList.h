
//metadoc List copyright Steve Dekorte 2002
//metadoc List license BSD revised

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

#define ISLIST(self) \
	IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoList_rawClone)

typedef IoObject IoList;

IOVM_API IoList *IoList_proto(void *state);
IOVM_API IoList *IoList_rawClone(IoList *self);
IOVM_API IoList *IoList_new(void *state);
IOVM_API IoList *IoList_newWithList_(void *state, List *list);
IOVM_API void IoList_free(IoList *self);
IOVM_API void IoList_mark(IoList *self);
IOVM_API int IoList_compare(IoList *self, IoList *otherList);

//IOVM_API void IoList_writeToStream_(IoList *self, BStream *stream);
//IOVM_API void IoList_readFromStream_(IoList *self, BStream *stream);

IOVM_API List *IoList_rawList(IoList *self);
IOVM_API IoObject *IoList_rawAt_(IoList *self, int i);
IOVM_API void IoList_rawAt_put_(IoList *self, int i, IoObject *v);
IOVM_API void IoList_rawAppend_(IoList *self, IoObject *v);
IOVM_API void IoList_rawRemove_(IoList *self, IoObject *v);
IOVM_API void IoList_rawAddIoList_(IoList *self, IoList *other);
IOVM_API void IoList_rawAddBaseList_(IoList *self, List *other);
IOVM_API size_t IoList_rawSize(IoList *self);

// immutable

IOVM_API IoObject *IoList_with(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_indexOf(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_contains(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_containsIdenticalTo(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_capacity(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_size(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_at(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_first(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_last(IoList *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoList_slice(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_sliceInPlace(IoList *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoList_mapInPlace(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_map(IoList *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoList_select(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_detect(IoList *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoList_foreach(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_reverseForeach(IoList *self, IoObject *locals, IoMessage *m);

// mutable

IOVM_API IoObject *IoList_preallocateToSize(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_append(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_prepend(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_appendIfAbsent(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_appendSeq(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_remove(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_push(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_pop(IoList *self, IoObject *locals, IoMessage *m);

IoObject *IoList_setSize(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_removeAll(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_atInsert(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_removeAt(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_atPut(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_removeAll(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_swapIndices(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_reverseInPlace(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_sortInPlace(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_sortInPlaceBy(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_selectInPlace(IoList *self, IoObject *locals, IoMessage *m);\

IOVM_API IoObject *IoList_asEncodedList(IoList *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_fromEncodedList(IoList *self, IoObject *locals, IoMessage *m);


#ifdef __cplusplus
}
#endif
#endif
