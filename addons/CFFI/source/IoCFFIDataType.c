//metadoc CFFIDataType copyright 2006 Trevor Fancher. All rights reserved.
//metadoc CFFIDataType license BSD revised
//metadoc CFFIDataType category Binding
//metadoc CFFIDataType description An Io interface to C

#include "IoCFFIDataType.h"
#include "IoCFFIPointer.h"
//#include "IoCFFIStructure.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoObject.h"
#include "IoState.h"
#include <stdlib.h>
#include <string.h>
#include <ffi.h>

#define DATA(self) ((IoCFFIDataTypeData *)(IoObject_dataPointer(self)))

void *IoCFFIDataType_null = NULL;

IoTag *IoCFFIDataType_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("DataType");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCFFIDataType_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCFFIDataType_rawClone);
	return tag;
}

IoCFFIDataType *IoCFFIDataType_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCFFIDataType_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCFFIDataTypeData)));
	DATA(self)->needToFreeStr = 0;

	IoState_registerProtoWithFunc_(state, self, IoCFFIDataType_proto);

	{
		IoMethodTable methodTable[] = {
			{"value", IoCFFIDataType_value},
			{"setValue", IoCFFIDataType_setValue},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoCFFIDataType *IoCFFIDataType_rawClone(IoCFFIDataType *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCFFIDataTypeData)));
	return self;
}

IoCFFIDataType *IoCFFIDataType_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoCFFIDataType_proto);
	return IOCLONE(proto);
}

void IoCFFIDataType_free(IoCFFIDataType *self)
{
	IoCFFIDataTypeData *data;

	data = DATA(self);
	if (data->needToFreeStr)
	{
		free(data->type.str);
		data->needToFreeStr = 0;
	}
	free(DATA(self));
}

/* ---------------------------------------------------------------- */

void *IoCFFIDataType_ValuePointerFromObject_(IoObject *o)
{
	IoObject *self, *number;

	// this is a hack so macros relying on self will work
	self = o;

	if (ISNUMBER(o))
	{
		number = IoState_doCString_(IoObject_state(o), "CFFI Types Double clone");
		DATA(number)->type.d = IoObject_dataDouble(o);

		return IoCFFIDataType_ValuePointerFromObject_(number);
	}
	else if (ISSEQ(o))
	{
		return (char **)IoObject_dataPointer(o);
	}
	else if (ISNIL(o))
	{
		return &IoCFFIDataType_null;
	}
	else if(ISCFFIDataType(o))
	{
		return IoCFFIDataType_valuePointer(o);
	}
	else if(ISCFFIPointer(o))
	{
		return IoCFFIPointer_valuePointer(o);
	}
	else
		return NULL;
}

IoCFFIDataType *IoCFFIDataType_value(IoCFFIDataType *self, IoObject *locals, IoMessage *m)
{
	return IoCFFIDataType_objectFromData_(self, &(DATA(self)->type));
}

IoCFFIDataType *IoCFFIDataType_setValue(IoCFFIDataType *self, IoObject *locals, IoMessage *m)
{
	char c, *cp;
	IoCFFIDataTypeData *data;
	void *value;

	data = DATA(self);
	if (data->needToFreeStr)
	{
		free(data->type.str);
		data->needToFreeStr = 0;
	}

	value = IoCFFIDataType_ValuePointerFromObject_(IoMessage_locals_valueArgAt_(m, locals, 0));
	switch (c = CSTRING(IoState_on_doCString_withLabel_(IOSTATE, self, "typeString", "IoCFFIDataType_setValue"))[0])
	{
		case 'c':
			data->type.C = **((char **)value); break;
		case 'C':
			data->type.C = (unsigned char)**((char **)value); break;
		case 's':
			data->type.s = (short)(*(double *)value); break;
		case 'S':
			data->type.S = (unsigned short)(*(double *)value); break;
		case 'i':
			data->type.i = (int)(*(double *)value); break;
		case 'I':
			data->type.I = (unsigned int)(*(double *)value); break;
		case 'l':
			data->type.l = (long)(*(double *)value); break;
		case 'L':
			data->type.L = (unsigned long)(*(double *)value); break;
		case 'f':
			data->type.f = (float)(*(double *)value); break;
		case 'd':
			data->type.d = *(double *)value; break;
		case '*':
			cp = *(char **)value;
			data->type.str = malloc(strlen(cp) + 1);
			data->needToFreeStr = 1;
			strcpy(data->type.str, cp);
			break;

		case 'v':
			IoState_error_(IOSTATE, NULL, "attempt to setValue on void DataType");
			return IONIL(self);

		default:
			IoState_error_(IOSTATE, NULL, "unknown character '%c' in typeString", c);
			return IONIL(self);
	}
	return self;
}

