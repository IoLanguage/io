
// metadoc Sandbox category Core
// metadoc Sandbox copyright Steve Dekorte 2002
// metadoc Sandbox license BSD revised
/*metadoc Sandbox description
Sandbox can be used to run separate instances of Io within the same process.
*/

/*cmetadoc Sandbox description
C implementation of Sandbox — a wrapper that lazily spins up an entire
nested IoState and runs strings of Io code inside it. The inner
IoState is stored in the IoObject's data pointer (see DATA(self)) and
created lazily by IoSandbox_boxState so empty Sandbox clones stay
cheap. Quota-style protection (messageCountLimit, timeLimit) is
enforced by the inner IoState's evaluator, not by this module;
setters here just poke those fields. Output from code run in the
sandbox is intercepted via IoState_printCallback_ and re-sent as a
printCallback message back to the outer VM, so the host can decide
what to do with sandboxed stdout.
*/

#include "IoSandbox.h"
#include "IoSeq.h"
#include "IoState.h"
#include "IoCFunction.h"
#include "IoObject.h"
#include "IoList.h"
#include "IoSeq.h"
#include "UArray.h"
#include "PortableTruncate.h"
#include <errno.h>
#include <stdio.h>

static const char *protoId = "Sandbox";
#define DATA(self) ((IoState *)IoObject_dataPointer(self))

/*cdoc Sandbox IoSandbox_newTag(state)
Builds the Sandbox tag with clone and free function pointers. No
markFunc is registered — the inner IoState owns its own GC and is
not walked by the outer collector.
*/
IoTag *IoSandbox_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSandbox_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSandbox_free);
    return tag;
}

/*cdoc Sandbox IoSandbox_proto(state)
Creates the Sandbox proto: no inner IoState yet (data pointer stays
NULL until first use). Installs the quota / evaluation method table
and registers the proto on the outer state.
*/
IoSandbox *IoSandbox_proto(void *state) {
    IoMethodTable methodTable[] = {
        {"messageCount", IoSandbox_messageCount},
        {"setMessageCount", IoSandbox_setMessageCount},
        {"timeLimit", IoSandbox_timeLimit},
        {"setTimeLimit", IoSandbox_setTimeLimit},
        {"doSandboxString", IoSandbox_doSandboxString},
        {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);
    IoObject_tag_(self, IoSandbox_newTag(state));

    IoState_registerProtoWithId_((IoState *)state, self, protoId);

    IoObject_addMethodTable_(self, methodTable);

    return self;
}

/*cdoc Sandbox IoSandbox_boxState(self)
Returns the inner IoState, creating it lazily on first access via
IoState_new() and wiring up the print callback. Called by every
quota setter, getter, and doSandboxString, so this is where the
nested VM really comes into existence.
*/
IoState *IoSandbox_boxState(IoSandbox *self) {
    if (!DATA(self)) {
        IoObject_setDataPointer_(self, IoState_new());
        IoSandbox_addPrintCallback(self);
    }

    return DATA(self);
}

/*cdoc Sandbox IoSandbox_rawClone(proto)
Registered as the tag's cloneFunc. Does not duplicate the inner
IoState — the clone starts fresh and will get its own IoState on
first boxState() call. Cheap to clone even when the proto has a
fully populated inner VM.
*/
IoSandbox *IoSandbox_rawClone(IoSandbox *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    return self;
}

/*cdoc Sandbox IoSandbox_addPrintCallback(self)
Sets up the inner IoState's print callback to route writes into
IoSandbox_printCallback, which forwards them back to the outer VM
as an Io-level printCallback message. Called from boxState so every
lazily-created inner state has I/O redirection wired up.
*/
void IoSandbox_addPrintCallback(IoSandbox *self) {
    IoState *boxState = IoSandbox_boxState(self);
    IoState_callbackContext_(boxState, self);
    IoState_printCallback_(boxState, IoSandbox_printCallback);
}

/*cdoc Sandbox IoSandbox_printCallback(voidSelf, ba)
Bridge from inner-VM print output to the outer VM. Copies the bytes
into a fresh IoSeq, builds a `printCallback(buffer)` message, and
performs it on the Sandbox object in the outer state's lobby scope —
letting Io code override printCallback to capture sandboxed output.
*/
void IoSandbox_printCallback(void *voidSelf, const UArray *ba) {
    IoSandbox *self = voidSelf;

    IoState *state = IOSTATE;
    IoSeq *buf = IoSeq_newWithUArray_copy_(IOSTATE, (UArray *)ba, 1);
    IoMessage *m = IoMessage_newWithName_(state, IOSYMBOL("printCallback"));
    IoMessage *arg =
        IoMessage_newWithName_returnsValue_(state, IOSYMBOL("buffer"), buf);
    IoMessage_addArg_(m, arg);
    IoMessage_locals_performOn_(m, state->lobby, self);
}

/*cdoc Sandbox IoSandbox_new(state)
Convenience constructor: look up the Sandbox proto and clone it.
The inner IoState is still not materialized until first use.
*/
IoSandbox *IoSandbox_new(void *state) {
    IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

/*cdoc Sandbox IoSandbox_free(self)
Registered as the tag's freeFunc. Tears down the inner IoState (via
IoState_free, which recursively frees every object in the nested
VM) if one was ever created; otherwise does nothing.
*/
void IoSandbox_free(IoSandbox *self) {
    if (IoObject_dataPointer(self)) {
        IoState_free(IoSandbox_boxState(self));
    }
}

/* ----------------------------------------------------------- */

IoNumber *IoSandbox_messageCount(IoSandbox *self, IoObject *locals,
                                 IoMessage *m) {
    /*doc Sandbox messageCount
    Returns a number containing the messageCount limit of the Sandbox.
    */

    IoState *boxState = IoSandbox_boxState(self);
    return IONUMBER(boxState->messageCountLimit);
}

IO_METHOD(IoSandbox, setMessageCount) {
    /*doc Sandbox setMessageCount(anInteger)
    Sets the messageCount limit of the receiver.
    */

    IoState *boxState = IoSandbox_boxState(self);
    boxState->messageCountLimit = IoMessage_locals_intArgAt_(m, locals, 0);
    return self;
}

IoNumber *IoSandbox_timeLimit(IoSandbox *self, IoObject *locals, IoMessage *m) {
    /*doc Sandbox timeLimit
    Returns a number containing the time limit of calls made to the Sandbox.
    */

    IoState *boxState = IoSandbox_boxState(self);
    return IONUMBER(boxState->timeLimit);
}

IO_METHOD(IoSandbox, setTimeLimit) {
    /*doc Sandbox setTimeLimit(aDouble)
    Sets the time limit of the Sandbox.
    */

    IoState *boxState = IoSandbox_boxState(self);
    boxState->timeLimit = IoMessage_locals_doubleArgAt_(m, locals, 0);
    return self;
}

IO_METHOD(IoSandbox, doSandboxString) {
    /*doc Sandbox doSandboxString(aString)
    Evaluate aString inside the Sandbox.
    */

    IoState *boxState = IoSandbox_boxState(self);
    char *s = IoMessage_locals_cStringArgAt_(m, locals, 0);

    IoObject *result = IoState_doSandboxCString_(boxState, s);

    if (ISSYMBOL(result)) {
        return IOSYMBOL(CSTRING(result));
    }

    if (ISSEQ(result)) {
        return IOSEQ(IOSEQ_BYTES(result), IOSEQ_LENGTH(result));
    }

    if (ISNUMBER(result)) {
        return IONUMBER(CNUMBER(result));
    }

    return IONIL(self);
}
