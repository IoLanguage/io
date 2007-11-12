/* CFFI - An Io interface to C
   Copyright (c) 2006 Trevor Fancher. All rights reserved.
   All code licensed under the New BSD license.
 */

#ifndef IoCFFILibrary_DEFINED
#define IoCFFILibrary_DEFINED 1

#include "DynLib.h"
#include "IoObject.h"

#define ISCFFILibrary(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCFFILibrary_rawClone)

typedef IoObject IoCFFILibrary;

typedef struct IoCFFILibrary
{
	DynLib *library;
} IoCFFILibraryData;

IoTag *IoCFFILibrary_newTag(void *state);
IoCFFILibrary *IoCFFILibrary_proto(void *state);
IoCFFILibrary *IoCFFILibrary_rawClone(IoCFFILibrary *self);
void IoCFFILibrary_free(IoCFFILibrary *self);

void *IoCFFILibrary_rawGetFuctionPointer_(IoCFFILibrary *self, const char *name);

#endif