IoObject *IoCFFIDataType_objectFromData_(IoCFFIDataType *self, void *data)
{
	char *typeString, c;
	IoCFFIPointer *pointer;

	typeString = CSTRING(IoState_on_doCString_withLabel_(IOSTATE, self, "typeString", "IoCFFIDataType_objectFromData_"));
	switch (c = typeString[0])
	{
		case 'c':
		case 'C':
			return IoSeq_newWithCString_length_(IOSTATE, (char *)data, 1);
		case 's':
			return IONUMBER(((double)(*((short *)data))));
		case 'S':
			return IONUMBER(((double)(*((unsigned short *)data))));
		case 'i':
			return IONUMBER(((double)(*((int *)data))));
		case 'I':
			return IONUMBER(((double)(*((unsigned int *)data))));
		case 'l':
			return IONUMBER(((double)(*((long *)data))));
		case 'L':
			return IONUMBER(((double)(*((unsigned long *)data))));
		case 'f':
			return IONUMBER(((double)(*((float *)data))));
		case 'd':
			return IONUMBER((*((double *)data)));
		case 'v':
			return IONIL(self);

		case '*':
			if (*(char **)data)
			{
				return IoSeq_newWithCString_(IOSTATE, *(char **)data);
			}
			else
			{
				return IoSeq_new(IOSTATE);
			}

		case '^':
			pointer = IOCLONE(self);
			((IoCFFIPointerData *)(IoObject_dataPointer(pointer)))->ptr = *(void **)data;
			return pointer;

		case '{':
		case '(':
			IoState_error_(IOSTATE, NULL, "structs and unions are not yet supported by CFFI");
			return IONIL(self);

		default:
			IoState_error_(IOSTATE, NULL, "unknown character '%c' in typeString", c);
			return IONIL(self);
	}
}

ffi_type *IoCFFIDataType_ffiType(IoCFFIDataType *self)
{
	char *typeString, c;

	typeString = CSTRING(IoState_on_doCString_withLabel_(IOSTATE, self, "typeString", "IoCFFIDataType_ffiType"));

	if (strlen(typeString) < 1)
	{
		return NULL;
	}

	switch (c = typeString[0])
	{
		case 'c':
			return &ffi_type_schar;
		case 'C':
			return &ffi_type_uchar;
		case 's':
			return &ffi_type_sshort;
		case 'S':
			return &ffi_type_ushort;
		case 'i':
			return &ffi_type_sint;
		case 'I':
			return &ffi_type_uint;
		case 'l':
			return &ffi_type_slong;
		case 'L':
			return &ffi_type_ulong;
		case 'f':
			return &ffi_type_float;
		case 'd':
			return &ffi_type_double;
		case 'v':
			return &ffi_type_void;

		case '*':
		case '^':
			return &ffi_type_pointer;

		//case '{':
		//	return IoCFFIStructure_ffiType(self);

		case '(':
			IoState_error_(IOSTATE, NULL, "unions are not yet supported by CFFI");
			return NULL;

		default:
			IoState_error_(IOSTATE, NULL, "unknown character '%c' in typeString", c);
			return NULL;
	}
}

void *IoCFFIDataType_valuePointer(IoCFFIDataType *self)
{
	char c, *typeString;
	IoCFFIDataTypeData *data;

	typeString = CSTRING(IoState_on_doCString_withLabel_(IOSTATE, self, "typeString", "IoCFFIDataType_valuePointer"));
	data = DATA(self);

	switch (c = typeString[0])
	{
		case 'c': return &(data->type.c);
		case 'C': return &(data->type.C);
		case 's': return &(data->type.s);
		case 'S': return &(data->type.S);
		case 'i': return &(data->type.i);
		case 'I': return &(data->type.I);
		case 'l': return &(data->type.l);
		case 'L': return &(data->type.L);
		case 'f': return &(data->type.f);
		case 'd': return &(data->type.d);
		case '*': return &(data->type.str);

		case 'v':
			IoState_error_(IOSTATE, NULL, "atempt to get data pointer from Void type");
			return NULL;

		default:
			IoState_error_(IOSTATE, NULL, "unknown character '%c' in typeString", c);
			return NULL;
	}
}

/*
switch (c)
{
	case 'c':
	case 'C':
	case 's':
	case 'S':
	case 'i':
	case 'I':
	case 'l':
	case 'L':
	case 'f':
	case 'd':
	case 'v':
	case '*':
	case '^':
	case '{':
	case '(':

	default:
		IoState_error_(IOSTATE, NULL, "unknown character '%c' in typeString", c);
		return NULL;
}
*/
