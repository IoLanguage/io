// metadoc List category Core
// metadoc List copyright Steve Dekorte 2002
// metadoc List license BSD revised
/*metadoc List description
A mutable array of values. The first index is 0.
*/

#include <math.h>
#include "IoList.h"
#include "IoObject.h"
#include "IoState.h"
#include "IoCFunction.h"
#include "IoSeq.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoBlock.h"

static const char *protoId = "List";

#define DATA(self) ((List *)(IoObject_dataPointer(self)))

IoTag *IoList_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoList_free);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoList_rawClone);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoList_mark);
    IoTag_compareFunc_(tag, (IoTagCompareFunc *)IoList_compare);
    // IoTag_writeToStreamFunc_(tag, (IoTagWriteToStreamFunc
    // *)IoList_writeToStream_); IoTag_readFromStreamFunc_(tag,
    // (IoTagReadFromStreamFunc *)IoList_readFromStream_);
    return tag;
}

/*
void IoList_writeToStream_(IoList *self, BStream *stream)
{
        List *list = DATA(self);

        BStream_writeTaggedInt32_(stream, List_size(list));

        LIST_FOREACH(list, i, v,
                BStream_writeTaggedInt32_(stream, IoObject_pid((IoObject *)v));
        );
}

void IoList_readFromStream_(IoList *self, BStream *stream)
{
        List *list = DATA(self);
        int i, max = BStream_readTaggedInt32(stream);

        for (i = 0; i < max; i ++)
        {
                int pid = BStream_readTaggedInt32(stream);
                IoObject *v = IoState_objectWithPid_(IOSTATE, pid);
                List_append_(list, v);
        }
}
*/

