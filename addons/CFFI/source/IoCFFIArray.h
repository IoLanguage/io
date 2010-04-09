//metadoc CFFIPointer copyright 2006 Trevor Fancher. All rights reserved.
//metadoc CFFIPointer license BSD revised
//metadoc CFFIPointer category Bridges
//metadoc CFFIPointer description An Io interface to C

#ifndef IoCFFIArray_DEFINED
#define IoCFFIArray_DEFINED 1

#include "IoObject.h"
#include "IoMessage.h"
#include "IoNumber.h"
#include <ffi.h>

#define ISCFFIArray(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCFFIArray_rawClone)

typedef IoObject IoCFFIArray;

typedef struct IoCFFIArray {
	void		*buffer;
	int		arraySize;
	ffi_type	ffiType;
	int		itemSize;
	int		needToFreeBuffer;
	int		needToFreeFFIType;
	IoObject	*keepRef;
	IoObject	**keepValuesRefs;
} IoCFFIArrayData;

IoTag		*IoCFFIArray_newTag(void *state);
IoCFFIArray	*IoCFFIArray_proto(void *state);
IoCFFIArray	*IoCFFIArray_rawClone(IoCFFIArray *self);
IoCFFIArray	*IoCFFIArray_new(void *state);
void		IoCFFIArray_free(IoCFFIArray *self);
void		IoCFFIArray_mark(IoCFFIArray *self);

IoObject	*IoCFFIArray_address(IoCFFIArray *self, IoObject *locals, IoMessage *m);
IoObject	*IoCFFIArray_asBuffer(IoCFFIArray *self, IoObject *locals, IoMessage *m);
IoCFFIArray	*IoCFFIArray_at(IoCFFIArray *self, IoObject *locals, IoMessage *m);
IoCFFIArray	*IoCFFIArray_atPut(IoCFFIArray *self, IoObject *locals, IoMessage *m);
IoCFFIArray	*IoCFFIArray_setValue(IoCFFIArray *self, IoObject *locals, IoMessage *m);
IoNumber	*IoCFFIArray_size(IoCFFIArray *self, IoObject *locals, IoMessage *m);
IoCFFIArray	*IoCFFIArray_with(IoCFFIArray *self, IoObject *locals, IoMessage *m);

IoCFFIArray	*IoCFFIArray_rawSetValue(IoCFFIArray *self, IoObject *source, void* data);
IoCFFIArray	*IoCFFIArray_cloneWithData(IoCFFIArray *self, void* data);
void		*IoCFFIArray_valuePointer(IoCFFIArray *self);
ffi_type	*IoCFFIArray_ffiType(IoCFFIArray *self);
void		IoCFFIArray_setValuePointer_offset_(IoCFFIArray* self, void *ptr, int offset);


#endif
