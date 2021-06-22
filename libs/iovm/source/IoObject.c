
// metadoc Object category Core
// metadoc Object copyright Steve Dekorte 2002
// metadoc Object license BSD revised
/*metadoc Object description
An Object is a key/value dictionary with string keys and values of any type.
The prototype Object contains a clone slot that is a CFunction that creates new
objects. When cloned, an Object will call its init slot (with no arguments).
*/

#include "IoState.h"
#define IOOBJECT_C
#include "IoObject.h"
#undef IOOBJECT_C
#include "IoCoroutine.h"
#include "IoTag.h"
#include "IoCFunction.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoMessage.h"
#include "IoMessage_parser.h"
#include "IoCFunction.h"
#include "IoBlock.h"
#include "IoList.h"
#include "IoObject.h"
#include "IoFile.h"
#include "IoSeq.h"
#include <string.h>
#include <stddef.h>

static const char *protoId = "Object";

IoObject *IoObject_activateFunc(IoObject *self, IoObject *target,
                                IoObject *locals, IoMessage *m,
                                IoObject *slotContext);

IoTag *IoObject_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoObject_rawClone);
    IoTag_activateFunc_(tag,
                        (IoTagActivateFunc *)NULL); // IoObject_activateFunc;
    return tag;
}

IoObject *IoObject_justAlloc(IoState *state) {
    IoObject *child = Collector_newMarker(state->collector);
    CollectorMarker_setObject_(child, io_calloc(1, sizeof(IoObjectData)));
    IoObject_protos_(child, (IoObject **)io_calloc(2, sizeof(IoObject *)));
    return child;
}

IoObject *IoObject_alloc(IoObject *self) {
    IoObject *child;

#ifdef IOSTATE_RECYCLING_ON
    child = List_pop(IOSTATE->recycledObjects);
    if (!child)
#endif
    {
        child = IoObject_justAlloc(IOSTATE);
    }

    IoObject_markerCount_(child, 0);

    return child;
}

IoObject *IoObject_proto(void *state) {
    IoObject *self = IoObject_justAlloc(state);

    IoObject_tag_(self, IoObject_newTag(state));

    IoObject_slots_(self, PHash_new());
    IoObject_ownsSlots_(self, 1);
    // IoObject_state_(self, state);
    IoState_registerProtoWithId_((IoState *)state, self, protoId);
    return self;
}

IO_METHOD(IoObject, protoOwnsSlots) {
    /*doc Object ownsSlots
    A debug method.
    */
    return IOBOOL(self, IoObject_ownsSlots(self));
}

size_t IoObject_memorySize(IoObject *self) {
    // return (IoObject_tag(self)->memorySizeFunc) ?
    // (IoObject_tag(self)->memorySizeFunc)(self) : 0;
    size_t size = sizeof(IoObject);

    if (IoObject_ownsSlots(self))
        size += PHash_memorySize(IoObject_slots(self));

    if (!ISNUMBER(self)) {
        //		if(IoObject_dataPointer(self)) size +=
        // io_memsize(IoObject_dataPointer(self));
    }

    return size;
}

IoObject *IoObject_protoFinish(void *state) {
    IoMethodTable methodTable[] = {
        {"clone", IoObject_clone},
        {"cloneWithoutInit", IoObject_cloneWithoutInit},
        {"shallowCopy", IoObject_shallowCopy},
        {"write", IoObject_protoWrite},
        {"writeln", IoObject_protoWriteLn},
        {"type", IoObject_type},

        // logic

        {"compare", IoObject_protoCompare},
        {"<", IoObject_isLessThan_},
        {">", IoObject_isGreaterThan_},
        {">=", IoObject_isGreaterThanOrEqualTo_},
        {"<=", IoObject_isLessThanOrEqualTo_},
        {"asBoolean", IoObject_asBoolean},

        // comparison

        {"isIdenticalTo", IoObject_isIdenticalTo},
        {"==", IoObject_equals},
        {"!=", IoObject_notEquals},

        // introspection

        //{"self", IoObject_self},
        {"setSlot", IoObject_protoSet_to_},
        {"setSlotWithType", IoObject_protoSetSlotWithType},
        {"updateSlot", IoObject_protoUpdateSlot_to_},
        {"getSlot", IoObject_protoGetSlot_},
        {"getLocalSlot", IoObject_protoGetLocalSlot_},
        {"hasLocalSlot", IoObject_protoHasLocalSlot},
        {"hasProto", IoObject_protoHasProto_},
        {"removeSlot", IoObject_protoRemoveSlot},
        {"removeAllSlots", IoObject_protoRemoveAllSlots},
        {"slotNames", IoObject_protoSlotNames},
        {"slotValues", IoObject_protoSlotValues},

        // method invocation

        {"perform", IoObject_protoPerform},
        {"performWithArgList", IoObject_protoPerformWithArgList},
        {"ancestorWithSlot", IoObject_ancestorWithSlot},
        {"contextWithSlot", IoObject_contextWithSlot},

        // control

        {"block", IoObject_block},
        {"method", IoBlock_method},
        {"for", IoObject_for},
        {"if", IoObject_if},
        {"", IoObject_evalArg},
        //{"truthValueOfArg", IoObject_truthValueOfArg},
        {"evalArg", IoObject_evalArg},
        {"evalArgAndReturnSelf", IoObject_evalArgAndReturnSelf},
        {"evalArgAndReturnNil", IoObject_evalArgAndReturnNil},

        {"return", IoObject_return},
        {"returnIfNonNil", IoObject_returnIfNonNil},
        {"loop", IoObject_loop},
        {"while", IoObject_while},
        {"break", IoObject_break},
        {"continue", IoObject_continue},
        {"stopStatus", IoObject_stopStatus},

        // utility

        {"print", IoObject_lobbyPrint},
        {"do", IoObject_do},
        {"lexicalDo", IoObject_lexicalDo},
        {"message", IoObject_message},
        {"doMessage", IoObject_doMessage},
        {"doString", IoObject_doString},
        {"doFile", IoObject_doFile},

        // reflection

        {"uniqueId", IoObject_uniqueId},

        //{"compact", IoObject_compactMethod},

        {"init", IoObject_self},

        // enumeration

        {"foreachSlot", IoObject_foreachSlot},
        {"-", IoObject_subtract},

        {"thisContext", IoObject_self},
        {"thisMessage", IoObject_thisMessage},
        {"thisLocalContext", IoObject_locals},

        // protos

        {"setProto", IoObject_setProto},
        {"setProtos", IoObject_setProtos},
        {"appendProto", IoObject_appendProto},
        {"prependProto", IoObject_prependProto},
        {"removeProto", IoObject_removeProto},
        {"removeAllProtos", IoObject_removeAllProtos},
        {"protos", IoObject_protosMethod},
        {"proto", IoObject_objectProto},
        {"setIsActivatable", IoObject_setIsActivatableMethod},
        {"isActivatable", IoObject_isActivatableMethod},
        {"argIsActivationRecord", IoObject_argIsActivationRecord},
        {"argIsCall", IoObject_argIsCall},
        {"become", IoObject_become},

        {"ownsSlots", IoObject_protoOwnsSlots}, // a debug method
        {"memorySize", IoObject_memorySizeMethod},

        {"hasDirtySlot", IoObject_hasDirtySlot_},
        {"markClean", IoObject_markClean},

        {NULL, NULL},
    };

    IoObject *self = IoState_protoWithId_((IoState *)state, protoId);

    IoObject_addTaglessMethodTable_(self, methodTable);
    return self;
}