IoList *IoList_proto(void *state) {
    IoMethodTable methodTable[] = {
        {"with", IoList_with},

        // access

        {"indexOf", IoList_indexOf},
        {"contains", IoList_contains},
        {"containsIdenticalTo", IoList_containsIdenticalTo},
        {"capacity", IoList_capacity},
        {"size", IoList_size},

        // mutation

        {"setSize", IoList_setSize},
        {"removeAll", IoList_removeAll},
        {"appendSeq", IoList_appendSeq},
        {"append", IoList_append},
        {"prepend", IoList_prepend},
        {"push", IoList_append},

        {"appendIfAbsent", IoList_appendIfAbsent},

        {"remove", IoList_remove},
        {"pop", IoList_pop},

        {"atInsert", IoList_atInsert},
        {"at", IoList_at},
        {"atPut", IoList_atPut},

        {"removeAt", IoList_removeAt},
        {"swapIndices", IoList_swapIndices},

        {"preallocateToSize", IoList_preallocateToSize},

        {"first", IoList_first},
        {"last", IoList_last},
        {"slice", IoList_slice},
        {"sliceInPlace", IoList_sliceInPlace},

        {"sortInPlace", IoList_sortInPlace},
        {"sortInPlaceBy", IoList_sortInPlaceBy},
        {"foreach", IoList_foreach},
        {"reverseInPlace", IoList_reverseInPlace},
        {"reverseForeach", IoList_reverseForeach},

        {"asEncodedList", IoList_asEncodedList},
        {"fromEncodedList", IoList_fromEncodedList},
        {"join", IoList_join},
        {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);
    IoObject_tag_(self, IoList_newTag(state));

    IoObject_setDataPointer_(self, List_new());
    IoState_registerProtoWithId_((IoState *)state, self, protoId);

    IoObject_addMethodTable_(self, methodTable);
    return self;
}

IoList *IoList_rawClone(IoList *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_tag_(self, IoObject_tag(proto));
    IoObject_setDataPointer_(self, List_clone(DATA(proto)));
    return self;
}

IoList *IoList_new(void *state) {
    IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

IoList *IoList_newWithList_(void *state, List *list) {
    IoList *self = IoList_new(state);
    // printf("IoList_newWithList_ %p %p\n", (void *)self, (void *)list);
    List_free(IoObject_dataPointer(self));
    IoObject_setDataPointer_(self, list);
    return self;
}

void IoList_free(IoList *self) {
    if (NULL == DATA(self)) {
        printf("IoList_free(%p) already freed\n", (void *)self);
        exit(1);
    }
    // printf("IoList_free(%p) List_free(%p)\n", (void *)self, (void
    // *)DATA(self));

    List_free(DATA(self));
    IoObject_setDataPointer_(self, NULL);
}

void IoList_mark(IoList *self) {
    LIST_FOREACH(DATA(self), i, item, IoObject_shouldMark(item));
}

int IoList_compare(IoList *self, IoList *otherList) {
    if (!ISLIST(otherList)) {
        return IoObject_defaultCompare(self, otherList);
    } else {
        size_t s1 = List_size(DATA(self));
        size_t s2 = List_size(DATA(otherList));
        size_t i;

        if (s1 != s2) {
            return s1 > s2 ? 1 : -1;
        }

        for (i = 0; i < s1; i++) {
            IoObject *v1 = LIST_AT_(DATA(self), i);
            IoObject *v2 = LIST_AT_(DATA(otherList), i);
            int c = IoObject_compare(v1, v2);

            if (c) {
                return c;
            }
        }
    }
    return 0;
}

List *IoList_rawList(IoList *self) { return DATA(self); }

IoObject *IoList_rawAt_(IoList *self, int i) { return List_at_(DATA(self), i); }

void IoList_rawAt_put_(IoList *self, int i, IoObject *v) {
    List_at_put_(DATA(self), i, IOREF(v));
    IoObject_isDirty_(self, 1);
}

void IoList_rawAppend_(IoList *self, IoObject *v) {
    List_append_(DATA(self), IOREF(v));
    IoObject_isDirty_(self, 1);
}

void IoList_rawRemove_(IoList *self, IoObject *v) {
    List_remove_(DATA(self), IOREF(v));
    IoObject_isDirty_(self, 1);
}

void IoList_rawAddBaseList_(IoList *self, List *otherList) {
    List *list = DATA(self);
    LIST_FOREACH(otherList, i, v, List_append_(list, IOREF((IoObject *)v)););
    IoObject_isDirty_(self, 1);
}

void IoList_rawAddIoList_(IoList *self, IoList *other) {
    IoList_rawAddBaseList_(self, DATA(other));
    IoObject_isDirty_(self, 1);
}

size_t IoList_rawSize(IoList *self) { return List_size(DATA(self)); }

long IoList_rawIndexOf_(IoList *self, IoObject *v) {
    List *list = DATA(self);

    LIST_FOREACH(
        list, i, item,
        if (IoObject_compare(v, (IoObject *)item) == 0) { return i; });

    return -1;
}

void IoList_checkIndex(IoList *self, IoMessage *m, char allowsExtending,
                       int index, const char *methodName) {
    size_t max = List_size(DATA(self));

    if (allowsExtending) {
        max += 1;
    }

    if (index < 0 || index >= max) {
        IoState_error_(IOSTATE, m, "index out of bounds\n");
    }
}

// immutable --------------------------------------------------------

IO_METHOD(IoList, with) {
    /*doc List with(anObject, ...)
    Returns a new List containing the arguments.
    */

    int n, argCount = (int)IoMessage_argCount(m);
    IoList *ioList = IOCLONE(self);

    for (n = 0; n < argCount; n++) {
        IoObject *v = IoMessage_locals_valueArgAt_(m, locals, n);
        IoList_rawAppend_(ioList, v);
    }

    return ioList;
}

IO_METHOD(IoList, indexOf) {
    /*doc List indexOf(anObject)
    Returns the index of the first occurrence of anObject
    in the receiver. Returns Nil if the receiver doesn't contain anObject.
    */

    int count = IoMessage_argCount(m);

    IOASSERT(count, "indexOf requires at least one argument");

    {
        IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
        size_t i = IoList_rawIndexOf_(self, v);

        return i == -1 ? IONIL(self)
                       : (IoObject *)IONUMBER(IoList_rawIndexOf_(self, v));
    }
}

IO_METHOD(IoList, contains) {
    /*doc List contains(anObject)
    Returns true if the receiver contains anObject, otherwise returns false.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    return IOBOOL(self, IoList_rawIndexOf_(self, v) != -1);
}

IO_METHOD(IoList, containsIdenticalTo) {
    /*doc List containsIdenticalTo(anObject)
    Returns true if the receiver contains a value identical to anObject,
    otherwise returns false.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    return IOBOOL(self, List_contains_(DATA(self), v) != 0);
}

IO_METHOD(IoList, capacity) {
    /*doc List capacity
    Returns the number of potential elements the receiver can hold before it
    needs to grow.
    */

    return IONUMBER(DATA(self)->memSize / sizeof(void *));
}

IO_METHOD(IoList, size) {
    /*doc List size
    Returns the number of items in the receiver.
    */

    return IONUMBER(List_size(DATA(self)));
}

IO_METHOD(IoList, at) {
    /*doc List at(index)
    Returns the value at index. Returns Nil if the index is out of bounds.
    */

    int index = IoMessage_locals_intArgAt_(m, locals, 0);
    IoObject *v;
    /*IoList_checkIndex(self, m, 0, index, "Io List at");*/
    v = List_at_(DATA(self), index);
    return (v) ? v : IONIL(self);
}

IO_METHOD(IoList, first) {
    /*doc List first(optionalSize)
    Returns the first item or Nil if the list is empty.
    If optionalSize is provided, that number of the first items in the list are
    returned.
    */

    IoObject *result = List_at_(DATA(self), 0);
    return result ? result : IONIL(self);
}

IO_METHOD(IoList, last) {
    /*doc List last(optionalSize)
    Returns the last item or Nil if the list is empty.
    If optionalSize is provided, that number of the last items in the list are
    returned.
    */

    IoObject *result = List_at_(DATA(self), List_size(DATA(self)) - 1);
    return result ? result : IONIL(self);
}

void IoList_sliceIndex(int *index, int step, int size) {
    /* The following code mimics Python's slicing behaviour. */
    if (*index < 0) {
        *index += size;
        if (*index < 0) {
            *index = (step < 0) ? -1 : 0;
        }
    } else if (*index >= size) {
        *index = (step < 0) ? size - 1 : size;
    }
}

void IoList_sliceArguments(IoList *self, IoObject *locals, IoMessage *m,
                           int *start, int *end, int *step) {
    size_t size = IoList_rawSize(self);
    /* Checking step, before any other arguments. */
    *step = (IoMessage_argCount(m) == 3)
                ? IoMessage_locals_intArgAt_(m, locals, 2)
                : 1;
    IOASSERT(step != 0, "step cannot be equal to zero");

    *start = IoMessage_locals_intArgAt_(m, locals, 0);
    *end = (IoMessage_argCount(m) >= 2)
               ? IoMessage_locals_intArgAt_(m, locals, 1)
               : (int)size;

    /* Fixing slice index values. */
    IoList_sliceIndex(start, *step, (int)size);
    IoList_sliceIndex(end, *step, (int)size);
}

IO_METHOD(IoList, slice) {
    /*doc List slice(startIndex, endIndex, step)
    Returns a new string containing the subset of the receiver
from the startIndex to the endIndex. The endIndex argument
    is optional. If not given, it is assumed to be the end of the string.
Step argument is also optional and defaults to 1, if not given.
However, since Io supports positional arguments only, you need to
explicitly specify endIndex, if you need a custom step.
    */
    List *list;
    int start, end, step;

    IoList_sliceArguments(self, locals, m, &start, &end, &step);

    if ((step > 0 && end < start) || (step < 0 && end > start)) {
        return IoList_new(IOSTATE);
    } else {
        list = List_cloneSlice(DATA(self), start, end, step);
        return IoList_newWithList_(IOSTATE, list);
    }
}

IO_METHOD(IoList, sliceInPlace) {
    /*doc List sliceInPlace(startIndex, endIndex, step)
    Returns the receiver containing the subset of the
    receiver from the startIndex to the endIndex. The endIndex argument
    is optional. If not given, it is assumed to be the end of the string.
Step argument is also optional and defaults to 1.
    */

    int start, end, step;

    IoList_sliceArguments(self, locals, m, &start, &end, &step);

    if ((step > 0 && end < start) || (step < 0 && end > start)) {
        return IoList_new(IOSTATE);
    } else {
        List_sliceInPlace(DATA(self), start, end, step);
    }

    IoObject_isDirty_(self, 1);

    return self;
}

IO_METHOD(IoList, each) {
    IoState *state = IOSTATE;
    IoObject *result = IONIL(self);
    IoMessage *doMessage = IoMessage_rawArgAt_(m, 0);
    List *list = DATA(self);

    IoState_pushRetainPool(state);

    LIST_SAFEFOREACH(
        list, i, v, IoState_clearTopPool(state);
        result = IoMessage_locals_performOn_(doMessage, locals, (IoObject *)v);
        if (IoState_handleStatus(IOSTATE)) goto done;);

done:
    IoState_popRetainPoolExceptFor_(state, result);
    return result;
}

IO_METHOD(IoList, foreach) {
    /*doc List foreach(optionalIndex, value, message)
Loops over the list values setting the specified index and
value slots and executing the message. Returns the result of the last
execution of the message. Example:
<p>
<pre>
list(1, 2, 3) foreach(i, v, writeln(i, " = ", v))
list(1, 2, 3) foreach(v, writeln(v))</pre>
*/

    IoState *state = IOSTATE;
    IoObject *result = IONIL(self);
    IoSymbol *slotName = NULL;
    IoSymbol *valueName;
    IoMessage *doMessage;
    List *list = DATA(self);

    if (IoMessage_argCount(m) == 1) {
        return IoList_each(self, locals, m);
    }

    IoMessage_foreachArgs(m, self, &slotName, &valueName, &doMessage);

    IoState_pushRetainPool(state);

    if (slotName) {
        LIST_SAFEFOREACH(
            list, i, value, IoState_clearTopPool(state);
            IoObject_setSlot_to_(locals, slotName, IONUMBER(i));
            IoObject_setSlot_to_(locals, valueName, (IoObject *)value);
            result = IoMessage_locals_performOn_(doMessage, locals, locals);
            if (IoState_handleStatus(IOSTATE)) goto done;);
    } else {
        LIST_SAFEFOREACH(
            list, i, value, IoState_clearTopPool(state);
            IoObject_setSlot_to_(locals, valueName, (IoObject *)value);
            result = IoMessage_locals_performOn_(doMessage, locals, locals);
            if (IoState_handleStatus(IOSTATE)) goto done;);
    }

done:
    IoState_popRetainPoolExceptFor_(state, result);
    return result;
}

IO_METHOD(IoList, reverseForeach) {
    /*doc List reverseForeach(index, value, message)
    Same as foreach, but in reverse order.
    */

    IoState *state = IOSTATE;
    IoObject *result = IONIL(self);
    IoSymbol *slotName, *valueName;
    IoMessage *doMessage;
    long i;

    IoMessage_foreachArgs(m, self, &slotName, &valueName, &doMessage);

    IoState_pushRetainPool(state);

    for (i = List_size(DATA(self)) - 1; i >= 0; i--) {
        IoState_clearTopPool(state);
        {
            IoObject *value = (IoObject *)LIST_AT_(DATA(self), i);

            if (slotName) {
                IoObject_setSlot_to_(locals, slotName, IONUMBER(i));
            }

            IoObject_setSlot_to_(locals, valueName, value);
            result = IoMessage_locals_performOn_(doMessage, locals, locals);

            if (IoState_handleStatus(IOSTATE)) {
                goto done;
            }
        }
        if (i > List_size(DATA(self)) - 1) {
            i = List_size(DATA(self)) - 1;
        }
    }
done:
    IoState_popRetainPoolExceptFor_(state, result);
    return result;
}

// mutable --------------------------------------------------------

IO_METHOD(IoList, appendIfAbsent) {
    /*doc List appendIfAbsent(anObject)
    Adds each value not already contained by the receiver. Returns self.
    */

    int n;

    for (n = 0; n < IoMessage_argCount(m); n++) {
        IoObject *v = IoMessage_locals_valueArgAt_(m, locals, n);

        if (IoList_rawIndexOf_(self, v) == -1) {
            IoState_stackRetain_(IOSTATE, v);
            List_append_(DATA(self), IOREF(v));
            IoObject_isDirty_(self, 1);
        }
    }

    return self;
}

IO_METHOD(IoList, appendSeq) {
    /*doc List appendSeq(aList1, aList2, ...)
    Add the items in the lists to the receiver. Returns self.
    */

    int i;

    for (i = 0; i < IoMessage_argCount(m); i++) {
        IoObject *other = IoMessage_locals_valueArgAt_(m, locals, i);

        IOASSERT(ISLIST(other), "requires List objects as arguments");

        if (other == self) {
            IoState_error_(IOSTATE, m, "can't add a list to itself\n");
        } else {
            List *selfList = DATA(self);
            List *otherList = DATA(other);
            size_t i, max = List_size(otherList);

            for (i = 0; i < max; i++) {
                IoObject *v = List_at_(otherList, i);
                List_append_(selfList, IOREF(v));
            }
            IoObject_isDirty_(self, 1);
        }
    }

    return self;
}

IO_METHOD(IoList, append) {
    /*doc List append(anObject1, anObject2, ...)
    Appends the arguments to the end of the list. Returns self.
    */

    /*doc List push(anObject1, anObject2, ...)
    Same as add(anObject1, anObject2, ...).
    */

    int n;

    IOASSERT(IoMessage_argCount(m), "requires at least one argument");

    for (n = 0; n < IoMessage_argCount(m); n++) {
        IoObject *v = IoMessage_locals_valueArgAt_(m, locals, n);
        List_append_(DATA(self), IOREF(v));
    }

    IoObject_isDirty_(self, 1);

    return self;
}

IO_METHOD(IoList, prepend) {
    /*doc List prepend(anObject1, anObject2, ...)
    Inserts the values at the beginning of the list. Returns self.
    */

    int n;

    IOASSERT(IoMessage_argCount(m), "requires at least one argument");

    for (n = 0; n < IoMessage_argCount(m); n++) {
        IoObject *v = IoMessage_locals_valueArgAt_(m, locals, n);
        List_at_insert_(DATA(self), 0, IOREF(v));
    }

    IoObject_isDirty_(self, 1);

    return self;
}

IO_METHOD(IoList, remove) {
    /*doc List remove(anObject, ...)
    Removes all occurrences of the arguments from the receiver. Returns self.
    */

    int count = IoMessage_argCount(m);
    int j;

    IOASSERT(count, "requires at least one argument");

    for (j = 0; j < count; j++) {
        IoObject *v = IoMessage_locals_valueArgAt_(m, locals, j);

        // a quick pass to remove values with equal pointers
        List_remove_(DATA(self), v);

        // slow pass to remove values that match comparision test
        for (;;) {
            long i = IoList_rawIndexOf_(self, v);

            if (i == -1) {
                break;
            }

            List_removeIndex_(DATA(self), i);
        }
    }

    IoObject_isDirty_(self, 1);

    return self;
}

IO_METHOD(IoList, pop) {
    /*doc List pop
    Returns the last item in the list and removes it
    from the receiver. Returns nil if the receiver is empty.
    */

    IoObject *v = List_pop(DATA(self));
    return (v) ? v : IONIL(self);
}

IO_METHOD(IoList, atInsert) {
    /*doc List atInsert(index, anObject)
    Inserts anObject at the index specified by index.
    Adds anObject if the index equals the current count of the receiver.
    Raises an exception if the index is out of bounds. Returns self.
    */

    int index = IoMessage_locals_intArgAt_(m, locals, 0);
    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 1);

    IoList_checkIndex(self, m, 1, index, "List atInsert");
    List_at_insert_(DATA(self), index, IOREF(v));
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoList, removeAt) {
    /*doc List removeAt(index)
    Removes the item at the specified index and returns the value removed.
    Raises an exception if the index is out of bounds.
    */

    int index = IoMessage_locals_intArgAt_(m, locals, 0);
    IoObject *v = List_at_(DATA(self), index);

    IoList_checkIndex(self, m, 0, index, "Io List atInsert");
    List_removeIndex_(DATA(self), index);
    IoObject_isDirty_(self, 1);
    return (v) ? v : IONIL(self);
}

void IoList_rawAtPut(IoList *self, int i, IoObject *v) {
    while (List_size(DATA(self)) < i) /* not efficient */
    {
        List_append_(DATA(self), IONIL(self));
    }

    List_at_put_(DATA(self), i, IOREF(v));
    IoObject_isDirty_(self, 1);
}

IO_METHOD(IoList, atPut) {
    /*doc List atPut(index, anObject)
    Replaces the existing value at index with anObject.
    Raises an exception if the index is out of bounds. Returns self.
    */

    int index = IoMessage_locals_intArgAt_(m, locals, 0);
    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 1);

    IoList_checkIndex(self, m, 0, index, "Io List atPut");
    IoList_rawAtPut(self, index, v);
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoList, setSize) {
    /*doc List setSize(newSize)
    Sets the size of the receiver by either removing excess items or adding nils
    as needed.
    */

    List *list = DATA(self);
    size_t newSize = IoMessage_locals_sizetArgAt_(m, locals, 0);
    size_t oldSize = List_size(list);

    if (newSize < oldSize) {
        List_setSize_(list, newSize);
    } else {
        size_t i, max = newSize - oldSize;
        IoObject *nilObject = IONIL(self);

        for (i = 0; i < max; i++) {
            List_append_(list, nilObject);
        }
    }

    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoList, removeAll) {
    /*doc List empty
    Removes all items from the receiver.
    */

    List_removeAll(DATA(self));
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoList, swapIndices) {
    /*doc List swapIndices(index1, index2)
    Exchanges the object at index1 with the object at index2.
    Raises an exception if either index is out of bounds. Returns self.
    */

    int i = IoMessage_locals_intArgAt_(m, locals, 0);
    int j = IoMessage_locals_intArgAt_(m, locals, 1);

    IoList_checkIndex(self, m, 0, i, "List swapIndices");
    IoList_checkIndex(self, m, 0, j, "List swapIndices");
    List_swap_with_(DATA(self), i, j);
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoList, reverseInPlace) {
    /*doc List reverseInPlace
    Reverses the ordering of all the items in the receiver. Returns self.
    */
    List_reverseInPlace(DATA(self));
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoList, preallocateToSize) {
    /*doc List preallocateToSize(aNumber)
    Preallocates array memory to hold aNumber number of items.
    */

    int newSize = IoMessage_locals_intArgAt_(m, locals, 0);
    List_preallocateToSize_(DATA(self), newSize);
    return self;
}

