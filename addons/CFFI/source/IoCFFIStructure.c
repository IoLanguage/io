//metadoc CFFIPointer copyright 2006 Trevor Fancher. All rights reserved.
//metadoc CFFIPointer license BSD revised
//metadoc CFFIPointer category Bridges
//metadoc CFFIPointer description An Io interface to C

// docDependsOn("CFFIDataType")

#include "IoCFFIPointer.h"
#include "IoCFFIStructure.h"
#include "IoCFFIDataType.h"
#include "IoList.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoObject.h"
#include "IoState.h"
#include "IoMessage.h"
#include <stdlib.h>
#include <string.h>
#include <ffi.h>

#ifndef MS_WIN32
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

#define DATA(self) ((IoCFFIStructureData *)(IoObject_dataPointer(self)))

void *IoCFFIStructure_null = NULL;

IoTag *IoCFFIStructure_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Structure");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCFFIStructure_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCFFIStructure_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCFFIStructure_mark);
	return tag;
}

IoCFFIStructure *IoCFFIStructure_proto(void *state)
{
	IoObject *self = IoCFFIDataType_new(state);
	IoObject_tag_(self, IoCFFIStructure_newTag(state));

	IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoCFFIStructureData)));
	memset(DATA(self), 0, sizeof(IoCFFIStructureData));

	IoState_registerProtoWithFunc_(state, self, IoCFFIStructure_proto);
	{
		IoMethodTable methodTable[] = {
			{"address", IoCFFIStructure_address},
			{"asBuffer", IoCFFIStructure_asBuffer},
			{"setMembers", IoCFFIStructure_setMembers},
			{"setValues", IoCFFIStructure_setValues},
			{"with", IoCFFIStructure_with},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

void IoCFFIStructure_mark(IoCFFIStructure *self)
{
}


int IoCFFIStructure_calcOffset(int isUnion, ffi_type* type, int* nextOffset)
{
	int size, align, offset = *nextOffset;

	if ( isUnion ) {
		*nextOffset = 0;
		return 0;
	}

	size = type->size;
	align = type->alignment;
	if ( align && (offset % align) ) {
		offset += align - (offset % align); 
	}

	*nextOffset = offset + size;
	return offset;
}

IoCFFIStructure *IoCFFIStructure_rawClone(IoCFFIStructure *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoCFFIStructureData)));
	memset(DATA(self), 0, sizeof(IoCFFIStructureData));

	IoMap* proto_slots = IoObject_getSlot_(proto, IOSYMBOL("_members"));

	if ( !ISNIL(proto_slots) ) {
		IoCFFIStructureData* data = DATA(self);
		data->isUnion = DATA(proto)->isUnion;
		data->ffiType = DATA(proto)->ffiType;
		data->needToFreeElements = 0;

		// Here we reserve the buffer space. Possibly setValuePointer_offset_ will
		// free it later if self is member of another Structure.
		data->buffer = io_calloc(1, data->ffiType.size);
		memset(data->buffer, 0, data->ffiType.size);
		data->needToFreeBuffer = 1;

		// When a Structure is cloned a new buffer is created to hold the values, thus
		// we have to travel through all the types to tell them where to store their values.
		IoMap* members = IoMap_new(IOSTATE);
		IoObject_setSlot_to_(self, IOSYMBOL("_members"), members);
		LIST_FOREACH(IoList_rawList(IoMap_rawKeys(proto_slots)), i, k,
			{
			IoObject* d = IOCLONE(IoMap_rawAt(proto_slots, k));
			IoCFFIDataType_setValuePointer_(d, data->buffer);
			IoMap_rawAtPut(members, k, d);
			}
		);
	}

	return self;
}

IoCFFIStructure *IoCFFIStructure_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoCFFIStructure_proto);
	return IOCLONE(proto);
}

void IoCFFIStructure_free(IoCFFIStructure *self)
{
	IoCFFIStructureData *data;
	
	data = DATA(self);

	if ( data->needToFreeBuffer ) {
		io_free(data->buffer);
		data->needToFreeBuffer = 0;
	}
	
	if ( data->needToFreeElements ) {
		io_free(data->ffiType.elements[0]);
		io_free(data->ffiType.elements);
		data->needToFreeElements = 0;
	}

	io_free(data);
}

/* ---------------------------------------------------------------- */

IoCFFIStructure *IoCFFIStructure_with(IoCFFIStructure *self, IoObject *locals, IoMessage *m)
{
	IoCFFIStructure* o = IOCLONE(self);
	IoState_on_doCString_withLabel_(IoObject_state(o), o, "init", "IoCFFIStructure_with");
	if ( IoSeq_rawEqualsCString_(IOSYMBOL(IoObject_name(o)), "Union") ) {
		DATA(o)->isUnion = 1;
	}
	return IoCFFIStructure_setMembers(o, locals, m);
}