IoObject *IoObject_localsProto(void *state) {
    IoObject *self = IoObject_new(state);

    IoObject_createSlotsIfNeeded(self);
    PHash_copy_(IoObject_slots(self),
                IoObject_slots(IoObject_firstProto(self)));

    IoObject_rawRemoveAllProtos(self);

    // Locals handles := and =
    IoObject_addMethod_(self, IOSYMBOL("setSlot"), IoObject_protoSet_to_);
    IoObject_addMethod_(self, IOSYMBOL("setSlotWithType"),
                        IoObject_protoSetSlotWithType);
    IoObject_addMethod_(self, IOSYMBOL("updateSlot"),
                        IoObject_localsUpdateSlot);
    IoObject_addMethod_(self, IOSYMBOL("thisLocalContext"), IoObject_locals);

    // Everything else is forwarded to self
    IoObject_addMethod_(self, IOSYMBOL("forward"), IoObject_localsForward);

    return self;
}

IoObject *IoObject_addMethod_(IoObject *self, IoSymbol *slotName,
                              IoMethodFunc *fp) {
    IoTag *t = IoObject_tag(self);
    IoCFunction *f;

    f = IoCFunction_newWithFunctionPointer_tag_name_(
        IOSTATE, (IoUserFunction *)fp, t, CSTRING(slotName));
    IoObject_setSlot_to_(self, slotName, f);
    return f;
}

void IoObject_addMethodTable_(IoObject *self, IoMethodTable *methodTable) {
    IoMethodTable *entry = methodTable;

    while (entry->name) {
        IoObject_addMethod_(self, IOSYMBOL(entry->name), entry->func);
        entry++;
    }
}

IoObject *IoObject_addTaglessMethod_(IoObject *self, IoSymbol *slotName,
                                     IoMethodFunc *fp) {
    IoCFunction *f;

    f = IoCFunction_newWithFunctionPointer_tag_name_(
        IOSTATE, (IoUserFunction *)fp, NULL, CSTRING(slotName));
    IoObject_setSlot_to_(self, slotName, f);
    return f;
}

void IoObject_addTaglessMethodTable_(IoObject *self,
                                     IoMethodTable *methodTable) {
    IoMethodTable *entry = methodTable;

    while (entry->name) {
        IoObject_addTaglessMethod_(self, IOSYMBOL(entry->name), entry->func);
        entry++;
    }
}

