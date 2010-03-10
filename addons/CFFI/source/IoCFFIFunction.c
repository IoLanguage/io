/* CFFI - An Io interface to C
Copyright (c) 2006 Trevor Fancher. All rights reserved.
All code licensed under the New BSD license.
*/
// docDependsOn("CFFIDataType")

#include "IoCFFIFunction.h"
#include "IoCFFILibrary.h"
#include "IoCFFIDataType.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoBlock.h"
#include "List.h"
#include <ffi.h>

#define DATA(self) ((IoCFFIFunctionData *)(IoObject_dataPointer(self)))
void IoCFFIFunction_closure(ffi_cif* cif, void* result, void** args, void* userdata);

IoTag *IoCFFIFunction_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Function");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCFFIFunction_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCFFIFunction_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCFFIFunction_mark);

	// We add the posibility to activate the Function (ie: as an Io method)
	// Having the activateFunc assigned does not mean the Function is activatable,
	// we have to call setIsActivatable(true) in the Io side to take effect
	IoTag_activateFunc_(tag, (IoTagActivateFunc *)IoCFFIFunction_activate);
	return tag;
}

IoCFFIFunction *IoCFFIFunction_proto(void *state)
{
	//IoObject *self = IoObject_new(state);
	IoObject *self = IoCFFIDataType_new(state);
	IoObject_tag_(self, IoCFFIFunction_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCFFIFunctionData)));
	memset(DATA(self), 0, sizeof(IoCFFIFunctionData));
	DATA(self)->valuePointer = &(DATA(self)->code);

	IoState_registerProtoWithFunc_(state, self, IoCFFIFunction_proto);

	{
		IoMethodTable methodTable[] = {
			{"call", IoCFFIFunction_call},
			{"setCallback", IoCFFIFunction_setCallback},
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
	memset(DATA(self), 0, sizeof(IoCFFIFunctionData));
	DATA(self)->valuePointer = &(DATA(self)->code);
	return self;
}

IoCFFIFunction *IoCFFIFunction_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoCFFIFunction_proto);
	return IOCLONE(proto);
}


void IoCFFIFunction_free(IoCFFIFunction *self)
{
	if(DATA(self)->pcl) {
		ffi_closure_free(DATA(self)->pcl);
	}
	
	if(DATA(self)->cbCtx) {
		io_free(DATA(self)->cbCtx);
	}
	free(DATA(self));
}

void IoCFFIFunction_mark(IoCFFIFunction *self)
{
	CallbackContext *ctx = DATA(self)->cbCtx;
	if(ctx) {
		IoObject_shouldMark(ctx->self);
		IoObject_shouldMark(ctx->block);
		IoObject_shouldMark(ctx->target);
		IoObject_shouldMark(ctx->locals);
		IoObject_shouldMark(ctx->context);
	}
}

