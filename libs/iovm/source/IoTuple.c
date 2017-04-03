//
// Created by michaelpollind on 3/18/17.
//

#include <UArray.h>
#include "IoTuple.h"
#include "IoNumber.h"
#include "IoObject.h"
#include "IoState.h"
#include "IoList.h"
#include "IoSeq_immutable.h"
#include "UArray.h"

static const char *protoId = "Tuple";

#define DATA(self) ((UArray*)(IoObject_dataPointer(self)))


IoTag *IoTuple_newTag(void *state)
{
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoTuple_free);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoTuple_rawClone);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoTuple_mark);
    IoTag_compareFunc_(tag, (IoTagCompareFunc *)IoTuple_compare);
    return tag;
}

IoTuple *IoTuple_proto(void *state)
{
    IoMethodTable methodTable[] ={
            {"set", IoTuple_set},
            {"toList", IoTuple_toList},
            {"at",IoTuple_at},
            {"size",IoTuple_size},
            {"slice",IoTuple_slice},
            {"empty",IoTuple_empty},

            //operator
            {"+",IoTuple_concat},
            {NULL,NULL}
    };

    IoObject *self = IoObject_new(state);
    IoObject_tag_(self,IoTuple_newTag(state));

    IoObject_setDataPointer_(self,UArray_new());
    IoState_registerProtoWithId_((IoState *)state,self,protoId);

   // IoSeq_addImmutableMethods(state);
    IoObject_addMethodTable_(self,methodTable);
    return  self;

}

int IoTuple_compare(IoObject *self, IoObject *otherObject)
{
    if(!ISTUPLE(otherObject))
    {
        return IoObject_defaultCompare(self,otherObject);
    } else
    {
        size_t s1 =  UArray_size(DATA(self));
        size_t s2 =  UArray_size(DATA(otherObject));
        size_t i;

        if (s1 != s2)
        {
            return 0;
        }

        for (i = 0; i < s1; i ++)
        {

            IoObject *v1 = UArray_pointerAt_(DATA(self),i);
            IoObject *v2 = UArray_pointerAt_(DATA(otherObject),i);
            int c = IoObject_compare(v1, v2);

            if (c)
            {
                return c;
            }
        }
    }
    return 0;
}


IoTuple *IoTuple_rawClone(IoTuple *proto)
{
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_tag_(self, IoObject_tag(proto));
    IoObject_setDataPointer_(self, UArray_clone(DATA(proto)));
    return self;

}

IOVM_API IoTuple *IoTuple_new(void *state)
{
    IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

void IoTuple_free(IoTuple *self)
{
    if (NULL == DATA(self))
    {
        printf("IoTuple_free(%p) already freed\n", (void *)self);
        exit(1);
    }
    //printf("IoList_free(%p) List_free(%p)\n", (void *)self, (void *)DATA(self));

    UArray_free(DATA(self));
    IoObject_setDataPointer_(self, NULL);
}

void IoTuple_mark(IoTuple *self)
{
    UARRAY_FOREACH(DATA(self), i, item, IoObject_shouldMark((uintptr_t)item));
}

IO_METHOD(IoTuple, size)
{

    /*doc Tuple size
    returns the number of elements inside a given Tuple
    */

    return IONUMBER(UArray_size(DATA(self)));
}

IO_METHOD(IoTuple, empty)
{
    /*doc Tuple empty
    Emptys the current tuple
    */

    UArray_free(DATA(self));
    IoObject_setDataPointer_(self,UArray_new());
    return  self;
}

IO_METHOD(IoTuple, set)
{
    /*doc Tuple set(..)
    Sets the current Tuple from a set of provided objects.
   */


    int n, argCount = (int)IoMessage_argCount(m);

    void** tuple = io_calloc((size_t)argCount,sizeof(void*));

    for (n = 0; n < argCount; ++n)
    {
        tuple[n] = IOREF(IoMessage_locals_valueArgAt_(m,locals,n));
    }

    UArray *ba = UArray_newWithData_type_encoding_size_copy_(tuple,CTYPE_uintptr_t,CENCODING_NUMBER,(size_t)argCount,0);

    IoObject_setDataPointer_(self, ba);
    IoObject_isDirty_(self,1);
    return self;
}

IO_METHOD(IoTuple,concat)
{
    /*doc +(Tuple)
      Returns a new Tuple where the previous and next are combined in series.
     */


    IOASSERT(IoMessage_argCount(m),"requires at least one argument");
    IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
    IOASSERT(ISTUPLE(other),"can only concatenate Tuple to Tuple");


    int n, argCount = IoMessage_argCount(m);

    size_t s1 = UArray_size(DATA(self));
    size_t s2 = UArray_size(DATA(other));

    IoTuple* ioTuple = IoTuple_new(IOSTATE);

    void** tuple = io_calloc((size_t)(s1+s2),sizeof(void*));

    for (size_t i = 0; i < s1; ++i) {
        tuple[i] = IOREF(UArray_pointerAt_(DATA(self),i));
    }

    for (size_t i = 0; i < s2; ++i) {
        tuple[i + s2] = IOREF(UArray_pointerAt_(DATA(other),i));
    }

    UArray *ba = UArray_newWithData_type_encoding_size_copy_(tuple,CTYPE_uintptr_t,CENCODING_NUMBER,(size_t)(s1+s2),0);
    IoTuple* out = (IoTuple*)IoTuple_new(IOSTATE);
    IoObject_setDataPointer_(out, ba);
    IoObject_isDirty_(out,1);
    return out;
}

IO_METHOD(IoTuple, at)
{
    /*doc Tuple at(index)
    Returns the value at index. Returns Nil if the index is out of bounds.
    */

    int index = IoMessage_locals_intArgAt_(m,locals,0);
    IoObject *v = UArray_pointerAt_(DATA(self),(size_t)index);
    return (v) ? v : IONIL(self);
}
IO_METHOD(IoTuple,toList)
{
    /*doc Tuple toList
    returns a new List from
    */

    IoState *state = IOSTATE;
    IoList* ioList = IoList_new(state);
    List* l = (List *)IoObject_dataPointer(ioList);

    UARRAY_FOREACH(DATA(self),i,v,List_append_(l,IOREF((uintptr_t)v)););
    IoObject_isDirty(ioList);
    return ioList;
}


IO_METHOD(IoTuple, slice)
{
    /*doc Tuple slice(startIndex, endIndex)
	Returns a new Tuple containing the subset of the receiver
    from the startIndex to the endIndex.
	*/

    long start = IoMessage_locals_longArgAt_(m, locals, 0);
    long end   = IoMessage_locals_longArgAt_(m, locals, 1);
    UArray* arr = UArray_slice(DATA(self),start,end);

    IoTuple* out = (IoTuple*)IoTuple_new(IOSTATE);
    IoObject_setDataPointer_(out, arr);
    IoObject_isDirty_(out,1);
    return out;
}