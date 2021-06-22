
// metadoc State copyright Steve Dekorte 2002
// metadoc State license BSD revised

#ifdef IOSTATE_C
#define IO_IN_C_FILE
#endif
#include "Common_inline.h"
#ifdef IO_DECLARE_INLINES

// booleans --------------------------------------------------------

#define SIOSYMBOL(stringConstant)                                              \
    IoState_symbolWithCString_(self, stringConstant)

#define IOASSERT(value, message)                                               \
    if (!(value)) {                                                            \
        IoState_error_(IOSTATE, m, "Io Assertion '%s'", message);              \
    }

#define IOCOLLECTOR (IOSTATE->collector)

#define IONIL(self) (IOSTATE->ioNil)
#define ISNIL(self) (self == IOSTATE->ioNil)

// booleans --------------------------------------------------------

#define ISBOOL(self)                                                           \
    (((self) == IOSTATE->ioFalse) || ((self) == IOSTATE->ioNil) ||             \
     ((self) == IOSTATE->ioTrue))

#define IOSUCCESS(self) IOTRUE(self)
#define ISSUCCESS(self) ISTRUE(self)

#define IOFAILURE(self) IOFALSE(self)
#define ISFAILURE(self) ISFALSE(self)

// inline IoObject *IOBOOL(IoObject *self, int b);

#if !defined(IoObjectDataDefined)
typedef struct IoObjectData IoObjectData;
#endif

IOINLINE IoObject *IOTRUE(IoObject *self) { return IOSTATE->ioTrue; }

IOINLINE int ISTRUE(IoObject *self) {
    return self != IOSTATE->ioNil && self != IOSTATE->ioFalse;
}

IOINLINE IoObject *IOFALSE(IoObject *self) { return IOSTATE->ioFalse; }

IOINLINE int ISFALSE(IoObject *self) {
    return self == IOSTATE->ioNil || self == IOSTATE->ioFalse;
}

IOINLINE IoObject *IOBOOL(IoObject *self, int b) {
    return b ? IOTRUE(self) : IOFALSE(self);
}

// collector --------------------------------------------------------

IOINLINE IoObject *IoState_retain_(IoState *self, IoObject *v) {
    IoObject_isReferenced_(v, 1);
    Collector_retain_(self->collector, v);
    return v;
}

IOINLINE void IoState_stopRetaining_(IoState *self, IoObject *v) {
    Collector_stopRetaining_(self->collector, v);
}

IOINLINE void *IoState_unreferencedStackRetain_(IoState *self, IoObject *v) {
    if (self->currentCoroutine) {
        Collector_value_addingRefTo_(self->collector, self->currentCoroutine,
                                     v);
    }

    Stack_push_(self->currentIoStack, v);
    return v;
}

IOINLINE void *IoState_stackRetain_(IoState *self, IoObject *v) {
    IoObject_isReferenced_(v, 1);
    IoState_unreferencedStackRetain_(self, v);
    return v;
}

IOINLINE void IoState_addValue_(IoState *self, IoObject *v) {
    Collector_addValue_(self->collector, v);
    IoState_unreferencedStackRetain_(self, v);
}

IOINLINE void IoState_addValueIfNecessary_(IoState *self, IoObject *v) {
    if (v->prev) {
        Collector_addValue_(self->collector, v);
    }
    IoState_unreferencedStackRetain_(self, v);
}

IOINLINE void IoState_pushCollectorPause(IoState *self) {
    Collector_pushPause(self->collector);
}

IOINLINE void IoState_popCollectorPause(IoState *self) {
    Collector_popPause(self->collector);
}

IOINLINE void IoState_clearRetainStack(IoState *self) {
    Stack_clear(((IoState *)self)->currentIoStack);
}

IOINLINE uintptr_t IoState_pushRetainPool(void *self) {
    uintptr_t m = Stack_pushMarkPoint(((IoState *)self)->currentIoStack);
    return m;
}

IOINLINE void IoState_clearTopPool(void *self) {
    Stack *stack = ((IoState *)self)->currentIoStack;
    // Stack_popMark(stack);
    // Stack_pushMark(stack);
    Stack_clearTop(stack);
}

IOINLINE void IoState_popRetainPool(void *self) {
    Stack *stack = ((IoState *)self)->currentIoStack;
    Stack_popMark(stack);
}

IOINLINE void IoState_popRetainPool_(void *self, uintptr_t mark) {
    Stack *stack = ((IoState *)self)->currentIoStack;
    Stack_popMarkPoint_(stack, mark);
}

