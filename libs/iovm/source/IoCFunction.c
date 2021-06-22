// metadoc CFunction copyright Steve Dekorte 2002
// metadoc CFunction license BSD revised
/*metadoc CFunction description
A container for a pointer to a C function binding.
CFunction's can only be defined from the C side and act
like blocks in that when placed in a slot, are called when the
slot is activated. The for, if, while and clone methods of the Lobby
are examples of CFunctions. CFunctions are useful for implementing
methods that require the speed of C or binding to a C library.
*/
// metadoc CFunction category Core

#include "IoCFunction.h"

#include "IoState.h"
#include "IoNumber.h"
#include <stddef.h>

static const char *protoId = "CFunction";

#define DATA(self) ((IoCFunctionData *)IoObject_dataPointer(self))

IoTag *IoCFunction_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCFunction_rawClone);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCFunction_mark);
    IoTag_activateFunc_(tag, (IoTagActivateFunc *)IoCFunction_activate);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCFunction_free);
    return tag;
}

IoCFunction *IoCFunction_proto(void *state) {
    IoObject *self = IoObject_new(state);
    IoObject_tag_(self, IoCFunction_newTag(state));

    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoCFunctionData)));
    DATA(self)->func = IoObject_self;
    // IoObject_isActivatable_(self, 1);
    IoState_registerProtoWithId_((IoState *)state, self, protoId);
    return self;
}

IoCFunction *IoCFunction_rawClone(IoCFunction *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self,
                             cpalloc(DATA(proto), sizeof(IoCFunctionData)));
    IoObject_isActivatable_(self, 1);
    return self;
}

void IoCFunction_mark(IoCFunction *self) {
    if (DATA(self)->uniqueName) {
        IoObject_shouldMark(DATA(self)->uniqueName);
    }
}

void IoCFunction_free(IoCFunction *self) {
    io_free(IoObject_dataPointer(self));
}

void IoCFunction_print(IoCFunction *self) {
    IoCFunctionData *data = DATA(self);

    printf("CFunction_%p", self);
    printf(" %p", (data->func));
    printf(" %s", data->typeTag ? data->typeTag->name : "?");
    if (data->uniqueName)
        printf(" %s", CSTRING(data->uniqueName));
    printf("\n");
}

IoCFunction *IoCFunction_newWithFunctionPointer_tag_name_(
    void *state, IoUserFunction *func, IoTag *typeTag, const char *funcName) {
    IoCFunction *proto = IoState_protoWithId_((IoState *)state, protoId);
    IoCFunction *self = IOCLONE(proto);
    DATA(self)->typeTag = typeTag;
    DATA(self)->func = func;
    DATA(self)->uniqueName =
        IoState_symbolWithCString_((IoState *)state, funcName);
    return self;
}

IO_METHOD(IoCFunction, id) {
    /*doc CFunction id
    Returns a number containing a unique id for the receiver's internal C
    function.
    */

    return IONUMBER(((uintptr_t)self));
}

IO_METHOD(IoCFunction, uniqueName) {
    /*doc CFunction uniqueName
    Returns the name given to the CFunction.
    */

    if (DATA(self)->uniqueName) {
        return DATA(self)->uniqueName;
    }

    return IONIL(self);
}

IO_METHOD(IoCFunction, typeName) {
    /*doc CFunction typeName
    Returns the owning type of the CFunction or nil if the CFunction can be
    called on any object.
    */

    if (DATA(self)->typeTag) {
        return IOSYMBOL(IoTag_name(DATA(self)->typeTag));
    }

    return IONIL(self);
}

IO_METHOD(IoCFunction, equals) {
    /*doc CFunction ==(anObject)
    Returns self if the argument is a CFunction with the same internal C
    function pointer.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);

    return IOBOOL(self, ISCFUNCTION(v) && (DATA(self)->func == DATA(v)->func));
}

IO_METHOD(IoCFunction, profilerTime) {
    /*doc IoCFunction profilerTime
    Returns clock() time spent in compiler in seconds.
    */

    return IONUMBER(((double)DATA(self)->profilerTime) /
                    ((double)CLOCKS_PER_SEC));
}

IoObject *IoCFunction_activateWithProfiler(IoCFunction *self, IoObject *target,
                                           IoObject *locals, IoMessage *m,
                                           IoObject *slotContext) {
    clock_t profilerMark = clock();
    IoObject *result =
        IoCFunction_activate(self, target, locals, m, slotContext);
    DATA(self)->profilerTime += clock() - profilerMark;
    return result;
}

IO_METHOD(IoCFunction, setProfilerOn) {
    /*doc IoCFunction setProfilerOn(aBool)
    If aBool is true, the global block profiler is enabled, if false it is
    disabled. Returns self.
    */

    IoObject *aBool = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoTag *tag = IoObject_tag(self);

    if (ISTRUE(aBool)) {
        IoTag_activateFunc_(
            tag, (IoTagActivateFunc *)IoCFunction_activateWithProfiler);
    } else {
        IoTag_activateFunc_(tag, (IoTagActivateFunc *)IoCFunction_activate);
    }

    return self;
}

IoObject *IoCFunction_activate(IoCFunction *self, IoObject *target,
                               IoObject *locals, IoMessage *m,
                               IoObject *slotContext) {
    IoCFunctionData *selfData = DATA(self);
    IoTag *t = selfData->typeTag;
    // IoObject_waitOnFutureIfNeeded(target); future forward will already deal
    // with this?
    IoObject *result;

    if (t &&
        t != IoObject_tag(target)) // eliminate t check by matching Object tag?
    {
        char *a = (char *)IoTag_name(t);
        char *b = (char *)IoTag_name(IoObject_tag(target));
        IoState_error_(IOSTATE, m,
                       "CFunction defined for type %s but called on type %s", a,
                       b);
    }

    // IoState_pushRetainPool(state);
    result = (*(IoUserFunction *)(selfData->func))(target, locals, m);
    // IoState_popRetainPoolExceptFor_(state, result);
    return result;
}

IO_METHOD(IoCFunction, performOn) {
    /*doc CFunction performOn(target, blockLocals, optionalMessage,
    optionalContext) Activates the CFunctions with the supplied settings.
    */

    IoObject *bTarget = IoMessage_locals_valueArgAt_(m, locals, 0);
    IoObject *bLocals = locals;
    IoObject *bMessage = m;
    IoObject *bContext = bTarget;
    int argCount = IoMessage_argCount(m);

    if (argCount > 1) {
        bLocals = IoMessage_locals_valueArgAt_(m, locals, 1);
    }

    if (argCount > 2) {
        bMessage = IoMessage_locals_valueArgAt_(m, locals, 2);
    }

    if (argCount > 3) {
        bContext = IoMessage_locals_valueArgAt_(m, locals, 3);
    }

    return IoCFunction_activate(self, bTarget, bLocals, bMessage, bContext);
}

void IoCFunction_protoFinish(void *state) {
    IoMethodTable methodTable[] = {
        {"id", IoCFunction_id},
        {"==", IoCFunction_equals},
        {"performOn", IoCFunction_performOn},
        {"uniqueName", IoCFunction_uniqueName},
        {"typeName", IoCFunction_typeName},
        {"setProfilerOn", IoCFunction_setProfilerOn},
        {"profilerTime", IoCFunction_profilerTime},
        {NULL, NULL},
    };

    IoObject *self = IoState_protoWithId_((IoState *)state, protoId);
    IoObject_setSlot_to_(self, IOSYMBOL("type"), IOSYMBOL("CFunction"));
    IoObject_addMethodTable_(self, methodTable);
}
