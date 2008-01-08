/* CFFI - An Io interface to C
   Copyright (c) 2006 Trevor Fancher. All rights reserved.
   All code licensed under the New BSD license.
 */

#include "IoCFFIPointer.h"
#include "IoCFFIDataType.h"
#include "IoMap.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "UArray.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h> // exit

#define DATA(self) ((IoCFFIPointerData *)(IoObject_dataPointer(self)))

IoTag *IoCFFIPointer_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Pointer");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCFFIPointer_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCFFIPointer_rawClone);
	return tag;
}

IoCFFIPointer *IoCFFIPointer_proto(void *state)
{
	IoObject *self = IoCFFIDataType_new(state);
	IoObject_tag_(self, IoCFFIPointer_newTag(state));

	IoObject_setSlot_to_(self, IOSYMBOL("pointers"), IoMap_new(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCFFIPointerData)));
	DATA(self)->ptr = NULL;

	IoState_registerProtoWithFunc_(state, self, IoCFFIPointer_proto);

	{
		IoMethodTable methodTable[] = {
			{"address", IoCFFIPointer_address},
			{"value", IoCFFIPointer_value},
			{"setValue", IoCFFIPointer_setValue},
			{"toType", IoCFFIPointer_toType},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoCFFIPointer *IoCFFIPointer_rawClone(IoCFFIPointer *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCFFIPointerData)));
	return self;
}

IoCFFIPointer *IoCFFIPointer_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoCFFIPointer_proto);
	return IOCLONE(proto);
}

void IoCFFIPointer_free(IoCFFIPointer *self)
{
	IoCFFIPointerData *data = DATA(self);

	free(data);
}

/* ---------------------------------------------------------------- */

IoCFFIPointer *IoCFFIPointer_ToType_(IoObject *type)
{
	IoObject *pointer, *self;
	IoMap *pointers;
	IoSymbol *key;

	// this is a hack so macros relying on self will work
	self = type;

	pointers = IoObject_getSlot_(IoState_protoWithInitFunction_(IOSTATE, IoCFFIPointer_proto), IOSYMBOL("pointers"));
	key = IoState_on_doCString_withLabel_(IOSTATE, type, "uniqueHexId", "IoCFFIPointer_ToType_");

	pointer = IoMap_rawAt(pointers, key);
	if (!pointer)
	{
		// create new pointer and add to cache
		pointer = IoCFFIPointer_new(IOSTATE);
		IoObject_setSlot_to_(pointer, IOSYMBOL("pointedToType"), type);

		IoMap_rawAtPut(pointers, key, pointer);
	}

	return pointer;
}

IoObject *IoCFFIPointer_address(IoCFFIPointer *self, IoObject *locals, IoMessage *m)
{
	char str[64] = {0};

	snprintf(str, 64, "%p", DATA(self)->ptr);
	return IOSYMBOL(str);
}

IoObject *IoCFFIPointer_value(IoCFFIPointer *self, IoObject *locals, IoMessage *m)
{
	IoObject *pointedToType;
	IoCFFIPointer *pointer;
	char *typeString, *cp, c;

	if (DATA(self)->ptr == NULL)
	{
		IoState_error_(IOSTATE, m, "attempt to dereference NULL pointer");
		return IONIL(self);
	}

	typeString = CSTRING(IoState_on_doCString_withLabel_(IOSTATE, self, "typeString", "IoCFFIPointer_value"));

	pointedToType = IoObject_getSlot_(self, IOSYMBOL("pointedToType"));
	if (ISCFFIPointer(pointedToType))
	{
		// we are a pointer to a pointer, so return a new Pointer
		// that points to the address of our ptr dereferenced

		cp = strrchr(typeString, '^');
		switch (c = *(++cp))
		{

#define IoCFFIPointer_value_SET_DATA_PTR(cType) \
	pointer = IOCLONE(IoCFFIPointer_ToType_(IoObject_getSlot_(pointedToType, IOSYMBOL("pointedToType")))); \
	DATA(pointer)->ptr = *((cType **)(DATA(self)->ptr)); break

			case 'c':
				IoCFFIPointer_value_SET_DATA_PTR(char);
			case 'C':
				IoCFFIPointer_value_SET_DATA_PTR(unsigned char);
			case 's':
				IoCFFIPointer_value_SET_DATA_PTR(short);
			case 'S':
				IoCFFIPointer_value_SET_DATA_PTR(unsigned short);
			case 'i':
				IoCFFIPointer_value_SET_DATA_PTR(int);
			case 'I':
				IoCFFIPointer_value_SET_DATA_PTR(unsigned int);
			case 'l':
				IoCFFIPointer_value_SET_DATA_PTR(long);
			case 'L':
				IoCFFIPointer_value_SET_DATA_PTR(unsigned long);
			case 'f':
				IoCFFIPointer_value_SET_DATA_PTR(float);
			case 'd':
				IoCFFIPointer_value_SET_DATA_PTR(double);

#undef IoCFFIPointer_value_SET_DATA_PTR

			case 'v':
				IoState_error_(IOSTATE, m, "attempt to dereference a void pointer");
				return IONIL(self);
			case '{':
			case '(':
				IoState_error_(IOSTATE, m, "structs and unions are not yet supported by CFFI");
				return IONIL(self);
			default:
				IoState_error_(IOSTATE, m, "unknown character '%c' in Pointer typeString", c);
				return IONIL(self);
			}
			return pointer;
	}
	else if(ISCFFIDataType(pointedToType))
	{
		return IoCFFIDataType_objectFromData_(pointedToType, DATA(self)->ptr);
	}
	else
	{
		IoState_error_(IOSTATE, m, "attempt to dereference Pointer object whose pointedToType was neither a Pointer or a DataType.");
		return IONIL(self);
	}
}

IoObject *IoCFFIPointer_setValue(IoCFFIPointer *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->ptr = IoCFFIDataType_ValuePointerFromObject_(IoMessage_locals_valueArgAt_(m, locals, 0));

	return self;
}

IoObject *IoCFFIPointer_toType(IoCFFIPointer *self, IoObject *locals, IoMessage *m)
{
	IoObject *type;

	type = IoMessage_locals_valueArgAt_(m, locals, 0);
	return IoCFFIPointer_ToType_(type);
}

void *IoCFFIPointer_valuePointer(IoCFFIPointer *self)
{
	return &(DATA(self)->ptr);
}
