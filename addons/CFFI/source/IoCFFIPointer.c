//metadoc CFFIPointer copyright 2006 Trevor Fancher. All rights reserved.
//metadoc CFFIPointer license BSD revised
//metadoc CFFIPointer category Bridges
//metadoc CFFIPointer description An Io interface to C

// docDependsOn("CFFIDataType")

#include "IoCFFIPointer.h"
#include "IoCFFIDataType.h"
#include "IoCFFIFunction.h"
#include "IoCFFIStructure.h"
#include "IoCFFIArray.h"
#include "IoMap.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "UArray.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h> // exit

#define DATA(self) ((IoCFFIPointerData *)(IoObject_dataPointer(self)))
#define POINTER(data) ((data)->valuePointer + (data)->valueOffset)

static const char *protoId = "Pointer";

IoTag *IoCFFIPointer_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCFFIPointer_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCFFIPointer_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCFFIPointer_mark);
	return tag;
}

IoCFFIPointer *IoCFFIPointer_proto(void *state)
{
	IoObject *self = IoCFFIDataType_new(state);
	IoObject_tag_(self, IoCFFIPointer_newTag(state));

	IoObject_setSlot_to_(self, IOSYMBOL("pointers"), IoMap_new(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCFFIPointerData)));
	memset(DATA(self), 0, sizeof(IoCFFIPointerData));
	DATA(self)->valuePointer = &(DATA(self)->ptr);

	IoState_registerProtoWithFunc_(state, self, IoCFFIPointer_proto);

	{
		IoMethodTable methodTable[] = {
			{"address", IoCFFIPointer_address},
			{"asBuffer", IoCFFIPointer_asBuffer},
			{"at", IoCFFIPointer_at},
			{"atPut", IoCFFIPointer_atPut},
			{"cast", IoCFFIPointer_cast},
			{"value", IoCFFIPointer_value},
			{"setValue", IoCFFIPointer_setValue},
			{"size", IoCFFIPointer_size},
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
	memset(DATA(self), 0, sizeof(IoCFFIPointerData));
	DATA(self)->valuePointer = &(DATA(self)->ptr);
	return self;
}

IoCFFIPointer *IoCFFIPointer_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoCFFIPointer_free(IoCFFIPointer *self)
{
	IoCFFIPointerData *data = DATA(self);
	free(data);
}

void IoCFFIPointer_mark(IoCFFIPointer *self)
{
	IoObject_shouldMarkIfNonNull(DATA(self)->keepRef);
}

/* ---------------------------------------------------------------- */

IoCFFIPointer *IoCFFIPointer_ToType_(IoObject *type)
{
	IoObject *pointer, *self;
	IoMap *pointers;
	IoSymbol *key;

	// this is a hack so macros relying on self will work
	self = type;

	pointers = IoObject_getSlot_(IoState_protoWithId_(IOSTATE, protoId), IOSYMBOL("pointers"));
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
	if ( *(DATA(self)->valuePointer) ) {
		char str[64] = {0};
		snprintf(str, 64, "%p", *(DATA(self)->valuePointer));
		return IOSYMBOL(str);
	}
	else {
		return IONIL(self);
	}
}

IoObject *IoCFFIPointer_value(IoCFFIPointer *self, IoObject *locals, IoMessage *m)
{
	IoObject *pointedToType;
	IoCFFIPointer *pointer;
	char *typeString, *cp, c;

	if (*(DATA(self)->valuePointer) == NULL)
	{
		//IoState_error_(IOSTATE, m, "attempt to dereference NULL pointer");
		return IONIL(self);
	}

	typeString = CSTRING(IoState_on_doCString_withLabel_(IOSTATE, self, "typeString", "IoCFFIPointer_value"));

	pointedToType = IoObject_getSlot_(self, IOSYMBOL("pointedToType"));

	if ( ISCFFIPointer(pointedToType) )
	{
		// we are a pointer to a pointer, so return a new Pointer
		// that points to the address of our ptr dereferenced

		cp = strrchr(typeString, '^');
		switch (c = *(++cp))
		{

#define IoCFFIPointer_value_SET_DATA_PTR(cType) \
	pointer = IOCLONE(IoCFFIPointer_ToType_(IoObject_getSlot_(pointedToType, IOSYMBOL("pointedToType")))); \
	*(DATA(pointer)->valuePointer) = *((cType **)(*(DATA(self)->valuePointer))); break
	//DATA(pointer)->ptr = *((cType **)(DATA(self)->ptr)); break

			case 'c':
			case 'b':
				IoCFFIPointer_value_SET_DATA_PTR(char);
			case 'C':
			case 'B':
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
			case '*':
				IoCFFIPointer_value_SET_DATA_PTR(char *);
			case '{':
			case '(':
			case '&':
			case '[':
			case 'v':
				IoCFFIPointer_value_SET_DATA_PTR(void);

#undef IoCFFIPointer_value_SET_DATA_PTR

			//case 'v':
			//	IoState_error_(IOSTATE, m, "attempt to dereference a void pointer");
			//	return IONIL(self);

			default:
				IoState_error_(IOSTATE, m, "unknown character '%c' in Pointer typeString", c);
				return IONIL(self);
			}
			return pointer;
	}
	else if( ISCFFIDataType(pointedToType) || ISCFFIStructure(pointedToType) || ISCFFIFunction(pointedToType) || ISCFFIArray(pointedToType) )
	{
		return IoCFFIDataType_objectFromData_(pointedToType, *(DATA(self)->valuePointer));
	}
    else
	{
		IoState_error_(IOSTATE, m, "attempt to dereference Pointer object whose pointedToType is unknown.");
		return IONIL(self);
	}
}


/*
	!!!!! IMPORTANT !!!!!
	
	pointer_setValue expects a value one indirection level less than self

*/
IoObject *IoCFFIPointer_setValue(IoCFFIPointer *self, IoObject *locals, IoMessage *m)
{
	IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);

	return IoCFFIPointer_rawSetValue(self, v, IoCFFIDataType_ValuePointerFromObject_(self, v));
}

