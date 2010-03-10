/* CFFI - An Io interface to C
Copyright (c) 2006 Trevor Fancher. All rights reserved.
All code licensed under the New BSD license.
*/

#include "IoCFFILibrary.h"
#include "IoCFFIFunction.h"
#include <string.h>
#include "IoSeq.h"

#define DATA(self) ((IoCFFILibraryData *)(IoObject_dataPointer(self)))

IoTag *IoCFFILibrary_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Library");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCFFILibrary_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCFFILibrary_rawClone);
	return tag;
}

IoCFFILibrary *IoCFFILibrary_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCFFILibrary_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCFFILibraryData)));

	IoState_registerProtoWithFunc_(state, self, IoCFFILibrary_proto);

	{
		IoMethodTable methodTable[] = {
			{"open", IoCFFILibrary_open},
			{"func", IoCFFILibrary_func},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoCFFILibrary *IoCFFILibrary_rawClone(IoCFFILibrary *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCFFILibraryData)));
	return self;
}

void IoCFFILibrary_free(IoCFFILibrary *self)
{
	DynLib *library = DATA(self)->library;

	if (library && DynLib_isOpen(library))
	{
		DynLib_close(library);
		DynLib_free(library);
	}

	free(DATA(self));
}

/* ---------------------------------------------------------------- */

//OMF
IoObject *IoCFFILibrary_open(IoObject *self, IoObject *locals, IoMessage *m)
{
	return IoCFFILibrary_rawOpen(self);
}

//OMF
IoObject *IoCFFILibrary_func(IoObject *self, IoObject *locals, IoMessage *m)
{
	char *name = IoMessage_locals_cStringArgAt_(m, locals, 0);
	void *funcPtr = IoCFFILibrary_rawGetFuctionPointer_(self, name);

	IoCFFIFunction *cffiFunc = IoCFFIFunction_new(IOSTATE);

	//TODO abstraer esto	
	*(((IoCFFIFunctionData *)(IoObject_dataPointer(cffiFunc)))->valuePointer) = funcPtr;

	return cffiFunc;
}

/* ---------------------------------------------------------------- */

IoObject *IoCFFILibrary_rawOpen(IoObject *self)
{
	DynLib *library = DATA(self)->library;

	if (!library)
	{
		const char *name = CSTRING(IoObject_getSlot_(self, IOSYMBOL("name")));

		library = DATA(self)->library = DynLib_new();
		DynLib_setPath_(library, name);
		DynLib_open(library);
	}

	// TODO OJO! DynLib_hasError no tiene declaracion en dynlib.h
	if(DynLib_hasError(library)) return IONIL(self);
	
	return self;
}

void *IoCFFILibrary_rawGetFuctionPointer_(IoCFFILibrary *self, const char *name)
{
	if(ISNIL(IoCFFILibrary_rawOpen(self))) return NULL;
		
	DynLib *library = DATA(self)->library;

	void *func_pointer = DynLib_pointerForSymbolName_(library, name);
	if(NULL == func_pointer)
		IoState_error_(IOSTATE, NULL, "Function \"%s\" not found in library \"%s\"", name, CSTRING(IoObject_getSlot_(self, IOSYMBOL("name"))));

	return func_pointer;
}