IoObject *IoObject_new(void *state) {
    IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

IoObject *IoObject_justClone(IoObject *self) {
    return (IoObject_tag(self)->cloneFunc)(self);
}

void IoObject_createSlots(IoObject *self) {
    IoObject_slots_(self, PHash_new());
    IoObject_ownsSlots_(self, 1);
}

// inline
void IoObject_freeData(IoObject *self) {
    IoTagFreeFunc *func = IoTag_freeFunc(IoObject_tag(self));

    if (func) {
        // if(func == free)
        {
            // printf("Tag func is free\n");
            // if (IoObject_name(self)) printf("free %s\n",
            // IoObject_name(self));
        }

        (*func)(self);
    } else if (IoObject_dataPointer(self)) {
        io_free(IoObject_dataPointer(self));
    }

    IoObject_setDataPointer_(self, NULL);
}

// inline
void IoObject_setProtoTo_(IoObject *self, IoObject *proto) {
    IoObject_rawSetProto_(self, proto);

    if (!IoObject_slots(self)) {
        IoObject_slots_(self, IoObject_slots(proto));
        IoObject_ownsSlots_(self, 0); // should be redundant
    }
}

IoObject *IoObject_rawClone(IoObject *proto) {
    IoObject *self = IoObject_alloc(proto);
    IoObject_tag_(self, IoObject_tag(proto));
    /*
    {
            IoObject **protos = IoObject_protos(self);
            protos[0] = proto;
    }
    */
    IoObject_setProtoTo_(self, proto);
    IoObject_isActivatable_(self, IoObject_isActivatable(proto));

    // IoObject_protos(self)[0] = proto;
    // IoObject_setDataPointer_(self, IoObject_dataPointer(proto)); // is this
    // right?
    IoObject_isDirty_(self, 1);
    return self;
}

IoObject *IoObject_rawClonePrimitive(IoObject *proto) {
    IoObject *self = IoObject_alloc(proto);
    IoObject_tag_(self, IoObject_tag(proto));
    IoObject_setProtoTo_(self, proto);
    IoObject_setDataPointer_(self, NULL);
    IoObject_isDirty_(self, 1);
    return self;
}

// protos ---------------------------------------------

void IoObject_rawPrintProtos(IoObject *self) {
    int count = 0;

    IOOBJECT_FOREACHPROTO(
        self, proto, printf("%i : %p\n", count, (void *)(proto)); count++;);

    printf("\n");
}

int IoObject_hasProtos(IoObject *self) {
    return (IoObject_firstProto(self) != NULL);
}

int IoObject_rawProtosCount(IoObject *self) {
    int count = 0;
    IOOBJECT_FOREACHPROTO(self, proto, if (proto) count++);
    return count;
}

void IoObject_rawAppendProto_(IoObject *self, IoObject *p) {
    int count = IoObject_rawProtosCount(self);
    IoObject_protos_(self, io_realloc(IoObject_protos(self),
                                      (count + 2) * sizeof(IoObject *)));
    IoObject_protos(self)[count] = IOREF(p);
    IoObject_protos(self)[count + 1] = NULL;
}

void IoObject_rawPrependProto_(IoObject *self, IoObject *p) {
    int count = IoObject_rawProtosCount(self);
    int oldSize = (count + 1) * sizeof(IoObject *);
    int newSize = oldSize + sizeof(IoObject *);

    IoObject_protos_(self, io_realloc(IoObject_protos(self), newSize));

    {
        void *src = IoObject_protos(self);
        void *dst = IoObject_protos(self) + 1;
        memmove(dst, src, oldSize);
    }

    IoObject_protoAtPut_(self, 0, IOREF(p));
}

void IoObject_rawRemoveProto_(IoObject *self, IoObject *p) {
    IoObject **proto = IoObject_protos(self);
    int count = IoObject_rawProtosCount(self);
    int index = 0;

    while (*proto) {
        if (*proto == p) {
            memmove(proto, proto + 1, (count - index) * sizeof(IoObject *));
        } else {
            proto++;
        }

        index++;
    }
}

/*
#include <assert.h>

void IoObject_testProtosCode(IoObject *self)
{
        IoObject *o1 = (IoObject *)0x1;
        IoObject *o2 = (IoObject *)0x2;
        int c;

        IoObject_rawRemoveAllProtos(self);
        c = IoObject_rawProtosCount(self);
        assert(c == 0);

        IoObject_rawAppendProto_(self, o1);
        assert(IoObject_protoAt_(self, 0) == o1);
        assert(IoObject_protoAt_(self, 1) == NULL);
        assert(IoObject_rawProtosCount(self) == 1);

        IoObject_rawPrependProto_(self, (IoObject *)0x2);
        assert(IoObject_rawProtosCount(self) == 2);
        assert(IoObject_protoAt_(self, 0) == o2);
        assert(IoObject_protoAt_(self, 1) == o1);
        assert(IoObject_protoAt_(self, 2) == NULL);

        IoObject_rawRemoveAllProtos(self);
        c = IoObject_rawProtosCount(self);
        assert(c == 0);
}
*/

void IoObject_rawSetProto_(IoObject *self, IoObject *proto) {
    IoObject_rawRemoveAllProtos(self);
    IoObject_protos(self)[0] = IOREF(proto);
}

IO_METHOD(IoObject, objectProto) {
    /*doc Object proto
    Same as; method(self protos first)
    */

    IoObject *proto = IoObject_firstProto(self);
    return proto ? proto : IONIL(self);
}

IO_METHOD(IoObject, setProto) {
    /*doc Object setProto(anObject)
    Sets the first proto of the receiver to anObject, replacing the
    current one, if any. Returns self.
    */

    IoObject_rawSetProto_(self, IoMessage_locals_valueArgAt_(m, locals, 0));
    return self;
}

IO_METHOD(IoObject, appendProto) {
    /*doc Object appendProto(anObject)
    Appends anObject to the receiver's proto list. Returns self.
    */

    IoObject *proto = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoObject_rawAppendProto_(self, proto);
    return self;
}

IO_METHOD(IoObject, prependProto) {
    /*doc Object prependProto(anObject)
    Prepends anObject to the receiver's proto list. Returns self.
    */

    IoObject *proto = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoObject_rawPrependProto_(self, proto);
    return self;
}

IO_METHOD(IoObject, removeProto) {
    /*doc Object removeProto(anObject)
    Removes anObject from the receiver's proto list if it
    is present. Returns self.
    */

    IoObject *proto = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoObject_rawRemoveProto_(self, proto);
    return self;
}

IO_METHOD(IoObject, removeAllProtos) {
    /*doc Object removeAllProtos
    Removes all of the receiver's protos. Returns self.
    */

    IoObject_rawRemoveAllProtos(self);
    return self;
}

IO_METHOD(IoObject, setProtos) {
    /*doc Object setProtos(aList)
    Replaces the receiver's protos with a copy of aList. Returns self.
    */

    IoList *ioList = IoMessage_locals_listArgAt_(m, locals, 0);
    IoObject_rawRemoveAllProtos(self);
    LIST_FOREACH(IoList_rawList(ioList), i, v,
                 IoObject_rawAppendProto_(self, (IoObject *)v));
    return self;
}

IO_METHOD(IoObject, protosMethod) {
    /*doc Object protos
    Returns a copy of the receiver's protos list.
    */

    IoList *ioList = IoList_new(IOSTATE);
    List *list = IoList_rawList(ioList);
    IOOBJECT_FOREACHPROTO(self, proto, List_append_(list, proto));
    return ioList;
}

// --------------------------------------------------------

// inline
void IoObject_freeSlots(
    IoObject *self) // prepare for io_free and possibly recycle
{
    if (IoObject_ownsSlots(self)) {
        PHash_free(IoObject_slots(self));
        IoObject_slots_(self, NULL);
        IoObject_ownsSlots_(self, 0);
    }

    IoObject_slots_(self, NULL);
}

void IoObject_willFree(IoObject *self) {
    /*
    // disabled until we keep a list of coros and can make sure their stacks are
    marked after the
    // willFree gc stage
    if (IoObject_sentWillFree(self) == 0)
    {
            IoObject *context;
            IoMessage *m = IOSTATE->willFreeMessage;
            IoObject *finalizeSlotValue = IoObject_rawGetSlot_context_(self,
    IoMessage_name(m), &context);

            if (finalizeSlotValue)
            {
                    IoObject_perform(self, self, m);
                    IoObject_sentWillFree_(self, 1);
                    //IoObject_makeGray(self);
            }
    }
    */
}

void IoObject_free(IoObject *self) // prepare for io_free and possibly recycle
{
#ifdef IOSTATE_RECYCLING_ON
    if (List_size(IOSTATE->recycledObjects) >= IOSTATE->maxRecycledObjects)
#endif
    {
        IoObject_dealloc(self);
        // CollectorMarker_free((CollectorMarker *)self);
    }
#ifdef IOSTATE_RECYCLING_ON
    else {
        // printf("recycling %p\n", (void *)self);
        IoObject_rawRemoveAllProtos(self);

#ifdef IOOBJECT_PERSISTENCE
        IoObject_isDirty_(self, 0);
#endif

        IoObject_hasDoneLookup_(self, 0);
        IoObject_isSymbol_(self, 0);
        IoObject_isLocals_(self, 0);
        IoObject_isActivatable_(self, 0);

        // if (IoObject_ownsSlots(self))
        {
            // IoObject_freeSlots(self);
            PHash_clean(IoObject_slots(self));
        }
        /*
        else
        {
                IoObject_slots_(self, NULL);
        }
        */

        List_append_(IOSTATE->recycledObjects, self);
    }
#endif
}

void IoObject_dealloc(IoObject *self) // really io_free it
{
    if (IoObject_markerCount(self) == 0) {
        if (IoObject_listeners(self)) {
            LIST_FOREACH(
                IoObject_listeners(self), i, v,
                IoObject_tag((IoObject *)v)->notificationFunc(v, self));
            List_free(IoObject_listeners(self));
            IoObject_listeners_(self, NULL);
        }

        IoObject_freeData(self);

        if (IoObject_ownsSlots(self)) {
            PHash_free(IoObject_slots(self));
        }

        io_free(IoObject_protos(self));
        // memset(self, 0, sizeof(IoObjectData));
        io_free(self->object);
    } else {
        // printf("IoObject_decrementMarkerCount(%p)\n", (void *)self);
        IoObject_decrementMarkerCount(self)
    }
}

// ----------------------------------------------------------------

IO_METHOD(IoObject, protoCompare) {
    /*doc Object compare(anObject)
    Returns a number containing the comparison value of the target with
    anObject.
    */

    IOASSERT(IoMessage_argCount(m), "compare requires argument");

    {
        IoSymbol *other = IoMessage_locals_valueArgAt_(m, locals, 0);
        return IONUMBER(IoObject_compare(self, other));
    }
}

// slot lookups with lookup loop detection

unsigned int IoObject_rawHasProto_(IoObject *self, IoObject *p) {
    if (self == p) {
        return 1;
    }

    if (IoObject_hasDoneLookup(self)) {
        return 0;
    } else {
        IoObject **proto = IoObject_protos(self);

        IoObject_hasDoneLookup_(self, 1);

        while (*proto) {
            if (IoObject_rawHasProto_(*proto, p)) {
                IoObject_hasDoneLookup_(self, 0);
                return 1;
            }

            proto++;
        }

        IoObject_hasDoneLookup_(self, 0);
        return 0;
    }
}

IO_METHOD(IoObject, protoHasProto_) {
    /*doc Object hasProto(anObject)
    Returns true if anObject is found in the proto path of the target, false
    otherwise.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    return IOBOOL(self, IoObject_rawHasProto_(self, v));
}

// ------------------------------------------------------

IoObject *IoObject_getSlot_(IoObject *self, IoSymbol *slotName) {
    IoObject *v = IoObject_rawGetSlot_(self, slotName);
    return v ? v : IONIL(self);
}

double IoObject_doubleGetSlot_(IoObject *self, IoSymbol *slotName) {
    IoObject *v = IoObject_rawGetSlot_(self, slotName);

    if (!v) {
        IoState_error_(IOSTATE, NULL, "missing slot %s in %s",
                       CSTRING(slotName), IoObject_name(self));
        return 0;
    }

    if (!ISNUMBER(v)) {
        IoState_error_(
            IOSTATE, NULL, "slot %s in %s must be a number, not a %s",
            CSTRING(slotName), IoObject_name(self), IoObject_name(v));
        return 0;
    }

    return CNUMBER(v);
}

IoObject *IoObject_symbolGetSlot_(IoObject *self, IoSymbol *slotName) {
    IoObject *v = IoObject_rawGetSlot_(self, slotName);

    if (!v) {
        IoState_error_(IOSTATE, NULL, "missing slot %s in %s",
                       CSTRING(slotName), IoObject_name(self));
        return NULL;
    }

    if (!ISSYMBOL(v)) {
        IoState_error_(
            IOSTATE, NULL, "slot %s in %s must be a symbol, not a %s",
            CSTRING(slotName), IoObject_name(self), IoObject_name(v));
        return NULL;
    }

    return v;
}

IoObject *IoObject_seqGetSlot_(IoObject *self, IoSymbol *slotName) {
    IoObject *v = IoObject_rawGetSlot_(self, slotName);

    if (!v) {
        IoState_error_(IOSTATE, NULL, "missing slot %s in %s",
                       CSTRING(slotName), IoObject_name(self));
    }

    if (!ISSEQ(v)) {
        IoState_error_(
            IOSTATE, NULL, "slot %s in %s must be a sequence, not a %s",
            CSTRING(slotName), IoObject_name(self), IoObject_name(v));
    }

    return v;
}

IoObject *IoObject_activateFunc(IoObject *self, IoObject *target,
                                IoObject *locals, IoMessage *m,
                                IoObject *slotContext) {
    IoState *state = IOSTATE;

    if (IoObject_isActivatable(self)) {
        IoObject *context;
        IoObject *slotValue =
            IoObject_rawGetSlot_context_(self, state->activateSymbol, &context);

        if (slotValue) {
            // return IoObject_activate(slotValue, self, locals, m, context);
            return IoObject_activate(slotValue, target, locals, m, context);
        }
    }

    return self;
}

// -----------------------------------------------------------

void IoObject_setSlot_to_(IoObject *self, IoSymbol *slotName, IoObject *value) {
    IoObject_inlineSetSlot_to_(self, slotName, value);
}

void IoObject_removeSlot_(IoObject *self, IoSymbol *slotName) {
    IoObject_createSlotsIfNeeded(self);
    PHash_removeKey_(IoObject_slots(self), slotName);
}

IoObject *IoObject_rawGetSlot_target_(IoObject *self, IoSymbol *slotName,
                                      IoObject **target) {
    IoObject *slotValue = IoObject_rawGetSlot_(self, slotName);

    if (!slotValue) {
        IoObject *selfDelegate =
            IoObject_rawGetSlot_(self, IOSTATE->selfSymbol);

        if (selfDelegate && selfDelegate != self) {
            slotValue = IoObject_rawGetSlot_(selfDelegate, slotName);

            if (slotValue) {
                *target = selfDelegate;
            }
        }
    }
    return slotValue;
}

IO_METHOD(IoObject, localsForward) {
    /*doc Object localsForward
    CFunction used by Locals prototype for forwarding.
    */

    // IoObject *selfDelegate = IoObject_rawGetSlot_(self, IOSTATE->selfSymbol);
    IoObject *selfDelegate = PHash_at_(
        IoObject_slots(self), IOSTATE->selfSymbol); // cheating a bit here

    if (selfDelegate && selfDelegate != self) {
        return IoObject_perform(selfDelegate, locals, m);
    }

    return IONIL(self);
}

// name ------------------------------------------------------

IO_METHOD(IoObject, lobbyPrint) {
    /*doc Object print
    Prints a string representation of the object. Returns Nil.
    */

    IoState *state = IOSTATE;
    const char *name = IoObject_name(self);

    IoObject_createSlotsIfNeeded(self);

    IoState_print_(state, "%s_%p do(\n", name, (void *)self, name);
    IoState_print_(state, "  appendProto(");

    {
        IoObject **proto = IoObject_protos(self);

        while (*proto) {
            IoState_print_(state, "%s_%p", name, (void *)*proto, name);
            proto++;

            if (*proto) {
                IoState_print_(state, ", ");
            }
        }
    }

    IoState_print_(state, ")\n");

    return state->ioNil;
}

size_t IoObject_memorySizeFunc(IoObject *self) {
    return sizeof(IoObjectData) + (IoObject_ownsSlots(self)
                                       ? PHash_memorySize(IoObject_slots(self))
                                       : 0);
}

void IoObject_compactFunc(IoObject *self) {
    PHash_compact(IoObject_slots(self));
}

// proto methods ----------------------------------------------

IO_METHOD(IoObject, protoPerform) {
    /*doc Object perform(methodName, <arg1>, <arg2>, ...)
    Performs the method corresponding to methodName with the arguments supplied.
    */

    IoObject *slotName = IoMessage_locals_valueArgAt_(m, locals, 0);

    if (ISMESSAGE(slotName)) {
        IOASSERT(IoMessage_argCount(m) == 1,
                 "perform takes a single argument when using a Message as an "
                 "argument");
        return IoObject_perform(self, locals, slotName);
    }

    IOASSERT(ISSYMBOL(slotName),
             "perform requires a Symbol or Message argument");

    {
        IoObject *context;
        IoObject *v = IoObject_rawGetSlot_context_(self, slotName, &context);
        IoMessage *newMessage = IoMessage_newWithName_label_(
            IOSTATE, slotName, IoMessage_rawLabel(m));
        IoMessage_rawSetLineNumber_(newMessage, IoMessage_rawLineNumber(m));
        if (v) {
            int i;
            List *args = IoMessage_rawArgList(m);

            for (i = 1; i < List_size(args); i++) {
                IoMessage_addArg_(newMessage,
                                  IoMessage_deepCopyOf_(List_at_(args, i)));
            }

            return IoObject_activate(v, self, locals, newMessage, context);
        }

        return IoObject_forward(self, locals, newMessage);
    }

    return IoObject_forward(self, locals, m);
}

IO_METHOD(IoObject, protoPerformWithArgList) {
    /*doc Object performWithArgList(methodName, argList)
    Performs the method corresponding to methodName with the arguments in the
    argList.
    */

    IoSymbol *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoList *args = IoMessage_locals_listArgAt_(m, locals, 1);
    List *argList = IoList_rawList(args);
    IoObject *context;
    IoObject *v = IoObject_rawGetSlot_context_(self, slotName, &context);

    if (v) {
        IoMessage *newMessage = IoMessage_newWithName_(IOSTATE, slotName);
        size_t i, max = List_size(argList);

        for (i = 0; i < max; i++) {
            IoMessage_addCachedArg_(newMessage, LIST_AT_(argList, i));
        }

        return IoObject_activate(v, self, locals, newMessage, context);
    }

    return IoObject_forward(self, locals, m);
}

IO_METHOD(IoObject, protoWrite) {
    /*doc Object write(<any number of arguments>)
    Sends a print message to the evaluated result of each argument. Returns Nil.
    */

    int n, max = IoMessage_argCount(m);
    IoState *state = IOSTATE;

    for (n = 0; n < max; n++) {
        IoObject *v = IoMessage_locals_valueArgAt_(m, locals, n);
        IoMessage_locals_performOn_(state->printMessage, locals, v);
    }

    return IONIL(self);
}

IO_METHOD(IoObject, protoWriteLn) {
    /*doc Object writeln(<any number of arguments>)
    Same as write() but also writes a return character at the end. Returns Nil.
    */

    IoObject_protoWrite(self, locals, m);
    IoState_print_(IOSTATE, "\n");
    return IONIL(self);
}

// inline
IoObject *IoObject_initClone_(IoObject *self, IoObject *locals, IoMessage *m,
                              IoObject *newObject) {
    IoState *state = IOSTATE;
    IoObject *context;
    IoObject *initSlotValue = IoObject_rawGetSlot_context_(
        newObject, IoMessage_name(state->initMessage), &context);

    if (initSlotValue) {
        IoObject_activate(initSlotValue, newObject, locals, state->initMessage,
                          context);
    }

    return newObject;
}

IoObject *IOCLONE(IoObject *self) {
    IoState *state = IOSTATE;
    IoObject *newObject;

    IoState_pushCollectorPause(state);
    newObject = IoObject_tag(self)->cloneFunc(self);
    IoState_addValueIfNecessary_(state, newObject);
    IoState_popCollectorPause(state);
    return newObject;
}

IO_METHOD(IoObject, clone) {
    /*doc Object clone
    Returns a clone of the receiver.
    */

    IoObject *newObject = IOCLONE(self);
    return IoObject_initClone_(self, locals, m, newObject);
}

IO_METHOD(IoObject, cloneWithoutInit) {
    /*doc Object cloneWithoutInit
    Returns a clone of the receiver but does not call init.
    */

    return IOCLONE(self);
}

IO_METHOD(IoObject, shallowCopy) {
    /*doc Object shallowCopy
    Returns a shallow copy of the receiver.
    */

    IOASSERT(ISOBJECT(self), "shallowCopy doesn't work on primitives");

    {
        IoObject *newObject = IoObject_new(IOSTATE);
        PHASH_FOREACH(IoObject_slots(self), k, v,
                      IoObject_setSlot_to_(newObject, k, v));
        return newObject;
    }
}

// lobby methods ----------------------------------------------

IO_METHOD(IoObject, protoSet_to_) {
    /*doc Object setSlot(slotNameString, valueObject)
    Sets the slot slotNameString in the receiver to
    hold valueObject. Returns valueObject.
    */

    IoSymbol *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoObject *slotValue = IoMessage_locals_valueArgAt_(m, locals, 1);
    IoObject_inlineSetSlot_to_(self, slotName, slotValue);
    return slotValue;
}

IO_METHOD(IoObject, protoSetSlotWithType) {
    /*doc Object setSlotWithType(slotNameString, valueObject)
    Sets the slot slotNameString in the receiver to
    hold valueObject and sets the type slot of valueObject
    to be slotNameString. Returns valueObject.
    */

    IoSymbol *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoObject *slotValue = IoMessage_locals_valueArgAt_(m, locals, 1);
    IoObject_inlineSetSlot_to_(self, slotName, slotValue);
    IoObject_createSlotsIfNeeded(slotValue);
    if (PHash_at_(IoObject_slots(slotValue), IOSTATE->typeSymbol) == NULL) {
        IoObject_inlineSetSlot_to_(slotValue, IOSTATE->typeSymbol, slotName);
    }
    return slotValue;
}

IO_METHOD(IoObject, localsUpdateSlot) {
    /*doc Object localsUpdateSlot(slotNameString, valueObject)
    Local's version of updateSlot mthod.
    */

    IoSymbol *slotName = IoMessage_locals_firstStringArg(m, locals);
    // IoSymbol *slotName  = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoObject *obj = IoObject_rawGetSlot_(self, slotName);

    if (obj) {
        // IoObject *slotValue = IoMessage_locals_valueArgAt_(m, locals, 1);
        IoObject *slotValue = IoMessage_locals_quickValueArgAt_(m, locals, 1);
        IoObject_inlineSetSlot_to_(self, slotName, slotValue);
        return slotValue;
    } else {
        IoObject *theSelf = IoObject_rawGetSlot_(self, IOSTATE->selfSymbol);

        if (theSelf) {
            return IoObject_perform(theSelf, locals, m);
        }
    }

    IoState_error_(IOSTATE, m,
                   "updateSlot - slot with name `%s' not found in `%s'. Use := "
                   "to create slots.",
                   CSTRING(slotName), IoObject_name(self));

    return IONIL(self);
}

IO_METHOD(IoObject, protoUpdateSlot_to_) {
    /*doc Object updateSlot(slotNameString, valueObject)
    Same as setSlot(), but raises an error if the slot does not
    already exist in the receiver's slot lookup path.
    */

    IoSymbol *slotName = IoMessage_locals_firstStringArg(m, locals);
    IoObject *slotValue = IoMessage_locals_quickValueArgAt_(m, locals, 1);
    IoObject *obj = IoObject_rawGetSlot_(self, slotName);

    if (obj) {
        IoObject_inlineSetSlot_to_(self, slotName, slotValue);
    } else {
        IoState_error_(IOSTATE, m,
                       "Slot %s not found. Must define slot using := operator "
                       "before updating.",
                       CSTRING(slotName));
    }

    return slotValue;
}

IO_METHOD(IoObject, protoGetSlot_) {
    /*doc Object getSlot(slotNameString)
    Returns the value of the slot named slotNameString
    (following the lookup path) or nil if no such slot is found.
    */

    IoSymbol *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    return IoObject_getSlot_(self, slotName);
}

IO_METHOD(IoObject, protoGetLocalSlot_) {
    /*doc Object getLocalSlot(slotNameString)
    Returns the value of the slot named slotNameString
    (not looking in the object's protos) or nil if no such slot is found.
    */

    IoSymbol *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);

    if (IoObject_ownsSlots(self)) {
        IoObject *v = PHash_at_(IoObject_slots(self), slotName);
        if (v)
            return v;
    }

    return IONIL(self);
}

