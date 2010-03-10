/* CFFI - An Io interface to C
Copyright (c) 2006 Trevor Fancher. All rights reserved.
All code licensed under the New BSD license.
*/

#ifndef IoCFFIFunction_DEFINED
#define IoCFFIFunction_DEFINED 1

#include "IoObject.h"
#include <ffi.h>

#define ISCFFIFunction(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCFFIFunction_rawClone)

typedef IoObject IoCFFIFunction;

typedef struct {
	IoObject	*self;
	IoObject	*block;
	IoObject	*target;
	IoObject	*locals;
	IoObject	*context;
} CallbackContext;

typedef struct IoCFFIFunction
{
	ffi_cif			interface;
	ffi_closure		*pcl;
	void			*code;
	void			**valuePointer;
	CallbackContext	*cbCtx;
} IoCFFIFunctionData;


IoTag			*IoCFFIFunction_newTag(void *state);
IoCFFIFunction	*IoCFFIFunction_proto(void *state);
IoCFFIFunction	*IoCFFIFunction_rawClone(IoCFFIFunction *self);
IoCFFIFunction	*IoCFFIFunction_new(void *state);
void			IoCFFIFunction_free(IoCFFIFunction *self);
void 			*IoCFFIFunction_activate(void *self, void *target, void *locals, void *m, void *slotContext);
void			IoCFFIFunction_mark(IoCFFIFunction *self);
IoCFFIFunction	*IoCFFIFunction_cloneWithData(IoCFFIFunction *self, void **data);

IoObject		*IoCFFIFunction_call(IoCFFIFunction *self, IoObject *locals, IoMessage *m);
IoObject		*IoCFFIFunction_setCallback(IoCFFIFunction *self, IoObject *locals, IoMessage *m);

IoCFFIFunction	*IoCFFIFunction_cloneWithData(IoCFFIFunction *self, void **data);
void			*IoCFFIFunction_valuePointer(IoCFFIFunction *self);
void 			IoCFFIFunction_setValuePointer_offset_(IoCFFIFunction* self, void *ptr, int offset);

#endif