// sorting -----------------------------------------------

typedef struct {
    IoState *state;
    IoObject *locals;
    IoMessage *exp;
    List *list;
} MSortContext;

int MSortContext_compareForSort(MSortContext *self, void *ap, void *bp) {
    IoObject *a = *(void **)ap;
    IoObject *b = *(void **)bp;
    int r;

    IoState_pushRetainPool(self->state);

    a = IoMessage_locals_performOn_(self->exp, self->locals, a);
    b = IoMessage_locals_performOn_(self->exp, self->locals, b);
    r = IoObject_compare(a, b);

    IoState_popRetainPool(self->state);
    return r;
}

IO_METHOD(IoList, sortInPlace) {
    /*doc List sortInPlace(optionalExpression)
    Sorts the list using the compare method on the items. Returns self.
    If an optionalExpression is provided, the sort is done on the result of the
    evaluation of the optionalExpression on each value.
    */

    if (IoMessage_argCount(m) == 0) {
        List_qsort(DATA(self), (ListSortCallback *)IoObject_sortCompare);
    } else {
        MSortContext sc;
        MSortContext *sortContext = &sc;
        sortContext->state = IOSTATE;
        sortContext->locals = locals;
        sortContext->exp = IoMessage_rawArgAt_(m, 0);

        List_qsort_r(DATA(self), sortContext,
                     (ListSortRCallback *)MSortContext_compareForSort);
    }

    IoObject_isDirty_(self, 1);
    return self;
}