IO_METHOD(IoObject, protoHasLocalSlot) {
    /*doc Object hasLocalSlot(slotNameString)
    Returns true if the slot exists in the receiver or false otherwise.
    */

    IoSymbol *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoObject_createSlotsIfNeeded(self);
    return IOBOOL(self, PHash_at_(IoObject_slots(self), slotName) != NULL);
}

IO_METHOD(IoObject, protoRemoveSlot) {
    /*doc Object removeSlot(slotNameString)
    Removes the specified slot (only) in the receiver if it exists. Returns
    self.
    */

    IoSymbol *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoObject_createSlotsIfNeeded(self);
    PHash_removeKey_(IoObject_slots(self), slotName);
    return self;
}

IO_METHOD(IoObject, protoRemoveAllSlots) {
    /*doc Object removeAllSlots
    Removes all of the receiver's slots. Returns self.
    */

    PHash_clean(IoObject_slots(self));
    return self;
}

IO_METHOD(IoObject, protoSlotNames) {
    /*doc Object slotNames
    Returns a list of strings containing the names of the
    slots in the receiver (but not in its lookup path).
    */

    IoObject_createSlotsIfNeeded(self);

    {
        IoList *slotNames = IoList_new(IOSTATE);
        PHASH_FOREACH(IoObject_slots(self), key, value,
                      IoList_rawAppend_(slotNames, key););
        return slotNames;
    }
}