IoObject *IoCFFIPointer_toType(IoCFFIPointer *self, IoObject *locals, IoMessage *m)
{
	IoObject *type;

	type = IoMessage_locals_valueArgAt_(m, locals, 0);
	return IoCFFIPointer_ToType_(type);
}

IoObject *IoCFFIPointer_at(IoCFFIPointer *self, IoObject *locals, IoMessage *m)
{
	int pos;
	IoObject *pointedToType;
	char *ptr;

	if(*(DATA(self)->valuePointer) == NULL) {
		IoState_error_(IOSTATE, m, "cannot read from a NULL pointer");
	}
	else {
		pos = CNUMBER(IoMessage_locals_numberArgAt_(m, locals, 0));
		pointedToType = IoObject_getSlot_(self, IOSYMBOL("pointedToType"));
		ptr = ((char *)*(DATA(self)->valuePointer)) + (IoCFFIDataType_ffiType(pointedToType)->size * pos);
		return IoCFFIDataType_objectFromData_(pointedToType, (void *)ptr);
	}
	return IONIL(self);
}

IoObject *IoCFFIPointer_atPut(IoCFFIPointer *self, IoObject *locals, IoMessage *m)
{
	int pos;
	IoObject *value, *pointedToType, *d;
	char *ptr;

	if(*(DATA(self)->valuePointer) == NULL) {
		IoState_error_(IOSTATE, m, "cannot write to a NULL pointer");
	}
	else {
		//TODO comprobar overrun y coincidencia de tipos
		pos = CNUMBER(IoMessage_locals_numberArgAt_(m, locals, 0));
		value = IoMessage_locals_valueArgAt_(m, locals, 1);

		pointedToType = IoObject_getSlot_(self, IOSYMBOL("pointedToType"));
		ptr = ((char *)*(DATA(self)->valuePointer)) + (IoCFFIDataType_ffiType(pointedToType)->size * pos);

		// We will have the GC problem here again, surely... should keep track of the cloned pointedToType...
		d = IOCLONE(pointedToType);
		IoCFFIDataType_rawSetValue(d, value);
		memcpy(ptr, (void *)IoCFFIDataType_ValuePointerFromObject_(self, d), IoCFFIDataType_ffiType(pointedToType)->size);
	}
	
	return self;
}