typedef struct {
    IoState *state;
    IoObject *locals;
    IoBlock *block;
    IoMessage *blockMsg;
    IoMessage *argMsg1;
    IoMessage *argMsg2;
    List *list;
} SortContext;

int SortContext_compareForSort(SortContext *self, void *ap, void *bp) {
    IoObject *a = *(void **)ap;
    IoObject *b = *(void **)bp;
    IoObject *cr;
    IoState_pushRetainPool(self->state);

    IoMessage_rawSetCachedResult_(self->argMsg1, a);
    IoMessage_rawSetCachedResult_(self->argMsg2, b);
    cr = IoBlock_activate(self->block, self->locals, self->locals,
                          self->blockMsg, self->locals);

    IoState_popRetainPool(self->state);
    return ISFALSE(cr) ? 1 : -1;
}

IO_METHOD(IoList, sortInPlaceBy) {
    /*doc List sortInPlaceBy(aBlock)
    Sort the list using aBlock as the compare function. Returns self.
    */

    SortContext sc;
    SortContext *sortContext = &sc;
    sc.state = IOSTATE;
    sc.locals = locals;
    sc.block = IoMessage_locals_blockArgAt_(m, locals, 0);
    sc.blockMsg = IoMessage_new(IOSTATE);
    sc.argMsg1 = IoMessage_new(IOSTATE);
    sc.argMsg2 = IoMessage_new(IOSTATE);

    IoMessage_addArg_(sortContext->blockMsg, sortContext->argMsg1);
    IoMessage_addArg_(sortContext->blockMsg, sortContext->argMsg2);

    List_qsort_r(DATA(self), &sc,
                 (ListSortRCallback *)SortContext_compareForSort);
    IoObject_isDirty_(self, 1);
    return self;
}