IO_METHOD(IoObject, protoSlotValues) {
    /*doc Object slotValues
    Returns a list of the values held in the slots of the receiver.
    */

    IoObject_createSlotsIfNeeded(self);

    {
        IoList *slotNames = IoList_new(IOSTATE);
        PHASH_FOREACH(IoObject_slots(self), key, value,
                      IoList_rawAppend_(slotNames, value););
        return slotNames;
    }
}

/*doc Object forward
Called when the receiver is sent a message it doesn't recognize.
Default implementation raises an "Object doesNotRespond" exception.
Subclasses can override this method to implement proxies or special error
handling. <p> Example: <p> <pre> myProxy forward = method( messageName :=
thisMessage name arguments := thisMessage arguments myObject
doMessage(thisMessage)
)
</pre>
*/

/*
IO_METHOD(IoObject, forward_)
{

        IoState_error_(IOSTATE, m, "%s does not respond to message '%s'",
                                   IoObject_name(self),
                                   CSTRING(IoMessage_name(m)));
        return IONIL(self);
}
*/

IO_METHOD(IoObject, ancestorWithSlot) {
    /*doc Object ancestorWithSlot(slotName)
    Returns the first ancestor of the receiver that contains
    a slot of the specified name or Nil if none is found.
    */

    IoObject *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoObject **proto = IoObject_protos(self);

    while (*proto) {
        IoObject *context = NULL;
        IoObject *v =
            IoObject_rawGetSlot_context_((*proto), slotName, &context);

        if (v) {
            return context;
        }

        proto++;
    }

    return IONIL(self);
}

