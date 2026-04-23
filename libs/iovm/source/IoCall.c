
// metadoc Call copyright Steve Dekorte 2002
// metadoc Call license BSD revised
// metadoc Call category Core
/*metadoc Call description
Call stores slots related to activation.
*/

/*cmetadoc Call description
C implementation of the Call object created once per block/method
activation. IoCallData captures the activation's sender (caller's
locals), the target receiver, the triggering message, the slotContext
(where the slot was found during lookup — can differ from target when
the slot lives on a proto), the activated Block/CFunction itself, the
coroutine that is running, and the stopStatus used to signal
return/continue/break. Each Call is exposed through the block's locals
as the "call" slot so Io code can read sender, message argAt, etc.
IoCall_rawStopStatus is the primary C-level read path; the iterative
evaluator (IoState_iterative.c) and IoBlock_activate both poke the
status here when an early exit is requested.
*/

#include "IoCall.h"
#include "IoState.h"
#include "IoObject.h"

static const char *protoId = "Call";

#define DATA(self) ((IoCallData *)IoObject_dataPointer(self))

/*cdoc Call IoCall_newTag(state)
Builds the Call tag with clone/mark/free function pointers. No
activateFunc — Call is data only, it does not run itself.
*/
IoTag *IoCall_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCall_rawClone);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCall_mark);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCall_free);
    return tag;
}

/*cdoc Call IoCall_initSlots(self)
Seeds all IoObject* slots with state->ioNil and the stopStatus with
MESSAGE_STOP_STATUS_NORMAL. Called by both proto and rawClone so no
Call is ever observed with uninitialized pointers — the GC mark
function would otherwise dereference garbage.
*/
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

/*cdoc Call IoCall_proto(state)
Creates the Call proto, allocates a zeroed IoCallData payload, and
wires up the Io-visible method table (sender, message, slotContext,
target, activated, coroutine, evalArgAt, argAt, stopStatus,
setStopStatus). Every activation clones this proto via IoCall_with.
*/
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

/*cdoc Call IoCall_rawClone(proto)
Registered as the tag's cloneFunc. Copies the proto's data block byte
for byte then re-initializes slots to nil/NORMAL so a clone never
inherits a stale sender/message pointer from the proto.
*/
IoCall *IoCall_rawClone(IoCall *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(
        self, cpalloc(IoObject_dataPointer(proto), sizeof(IoCallData)));
    // printf("IoCall_rawClone() %p|%p\n", (void *)self,
    // IoObject_dataPointer(self));
    IoCall_initSlots(self);
    return self;
}

/*cdoc Call IoCall_new(state)
Convenience constructor: looks up the registered proto and clones it.
Used by IoCall_with; direct callers are rare since activation always
wants the fields filled in at construction.
*/
IoCall *IoCall_new(IoState *state) {
    IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

/*cdoc Call IoCall_with(state, sender, target, message, slotContext, activated, coroutine)
Canonical constructor invoked from IoBlock_activate and the iterative
evaluator's block activation path. Clones the Call proto and fills
every slot in one pass, leaving stopStatus at NORMAL. The returned
Call is stashed in the block's locals as the "call" slot so Io code
can query sender/message at runtime.
*/
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

/*cdoc Call IoCall_mark(self)
Registered as the tag's markFunc. Marks every field: sender, target,
message, slotContext, activated block/cfunction, and the owning
coroutine. No nil check because initSlots guarantees all fields are
valid IoObjects.
*/
void IoCall_mark(IoCall *self) {
    IoCallData *d = DATA(self);

    IoObject_shouldMark(d->sender);
    IoObject_shouldMark(d->target);
    IoObject_shouldMark(d->message);
    IoObject_shouldMark(d->slotContext);
    IoObject_shouldMark(d->activated);
    IoObject_shouldMark(d->coroutine);
}

/*cdoc Call IoCall_free(self)
Registered as the tag's freeFunc. Frees the IoCallData payload; all
referenced IoObjects are GC-managed.
*/
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

/*cdoc Call IoCall_rawStopStatus(self)
Direct C accessor for the stopStatus field. Hot-path read: the
iterative evaluator and IoBlock_activate both call it on every
block exit to decide whether to propagate return/continue/break.
*/
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
