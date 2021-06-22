
// metadoc Object copyright Steve Dekorte 2002
// metadoc Object license BSD revised

#ifndef OBJECT_DEFINED
#define OBJECT_DEFINED 1

#include "IoVMApi.h"

#include "Common.h"
#include "PHash.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IoObject_clean(self) PHash_clean(IoObject_slots(self));
#define IOREF(value) IoObject_addingRef_((IoObject *)self, (IoObject *)value)
//#define IOALLOCREF(value) IoObject_isReferenced_(value, 1)

#define IOOBJECT_ISTYPE(self, typeName)                                        \
    IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)Io##typeName##_rawClone)

#define ISOBJECT(self) IOOBJECT_ISTYPE(self, Object)

#include "IoObject_struct.h"
#include "IoMessage.h"
#include "IoSeq.h"

IOVM_API IoObject *IoObject_proto(void *state);
IOVM_API IoObject *IoObject_protoFinish(void *state);
IOVM_API IoObject *IoObject_localsProto(void *state);

IOVM_API IoObject *IOCLONE(IoObject *self);
IOVM_API IoObject *IoObject_rawClone(IoObject *self);
IOVM_API IoObject *IoObject_justClone(IoObject *self);
IOVM_API IoObject *IoObject_rawClonePrimitive(IoObject *self);
IOVM_API IoObject *IoObject_new(void *state);

IOVM_API IoObject *IoObject_addMethod_(IoObject *self, IoSymbol *slotName,
                                       IoMethodFunc *fp);
IOVM_API void IoObject_addMethodTable_(IoObject *self,
                                       IoMethodTable *methodTable);
IOVM_API IoObject *IoObject_addTaglessMethod_(IoObject *self,
                                              IoSymbol *slotName,
                                              IoMethodFunc *fp);
IOVM_API void IoObject_addTaglessMethodTable_(IoObject *self,
                                              IoMethodTable *methodTable);

IOVM_API void IoObject_dealloc(IoObject *self);
IOVM_API void IoObject_willFree(IoObject *self);
IOVM_API void IoObject_free(IoObject *self);

// inheritance

IOVM_API int IoObject_hasProtos(IoObject *self);
IOVM_API int IoObject_rawProtosCount(IoObject *self);
IOVM_API void IoObject_rawAppendProto_(IoObject *self, IoObject *p);
IOVM_API void IoObject_rawPrependProto_(IoObject *self, IoObject *p);
IOVM_API void IoObject_rawRemoveProto_(IoObject *self, IoObject *p);
// IOVM_API void IoObject_rawRemoveAllProtos(IoObject *self);
IOVM_API void IoObject_rawSetProto_(IoObject *self, IoObject *proto);

IOVM_API IO_METHOD(IoObject, objectProto);
IOVM_API IO_METHOD(IoObject, setProto);
IOVM_API IO_METHOD(IoObject, setProtos);
IOVM_API IO_METHOD(IoObject, appendProto);
IOVM_API IO_METHOD(IoObject, prependProto);
IOVM_API IO_METHOD(IoObject, removeProto);
IOVM_API IO_METHOD(IoObject, removeAllProtos);
IOVM_API IO_METHOD(IoObject, protosMethod);
IOVM_API unsigned int IoObject_rawHasProto_(IoObject *self, IoObject *p);

// slots

IOVM_API void IoObject_createSlots(IoObject *self);
IOVM_API void IoObject_setSlot_to_(IoObject *self, IoSymbol *slotName,
                                   IoObject *value);

IOVM_API IoObject *IoObject_getSlot_(IoObject *self, IoSymbol *slotName);
IOVM_API IoObject *IoObject_symbolGetSlot_(IoObject *self, IoSymbol *slotName);
IOVM_API IoObject *IoObject_seqGetSlot_(IoObject *self, IoSymbol *slotName);
IOVM_API double IoObject_doubleGetSlot_(IoObject *self, IoSymbol *slotName);

// IoObject *IoObject_objectWithSlotValue_(IoObject *self, IoObject *slotValue);
IOVM_API void IoObject_removeSlot_(IoObject *self, IoSymbol *slotName);

// perform and activate

// IOVM_API IoObject *IoObject_activate(IoObject *self, IoObject *target,
// IoObject *locals, IoMessage *m, IoObject *slotContext); IOVM_API
// IO_METHOD(IoObject, perform); IO_METHOD(IoObject, forward);
IOVM_API IO_METHOD(IoObject, localsForward);

// tag functions

IOVM_API int IoObject_compare(IoObject *self, IoObject *v);
IOVM_API int IoObject_defaultCompare(IoObject *self, IoObject *v);
IOVM_API const char *IoObject_name(IoObject *self);
IOVM_API void IoObject_print(IoObject *self);

// memory

IOVM_API size_t IoObject_memorySize(IoObject *self);
IOVM_API void IoObject_compact(IoObject *self);

IOVM_API char *IoObject_markColorName(IoObject *self);
IOVM_API void IoObject_show(IoObject *self);

// proto

IOVM_API IO_METHOD(IoObject, clone);
IOVM_API IO_METHOD(IoObject, cloneWithoutInit);
IOVM_API IO_METHOD(IoObject, shallowCopy);
IOVM_API IoObject *IoObject_initClone_(IoObject *self, IoObject *locals,
                                       IoMessage *m, IoObject *newObject);

// printing

IOVM_API IO_METHOD(IoObject, protoWrite);
IOVM_API IO_METHOD(IoObject, protoWriteLn);

