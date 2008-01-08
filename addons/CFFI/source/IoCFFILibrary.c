/* CFFI - An Io interface to C
   Copyright (c) 2006 Trevor Fancher. All rights reserved.
   All code licensed under the New BSD license.
 */

#include "IoCFFILibrary.h"
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

void *IoCFFILibrary_rawGetFuctionPointer_(IoCFFILibrary *self, const char *name)
{
	DynLib *library = DATA(self)->library;

	if (!library)
	{
		const char *name = CSTRING(IoObject_getSlot_(self, IOSYMBOL("name")));

		library = DATA(self)->library = DynLib_new();
		DynLib_setPath_(library, name);
		DynLib_open(library);
	}

	return DynLib_pointerForSymbolName_(library, name);
}