IO_METHOD(IoObject, contextWithSlot) {
    /*doc Object contextWithSlot(slotName)
    Returns the first context (starting with the receiver and following the
    lookup path) that contains a slot of the specified name or Nil if none is
    found.
    */

    IoObject *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoObject *context = NULL;
    IoObject_rawGetSlot_context_(self, slotName, &context);
    return context ? context : IONIL(self);
}

// ---------------------------------------------------------------------------

IoObject *IoObject_rawDoString_label_(IoObject *self, IoSymbol *string,
                                      IoSymbol *label) {
    IoMessage *cm = NULL;
    IoMessage *messageForString = NULL;
    IoMessage *newMessage = NULL;
    IoState *state = IOSTATE;

    if (!ISSEQ(string)) {
        IoState_error_(
            state, NULL,
            "IoObject_rawDoString_label_ requires a string argument");
    }

    {
        IoSymbol *internal;
        IoState_pushCollectorPause(state);

        internal = IOSYMBOL("[internal]");
        cm =
            IoMessage_newWithName_label_(state, IOSYMBOL("Compiler"), internal);
        messageForString = IoMessage_newWithName_label_(
            state, IOSYMBOL("messageForString"), internal);

        IoMessage_rawSetNext_(cm, messageForString);
        IoMessage_addCachedArg_(messageForString, string);
        IoMessage_addCachedArg_(messageForString, label);

        newMessage = IoMessage_locals_performOn_(cm, self, self);
        IoState_stackRetain_(state, newMessage); // needed?

        IoState_popCollectorPause(state);

        if (newMessage) {
            return IoMessage_locals_performOn_(newMessage, self, self);
        }

        IoState_error_(state, NULL, "no message compiled\n");
        return IONIL(self);
    }
}

IO_METHOD(IoObject, doMessage) {
    /*doc Object doMessage(aMessage, optionalContext)
    Evaluates the message object in the context of the receiver.
    Returns the result. optionalContext can be used to specific the locals
    context in which the message is evaluated.
    */

    IoMessage *aMessage = IoMessage_locals_messageArgAt_(m, locals, 0);
    IoObject *context = self;

    if (IoMessage_argCount(m) >= 2) {
        context = IoMessage_locals_valueArgAt_(m, locals, 1);
    }

    return IoMessage_locals_performOn_(aMessage, context, self);
}

IO_METHOD(IoObject, doString) {
    /*doc Object doString(aString)
    Evaluates the string in the context of the receiver. Returns the result.
    */

    IoSymbol *string = IoMessage_locals_seqArgAt_(m, locals, 0);
    IoSymbol *label;
    IoObject *result;

    if (IoMessage_argCount(m) > 1) {
        label = IoMessage_locals_symbolArgAt_(m, locals, 1);
    } else {
        label = IOSYMBOL("doString");
    }

    IoState_pushRetainPool(IOSTATE);
    result = IoObject_rawDoString_label_(self, string, label);
    IoState_popRetainPoolExceptFor_(IOSTATE, result);
    return result;
}

IO_METHOD(IoObject, doFile) {
    /*doc Object doFile(pathString)
    Evaluates the File in the context of the receiver. Returns the result.
    pathString is relative to the current working directory.
    */

    IoSymbol *path = IoMessage_locals_symbolArgAt_(m, locals, 0);
    IoFile *file = IoFile_newWithPath_(IOSTATE, path);
    IoSymbol *string =
        (IoSymbol *)IoSeq_rawAsSymbol(IoFile_contents(file, locals, m));

    if (IoSeq_rawSize(string)) {
        return IoObject_rawDoString_label_(self, string, path);
    } else {
        return IONIL(self);
    }
}

IO_METHOD(IoObject, isIdenticalTo) {
    /*doc Object isIdenticalTo(aValue)
    Returns true if the receiver is identical to aValue, false otherwise.
    */

    IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
    return IOBOOL(self, self == other);
}

IO_METHOD(IoObject, equals) {
    /*doc Object ==(aValue)
    Returns true if receiver and aValue are equal, false otherwise.
*/

    IOASSERT(IoMessage_argCount(m), "compare requires argument");

    {
        IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
        return IOBOOL(self, IoObject_compare(self, other) == 0);
    }
}

IO_METHOD(IoObject, notEquals) {
    /*doc Object !=(aValue)
    Returns true the receiver is not equal to aValue, false otherwise.
    */

    IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
    return IOBOOL(self, IoObject_compare(self, other) != 0);
}

IO_METHOD(IoObject, foreachSlot) {
    /*doc Object foreach([name,] value, message)
    For each slot, set name to the slot's
    name and value to the slot's value and execute message. Examples:
    <p>
    <pre>
    myObject foreach(n, v,
            writeln("slot ", n, " = ", v type)
    )

    myObject foreach(v,
            writeln("slot type ", v type)
    )
    </pre>
    */

    IoSymbol *keyName;
    IoSymbol *valueName;
    IoMessage *doMessage;
    IoObject *result = IONIL(self);

    IoState_pushRetainPool(IOSTATE);
    IoMessage_foreachArgs(m, self, &keyName, &valueName, &doMessage);

    PHASH_FOREACH(
        IoObject_slots(self), key, value, IoState_clearTopPool(IOSTATE);

        if (keyName) { IoObject_setSlot_to_(locals, keyName, key); }

        IoObject_setSlot_to_(locals, valueName, value);
        result = IoMessage_locals_performOn_(doMessage, locals, locals);

        if (IoState_handleStatus(IOSTATE)) { goto done; });
done:
    IoState_popRetainPoolExceptFor_(IOSTATE, result);
    return result;
}

IO_METHOD(IoObject, subtract) {
    /*doc Object -(aNumber)
    Returns the negative version of aNumber.
    Raises an exception if argument is not a number.
    */

    IoNumber *num = IoMessage_locals_numberArgAt_(m, locals, 0);
    return IONUMBER(-IoNumber_asDouble(num));
}

IO_METHOD(IoObject, self) {
    /*doc Object self
    Returns self.
    */
    /*doc Object thisContext
    Synonym to self.
    */

    return self;
}

IO_METHOD(IoObject, thisMessage) {
    /*doc Object thisMessage
    Returns the calling message (i.e. thisMessage itself, huh).
    */

    return m;
}

IO_METHOD(IoObject, locals) {
    /*doc Object thisLocalContext
    Returns current locals.
    */

    return locals;
}

// message callbacks --------------------------------------

const char *IoObject_name(IoObject *self) {
    // If self has a type slot which is a string, then use that instead of the
    // tag name
    IoObject *type = IoObject_rawGetSlot_(self, IOSYMBOL("type"));
    if (type && ISSEQ(type)) {
        return CSTRING(type);
    }

    return IoTag_name(IoObject_tag(self));
}

