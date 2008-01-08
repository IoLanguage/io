/* CFFI - An Io interface to C
   Copyright (c) 2006 Trevor Fancher. All rights reserved.
   All code licensed under the New BSD license.
 */

#include "IoCFFIFunction.h"
#include "IoCFFILibrary.h"
#include "IoCFFIDataType.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include "List.h"
#include <ffi.h>

#define DATA(self) ((IoCFFIFunctionData *)(IoObject_dataPointer(self)))

IoTag *IoCFFIFunction_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Function");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCFFIFunction_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCFFIFunction_rawClone);
	return tag;
}

IoCFFIFunction *IoCFFIFunction_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCFFIFunction_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCFFIFunctionData)));

	IoState_registerProtoWithFunc_(state, self, IoCFFIFunction_proto);

	{
		IoMethodTable methodTable[] = {
			{"call", IoCFFIFunction_call},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoCFFIFunction *IoCFFIFunction_rawClone(IoCFFIFunction *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCFFIFunctionData)));
	return self;
}

void IoCFFIFunction_free(IoCFFIFunction *self)
{
	free(DATA(self));
}

/* ---------------------------------------------------------------- */

IoObject *IoCFFIFunction_call(IoCFFIFunction *self, IoObject *locals, IoMessage *m)
{
	IoCFFILibrary *library;
	const char *funName;
	void *funPointer, **funArgVals, *funRetVal;
	ffi_type **funArgTypes, *funRetType;
	ffi_cif *funInterface;
	int funArgCount, i;
	ffi_status status;
	IoObject *returnValAsObj, *funRetTypeObject, *o;
	List *funArgTypeObjects;

	library = IoObject_getSlot_(self, IOSYMBOL("library"));
	funInterface = &(DATA(self)->interface);
	funName = CSTRING(IoObject_getSlot_(self, IOSYMBOL("name")));
	funPointer = IoCFFILibrary_rawGetFuctionPointer_(library, funName);
	funArgTypeObjects = IoList_rawList(IoObject_getSlot_(self, IOSYMBOL("argumentTypes")));
	funRetTypeObject = IoObject_getSlot_(self, IOSYMBOL("returnType"));

	funArgCount = (int)List_size(funArgTypeObjects);
	funArgTypes = calloc(funArgCount, sizeof(ffi_type *));
	for (i = 0; i < funArgCount; i++)
	{
		o = List_at_(funArgTypeObjects, i);
		funArgTypes[i] = IoCFFIDataType_ffiType(o);
	}
	funRetType = IoCFFIDataType_ffiType(funRetTypeObject);

	status = ffi_prep_cif(funInterface, FFI_DEFAULT_ABI, funArgCount, funRetType, funArgTypes);
	if (status != FFI_OK)
	{
		printf("\n\nUh oh.  Something went wrong in IoCFFIFunction_call.\n\n");
		free(funArgTypes);
		return IONIL(self);
	}

	funArgVals = calloc(funArgCount, sizeof(void *));
	funRetVal = calloc(1, funRetType->size);
	IoState_pushCollectorPause(IOSTATE);
	{
		for (i = 0; i < funArgCount; i++)
		{
			o = IoMessage_locals_valueArgAt_(m, locals, i);
			funArgVals[i] = IoCFFIDataType_ValuePointerFromObject_(o);
		}

		ffi_call(funInterface, funPointer, funRetVal, funArgVals);
		returnValAsObj = IoCFFIDataType_objectFromData_(funRetTypeObject, funRetVal);
	}
	IoState_popCollectorPause(IOSTATE);

	free(funArgTypes);
	free(funArgVals);
	free(funRetVal);

	return returnValAsObj;
}
