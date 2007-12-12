/*#io
List ioDoc(
           docCopyright("Steve Dekorte", 2002)
           docLicense("BSD revised")
           docObject("List")
           docInclude("_ioCode/List.io")
           docDescription("A mutable array of values. The first index is 0.")
		 docCategory("DataStructures")
           */

#include "IoList.h"
#include "IoObject.h"
#include "IoState.h"
#include "IoCFunction.h"
#include "IoSeq.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoBlock.h"
#include "Sorting.h"
#include <math.h>

#define DATA(self) ((List *)(IoObject_dataPointer(self)))

IoTag *IoList_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("List");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoList_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoList_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoList_mark);
	IoTag_compareFunc_(tag, (IoTagCompareFunc *)IoList_compare);
	IoTag_writeToStreamFunc_(tag, (IoTagWriteToStreamFunc *)IoList_writeToStream_);
	IoTag_readFromStreamFunc_(tag, (IoTagReadFromStreamFunc *)IoList_readFromStream_);
	return tag;
}

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

IoList *IoList_proto(void *state)
{
	IoMethodTable methodTable[] = {
	{"with",        IoList_with},

	// access

	{"indexOf",     IoList_indexOf},
	{"contains",    IoList_contains},
	{"containsIdenticalTo", IoList_containsIdenticalTo},
	{"capacity",    IoList_capacity},
	{"size",        IoList_size},

	// mutation

	{"setSize",     IoList_setSize},
	{"removeAll",   IoList_removeAll},
	{"appendSeq",   IoList_appendSeq},
	{"append",      IoList_append},
	{"prepend",     IoList_prepend},
	{"push",        IoList_append},

	{"appendIfAbsent", IoList_appendIfAbsent},

	{"remove",      IoList_remove},
	{"pop",         IoList_pop},

	{"atInsert",    IoList_atInsert},
	{"at",          IoList_at},
	{"atPut",       IoList_atPut},

	{"removeAt",    IoList_removeAt},
	{"swapIndices", IoList_swapIndices},

	{"preallocateToSize", IoList_preallocateToSize},

	{"first",          IoList_first},
	{"last",           IoList_last},
	{"slice",          IoList_slice},
	{"sliceInPlace",   IoList_sliceInPlace},


	{"sortInPlace",           IoList_sortInPlace},
	{"sortInPlaceBy",         IoList_sortInPlaceBy},
	{"foreach",        IoList_foreach},
	{"reverse",        IoList_reverse},
	{"reverseForeach", IoList_reverseForeach},
	{NULL, NULL},
	};

	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoList_newTag(state));

	IoObject_setDataPointer_(self, List_new());
	IoState_registerProtoWithFunc_((IoState *)state, self, IoList_proto);

	IoObject_addMethodTable_(self, methodTable);
	return self;
}

IoList *IoList_rawClone(IoList *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_tag_(self, IoObject_tag(proto));
	IoObject_setDataPointer_(self, List_clone(DATA(proto)));
	return self;
}

IoList *IoList_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoList_proto);
	return IOCLONE(proto);
}

IoList *IoList_newWithList_(void *state, List *list)
{
	IoList *self = IoList_new(state);
	//printf("IoList_newWithList_ %p %p\n", (void *)self, (void *)list);
	List_free(IoObject_dataPointer(self));
	IoObject_setDataPointer_(self, list);
	return self;
}

void IoList_free(IoList *self)
{
	if (NULL == DATA(self))
	{
		printf("IoList_free(%p) already freed\n", (void *)self);
		exit(1);
	}
	//printf("IoList_free(%p) List_free(%p)\n", (void *)self, (void *)DATA(self));

	List_free(DATA(self));
	IoObject_setDataPointer_(self, NULL);

}

void IoList_mark(IoList *self)
{
	LIST_FOREACH(DATA(self), i, item, IoObject_shouldMark(item));
}

int IoList_compare(IoList *self, IoList *otherList)
{
	if (!ISLIST(otherList))
	{
		return IoObject_defaultCompare(self, otherList);
	}
	else
	{
		size_t s1 =  List_size(DATA(self));
		size_t s2 =  List_size(DATA(otherList));
		size_t i;

		if (s1 != s2)
		{
			return s1 > s2 ? 1 : -1;
		}

		for (i = 0; i < s1; i ++)
		{
			IoObject *v1 = LIST_AT_(DATA(self), i);
			IoObject *v2 = LIST_AT_(DATA(otherList), i);
			int c = IoObject_compare(v1, v2);

			if (c)
			{
				return c;
			}
		}
	}
	return 0;
}

