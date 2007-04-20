/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

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

#define IOOBJECT_ISTYPE(self, typeName) \
	IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)Io ## typeName ## _rawClone)

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

IOVM_API IoObject *IoObject_addMethod_(IoObject *self, IoSymbol *slotName, IoMethodFunc *fp);
IOVM_API void IoObject_addMethodTable_(IoObject *self, IoMethodTable *methodTable);

IOVM_API void IoObject_dealloc(IoObject *self);
IOVM_API void IoObject_willFree(IoObject *self);
IOVM_API void IoObject_free(IoObject *self);

// inheritance

IOVM_API int IoObject_hasProtos(IoObject *self);
IOVM_API int IoObject_rawProtosCount(IoObject *self);
IOVM_API void IoObject_rawAppendProto_(IoObject *self, IoObject *p);
IOVM_API void IoObject_rawPrependProto_(IoObject *self, IoObject *p);
IOVM_API void IoObject_rawRemoveProto_(IoObject *self, IoObject *p);
IOVM_API void IoObject_rawRemoveAllProtos(IoObject *self);
IOVM_API void IoObject_rawSetProto_(IoObject *self, IoObject *proto);

IOVM_API IoObject *IoObject_objectProto(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_setProto(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_setProtos(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_appendProto(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_prependProto(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_removeProto(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_removeAllProtos(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_protosMethod(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API unsigned int IoObject_rawHasProto_(IoObject *self, IoObject *p);

// slots

IOVM_API void IoObject_createSlots(IoObject *self);
IOVM_API void IoObject_setSlot_to_(IoObject *self, IoSymbol *slotName, IoObject *value);

IOVM_API IoObject *IoObject_getSlot_(IoObject *self, IoSymbol *slotName);
IOVM_API IoObject *IoObject_symbolGetSlot_(IoObject *self, IoSymbol *slotName);
IOVM_API IoObject *IoObject_seqGetSlot_(IoObject *self, IoSymbol *slotName);
IOVM_API double IoObject_doubleGetSlot_(IoObject *self, IoSymbol *slotName);

//IoObject *IoObject_objectWithSlotValue_(IoObject *self, IoObject *slotValue);
IOVM_API void IoObject_removeSlot_(IoObject *self, IoSymbol *slotName);

// perform and activate

IOVM_API IoObject *IoObject_activate(IoObject *self, IoObject *target, IoObject *locals, IoMessage *m, IoObject *slotContext);
IOVM_API IoObject *IoObject_perform(IoObject *self, IoObject *locals, IoMessage *m);
//IoObject *IoObject_forward(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_localsForward(IoObject *self, IoObject *locals, IoMessage *m);

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

IOVM_API IoObject *IoObject_clone(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_cloneWithoutInit(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_shallowCopy(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_duplicate(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_initClone_(IoObject *self, IoObject *locals, IoMessage *m, IoObject *newObject);

// printing

IOVM_API IoObject *IoObject_protoPrint(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_protoWrite(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_protoWriteLn(IoObject *self, IoObject *locals, IoMessage *m);

// reflection

IOVM_API IoObject *IoObject_protoPerform(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_protoPerformWithArgList(IoObject *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoObject_protoSet_to_(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_protoSetSlotWithType(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_localsUpdateSlot(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_protoUpdateSlot_to_(IoObject *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoObject_protoGetSlot_(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_protoGetLocalSlot_(IoObject *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoObject_protoHasLocalSlot(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_protoHasProto_(IoObject *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoObject_protoRemoveSlot(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_protoRemoveAllSlots(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_protoSlotNames(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_protoSlotValues(IoObject *self, IoObject *locals, IoMessage *m);

//IoObject *IoObject_forward_(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_super(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_contextWithSlot(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_ancestorWithSlot(IoObject *self, IoObject *locals, IoMessage *m);

IOVM_API IoObject *IoObject_doMessage(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_self(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_locals(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_thisMessage(IoObject *self, IoObject *locals, IoMessage *m);

// memory

IOVM_API IoObject *IoObject_memorySizeMethod(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_compactMethod(IoObject *self, IoObject *locals, IoMessage *m);

// description

IOVM_API void IoObject_defaultPrint(IoObject *self);

IOVM_API IoObject *IoObject_type(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_lobbyPrint(IoObject *self, IoObject *locals, IoMessage *m);

// logic

IOVM_API IoObject *IoObject_and(IoObject *self, IoObject *locals, IoMessage *m);

// math

IOVM_API IoObject *IoObject_subtract(IoObject *self, IoObject *locals, IoMessage *m);

// comparison

IOVM_API int IoObject_sortCompare(IoObject **self, IoObject **v);

IOVM_API IoObject *IoObject_isIdenticalTo(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_equals(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_notEquals(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_protoCompare(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_isLessThan_(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_isLessThanOrEqualTo_(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_isGreaterThan_(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_isGreaterThanOrEqualTo_(IoObject *self, IoObject *locals, IoMessage *m);

// meta

IOVM_API IoObject *IoObject_evalArg(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_evalArgAndReturnNil(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_evalArgAndReturnSelf(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_uniqueId(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_do(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_message(IoObject *self, IoObject *locals, IoMessage *m);

// compiler

IOVM_API IoObject *IoObject_rawDoString_label_(IoObject *self, IoSymbol *string, IoSymbol *label);
IOVM_API IoObject *IoObject_doString(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_doFile(IoObject *self, IoObject *locals, IoMessage *m);
//IoObject *IoObject_unpack(IoObject *self, IoObject *locals, IoMessage *m);

// activatable

IOVM_API IoObject *IoObject_setIsActivatableMethod(IoObject *self, IoObject *locals, IoMessage *m);
IOVM_API IoObject *IoObject_isActivatableMethod(IoObject *self, IoObject *locals, IoMessage *m);

// eval

IOVM_API IoObject *IoObject_rawDoMessage(IoObject *self, IoMessage *m);
IOVM_API IoObject *IoObject_eval(IoObject *self, IoMessage *m, IoObject *locals);

IOVM_API IoObject *IoObject_argIsActivationRecord(IoObject *self, IoMessage *m, IoObject *locals);
IOVM_API IoObject *IoObject_argIsCall(IoObject *self, IoMessage *m, IoObject *locals);

IOVM_API UArray *IoObject_rawGetUArraySlot(IoObject *self,
								IoObject *locals,
								IoMessage *m,
								IoSymbol *slotName);

IOVM_API UArray *IoObject_rawGetMutableUArraySlot(IoObject *self,
								IoObject *locals,
								IoMessage *m,
								IoSymbol *slotName);

IOVM_API IoObject *IoObject_become(IoObject *self, IoObject *locals, IoMessage *m);

// io_free listeners ---------------------------------------------

IOVM_API void IoObject_addListener_(IoObject *self, void *listener);
IOVM_API void IoObject_removeListener_(IoObject *self, void *listener);

// persistence

IOVM_API PID_TYPE IoObject_pid(IoObject *self);

// asString helper

IOVM_API IoSeq *IoObject_asString_(IoObject *self, IoMessage *m);

#include "IoObject_flow.h"
#include "IoObject_inline.h"

#ifdef __cplusplus
}
#endif
#endif
