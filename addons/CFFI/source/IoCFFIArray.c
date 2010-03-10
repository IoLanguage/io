/* CFFI - An Io interface to C
Copyright (c) 2006 Trevor Fancher. All rights reserved.
All code licensed under the New BSD license.
*/
// docDependsOn("CFFIDataType")

#include "IoCFFIArray.h"
#include "IoCFFIDataType.h"
#include <ffi.h>

#define DATA(self) ((IoCFFIArrayData *)(IoObject_dataPointer(self)))

IoTag *IoCFFIArray_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Array");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCFFIArray_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCFFIArray_rawClone);
	//IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCFFIArray_mark);
	return tag;
}

IoCFFIArray *IoCFFIArray_proto(void *state)
{
	//IoObject *self = IoObject_new(state);
	IoObject *self = IoCFFIDataType_new(state);
	IoObject_tag_(self, IoCFFIArray_newTag(state));

	IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoCFFIArrayData)));
	memset(DATA(self), 0, sizeof(IoCFFIArrayData));
	DATA(self)->needToFreeBuffer = 0;

	IoState_registerProtoWithFunc_(state, self, IoCFFIArray_proto);
	{
		IoMethodTable methodTable[] = {
			{"address", IoCFFIArray_address},
			{"asBuffer", IoCFFIArray_asBuffer},
			{"at", IoCFFIArray_at},
			{"atPut", IoCFFIArray_atPut},
			{"size", IoCFFIArray_size},
			{"with", IoCFFIArray_with},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

void IoCFFIArray_mark(IoCFFIArray *self)
{
}

IoCFFIArray *IoCFFIArray_rawClone(IoCFFIArray *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoCFFIArrayData)));
	memset(DATA(self), 0, sizeof(IoCFFIArrayData));

	DATA(self)->ffiType = DATA(proto)->ffiType;
	DATA(self)->itemSize = DATA(proto)->itemSize;
	DATA(self)->arraySize = DATA(proto)->arraySize;
	DATA(self)->buffer = io_calloc(DATA(self)->arraySize, DATA(self)->itemSize);
	DATA(self)->needToFreeBuffer = 1;

	return self;
}

IoCFFIArray *IoCFFIArray_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoCFFIArray_proto);
	return IOCLONE(proto);
}

void IoCFFIArray_free(IoCFFIArray *self)
{
	IoCFFIArrayData *data;
	
	data = DATA(self);
	
	if (data->needToFreeBuffer) {
		io_free(data->buffer);
		data->buffer = NULL;
	}
	//TODO liberar ELEMENTS

	io_free(data);
}

/* ---------------------------------------------------------------- */

IoCFFIArray *IoCFFIArray_with(IoCFFIArray *self, IoObject *locals, IoMessage *m)
{
	IoCFFIDataType *type;
	int size, item_size, i;
	ffi_type *item_type;
	IoCFFIArray *o = IOCLONE(self);

	IoState_on_doCString_withLabel_(IoObject_state(o), o, "init", "IoCFFIArray_with");

	type = IOCLONE(IoMessage_locals_valueArgAt_(m, locals, 0));
	IoObject_setSlot_to_(o, IOSYMBOL("arrayType"), type);

	size = IoMessage_locals_intArgAt_(m, locals, 1);
	DATA(o)->arraySize = size;

	item_type = IoCFFIDataType_ffiType(type);
	item_size = item_type->size;

	DATA(o)->buffer = io_calloc(size, item_size);
	DATA(o)->needToFreeBuffer = 1;
	DATA(o)->itemSize = item_size;

	// Fake libffi to think we are a Struct
	DATA(o)->ffiType.size = 0;
	DATA(o)->ffiType.alignment = 0;
	DATA(o)->ffiType.type = FFI_TYPE_STRUCT;
	DATA(o)->ffiType.elements = io_calloc(size + 1, sizeof(ffi_type *));
	for (i = 0 ; i < size ; i++) {
		DATA(o)->ffiType.elements[i] = item_type;
	}
	DATA(o)->ffiType.elements[size] = NULL;

	ffi_cif cif;
	ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 0, &(DATA(o)->ffiType), NULL);

	return o;
}

IoObject *IoCFFIArray_address(IoCFFIArray *self, IoObject *locals, IoMessage *m)
{
	char str[64] = {0};

	snprintf(str, 64, "%p", DATA(self)->buffer);
	return IOSYMBOL(str);
}

IoObject *IoCFFIArray_asBuffer(IoCFFIArray *self, IoObject *locals, IoMessage *m)
{
	return IoCFFIDataType_asBuffer(self, locals, m);
}

IoCFFIArray *IoCFFIArray_at(IoCFFIArray *self, IoObject *locals, IoMessage *m)
{
	int pos;
	char *ptr;

	//TODO comprobar overrun
	pos = CNUMBER(IoMessage_locals_numberArgAt_(m, locals, 0));
	ptr = ((char *)DATA(self)->buffer) + (DATA(self)->itemSize * pos);
	return IoCFFIDataType_objectFromData_(IoObject_getSlot_(self, IOSYMBOL("arrayType")), (void *)ptr);
}

IoCFFIArray *IoCFFIArray_atPut(IoCFFIArray *self, IoObject *locals, IoMessage *m)
{
	int pos;
	IoObject *value, *d;
	char *ptr;

	//TODO comprobar overrun y coincidencia de tipos
	pos = CNUMBER(IoMessage_locals_numberArgAt_(m, locals, 0));
	value = IoMessage_locals_valueArgAt_(m, locals, 1);

	ptr = ((char *)DATA(self)->buffer) + (DATA(self)->itemSize * pos);

	d = IOCLONE(IoObject_getSlot_(self, IOSYMBOL("arrayType")));
	IoCFFIDataType_rawSetValue(d, value);
	memcpy(ptr, (void *)IoCFFIDataType_ValuePointerFromObject_(NULL, d), DATA(self)->itemSize);

	return self;
}

IoNumber *IoCFFIArray_size(IoCFFIArray *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->arraySize);
}

/* ---------------------------------------------------------------- */
IoCFFIArray* IoCFFIArray_cloneWithData(IoCFFIArray *self, void* data)
{
	//TODO excepcion si los tamaños no coinciden
	IoCFFIArray *new = IOCLONE(self);
	memcpy(DATA(new)->buffer, data, DATA(new)->itemSize * DATA(self)->arraySize);
	return new;
}

void *IoCFFIArray_valuePointer(IoCFFIArray *self)
{
	//Array es como Struct
	return DATA(self)->buffer;
}

ffi_type *IoCFFIArray_ffiType(IoCFFIArray *self)
{
	return &(DATA(self)->ffiType);
}

//Func called when we are a member of a Struct
void IoCFFIArray_setValuePointer_offset_(IoCFFIArray* self, void *ptr, int offset)
{
	if(DATA(self)->needToFreeBuffer) {
		io_free(DATA(self)->buffer);
		DATA(self)->needToFreeBuffer = 0;
	}

	DATA(self)->buffer = ptr + offset;
}