List *IoList_rawList(IoList *self)
{
	return DATA(self);
}

IoObject *IoList_rawAt_(IoList *self, int i)
{
	return List_at_(DATA(self), i);
}

void IoList_rawAt_put_(IoList *self, int i, IoObject *v)
{
	List_at_put_(DATA(self), i, IOREF(v));
}

void IoList_rawAppend_(IoList *self, IoObject *v)
{
	List_append_(DATA(self), IOREF(v));
}

void IoList_rawRemove_(IoList *self, IoObject *v)
{
	List_remove_(DATA(self), IOREF(v));
}

void IoList_rawAddBaseList_(IoList *self, List *otherList)
{
	List *list = DATA(self);
	LIST_FOREACH(otherList, i, v, List_append_(list, IOREF((IoObject *)v)); );
}

void IoList_rawAddIoList_(IoList *self, IoList *other)
{
	IoList_rawAddBaseList_(self, DATA(other));
}

size_t IoList_rawSize(IoList *self)
{
	return List_size(DATA(self));
}

int IoList_rawIndexOf_(IoList *self, IoObject *v)
{
	List *list = DATA(self);

	LIST_FOREACH(list, i, item,
		if (IoObject_compare(v, (IoObject *)item) == 0)
		{
			return i;
		}
	);

	return -1;
}

void IoList_checkIndex(IoList *self, IoMessage *m, char allowsExtending, int index, const char *methodName)
{
	int max = List_size(DATA(self));

	if (allowsExtending)
	{
		max += 1;
	}

	if (index < 0 || index >= max)
	{
		IoState_error_(IOSTATE, m, "index out of bounds\n");
	}
}

// immutable --------------------------------------------------------

IoObject *IoList_with(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("with(anObject, ...)",
		   "Returns a new List containing the arguments. ")
	*/

	int n, argCount = IoMessage_argCount(m);
	IoList *ioList = IOCLONE(self);

	for (n = 0; n < argCount; n ++)
	{
		IoObject *v = IoMessage_locals_valueArgAt_(m, locals, n);
		IoList_rawAppend_(ioList, v);
	}

	return ioList;
}


