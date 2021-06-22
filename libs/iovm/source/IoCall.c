
// metadoc Call copyright Steve Dekorte 2002
// metadoc Call license BSD revised
// metadoc Call category Core
/*metadoc Call description
Call stores slots related to activation.
*/

#include "IoCall.h"
#include "IoState.h"
#include "IoObject.h"

static const char *protoId = "Call";

#define DATA(self) ((IoCallData *)IoObject_dataPointer(self))

IoTag *IoCall_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCall_rawClone);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCall_mark);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCall_free);
    return tag;
}

void IoCall_initSlots(IoCall *self) {
    IoObject *ioNil = IOSTATE->ioNil;
    DATA(self)->sender = ioNil;
    DATA(self)->message = ioNil;
    DATA(self)->slotContext = ioNil;
    DATA(self)->target = ioNil;
    DATA(self)->activated = ioNil;
    DATA(self)->coroutine = ioNil;
    DATA(self)->stopStatus = MESSAGE_STOP_STATUS_NORMAL;
}

IoCall *IoCall_proto(void *vState) {
    IoState *state = (IoState *)vState;

    IoMethodTable methodTable[] = {
        {"sender", IoCall_sender},
        {"message", IoCall_message},
        {"slotContext", IoCall_slotContext},
        {"target", IoCall_target},
        {"activated", IoCall_activated},
        {"coroutine", IoCall_coroutine},
        {"evalArgAt", IoCall_evalArgAt},
        {"argAt", IoCall_argAt},
        {"stopStatus", IoCall_stopStatus},
        {"setStopStatus", IoCall_setStopStatus},
        {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);

    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoCallData)));
    IoObject_tag_(self, IoCall_newTag(state));
    IoCall_initSlots(self);

    IoState_registerProtoWithId_((IoState *)state, self, protoId);

    IoObject_addMethodTable_(self, methodTable);
    return self;
}

IoCall *IoCall_rawClone(IoCall *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(
        self, cpalloc(IoObject_dataPointer(proto), sizeof(IoCallData)));
    // printf("IoCall_rawClone() %p|%p\n", (void *)self,
    // IoObject_dataPointer(self));
    IoCall_initSlots(self);
    return self;
}

IoCall *IoCall_new(IoState *state) {
    IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

IoCall *IoCall_with(void *state, IoObject *sender, IoObject *target,
                    IoObject *message, IoObject *slotContext,
                    IoObject *activated, IoObject *coroutine) {
    IoCall *self = IoCall_new(state);

    DATA(self)->sender = sender;
    DATA(self)->target = target;
    DATA(self)->message = message;
    DATA(self)->slotContext = slotContext;
    DATA(self)->activated = activated;
    DATA(self)->coroutine = coroutine;
    DATA(self)->stopStatus = MESSAGE_STOP_STATUS_NORMAL;
    return self;
}

void IoCall_mark(IoCall *self) {
    IoCallData *d = DATA(self);

    IoObject_shouldMark(d->sender);
    IoObject_shouldMark(d->target);
    IoObject_shouldMark(d->message);
    IoObject_shouldMark(d->slotContext);
    IoObject_shouldMark(d->activated);
    IoObject_shouldMark(d->coroutine);
}

void IoCall_free(IoCall *self) { io_free(IoObject_dataPointer(self)); }

IO_METHOD(IoCall, sender) {
    /*doc Call sender
    Returns the sender value.
    */

    return DATA(self)->sender;
}

IO_METHOD(IoCall, message) {
    /*doc Call message
    Returns the message value.
    */

    return DATA(self)->message;
}

IO_METHOD(IoCall, target) {
    /*doc Call target
    Returns the target value.
    */

    return DATA(self)->target;
}

IO_METHOD(IoCall, slotContext) {
    /*doc Call slotContext
    Returns the slotContext value.
    */

    return DATA(self)->slotContext;
}

IO_METHOD(IoCall, activated) {
    /*doc Call activated
    Returns the activated value.
    */

    return DATA(self)->activated;
}

IO_METHOD(IoCall, coroutine) {
    /*doc Call coroutine
    Returns the coroutine in which the message was sent.
    */

    return DATA(self)->coroutine;
}

IO_METHOD(IoCall, evalArgAt) {
    /*doc Call evalArgAt(argNumber)
    Evaluates the specified argument of the Call's message in the context of its
    sender.
    */

    int n = IoMessage_locals_intArgAt_(m, locals, 0);
    IoCallData *data = DATA(self);
    return IoMessage_locals_valueArgAt_(data->message, data->sender, n);
}

IO_METHOD(IoCall, argAt) {
    /*doc Call argAt(argNumber)
    Returns the message's argNumber arg. Shorthand for same as call message
    argAt(argNumber).
    */

    return IoMessage_argAt(DATA(self)->message, locals, m);
}

int IoCall_rawStopStatus(IoCall *self) { return DATA(self)->stopStatus; }

IO_METHOD(IoCall, stopStatus) {
    /*doc Call stopStatus
    Returns the stop status on the call. (description of stopStatus will
    be added once we decide whether or not to keep it)
    */
    return IoState_stopStatusObject(IOSTATE, DATA(self)->stopStatus);
}

IO_METHOD(IoCall, setStopStatus) {
    /*doc Call setStopStatus(aStatusObject)
    Sets the stop status on the call.
    */
    IoObject *status = IoMessage_locals_valueArgAt_(m, locals, 0);
    DATA(self)->stopStatus = IoState_stopStatusNumber(IOSTATE, status);
    return self;
}