// self, target, locals, message, slotContext
void *IoCFFIFunction_activate(void *self, void *target, void *locals, void *m, void *slotContext)
{
	return (void *)IoCFFIFunction_call((IoCFFIFunction *)self, (IoObject *)locals, (IoMessage *)m);
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

	funName = CSTRING(IoObject_getSlot_(self, IOSYMBOL("name")));
	if(strlen(funName) > 0) {
		library = IoObject_getSlot_(self, IOSYMBOL("library"));
		funPointer = IoCFFILibrary_rawGetFuctionPointer_(library, funName);
	}
	else funPointer = *(DATA(self)->valuePointer);

	if(NULL == funPointer) {
		printf("\n\nNULL function pointer\n\n");
		return IONIL(self);
	}

	funInterface = &(DATA(self)->interface);
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
			funArgVals[i] = IoCFFIDataType_ValuePointerFromObject_(NULL, o);
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


// TODO cleanup
IoObject *IoCFFIFunction_setCallback(IoCFFIFunction *self, IoObject *locals, IoMessage *m)
{
	IoObject *funRetTypeObject, *o;
	int funArgCount, i;
	ffi_status status;
	ffi_cif *funInterface;
	ffi_type **funArgTypes, *funRetType;
	List *funArgTypeObjects;

	void** code = DATA(self)->valuePointer;

	//Alloc the closure object
	DATA(self)->pcl = ffi_closure_alloc(sizeof(ffi_closure), code);

	//Prepare cif for callback
	funInterface = &(DATA(self)->interface);
	funArgTypeObjects = IoList_rawList(IoObject_getSlot_(self, IOSYMBOL("argumentTypes")));
	funRetTypeObject = IoObject_getSlot_(self, IOSYMBOL("returnType"));
	funArgCount = (int)List_size(funArgTypeObjects);
	funArgTypes = io_calloc(funArgCount, sizeof(ffi_type *));
	for (i = 0; i < funArgCount; i++)
	{   
		funArgTypes[i] = IoCFFIDataType_ffiType(List_at_(funArgTypeObjects, i));
	}
	funRetType = IoCFFIDataType_ffiType(funRetTypeObject);

	status = ffi_prep_cif(funInterface, FFI_DEFAULT_ABI, funArgCount, funRetType, funArgTypes);
	if (status != FFI_OK)
	{
		printf("\n\nffi_prep_cif status != FFI_OK\n\n");
		io_free(funArgTypes);
		ffi_closure_free(DATA(self)->pcl);
		return IONIL(self);
	}

	//Prepare closure object
	CallbackContext *ctx = io_calloc(1, sizeof(CallbackContext));
	DATA(self)->cbCtx = ctx;
	ctx->self = IOREF(self);
	ctx->block = IOREF(IoMessage_locals_blockArgAt_(m, locals, 0));
	o = IoMessage_locals_valueArgAt_(m, locals, 1);
	if(!ISNIL(o)) { 
		ctx->target = ctx->locals = ctx->context = IOREF(o);
		//IoState_on_doCString_withLabel_(IOSTATE, self, "getSlot(\"_callback\") setScope(_callbackLocals)", "IoCFFIFunction_setCallback");
		((IoBlockData *)IoObject_dataPointer(ctx->block))->scope = IOREF(o);
	}
	else {
		ctx->target = ctx->locals = ctx->context = IoState_lobby(IOSTATE);
	}
	
	status = ffi_prep_closure_loc(DATA(self)->pcl, funInterface, IoCFFIFunction_closure, ctx, *code);
	if (status != FFI_OK)
	{
		printf("\n\nffi_prep_closure_loc status != FFI_OK\n\n");
		io_free(funArgTypes);
		ffi_closure_free(DATA(self)->pcl);
		return IONIL(self);
	}

	return self;
}

//When the callback is invoked from the C side, ffi transfers control here
void IoCFFIFunction_closure(ffi_cif* cif, void* result, void** args, void* userdata)
{
	IoObject* ret;
	IoCFFIDataType* retType;

	CallbackContext *ctx = (CallbackContext *)userdata;
	IoObject *self = ctx->self;
	IoMessage *newMessage = IoMessage_new(IOSTATE);

	LIST_FOREACH(IoList_rawList(IoObject_getSlot_(self, IOSYMBOL("argumentTypes"))), i, v,
		{
		IoObject* obj = IoCFFIDataType_objectFromData_(v, args[i]);
		IoMessage_addCachedArg_(newMessage, obj);
		}
	);
	ret = IoBlock_activate(ctx->block, ctx->target, ctx->locals, newMessage, ctx->context);

	//these three steps are needed bacause of Numbers being always double
	retType = IOCLONE(IoObject_getSlot_(self, IOSYMBOL("returnType")));
	IoCFFIDataType_rawSetValue(retType, ret);
	memcpy(result, (void *)IoCFFIDataType_ValuePointerFromObject_(NULL, retType), cif->rtype->size);
}

IoCFFIFunction *IoCFFIFunction_cloneWithData(IoCFFIFunction *self, void **data)
{
	IoCFFIFunction *new = IOCLONE(self);
	*(DATA(new)->valuePointer) = *data;
	return new;
}

void *IoCFFIFunction_valuePointer(IoCFFIFunction *self)
{
	return DATA(self)->valuePointer;
}

//Func called when we are member of a Struct
void IoCFFIFunction_setValuePointer_offset_(IoCFFIFunction* self, void *ptr, int offset)
{
	DATA(self)->valuePointer = ptr + offset;
}
