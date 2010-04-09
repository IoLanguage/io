//metadoc CFFIPointer copyright 2006 Trevor Fancher. All rights reserved.
//metadoc CFFIPointer license BSD revised
//metadoc CFFIPointer category Bridges
//metadoc CFFIPointer description An Io interface to C

#ifndef IoCFFILibrary_DEFINED
#define IoCFFILibrary_DEFINED 1

#include "DynLib.h"
#include "IoObject.h"

#define ISCFFILibrary(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCFFILibrary_rawClone)

typedef IoObject IoCFFILibrary;

typedef struct IoCFFILibrary {
	DynLib	*library;
} IoCFFILibraryData;

IoTag		*IoCFFILibrary_newTag(void *state);
IoCFFILibrary	*IoCFFILibrary_proto(void *state);
IoCFFILibrary	*IoCFFILibrary_rawClone(IoCFFILibrary *self);
void		IoCFFILibrary_free(IoCFFILibrary *self);

IoObject	*IoCFFILibrary_open(IoObject *self, IoObject *locals, IoMessage *m);
IoObject	*IoCFFILibrary_func(IoObject *self, IoObject *locals, IoMessage *m);


IoObject	*IoCFFILibrary_rawOpen(IoObject *self);
void		*IoCFFILibrary_rawGetFuctionPointer_(IoCFFILibrary *self, const char *name);

#endif
