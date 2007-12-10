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

typedef struct IoCFFIFunction
{
	ffi_cif interface;
} IoCFFIFunctionData;

IoTag *IoCFFIFunction_newTag(void *state);
IoCFFIFunction *IoCFFIFunction_proto(void *state);
IoCFFIFunction *IoCFFIFunction_rawClone(IoCFFIFunction *self);
void IoCFFIFunction_free(IoCFFIFunction *self);

IoObject *IoCFFIFunction_call(IoCFFIFunction *self, IoObject *locals, IoMessage *m);

#endif
