/* CFFI - An Io interface to C
   Copyright (c) 2006 Trevor Fancher. All rights reserved.
   All code licensed under the New BSD license.
 */

#ifndef IoCFFIDataType_DEFINED
#define IoCFFIDataType_DEFINED 1

#include "IoObject.h"
#include <ffi.h>

#define ISCFFIDataType(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCFFIDataType_rawClone)

typedef IoObject IoCFFIDataType;

typedef struct IoCFFIDataType
{
	union 
	{
	          char c;
	 unsigned char C;
	         short s;
	unsigned short S;
	           int i;
	  unsigned int I;
	          long l;
	 unsigned long L;
	         float f;
	        double d;
	       char *str;
	} type;
	int needToFreeStr;
} IoCFFIDataTypeData;

IoTag *IoCFFIDataType_newTag(void *state);
IoCFFIDataType *IoCFFIDataType_proto(void *state);
IoCFFIDataType *IoCFFIDataType_rawClone(IoCFFIDataType *self);
IoCFFIDataType *IoCFFIDataType_new(void *state);
void IoCFFIDataType_free(IoCFFIDataType *self);

void *IoCFFIDataType_ValuePointerFromObject_(IoObject *o);

IoCFFIDataType *IoCFFIDataType_ptr(IoCFFIDataType *self, IoObject *locals, IoMessage *m);
IoCFFIDataType *IoCFFIDataType_value(IoCFFIDataType *self, IoObject *locals, IoMessage *m);
IoCFFIDataType *IoCFFIDataType_setValue(IoCFFIDataType *self, IoObject *locals, IoMessage *m);

ffi_type *IoCFFIDataType_ffiType(IoCFFIDataType *self);
IoObject *IoCFFIDataType_objectFromData_(IoCFFIDataType *self, void *data);
void *IoCFFIDataType_valuePointer(IoCFFIDataType *self);

#endif