typedef enum {
    IOLIST_ENCODING_TYPE_NIL,
    IOLIST_ENCODING_TYPE_NUMBER,
    IOLIST_ENCODING_TYPE_SYMBOL,
    IOLIST_ENCODING_TYPE_REFERENCE,

} IOLIST_ENCODING_TYPE;

IO_METHOD(IoList, asEncodedList) {
    /*doc List asEncodedList
    Returns a Sequence with an encoding of the list.
    Nil, Number and Symbol objects are copied into the encoding, for other
    object types, referenceIdForObject(item) will be called to request a
    reference id for the object.

    Also see: List fromEncodedList.
    */

    UArray *u = UArray_new();
    List *list = IoList_rawList(self);
    size_t i, max = List_size(list);
    IoMessage *rm = IOSTATE->referenceIdForObjectMessage;

    UArray_setItemType_(u, CTYPE_uint8_t);
    UArray_setEncoding_(u, CENCODING_NUMBER);

    // UArray_appendBytes_size_(u, "    ", 4); // placeholder until we know the
    // size

    for (i = 0; i < max; i++) {
        IoObject *item = List_at_(list, i);

        if (ISNIL(item)) {
            UArray_appendLong_(u, IOLIST_ENCODING_TYPE_NIL);
            UArray_appendLong_(u, 0);
            UArray_appendLong_(u, 0);
        } else if (ISNUMBER(item)) {
            float32_t f = CNUMBER(item);

            UArray_appendLong_(u, IOLIST_ENCODING_TYPE_NUMBER);
            UArray_appendLong_(u, CENCODING_NUMBER);
            UArray_appendLong_(u, CTYPE_float32_t);
            UArray_appendBytes_size_(u, (const uint8_t *)(&f),
                                     sizeof(float32_t));
        } else if (ISSEQ(item)) {
            UArray *s = IoSeq_rawUArray(item);
            uint32_t size = (uint32_t)UArray_size(s);

            UArray_appendLong_(u, IOLIST_ENCODING_TYPE_SYMBOL);
            UArray_appendLong_(u, UArray_encoding(s));
            UArray_appendLong_(u, UArray_itemType(s));
            UArray_appendBytes_size_(u, (const uint8_t *)(&size),
                                     sizeof(uint32_t));
            UArray_appendBytes_size_(u, (const uint8_t *)UArray_bytes(s),
                                     UArray_sizeInBytes(s));
        } else {
            IoObject *result;

            IoMessage_setCachedArg_to_(rm, 0, item);
            result = IoObject_perform(locals, locals, rm);
            IoMessage_setCachedArg_to_(rm, 0, IONIL(self));

            IOASSERT(ISNUMBER(result),
                     "referenceIdForObject() must return a Number");

            {
                uint32_t id = CNUMBER(result);

                UArray_appendLong_(u, IOLIST_ENCODING_TYPE_REFERENCE);
                UArray_appendLong_(u, 0);
                UArray_appendLong_(u, 0);
                UArray_appendBytes_size_(u, (const uint8_t *)(&id),
                                         sizeof(uint32_t));
            }
        }
    }

    return IoSeq_newWithUArray_copy_(IOSTATE, u, 0);
}