IoObject *IoCFFIPointer_asBuffer(IoCFFIPointer *self, IoObject *locals, IoMessage *m)
{
	return IoCFFIDataType_asBuffer(self, locals, m);
}

IoObject *IoCFFIPointer_cast(IoCFFIPointer *self, IoObject *locals, IoMessage *m)
{
	// obj cast(to)
	// Pointer cast(obj, to)
	IoObject *from, *toType, *o;
	int count;

	count = IoMessage_argCount(m);
	if ( 1 == count ) {
		from = self;
		toType = IoMessage_locals_valueArgAt_(m, locals, 0);
	}
	else if ( 2 == count ) {
		from = IoMessage_locals_valueArgAt_(m, locals, 0);
		toType = IoMessage_locals_valueArgAt_(m, locals, 1);
	}
	else {
		IoState_error_(IOSTATE, m, "Wrong number of arguments");
		return IONIL(self);
	}
	
	o = IoState_on_doCString_withLabel_(IOSTATE, toType, "?typeString", "IoCFFIPointer_castTo");

	if ( !ISNIL(o) ) {
		char *typeStr = CSTRING(o);

		switch ( typeStr[0] ) {
			case '^':
				toType = IOCLONE(toType);
				//IoCFFIDataType_rawSetValue(toType, self); //same indirection levels...
				*(DATA(toType)->valuePointer) = *((void **)IoCFFIDataType_ValuePointerFromObject_(toType, from));
				DATA(toType)->keepRef = IOREF(from);
				break;
			case '*':
				toType = IOCLONE(toType);
				IoCFFIDataType_rawSetValue(toType, self);
				break;
			default:
				IoState_error_(IOSTATE, m, "Wrong type to cast to.");
				return IONIL(self);
		}

		return toType;
	}
	else {
		// Mm... well, if the type to cast to does not have a typeString slot,
		// it should be an Io Object, so the address stored here is a pointer to an
		// Io Object. Simply cast the pointer and return it... dangerous but...

		IoObject *obj = (IoObject *)*(DATA(self)->valuePointer);
		if( ISOBJECT(obj) )
			return (IoObject *)*(DATA(self)->valuePointer);
	}

	return IONIL(self);
}

IoObject *IoCFFIPointer_size(IoCFFIPointer *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(ffi_type_pointer.size);
}

/********************************************/

IoCFFIPointer *IoCFFIPointer_rawSetValue(IoCFFIPointer *self, IoObject *source, void *value)
{
	IoSeq *typeStringSelf, *typeStringValue;

	typeStringSelf = IoState_on_doCString_withLabel_(IOSTATE, self, "typeString", "IoCFFIPointer_value");
	typeStringValue = IoState_on_doCString_withLabel_(IOSTATE, source, "?typeString", "IoCFFIPointer_value");
	if ( ISNIL(typeStringValue) ) {
		IoState_error_(IOSTATE, NULL, "value is not a CFFI object");
		return IONIL(self);
	}
	else {
		char *c_self = CSTRING(typeStringSelf) + 1;
		char *c_value = CSTRING(typeStringValue);

		if ( 0 != strncmp(c_self, c_value, strlen(c_self)) ) {
			IoState_error_(IOSTATE, NULL, "expected a type %s and got a type %s", c_self, c_value);
			return IONIL(self);
		}
		else {
			*(DATA(self)->valuePointer) = value;
			// The GC problem has to do with the fact that Pointer objects (and
			// CFFI types that takes the data pointer and throw away the owner of
			// the pointer, ie: Array types) should keep a reference to the owner
			// of the pointer.
			DATA(self)->keepRef = IOREF(source);
		}
	}

	return self;
}

IoCFFIPointer *IoCFFIPointer_cloneWithData(IoCFFIPointer *self, void **data)
{
	IoCFFIPointer *new = IOCLONE(self);
	*(DATA(new)->valuePointer) = *data;
	return new;
}

void *IoCFFIPointer_valuePointer(IoCFFIPointer *self)
{
	return DATA(self)->valuePointer;
}

//Func called when we are a member of a Struct
void IoCFFIPointer_setValuePointer_offset_(IoCFFIPointer* self, void *ptr, int offset)
{
	DATA(self)->valuePointer = (void **)((char *)ptr + offset);
}