IoObject *IoList_indexOf(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("indexOf(anObject)",
		   "Returns the index of the first occurrence of anObject
in the receiver. Returns Nil if the receiver doesn't contain anObject. ")
	*/

	int count = IoMessage_argCount(m);

	IOASSERT(count, "remove requires at least one argument");

	{
		IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
		int i = IoList_rawIndexOf_(self, v);

		return i == -1 ? IONIL(self) :
			(IoObject *)IONUMBER(IoList_rawIndexOf_(self, v));
	}
}

IoObject *IoList_contains(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("contains(anObject)",
		   "Returns true if the receiver contains anObject, otherwise returns false. ")
	*/

	IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
	return IOBOOL(self, IoList_rawIndexOf_(self, v) != -1);
}

IoObject *IoList_containsIdenticalTo(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("containsIdenticalTo(anObject)",
		   "Returns true if the receiver contains a value identical to anObject, otherwise returns false. ")
	*/

	IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
	return IOBOOL(self, List_contains_(DATA(self), v) != 0);
}

IoObject *IoList_capacity(IoList *self, IoObject *locals, IoMessage *m)
{
    /*#io
	docSlot("capacity", "Returns the number of potential elements the receiver can hold before it needs to grow.")
    */
    return IONUMBER(DATA(self)->memSize / sizeof(void *));
}

IoObject *IoList_size(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("size", "Returns the number of items in the receiver. ")
	*/

	return IONUMBER(List_size(DATA(self)));
}

IoObject *IoList_at(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("at(index)",
		   "Returns the value at index. Returns Nil if the index is out of bounds. ")
	*/

	int index = IoMessage_locals_intArgAt_(m, locals, 0);
	IoObject *v;
	/*IoList_checkIndex(self, m, 0, index, "Io List at");*/
	v = List_at_(DATA(self), index);
	return (v) ? v : IONIL(self);
}

IoObject *IoList_first(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("first(optionalSize)",
		   "Returns the first item or Nil if the list is empty.
If optionalSize is provided, that number of the first items in the list are returned. ")
	*/

	if (IoMessage_argCount(m) == 0)
	{
		IoObject *result = List_at_(DATA(self), 0);

		return result ? result : ((IoState *)IOSTATE)->ioNil;
	}
	else
	{
		int end = IoMessage_locals_intArgAt_(m, locals, 0);

		if (end <= 0)
		{
			return IoList_new(IOSTATE);
		}
		else
		{
			List *list = List_cloneSlice(DATA(self), 0, end - 1);
			return IoList_newWithList_(IOSTATE, list);
		}
	}
}

IoObject *IoList_last(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("last(optionalSize)",
		   "Returns the last item or Nil if the list is empty.
If optionalSize is provided, that number of the last items in the list are returned. ")
	*/

	if (IoMessage_argCount(m) == 0)
	{
		IoObject *result = List_at_(DATA(self), List_size(DATA(self))-1);
		return result ? result : ((IoState *)IOSTATE)->ioNil;
	}
	else
	{
		size_t size = IoList_rawSize(self);
		int start = size - IoMessage_locals_intArgAt_(m, locals, 0);
		List *list;

		if (start < 0)
		{
			start = 0;
		}

		list = List_cloneSlice(DATA(self), start, size);
		return IoList_newWithList_(IOSTATE, list);
	}
}

void IoList_sliceArguments(IoList *self, IoObject *locals, IoMessage *m, int *start, int *end)
{
        int size = IoList_rawSize(self);

	*start = IoMessage_locals_intArgAt_(m, locals, 0);
        if (*start < 0)
        {
            *start += size;
            if (*start < 0)
            {
                *start = 0;
            }
        }

	if (IoMessage_argCount(m) == 2)
	{
		*end = IoMessage_locals_intArgAt_(m, locals, 1);
                if (*end < 0)
                {
                    *end += size;
                }
                (*end)--;
	}
	else
	{
		*end = size;
	}
}

IoObject *IoList_slice(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("slice(startIndex, endIndex)",
		   "Returns a new string containing the subset of the
receiver from the startIndex to the endIndex. The endIndex argument
is optional. If not given, it is assumed to be the end of the string. ")
	*/

	List *list;
	int start, end;

        IoList_sliceArguments(self, locals, m, &start, &end);

        if (end < start)
        {
                return IoList_new(IOSTATE);
        }
        else
        {
            list = List_cloneSlice(DATA(self), start, end);
            return IoList_newWithList_(IOSTATE, list);
        }
}

IoObject *IoList_sliceInPlace(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("sliceInPlace(startIndex, endIndex)",
		   "Returns the receiver containing the subset of the
receiver from the startIndex to the endIndex. The endIndex argument
is optional. If not given, it is assumed to be the end of the string. ")
	*/

	int start, end;

        IoList_sliceArguments(self, locals, m, &start, &end);

        if (end < start)
        {
                List_removeAll(DATA(self));
        }
        else
        {
                List_sliceInPlace(DATA(self), start, end);
        }
	return self;
}

IoObject *IoList_each(IoList *self, IoObject *locals, IoMessage *m)
{
	IoState *state = IOSTATE;
	IoObject *result = IONIL(self);
	IoMessage *doMessage = IoMessage_rawArgAt_(m, 0);
	List *list = DATA(self);

	IoState_pushRetainPool(state);

	LIST_SAFEFOREACH(list, i, v,
		IoState_clearTopPool(state);
		result = IoMessage_locals_performOn_(doMessage, locals, (IoObject *)v);
		if (IoState_handleStatus(IOSTATE)) goto done;
	);

done:
	IoState_popRetainPoolExceptFor_(state, result);
	return result;
}


IoObject *IoList_foreach(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("foreach(optionalIndex, value, message)",
		   """Loops over the list values setting the specified index and
value slots and executing the message. Returns the result of the last
execution of the message. Example:
<pre>list(1, 2, 3) foreach(i, v, writeln(i, " = ", v))
list(1, 2, 3) foreach(v, writeln(v))</pre>
""")
	*/

	IoState *state = IOSTATE;
	IoObject *result = IONIL(self);
	IoSymbol *slotName = NULL;
	IoSymbol *valueName;
	IoMessage *doMessage;
	List *list = DATA(self);

	if (IoMessage_argCount(m) == 1)
	{
		return IoList_each(self, locals, m);
	}

	IoMessage_foreachArgs(m, self, &slotName, &valueName, &doMessage);

	IoState_pushRetainPool(state);

	if (slotName)
	{
		LIST_SAFEFOREACH(list, i, value,
			IoState_clearTopPool(state);
			IoObject_setSlot_to_(locals, slotName, IONUMBER(i));
			IoObject_setSlot_to_(locals, valueName, (IoObject *)value);
			result = IoMessage_locals_performOn_(doMessage, locals, locals);
			if (IoState_handleStatus(IOSTATE)) goto done;
		);
	}
	else
	{
		LIST_SAFEFOREACH(list, i, value,
				   IoState_clearTopPool(state);
				   IoObject_setSlot_to_(locals, valueName, (IoObject *)value);
				   result = IoMessage_locals_performOn_(doMessage, locals, locals);
				   if (IoState_handleStatus(IOSTATE)) goto done;
				   );
	}

done:
		IoState_popRetainPoolExceptFor_(state, result);
	return result;
}

IoObject *IoList_reverseForeach(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("reverseForeach(index, value, message)", "Same as foreach, but in reverse order.")
	*/

	IoState *state = IOSTATE;
	IoObject *result = IONIL(self);
	IoSymbol *slotName, *valueName;
	IoMessage *doMessage;
	int i;

	IoMessage_foreachArgs(m, self, &slotName, &valueName, &doMessage);

	IoState_pushRetainPool(state);

	for (i = List_size(DATA(self)) - 1; i >= 0; i --)
	{
		IoState_clearTopPool(state);
		{
			IoObject *value = (IoObject *)LIST_AT_(DATA(self), i);

			if (slotName)
			{
				IoObject_setSlot_to_(locals, slotName, IONUMBER(i));
			}

			IoObject_setSlot_to_(locals, valueName, value);
			result = IoMessage_locals_performOn_(doMessage, locals, locals);

			if (IoState_handleStatus(IOSTATE))
			{
				goto done;
			}
		}
		if(i > List_size(DATA(self)) - 1) { i = List_size(DATA(self)) - 1; }
	}
done:
		IoState_popRetainPoolExceptFor_(state, result);
	return result;
}

// mutable --------------------------------------------------------

IoObject *IoList_appendIfAbsent(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("appendIfAbsent(anObject)",
		   "Adds each value not already contained by the receiver, returns self. ")
	*/

	int n;

	for (n = 0; n < IoMessage_argCount(m); n ++)
	{
		IoObject *v = IoMessage_locals_valueArgAt_(m, locals, n);

		if (IoList_rawIndexOf_(self, v) == -1)
		{
			IoState_stackRetain_(IOSTATE, v);
			List_append_(DATA(self), IOREF(v));
		}
	}

	return self;
}

IoObject *IoList_appendSeq(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("appendSeq(aList1, aList2, ...)",
		   "Add the items in the lists to the receiver. Returns self.")
	*/

	int i;

	for (i = 0; i < IoMessage_argCount(m); i ++)
	{
		IoObject *other = IoMessage_locals_valueArgAt_(m, locals, i);

		IOASSERT(ISLIST(other), "requires List objects as arguments");

		if (other == self)
		{
			IoState_error_(IOSTATE, m, "can't add a list to itself\n");
		}
		else
		{
			List *selfList  = DATA(self);
			List *otherList = DATA(other);
			int i, max = List_size(otherList);

			for (i = 0; i < max; i ++)
			{
				IoObject *v = List_at_(otherList, i);
				List_append_(selfList, IOREF(v));
			}
		}
	}
	return self;
}

IoObject *IoList_append(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("append(anObject1, anObject2, ...)",
		   """Appends the arguments to the end of the list. Returns self.""")
	*/
	/*#io
	docSlot("push(anObject1, anObject2, ...)",
		   "Same as add(anObject1, anObject2, ...).")
	*/

	int n;

	IOASSERT(IoMessage_argCount(m), "requires at least one argument");

	for (n = 0; n < IoMessage_argCount(m); n ++)
	{
		IoObject *v = IoMessage_locals_valueArgAt_(m, locals, n);
		List_append_(DATA(self), IOREF(v));
	}

	return self;
}

IoObject *IoList_prepend(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("prepend(anObject1, anObject2, ...)",
		   "Inserts the values at the beginning of the list. Returns self.")
	*/

	int n;

	IOASSERT(IoMessage_argCount(m), "requires at least one argument");

	for (n = 0; n < IoMessage_argCount(m); n ++)
	{
		IoObject *v = IoMessage_locals_valueArgAt_(m, locals, n);
		List_at_insert_(DATA(self), 0, IOREF(v));
	}

	return self;
}


IoObject *IoList_remove(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("remove(anObject, ...)",
		   "Removes all occurrences of the arguments from the receiver. Returns self. ")
	*/

	int count = IoMessage_argCount(m);
	int j;

	IOASSERT(count, "requires at least one argument");

	for (j = 0; j < count; j++)
	{
		IoObject *v = IoMessage_locals_valueArgAt_(m, locals, j);

		// a quick pass to remove values with equal pointers
		List_remove_(DATA(self), v);

		// slow pass to remove values that match comparision test
		for (;;)
		{
			int i = IoList_rawIndexOf_(self, v);

			if (i == -1)
			{
				break;
			}

			List_removeIndex_(DATA(self), i);
		}
	}

	return self;
}

IoObject *IoList_pop(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("pop",
		   "Returns the last item in the list and removes it
from the receiver. Returns nil if the receiver is empty. ")
	*/

	IoObject *v = List_pop(DATA(self));
	return (v) ? v : IONIL(self);
}

IoObject *IoList_atInsert(IoList *self, IoObject *locals, IoMessage *m)
{
	/*
	 docSlot("atInsert(index, anObject)",
		    "Inserts anObject at the index specified by index.
	 Adds anObject if the index equals the current count of the receiver.
	 Raises an exception if the index is out of bounds. Returns self. ")
	 */

	int index = IoMessage_locals_intArgAt_(m, locals, 0);
	IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 1);

	IoList_checkIndex(self, m, 1, index, "List atInsert");
	List_at_insert_(DATA(self), index, IOREF(v));
	return self;
}

IoObject *IoList_removeAt(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("removeAt(index)",
		   "Removes the item at the specified index and returns the value removed.
Raises an exception if the index is out of bounds. ")
	*/

	int index = IoMessage_locals_intArgAt_(m, locals, 0);
	IoObject *v = List_at_(DATA(self), index);

	IoList_checkIndex(self, m, 0, index, "Io List atInsert");
	List_removeIndex_(DATA(self), index);
	return (v) ? v : IONIL(self);
}

void IoList_rawAtPut(IoList *self, int i, IoObject *v)
{
	while (List_size(DATA(self)) < i) /* not efficient */
	{
		List_append_(DATA(self), IONIL(self));
	}

	List_at_put_(DATA(self), i, IOREF(v));
}

IoObject *IoList_atPut(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("atPut(index, anObject)",
		   "Replaces the existing value at index with anObject.
Raises an exception if the index is out of bounds. Returns self.")
	*/

	int index = IoMessage_locals_intArgAt_(m, locals, 0);
	IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 1);

	IoList_checkIndex(self, m, 0, index, "Io List atPut");
	IoList_rawAtPut(self, index, v);
	return self;
}

IoObject *IoList_setSize(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setSize", "Sets the size of the receiver by either removing excess items or adding nils as needed.")
	*/
	List *list = DATA(self);
	size_t newSize = IoMessage_locals_sizetArgAt_(m, locals, 0);
	size_t oldSize =  List_size(list);
	
	if(newSize < oldSize)
	{
		List_setSize_(list, newSize);
	}
	else
	{
		size_t i, max = newSize - oldSize;
		IoObject *nilObject = IONIL(self);
		
		for(i = 0; i < max; i ++)
		{
			List_append_(list, nilObject);
		}
	}
	
	return self;
}

IoObject *IoList_removeAll(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("empty", "Removes all items from the receiver.")
	*/

	List_removeAll(DATA(self));
	return self;
}

IoObject *IoList_swapIndices(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("swapIndices(index1, index2)",
		   "Exchanges the object at index1 with the object at index2.
Raises an exception if either index is out of bounds. Returns self.")
	*/

	int i = IoMessage_locals_intArgAt_(m, locals, 0);
	int j = IoMessage_locals_intArgAt_(m, locals, 1);

	IoList_checkIndex(self, m, 0, i, "List swapIndices");
	IoList_checkIndex(self, m, 0, j, "List swapIndices");
	List_swap_with_(DATA(self), i, j);
	return self;
}

IoObject *IoList_reverse(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("reverse",
		   "Reverses the ordering of all the items in the receiver. Returns self.")
	*/

	List_reverse(DATA(self));
	return self;
}

IoObject *IoList_preallocateToSize(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("preallocateToSize(aNumber)",
		   "Pio_reallocate array memory to hold aNumber number of items.")
	*/

	int newSize = IoMessage_locals_intArgAt_(m, locals, 0);
	List_preallocateToSize_(DATA(self), newSize);
	return self;
}

// sorting -----------------------------------------------

typedef struct
{
	IoState *state;
	IoObject *locals;
	IoMessage *exp;
	List *list;
} MSortContext;

int MSortContext_compareForSort(MSortContext *self, int i, int j)
{
	IoObject *a = List_at_(self->list, i);
	IoObject *b = List_at_(self->list, j);
	int r;

	IoState_pushRetainPool(self->state);

	a = IoMessage_locals_performOn_(self->exp, self->locals, a);
	b = IoMessage_locals_performOn_(self->exp, self->locals, b);
	r = IoObject_compare(a, b);

	IoState_popRetainPool(self->state);
	return r;
}

void MSortContext_swapForSort(MSortContext *self, int i, int j)
{
	List_swap_with_(self->list, i, j);
}

IoObject *IoList_sortInPlace(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("sortInPlace(optionalExpression)",
		   "Sorts the list using the compare method on the items. Returns self.
If an optionalExpression is provided, the sort is done on the result of the evaluation
of the optionalExpression on each value.")
	*/

	if (IoMessage_argCount(m) == 0)
	{
		List_qsort(DATA(self), (ListSortCallback *)IoObject_sortCompare);
	}
	else
	{
		MSortContext sc;
		MSortContext *sortContext = &sc;
		sortContext->state = IOSTATE;

		sortContext->list = DATA(self);
		sortContext->locals = locals;
		sortContext->exp = IoMessage_rawArgAt_(m, 0);

		Sorting_context_comp_swap_size_type_(sortContext,
										(SDSortCompareCallback *)MSortContext_compareForSort,
										(SDSortSwapCallback *)MSortContext_swapForSort,
										List_size(DATA(self)), SDQuickSort);

	}

	return self;
}

typedef struct
{
	IoState *state;
	IoObject *locals;
	IoBlock *block;
	IoMessage *blockMsg;
	IoMessage *argMsg1;
	IoMessage *argMsg2;
	List *list;
} SortContext;

int SortContext_compareForSort(SortContext *self, int i, int j)
{
	IoObject *cr;
	IoState_pushRetainPool(self->state);

	IoMessage_cachedResult_(self->argMsg1, LIST_AT_(self->list, i));
	IoMessage_cachedResult_(self->argMsg2, LIST_AT_(self->list, j));
	cr = IoBlock_activate(self->block, self->locals, self->locals, self->blockMsg, self->locals);
	//cr = IoMessage_locals_performOn_(self->block->message, self->locals, self->locals);

	IoState_popRetainPool(self->state);
	return ISFALSE(cr) ? 1 : -1;
}

void SortContext_swapForSort(SortContext *self, int i, int j)
{
	List_swap_with_(self->list, i, j);
}

IoObject *IoList_sortInPlaceBy(IoList *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("sortBy(aBlock)",
		   "Sort the list using aBlock as the compare function. Returns self.")
	*/

	SortContext sc;
	SortContext *sortContext = &sc;
	sortContext->state = IOSTATE;

	sortContext->list = DATA(self);
	sortContext->locals = locals;
	sortContext->block = IoMessage_locals_blockArgAt_(m, locals, 0);
	sortContext->blockMsg = IoMessage_new(IOSTATE);
	sortContext->argMsg1  = IoMessage_new(IOSTATE);
	sortContext->argMsg2  = IoMessage_new(IOSTATE);

	IoMessage_addArg_(sortContext->blockMsg, sortContext->argMsg1);
	IoMessage_addArg_(sortContext->blockMsg, sortContext->argMsg2);

	Sorting_context_comp_swap_size_type_(sortContext,
									(SDSortCompareCallback *)SortContext_compareForSort,
									(SDSortSwapCallback *)SortContext_swapForSort,
									List_size(DATA(self)), SDQuickSort);

	return self;
}
