/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

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
IOVM_API IoList *IoList_rawClone(IoObject *self);
IOVM_API IoList *IoList_new(void *state);
IOVM_API IoList *IoList_newWithList_(void *state, List *list);
IOVM_API void IoList_free(IoObject *self);
IOVM_API void IoList_mark(IoObject *self);
IOVM_API int IoList_compare(IoObject *self, IoList *otherList);

IOVM_API void IoList_writeToStream_(IoObject *self, BStream *stream);
IOVM_API void IoList_readFromStream_(IoObject *self, BStream *stream);

IOVM_API List *IoList_rawList(IoObject *self);
IOVM_API IoObject *IoList_rawAt_(IoObject *self, int i);
IOVM_API void IoList_rawAt_put_(IoObject *self, int i, IoObject *v);
IOVM_API void IoList_rawAppend_(IoObject *self, IoObject *v);
IOVM_API void IoList_rawRemove_(IoObject *self, IoObject *v);
IOVM_API void IoList_rawAddIoList_(IoObject *self, IoList *other);
IOVM_API void IoList_rawAddBaseList_(IoObject *self, List *other);
IOVM_API size_t IoList_rawSize(IoObject *self);

// immutable 

IOVM_API IoObject *IoList_with(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_indexOf(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_contains(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_containsIdenticalTo(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_capacity(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_size(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_at(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_first(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_last(IoObject *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoList_slice(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_sliceInPlace(IoObject *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoList_mapInPlace(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_map(IoObject *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoList_select(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_detect(IoObject *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoList_foreach(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_reverseForeach(IoObject *self, IoObject *locals, IoMessage *m);

// mutable

IOVM_API IoObject *IoList_preallocateToSize(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_append(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_prepend(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_appendIfAbsent(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_appendSeq(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_remove(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_push(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_pop(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_removeAll(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_atInsert(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_removeAt(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_atPut(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_removeAll(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_swapIndices(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_reverse(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_sortInPlace(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_sortInPlaceBy(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoList_selectInPlace(IoObject *self, IoObject *locals, IoMessage *m);\

#ifdef __cplusplus
}
#endif
#endif
