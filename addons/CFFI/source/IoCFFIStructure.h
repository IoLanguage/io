//metadoc CFFIPointer copyright 2006 Trevor Fancher. All rights reserved.
//metadoc CFFIPointer license BSD revised
//metadoc CFFIPointer category Bridges
//metadoc CFFIPointer description An Io interface to C

#ifndef IoCFFIStructure_DEFINED
#define IoCFFIStructure_DEFINED 1

#include "IoObject.h"
#include "IoMap.h"
#include <ffi.h>

#define ISCFFIStructure(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCFFIStructure_rawClone)

typedef IoObject IoCFFIStructure;

typedef struct IoCFFIStructure {
	void		*buffer;
	int		needToFreeBuffer;
	ffi_type	ffiType;
	int		needToFreeElements;
	int		isUnion;
} IoCFFIStructureData;

IoTag		*IoCFFIStructure_newTag(void *state);
IoCFFIStructure	*IoCFFIStructure_proto(void *state);
IoCFFIStructure	*IoCFFIStructure_rawClone(IoCFFIStructure *self);
IoCFFIStructure	*IoCFFIStructure_new(void *state);
void		IoCFFIStructure_free(IoCFFIStructure *self);
void		IoCFFIStructure_mark(IoCFFIStructure *self);

IoObject	*IoCFFIStructure_address(IoCFFIStructure *self, IoObject *locals, IoMessage *m);
IoObject	*IoCFFIStructure_asBuffer(IoCFFIStructure *self, IoObject *locals, IoMessage *m);
IoCFFIStructure	*IoCFFIStructure_setMembers(IoCFFIStructure *self, IoObject *locals, IoMessage *m);
IoCFFIStructure	*IoCFFIStructure_setValues(IoCFFIStructure *self, IoObject *locals, IoMessage *m);
IoCFFIStructure	*IoCFFIStructure_with(IoCFFIStructure *self, IoObject *locals, IoMessage *m);

IoCFFIStructure	*IoCFFIStructure_cloneWithData(IoCFFIStructure *self, void* data);
void		IoCFFIStructure_setValuesFromData(IoCFFIStructure *self, void *data);
void		*IoCFFIStructure_valuePointer(IoCFFIStructure *self);
ffi_type	*IoCFFIStructure_ffiType(IoCFFIStructure *self);
void		IoCFFIStructure_setValuePointer_offset_(IoCFFIStructure* self, void *ptr, int offset);


#endif