IOINLINE void IoState_popRetainPoolExceptFor_(void *state, void *obj) {
    IoState *self = (IoState *)state;
#ifdef STACK_POP_CALLBACK
    IoObject_isReferenced_(((IoObject *)obj), 1);
#endif
    IoState_popRetainPool(self);
    IoState_stackRetain_(self, (IoObject *)obj);
}

// message args --------------------------------------------------------

#define IOMESSAGEDATA(self) ((IoMessageData *)IoObject_dataPointer(self))

IOINLINE IoObject *IoMessage_locals_quickValueArgAt_(IoMessage *self,
                                                     IoObject *locals, int n) {
    IoMessage *m = (IoMessage *)List_at_(IOMESSAGEDATA(self)->args, n);

    if (m) {
        IoMessageData *md = IOMESSAGEDATA(m);
        IoObject *v = md->cachedResult;

        if (v && !md->next) {
            return v;
        }

        return IoMessage_locals_performOn_(m, locals, locals);
    }

    return IOSTATE->ioNil;
}

IOINLINE IoObject *IoMessage_locals_valueArgAt_(IoMessage *self,
                                                IoObject *locals, int n) {
    return IoMessage_locals_quickValueArgAt_(self, locals, n);
    /*
    List *args = IOMESSAGEDATA(self)->args;
    IoMessage *m = (IoMessage *)List_at_(args, n);

    if (m)
    {
            return IoMessage_locals_performOn_(m, locals, locals);
    }

    return IOSTATE->ioNil;
    */
}

IOINLINE IoObject *IoMessage_locals_firstStringArg(IoMessage *self,
                                                   IoObject *locals) {
    // special case this, since it's used for setSlot()
    List *args = IOMESSAGEDATA(self)->args;

    if (List_size(args)) {
        IoMessage *m = (IoMessage *)List_rawAt_(args, 0);

        if (m) {
            IoMessageData *md = IOMESSAGEDATA(m);
            IoObject *v = md->cachedResult;

            // avoid calling IoMessage_locals, if possible

            if (v && IoObject_isSymbol(v) && (md->next == NULL)) {
                return v;
            }
        }
    }

    return IoMessage_locals_symbolArgAt_(self, locals, 0);
}

// --------------------------

IOINLINE void IoState_break(IoState *self, IoObject *v) {
    self->stopStatus = MESSAGE_STOP_STATUS_BREAK;
    self->returnValue = v;
}

IOINLINE void IoState_continue(IoState *self) {
    self->stopStatus = MESSAGE_STOP_STATUS_CONTINUE;
}

IOINLINE void IoState_eol(IoState *self) {
    self->stopStatus = MESSAGE_STOP_STATUS_EOL;
}

IOINLINE void IoState_return(IoState *self, IoObject *v) {
    self->stopStatus = MESSAGE_STOP_STATUS_RETURN;
    self->returnValue = v;
}

IOINLINE void IoState_resetStopStatus(IoState *self) {
    self->stopStatus = MESSAGE_STOP_STATUS_NORMAL;
}

IOINLINE int IoState_handleStatus(IoState *self) {
    switch (self->stopStatus) {
    case MESSAGE_STOP_STATUS_RETURN:
        return 1;

    case MESSAGE_STOP_STATUS_BREAK:
        IoState_resetStopStatus(self);
        return 1;

    case MESSAGE_STOP_STATUS_CONTINUE:
        IoState_resetStopStatus(self);
        return 0;

    default:
        return 0;
    }
}

IOINLINE IoObject *IoState_stopStatusObject(IoState *self, int stopStatus) {
    switch (stopStatus) {
    case MESSAGE_STOP_STATUS_NORMAL:
        return self->ioNormal;

    case MESSAGE_STOP_STATUS_BREAK:
        return self->ioBreak;

    case MESSAGE_STOP_STATUS_CONTINUE:
        return self->ioContinue;

    case MESSAGE_STOP_STATUS_RETURN:
        return self->ioReturn;

    case MESSAGE_STOP_STATUS_EOL:
        return self->ioEol;

    default:
        return self->ioNormal;
    }
}

IOINLINE int IoState_stopStatusNumber(IoState *self, IoObject *obj) {
    if (obj == self->ioNormal)
        return MESSAGE_STOP_STATUS_NORMAL;

    if (obj == self->ioBreak)
        return MESSAGE_STOP_STATUS_BREAK;

    if (obj == self->ioContinue)
        return MESSAGE_STOP_STATUS_CONTINUE;

    if (obj == self->ioReturn)
        return MESSAGE_STOP_STATUS_RETURN;

    if (obj == self->ioEol)
        return MESSAGE_STOP_STATUS_EOL;

    return MESSAGE_STOP_STATUS_NORMAL;
}

#undef IO_IN_C_FILE
#endif