IO_METHOD(IoList, fromEncodedList) {
    /*doc List fromEncodedList(aSeq)
    Returns a List with the decoded Nils, Symbols and Numbers from the input raw
    array. For each object reference encounters, objectForReferenceId(id) will
    be called to allow the reference to be resolved.

    Also see: List asEncodedList.
    */

    IoMessage *rm = IOSTATE->objectForReferenceIdMessage;
    IoSeq *s = IoMessage_locals_seqArgAt_(m, locals, 0);
    UArray *u = IoSeq_rawUArray(s);
    List *list = List_new();
    const uint8_t *d = UArray_bytes(u);
    size_t uSize = UArray_sizeInBytes(u);
    size_t index = 0;

    // add bounds checks

    while (index < uSize) {
        if (index + 3 > uSize) {
            break;
        }
        uint8_t type = d[index + 0];
        uint8_t encoding = d[index + 1];
        uint8_t itemType = d[index + 2];
        index += 3;

        if (type == IOLIST_ENCODING_TYPE_NIL) {
            List_append_(list, IONIL(self));
        } else if (type == IOLIST_ENCODING_TYPE_NUMBER) {
            if (index + sizeof(float32_t) > uSize) {
                break;
            }
            float32_t f = *((float32_t *)(d + index));

            index += sizeof(float32_t);
            List_append_(list, IONUMBER(f));
        } else if (type == IOLIST_ENCODING_TYPE_SYMBOL) {
            if (index + sizeof(uint32_t) > uSize) {
                break;
            }
            uint32_t size = *((uint32_t *)(d + index));
            UArray *o;

            index += sizeof(uint32_t);

            if (index + size > uSize) {
                break;
            }

            o = UArray_newWithData_type_size_copy_((void *)(d + index),
                                                   itemType, size, 1);
            UArray_setEncoding_(o, encoding);
            List_append_(list, IoSeq_newWithUArray_copy_(IOSTATE, o, 0));

            index += size;
        } else if (type == IOLIST_ENCODING_TYPE_REFERENCE) {
            // should we have a Number reference encoding?
            if (index + sizeof(uint32_t) > uSize) {
                break;
            }
            uint32_t id = *((uint32_t *)(d + index));
            IoMessage_setCachedArg_to_(rm, 0, IONUMBER(id));
            IoMessage_setCachedArg_to_(rm, 0, IONIL(self));

            index += sizeof(uint32_t);

            {
                IoObject *result = IoObject_perform(locals, locals, rm);
                List_append_(list, result);
            }
        } else {
            IOASSERT(0, "unrecognized encoded type");
        }
    }
    if (index == uSize) {
        return IoList_newWithList_(IOSTATE, list);
    } else {
        /* Decode error for some reason */
        List_free(list);
        return IONIL(self);
    }
}