IoCFFIStructure *IoCFFIStructure_setMembers(IoCFFIStructure *self, IoObject *locals, IoMessage *m)
{
	int count = IoMessage_argCount(m);

	if ( count ) {
		ffi_type* elements;
		int i = 0, offset = 0, nextOffset = 0, maxsize = 0;
		IoMap* members;

		// Basic types inside libffi are defined as "const". We need to copy the whole type
		// as we could need to write to them (Union case)
		elements = io_calloc(count, sizeof(ffi_type));
		DATA(self)->ffiType.elements = io_calloc(count + 1, sizeof(ffi_type *));
		DATA(self)->needToFreeElements = 1;
	
		for ( i = 0 ; i < count ; i ++ ) {
			IoList* l = IoMessage_locals_listArgAt_(m, locals, i);
			IoCFFIDataType* d = IoList_rawAt_(l, 1);
			elements[i] = *(IoCFFIDataType_ffiType(d));
			DATA(self)->ffiType.elements[i] = &elements[i];
			maxsize = max(maxsize, elements[i].size);
		}
		DATA(self)->ffiType.elements[count] = NULL;
		DATA(self)->ffiType.size = 0;
		DATA(self)->ffiType.alignment = 0;
		DATA(self)->ffiType.type = FFI_TYPE_STRUCT;
		ffi_cif cif;
		ffi_prep_cif(&cif, FFI_DEFAULT_ABI, 0, &(DATA(self)->ffiType), NULL);
	
		if ( DATA(self)->isUnion ) {
			DATA(self)->ffiType.size = maxsize;
		}
	
		members = IoMap_new(IOSTATE);
		IoObject_setSlot_to_(self, IOSYMBOL("_members"), members);
		for ( i = 0 ; i < count ; i ++ ) {
			IoList* l = IoMessage_locals_listArgAt_(m, locals, i);
			IoSeq* s = IOREF(IoList_rawAt_(l, 0));
			IoCFFIDataType* d = IOCLONE(IoList_rawAt_(l, 1));
			
			IoMap_rawAtPut(members, s, d);
	
			if ( DATA(self)->isUnion ) {
				DATA(self)->ffiType.elements[i]->alignment = 0;
			}
			offset = IoCFFIStructure_calcOffset(DATA(self)->isUnion, DATA(self)->ffiType.elements[i], &nextOffset);

			IoObject_setSlot_to_(IoMap_rawAt(members, s), IOSYMBOL("_offset"), IONUMBER(offset));
			IoObject_setSlot_to_(IoMap_rawAt(members, s), IOSYMBOL("_order"), IONUMBER(i));
		}
	}
	
	return self;

}

IoObject *IoCFFIStructure_address(IoCFFIStructure *self, IoObject *locals, IoMessage *m)
{
	char str[64] = {0};

	snprintf(str, 64, "%p", DATA(self)->buffer);
	return IOSYMBOL(str);
}

IoObject *IoCFFIStructure_asBuffer(IoCFFIStructure *self, IoObject *locals, IoMessage *m)
{
	return IoCFFIDataType_asBuffer(self, locals, m);
}

IoCFFIStructure *IoCFFIStructure_setValues(IoCFFIStructure *self, IoObject *locals, IoMessage *m)
{
	//TODO check sizes and types
	int count = IoMessage_argCount(m);
	int i = 0;
	IoList* members = IoState_on_doCString_withLabel_(IOSTATE, self, 
							"_members values sortBy(method(a, b, a _order < (b _order)))",
							"IoCFFIStructure_setValues");

	for ( i = 0 ; i < count ; i ++ ) {
		IoObject *value = IoMessage_locals_valueArgAt_(m, locals, i);
		IoCFFIDataType_rawSetValue(IoList_rawAt_(members, i), value);
	}

	return self;
}

/* ---------------------------------------------------------------- */

IoCFFIStructure *IoCFFIStructure_cloneWithData(IoCFFIStructure *self, void* data)
{
	//TODO check sizes and types
	IoCFFIStructure* new = IOCLONE(self);
	memcpy(DATA(new)->buffer, data, DATA(new)->ffiType.size);
	return new;
}

void *IoCFFIStructure_valuePointer(IoCFFIStructure *self)
{
	return DATA(self)->buffer;
}

ffi_type *IoCFFIStructure_ffiType(IoCFFIStructure *self)
{
	return &(DATA(self)->ffiType);
}

void IoCFFIStructure_setValuePointer_offset_(IoCFFIStructure* self, void *ptr, int offset)
{
	// If self is member of another Structure (nested) we have to free self buffer if
	// needed and take the pointer and offset passed to store self values. Then tell all
	// self members to do the same.
	if ( DATA(self)->needToFreeBuffer ) {
		io_free(DATA(self)->buffer);
		DATA(self)->needToFreeBuffer = 0;
	}

	DATA(self)->buffer = ptr + offset;

	IoMap* members = IoObject_getSlot_(self, IOSYMBOL("_members"));
	LIST_FOREACH(IoList_rawList(IoMap_rawKeys(members)), i, v,
		IoCFFIDataType_setValuePointer_(IoMap_rawAt(members, v), DATA(self)->buffer);
	);
}