int IoObject_compare(IoObject *self, IoObject *v) {
    if (self == v) {
        return 0;
    }

    if (IoObject_tag(self)->compareFunc) {
        return (IoObject_tag(self)->compareFunc)(self, v);
    }

    return IoObject_defaultCompare(self, v);
}

int IoObject_defaultCompare(IoObject *self, IoObject *v) {

    // IoState_error_(IOSTATE, NULL, "attempt to compare %s to %s",
    // IoObject_name(self), IoObject_name(v)); return 0;

    ptrdiff_t d = -((ptrdiff_t)IoObject_tag(self) - (ptrdiff_t)IoObject_tag(v));
    // printf("warning: IoObject_defaultCompare attempt to compare %s to %s\n",
    // IoObject_name(self), IoObject_name(v));

    if (d == 0) {
        d = ((ptrdiff_t)self) - ((ptrdiff_t)v);
    }

    if (d == 0)
        return 0;

    return d > 0 ? 1 : -1;
}

int IoObject_sortCompare(IoObject **self, IoObject **v) {
    return IoObject_compare(*self, *v);
}

void IoObject_compact(IoObject *self) {
    /*
    if (IoObject_tag(self)->compactFunc)
    {
            (IoObject_tag(self)->compactFunc)(self);
    }
    */
}

// lobby methods ----------------------------------------------

IO_METHOD(IoObject, memorySizeMethod) {
    /*doc Object memorySize
    Return the amount of memory used by the object.
    */

    return IONUMBER(IoObject_memorySize(self));
}

/*doc Object checkMemory()
        Accesses memory in the IoObjectData struct that should be accessible.
   Should cause a memory access exception if memory is corrupt.
        */
int IoObject_rawCheckMemory(IoObject *self) { return IOCOLLECTOR != 0x0; }

IO_METHOD(IoObject, compactMethod) {
    /*doc Object compact
    Compact the memory for the object if possible. Returns self.
    */

    IoObject_compact(self);
    return self;
}

IO_METHOD(IoObject, type) {
    /*doc Object type
    Returns a string containing the name of the type of Object (Number, String,
    etc).
    */

    return IOSYMBOL((char *)IoObject_name(self));
}

void IoObject_defaultPrint(IoObject *self) {
    if (ISSYMBOL(self)) {
        IoSeq_rawPrint(self);
    } else if (ISNUMBER(self)) {
        IoNumber_print(self);
    } else {
        IoState_print_(IOSTATE, "%s_%p", IoObject_name(self), self);

        if (ISMESSAGE(self)) {
            IoState_print_(IOSTATE, " '%s'", CSTRING(IoMessage_name(self)));
        }
    }
}

void IoObject_print(IoObject *self) {
    IoMessage_locals_performOn_(IOSTATE->printMessage, self, self);
    // using self as locals hack
}

/*
IO_METHOD(IoObject, truthValueOfArg);
{
        //-doc Object The '' method evaluates the argument and returns the
result.

        IOASSERT(IoMessage_argCount(m) > 0, "argument required");
        return IoMessage_locals_valueArgAt_(m, locals, 0);
}
*/

IO_METHOD(IoObject, evalArg) {
    /*doc Object (expression)
    The '' method evaluates the argument and returns the result.
    */

    /*doc Object evalArg(expression)
    The '' method evaluates the argument and returns the result.
    */

    IOASSERT(IoMessage_argCount(m) > 0, "argument required");
    /* eval the arg and return a non-Nil so an attached else() won't get
     * performed */
    return IoMessage_locals_valueArgAt_(m, locals, 0);
}

IO_METHOD(IoObject, evalArgAndReturnSelf) {
    /*doc Object evalArgAndReturnSelf(expression)
    Evaluates the argument and returns the target.
    */

    IoObject_evalArg(self, locals, m);
    return self;
}

IO_METHOD(IoObject, evalArgAndReturnNil) {
    /*doc Object evalArgAndReturnNil(expression)
    Evaluates the argument and returns nil.
    */

    IoObject_evalArg(self, locals, m);
    return IONIL(self);
}

IO_METHOD(IoObject, isLessThan_) {
    /*doc Object <(expression)
    Evaluates argument and returns self if self is less or Nil if not.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    return IOBOOL(self, IoObject_compare(self, v) < 0);
}

IO_METHOD(IoObject, isLessThanOrEqualTo_) {
    /*doc Object <=(expression)
    Evaluates argument and returns self if self is less
    than or equal to it, or Nil if not.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    return IOBOOL(self, IoObject_compare(self, v) <= 0);
}

IO_METHOD(IoObject, asBoolean) {
    if (self == IOSTATE->ioFalse || self == IOSTATE->ioNil)
        return self;
    else
        return IOSTATE->ioTrue;
}

IO_METHOD(IoObject, isGreaterThan_) {
    /*doc Object >(expression)
    Evaluates argument and returns self if self is greater than it, or Nil if
    not.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    return IOBOOL(self, IoObject_compare(self, v) > 0);
}

IO_METHOD(IoObject, isGreaterThanOrEqualTo_) {
    /*doc Object >=(expression)
    Evaluates argument and returns self if self is greater
    than or equal to it, or Nil if not.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    return IOBOOL(self, IoObject_compare(self, v) >= 0);
}

IO_METHOD(IoObject, uniqueId) {
    /*doc Object uniqueId
    Returns a Number containing a unique id for the receiver.
    */
    char s[32];
#if defined(_MSC_VER) || defined(__MINGW32__)
    sprintf(s, "0x%p", (void *)IoObject_deref(self));
#else
    sprintf(s, "%p", (void *)IoObject_deref(self));
#endif
    return IOSYMBOL(s);
    // return IONUMBER((double)((size_t)IoObject_deref(self)));
}

IO_METHOD(IoObject, do) {
    /*doc Object do(expression)
    Evaluates the message in the context of the receiver. Returns self.
    */

    if (IoMessage_argCount(m) != 0) {
        IoMessage *argMessage = IoMessage_rawArgAt_(m, 0);
        IoMessage_locals_performOn_(argMessage, self, self);
    }

    return self;
}

IO_METHOD(IoObject, lexicalDo) {
    /*doc Object lexicalDo(expression)
    Evaluates the message in the context of the receiver.
    The lexical context is added as a proto of the receiver while the argument
    is evaluated. Returns self.
    */

    if (IoMessage_argCount(m) != 0) {
        IoMessage *argMessage = IoMessage_rawArgAt_(m, 0);
        IoObject_rawAppendProto_(self, locals);
        IoMessage_locals_performOn_(argMessage, self, self);
        IoObject_rawRemoveProto_(self, locals);
    }

    return self;
}

IO_METHOD(IoObject, message) {
    /*doc Object message(expression)
    Return the message object for the argument or Nil if there is no argument.
    Note: returned object is a mutable singleton. Use "message(foo) clone" if
you wish to modify it.
    */

    return IoMessage_argCount(m) ? IoMessage_rawArgAt_(m, 0) : IONIL(self);
}

// inline these -------------------------------------------------

int IoObject_hasCloneFunc_(IoObject *self, IoTagCloneFunc *func) {
    return (IoObject_tag(self)->cloneFunc == func);
}

// --------------------------------------------

char *IoObject_markColorName(IoObject *self) {
    return Collector_colorNameFor_(IOCOLLECTOR, self);
}

void IoSymbol_println(IoSymbol *self) { printf("%s\n", CSTRING(self)); }