IO_METHOD(IoList, join) {
    /*doc List join(optionalSeparator)
    Returns a String with the elements of the receiver concatenated into one
    String. If optionalSeparator is provided, it is used to separate the
    concatenated strings. This operation does not respect string encodings.
    */

    List *items = IoList_rawList(self);
    size_t itemCount = List_size(items);
    IoSeq *separator = IoMessage_locals_seqArgAt_(m, locals, 0);
    UArray *out = UArray_new();
    int totalSize = 0;
    int hasSeparator = !ISNIL(separator);
    size_t separatorSize = hasSeparator ? IOSEQ_LENGTH(separator) : 0;
    uint8_t *bytes;
    IOASSERT(ISSEQ(separator), "separator must be of type Sequence");

    LIST_FOREACH(
        items, i, v,
        if (!ISSEQ(v)) {
            // printf("type: %s\n", IoObject_name(v));
            IOASSERT(ISSEQ(v), "values must be of type Sequence");
        } totalSize += IoSeq_rawSizeInBytes(v);
        // printf("UArray_sizeInBytes(v): %i\n", (int) IoSeq_rawSizeInBytes(v));
        if (hasSeparator) totalSize += separatorSize;)

    if (hasSeparator)
        totalSize -= separatorSize;

    // printf("separatorSize: %i\n", (int) separatorSize);
    // printf("totalSize: %i\n", (int) totalSize);
    UArray_sizeTo_(out, totalSize + 1);

    bytes = UArray_mutableBytes(out);

    LIST_FOREACH(
        items, i, v, size_t vsize = IoSeq_rawSizeInBytes(v);
        memcpy((char *)bytes, (char *)IoSeq_rawBytes(v), (int)vsize);
        bytes += vsize;

        if (hasSeparator && i != itemCount - 1) {
            memcpy(bytes, (char *)IoSeq_rawBytes(separator), separatorSize);
            bytes += separatorSize;
        })

    return IoSeq_newWithUArray_copy_(IOSTATE, out, 0);
}
