
// metadoc DynLib copyright Steve Dekorte 2002
// metadoc DynLib license BSD revised
/*metadoc DynLib description
        Contributed by Kentaro A. Kurahone <kurahone@sigusr1.org>
        This object can dynamically load a shared library
*/

#ifndef IODYNLIB_DEFINED
#define IODYNLIB_DEFINED 1

#include "Common.h"
#include "IoObject.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoMessage.h"

#ifdef __cplusplus
extern "C" {
#endif

// void (*initFunc)(void);

typedef IoObject IoDynLib;

// Generic functions

IoObject *IoDynLib_proto(void *state);
IoDynLib *IoDynLib_new(void *state);
IoDynLib *IoDynLib_rawClone(IoDynLib *self);
void IoDynLib_free(IoDynLib *self);

// Methods

IoDynLib *IoDynLib_setPath(IoDynLib *self, IoObject *locals, IoMessage *m);
IoDynLib *IoDynLib_path(IoDynLib *self, IoObject *locals, IoMessage *m);

IoDynLib *IoDynLib_setInitFuncName(IoDynLib *self, IoObject *locals,
                                   IoMessage *m);
IoDynLib *IoDynLib_initFuncName(IoDynLib *self, IoObject *locals, IoMessage *m);

IoDynLib *IoDynLib_setFreeFuncName(IoDynLib *self, IoObject *locals,
                                   IoMessage *m);
IoDynLib *IoDynLib_freeFuncName(IoDynLib *self, IoObject *locals, IoMessage *m);

IO_METHOD(IoDynLib, open);
IO_METHOD(IoDynLib, close);
IO_METHOD(IoDynLib, isOpen);

IO_METHOD(IoDynLib, call);
IoDynLib *IoDynLib_voidCall(IoDynLib *self, IoObject *locals, IoMessage *m);

IoDynLib *IoDynLib_callPluginInitFunc(IoDynLib *self, IoObject *locals,
                                      IoMessage *m);
// IoSeq *IoDynLib_returnsString(IoDynLib *self, IoObject *locals, IoMessage
// *m);

#ifdef __cplusplus
}
#endif
#endif