void IoObject_show(IoObject *self) {
    printf("  %p %s\n", (void *)self, IoObject_name(self));
    // PHash_doOnKeys_(IoObject_slots(self), (PHashDoCallback
    // *)IoSymbol_println);
    PHASH_FOREACH(IoObject_slots(self), k, v, IoSymbol_println(k));
}

IO_METHOD(IoObject, setIsActivatableMethod) {
    /*doc Object setIsActivatable(aValue)
    When called with a non-Nil aValue, sets the object
    to call its activate slot when accessed as a value. Turns this behavior
    off if aValue is Nil. Only works on Objects which are not Activatable
    Primitives (such as CFunction or Block). Returns self.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoObject *objectProto = IoState_protoWithId_(IOSTATE, protoId);

    IoTag_activateFunc_(IoObject_tag(objectProto),
                        (IoTagActivateFunc *)IoObject_activateFunc);

    IoObject_isActivatable_(self, ISTRUE(v)) return self;
}

IO_METHOD(IoObject, isActivatableMethod) {
    /*doc Object isActivatable
    Returns true if the receiver is activatable, false otherwise.
    */

    return IoObject_isActivatable(self) ? IOTRUE(self) : IOFALSE(self);
}

/*
IoNumber *IoObject_getNumberSlot(IoObject *self,
                                                                IoObject
*locals, IoMessage *m, IoSymbol *slotName)
{
        IoObject *v  = IoObject_getSlot_(self, slotName);
        IOASSERT(ISNUMBER(v),  CSTRING(slotName));
        return v;
}
*/

UArray *IoObject_rawGetUArraySlot(IoObject *self, IoObject *locals,
                                  IoMessage *m, IoSymbol *slotName) {
    IoSeq *seq = IoObject_getSlot_(self, slotName);
    IOASSERT(ISSEQ(seq), CSTRING(slotName));
    return IoSeq_rawUArray(seq);
}

UArray *IoObject_rawGetMutableUArraySlot(IoObject *self, IoObject *locals,
                                         IoMessage *m, IoSymbol *slotName) {
    IoSeq *seq = IoObject_getSlot_(self, slotName);
    IOASSERT(ISSEQ(seq), CSTRING(slotName));
    return IoSeq_rawUArray(seq);
}

IO_METHOD(IoObject, argIsActivationRecord) {
    /*doc Object argIsActivationRecord
    Note: seems to be an obsolete method.
          */
    return IOBOOL(self,
                  PHash_at_(IoObject_slots(self), IOSTATE->callSymbol) != NULL);
}

/*void *testStack(void *a)
{
        int v[256];
        memset(v, 0x1, 128);
        return a + v[0];
}*/

IO_METHOD(IoObject, argIsCall) {
    /*doc Object argIsCall(arg)
          Returns true if arg is an activation context (i.e. Call object)
          <br/>
          Note: this is used internally in one place only (Coroutine callStack).
          Refactoring should be considered.
          */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
    // printf("IoObject_tag(v)->name = '%s'\n", IoObject_tag(v)->name);

    /*
            printf("Collector_checkObjectPointers\n");
            Collector_check(IOSTATE->collector);
            Collector_checkObjectPointers(IOSTATE->collector);
            printf("Collector_checkObjectPointers done\n");
            printf("self = %p\n", self);
            printf("v    = %p\n", v);
            printf("tag  = %p\n", IoObject_tag(self));
            */
    /*
            printf("tag->cloneFunc   = %p\n", IoObject_tag(v)->cloneFunc);
            printf("IoCall_rawClone  = %p\n", IoCall_rawClone);

            //testStack(self);
    <<<<<<< HEAD


            printf("ISACTIVATIONCONTEXT = %i\n", isAct);
    */

    int isAct =
        ((void *)(IoObject_tag(v)->cloneFunc) == (void *)IoCall_rawClone);
    IoObject *t = IOSTATE->ioTrue;  // IOTRUE(self);
    IoObject *f = IOSTATE->ioFalse; // IOFALSE(self);
    // return isAct ? t : f;
    if (isAct) {
        return t;
    }
    return f;
    // return IOBOOL(self, ISACTIVATIONCONTEXT(v));

    /*
            int isAct = ((void *)(IoObject_tag(self)->cloneFunc) == (void
    *)IoCall_rawClone);

    //	printf("ISACTIVATIONCONTEXT = %i\n", isAct);

            void *t = IOTRUE(self);
            void *f = IOFALSE(self);
            return isAct ? t : f;
    */

    return IOBOOL(self, ISACTIVATIONCONTEXT(v));
}

IO_METHOD(IoObject, become) {
    /*doc Object become(anotherObject)
          Replaces receiver with <tt>anotherObject</tt> and returns self.
          Useful for implementing transparent proxies. See also
       <tt>FutureProxy</tt> and <tt>Object @</tt>. <br/> Note: primitives cannot
       become new values.
          */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);

    if (self == v || IoObject_deref(v) == IoObject_deref(self))
        return self;

    // IOASSERT(!IoObject_isSymbol(self), "Symbols cannot become new values");
    IOASSERT(ISOBJECT(self), "Primitives cannot become new values");
    // printf("IoObject_become(%p, %p) data %p\n", (void *)self, (void *)v,
    // (void *)IoObject_deref(v));
    IoObject_incrementMarkerCount(v);
    IoObject_dealloc(self);
    CollectorMarker_setObject_(self, IoObject_deref(v));
    return self;
}

IOVM_API IoObject *IoObject_hasDirtySlot_(IoObject *self, IoObject *locals,
                                          IoMessage *m) {
    // IoSymbol *slotName = IoMessage_locals_symbolArgAt_(m, locals, 0);
    int result =
        PHash_hasDirtyKey_(IoObject_slots(self),
                           IOREF(IoMessage_locals_symbolArgAt_(m, locals, 0)));
    return IOBOOL(self, result);
}

void IoObject_protoClean(IoObject *self) {
    IoObject_isDirty_(self, 0);
    PHash_cleanSlots(IoObject_slots(self));
}

IO_METHOD(IoObject, markClean) {
    // doc Object markClean Cleans object's slots.
    PHash_cleanSlots(IoObject_slots(self));
    return self;
}

// io_free listeners ---------------------------------------------

void IoObject_addListener_(IoObject *self, void *listener) {
    if (IoObject_listeners(self) == NULL) {
        IoObject_listeners_(self, List_new());
    }

    List_append_(IoObject_listeners(self), listener);
}

void IoObject_removeListener_(IoObject *self, void *listener) {
    List *listeners = IoObject_listeners(self);

    if (listeners) {
        List_remove_(listeners, listener);

        if (List_size(listeners) == 0) {
            List_free(listeners);
            IoObject_listeners_(self, NULL);
        }
    }
}

// persistence ------------------------------------------------

/*
PID_TYPE IoObject_pid(IoObject *self)
{
        return 0;
}
*/

// asString helper

IoSeq *IoObject_asString_(IoObject *self, IoMessage *m) {
    IoSeq *result =
        IoMessage_locals_performOn_(IOSTATE->asStringMessage, self, self);
    /*
    IoSymbol *string = IOSYMBOL("asString");
    IoSymbol *label = string;
    IoSeq *result;

    IoState_pushRetainPool(IOSTATE);

    result = IoObject_rawDoString_label_(self, string, label);
    IoState_popRetainPoolExceptFor_(IOSTATE, result);
    */

    if (!ISSEQ(result)) {
        IoState_error_(IOSTATE, m, "%s asString didn't return a Sequence",
                       IoObject_name(self));
    }

    return result;
}