// reflection

IOVM_API IO_METHOD(IoObject, protoPerform);
IOVM_API IO_METHOD(IoObject, protoPerformWithArgList);

IOVM_API IO_METHOD(IoObject, protoSet_to_);
IOVM_API IO_METHOD(IoObject, protoSetSlotWithType);
IOVM_API IO_METHOD(IoObject, localsUpdateSlot);
IOVM_API IO_METHOD(IoObject, protoUpdateSlot_to_);

IOVM_API IO_METHOD(IoObject, protoGetSlot_);
IOVM_API IO_METHOD(IoObject, protoGetLocalSlot_);

IOVM_API IO_METHOD(IoObject, protoHasLocalSlot);
IOVM_API IO_METHOD(IoObject, protoHasProto_);

IOVM_API IO_METHOD(IoObject, protoRemoveSlot);
IOVM_API IO_METHOD(IoObject, protoRemoveAllSlots);
IOVM_API IO_METHOD(IoObject, protoSlotNames);
IOVM_API IO_METHOD(IoObject, protoSlotValues);

// IO_METHOD(IoObject, forward_);
IOVM_API IO_METHOD(IoObject, super);
IOVM_API IO_METHOD(IoObject, contextWithSlot);
IOVM_API IO_METHOD(IoObject, ancestorWithSlot);

IOVM_API IO_METHOD(IoObject, doMessage);
IOVM_API IO_METHOD(IoObject, self);
IOVM_API IO_METHOD(IoObject, locals);
IOVM_API IO_METHOD(IoObject, thisMessage);

// memory

IOVM_API int IoObject_rawCheckMemory(IoObject *self);
IOVM_API IO_METHOD(IoObject, memorySizeMethod);
IOVM_API IO_METHOD(IoObject, compactMethod);

// description

IOVM_API void IoObject_defaultPrint(IoObject *self);

IOVM_API IO_METHOD(IoObject, type);
IOVM_API IO_METHOD(IoObject, lobbyPrint);

// logic

IOVM_API IO_METHOD(IoObject, and);

// math

IOVM_API IO_METHOD(IoObject, subtract);

// comparison

IOVM_API int IoObject_sortCompare(IoObject **self, IoObject **v);

IOVM_API IO_METHOD(IoObject, isIdenticalTo);
IOVM_API IO_METHOD(IoObject, equals);
IOVM_API IO_METHOD(IoObject, notEquals);
IOVM_API IO_METHOD(IoObject, protoCompare);
IOVM_API IO_METHOD(IoObject, isLessThan_);
IOVM_API IO_METHOD(IoObject, isLessThanOrEqualTo_);
IOVM_API IO_METHOD(IoObject, isGreaterThan_);
IOVM_API IO_METHOD(IoObject, isGreaterThanOrEqualTo_);
IOVM_API IO_METHOD(IoObject, asBoolean);

// meta

// IOVM_API IO_METHOD(IoObject, truthValueOfArg);
IOVM_API IO_METHOD(IoObject, evalArg);
IOVM_API IO_METHOD(IoObject, evalArgAndReturnNil);
IOVM_API IO_METHOD(IoObject, evalArgAndReturnSelf);
IOVM_API IO_METHOD(IoObject, uniqueId);
IOVM_API IO_METHOD(IoObject, do);
IOVM_API IO_METHOD(IoObject, lexicalDo);
IOVM_API IO_METHOD(IoObject, message);

// compiler

IOVM_API IoObject *IoObject_rawDoString_label_(IoObject *self, IoSymbol *string,
                                               IoSymbol *label);
IOVM_API IO_METHOD(IoObject, doString);
IOVM_API IO_METHOD(IoObject, doFile);
// IO_METHOD(IoObject, unpack);

// activatable

IOVM_API IO_METHOD(IoObject, setIsActivatableMethod);
IOVM_API IO_METHOD(IoObject, isActivatableMethod);

// eval

IOVM_API IoObject *IoObject_rawDoMessage(IoObject *self, IoMessage *m);
IOVM_API IoObject *IoObject_eval(IoObject *self, IoMessage *m,
                                 IoObject *locals);

IOVM_API IO_METHOD(IoObject, argIsActivationRecord);
IOVM_API IO_METHOD(IoObject, argIsCall);

IOVM_API UArray *IoObject_rawGetUArraySlot(IoObject *self, IoObject *locals,
                                           IoMessage *m, IoSymbol *slotName);

IOVM_API UArray *IoObject_rawGetMutableUArraySlot(IoObject *self,
                                                  IoObject *locals,
                                                  IoMessage *m,
                                                  IoSymbol *slotName);

IOVM_API IO_METHOD(IoObject, become);

// io_free listeners ---------------------------------------------

IOVM_API void IoObject_addListener_(IoObject *self, void *listener);
IOVM_API void IoObject_removeListener_(IoObject *self, void *listener);

// persistence

IOVM_API void IoObject_protoClean(IoObject *self);
IOVM_API IoObject *IoObject_hasDirtySlot_(IoObject *self, IoMessage *m,
                                          IoObject *locals);
IOVM_API IO_METHOD(IoObject, markClean);

// IOVM_API PID_TYPE IoObject_pid(IoObject *self);

// asString helper

IOVM_API IoSeq *IoObject_asString_(IoObject *self, IoMessage *m);

#include "IoObject_flow.h"
#include "IoObject_inline.h"

#ifdef __cplusplus
}
#endif
#endif
