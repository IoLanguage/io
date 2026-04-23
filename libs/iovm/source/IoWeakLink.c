// metadoc WeakLink copyright Steve Dekorte, 2002
// metadoc WeakLink license BSD revised
// metadoc WeakLink category Core

/*metadoc WeakLink description
        A WeakLink is a primitive that can hold a reference to
        an object without preventing the garbage collector from
        collecting it. The link reference is set with the setLink() method.
        After the garbage collector collects an object, it informs any
        (uncollected) WeakLink objects whose link value pointed to that
        object by calling their "collectedLink" method.
*/

/*cmetadoc WeakLink description
C implementation of WeakLink. Holds a single non-owning pointer
(DATA(self)->link) that is deliberately NOT marked by the GC, so
the referent can be collected even while a WeakLink names it. The
mechanism for notification is the IoObject listener list plus the
tag's notificationFunc: on setLink, the WeakLink registers as a
listener on the target via IoObject_addListener_; when the target
is freed, the collector fires IoWeakLink_notification which clears
the pointer back to NULL. There is no markFunc registered — that is
the whole point. The commented-out writeToStream/readFromStream
pair persists WeakLinks by PID (IoObject_pid) so they can be
rehydrated through IoState_objectWithPid_.
*/

#include "IoWeakLink.h"
#include "IoSeq.h"
#include "IoState.h"
#include "IoObject.h"
#include "IoNumber.h"

static const char *protoId = "WeakLink";
#define DATA(self) ((IoWeakLinkData *)IoObject_dataPointer(self))

/*cdoc WeakLink IoWeakLink_newTag(state)
Builds the WeakLink tag. Installs a notificationFunc — unique to
WeakLink among the core protos — which fires when a listened-to
target is collected. Deliberately registers no markFunc so the link
target is never retained by this object.
*/
IoTag *IoWeakLink_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoWeakLink_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoWeakLink_free);
    IoTag_notificationFunc_(tag,
                            (IoTagNotificationFunc *)IoWeakLink_notification);
    // IoTag_writeToStreamFunc_(tag, (IoTagWriteToStreamFunc
    // *)IoWeakLink_writeToStream_); IoTag_readFromStreamFunc_(tag,
    // (IoTagReadFromStreamFunc *)IoWeakLink_readFromStream_);
    return tag;
}

/*
void IoWeakLink_writeToStream_(IoWeakLink *self, BStream *stream)
{
        if (DATA(self)->link)
        {
                BStream_writeTaggedInt32_(stream,
IoObject_pid(DATA(self)->link));
        }
        else
        {
                BStream_writeTaggedInt32_(stream, 0);
        }
}
*/

/*cdoc WeakLink IoWeakLink_readFromStream_(self, stream)
Restores a serialized WeakLink by reading a tagged PID and resolving
it against the current IoState's object table. Not currently
registered on the tag (see commented-out setup in IoWeakLink_newTag)
but kept for potential future persistence layers.
*/
void IoWeakLink_readFromStream_(IoWeakLink *self, BStream *stream) {
    PID_TYPE linkid = BStream_readTaggedInt32(stream);

    if (linkid != 0) {
        IoObject *link = IoState_objectWithPid_(IOSTATE, linkid);
        IoWeakLink_rawSetLink(self, link);
    }
}

/*cdoc WeakLink IoWeakLink_proto(state)
Creates the WeakLink proto: allocates an empty IoWeakLinkData with
link set to NULL, attaches the tag, registers the proto, and
installs the Io-visible method table (setLink, link).
*/
IoObject *IoWeakLink_proto(void *state) {
    IoMethodTable methodTable[] = {
        {"setLink", IoWeakLink_setLink},
        {"link", IoWeakLink_link},
        {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);

    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoWeakLinkData)));
    IoObject_tag_(self, IoWeakLink_newTag(state));
    DATA(self)->link = NULL;
    IoState_registerProtoWithId_((IoState *)state, self, protoId);

    IoObject_addMethodTable_(self, methodTable);
    return self;
}

/*cdoc WeakLink IoWeakLink_rawClone(proto)
Registered as the tag's cloneFunc. Fresh clones start with a null
link rather than inheriting the proto's target — a WeakLink chained
through cloning would otherwise register multiple listeners on one
target and break the unregister-on-free invariant.
*/
IoObject *IoWeakLink_rawClone(IoWeakLink *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoWeakLinkData)));
    DATA(self)->link = NULL;
    return self;
}

/*cdoc WeakLink IoWeakLink_new(state)
Convenience constructor: look up the WeakLink proto and clone it.
*/
IoObject *IoWeakLink_new(void *state) {
    IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

/*cdoc WeakLink IoWeakLink_rawStopListening(self)
Removes this WeakLink from the current target's listener list so the
collector stops notifying it. Used before rebinding the link and on
free — without this, a freed WeakLink could still be notified and
dereference its own freed data pointer.
*/
void IoWeakLink_rawStopListening(IoWeakLink *self) {
    if (DATA(self)->link)
        IoObject_removeListener_(DATA(self)->link, self);
}

/*cdoc WeakLink IoWeakLink_free(self)
Registered as the tag's freeFunc. Unregisters from the target's
listener list first, then releases the IoWeakLinkData payload.
*/
void IoWeakLink_free(IoWeakLink *self) {
    IoWeakLink_rawStopListening(self);
    io_free(IoObject_dataPointer(self));
}

/*cdoc WeakLink IoWeakLink_newWithValue_(state, v)
Creates a WeakLink whose target is v. Note this helper stashes v
directly without registering as a listener — most callers should use
IoWeakLink_rawSetLink instead so collection notification fires.
*/
IoObject *IoWeakLink_newWithValue_(void *state, IoObject *v) {
    IoObject *self = IoWeakLink_new(state);
    DATA(self)->link = v;
    return self;
}

/*cdoc WeakLink IoWeakLink_notification(self, notification)
Registered as the tag's notificationFunc. The GC calls this when
the target object of this WeakLink is collected; we null out the
link pointer so the next `link` query returns nil. The commented-
out IoMessage_locals_performOn_ would fire an Io-level callback,
left in place as a stub for future support.
*/
void IoWeakLink_notification(IoWeakLink *self,
                             void *notification) // called when link is freed
{
    DATA(self)->link = NULL;
    // IoMessage_locals_performOn_(IOSTATE->collectedLinkMessage, self, self);
}

// -----------------------------------------------------------

IO_METHOD(IoWeakLink, setLink) {
    /*doc WeakLink setLink(aValue)
    Sets the link pointer. Returns self.
    */

    IoWeakLink_rawSetLink(self, IoMessage_locals_valueArgAt_(m, locals, 0));
    return self;
}

/*cdoc WeakLink IoWeakLink_rawSetLink(self, v)
Internal setter used by setLink. Unregisters from the previous
target first, then either clears the link (if v is nil) or stores
v and registers for collection notification. No IOREF — this is a
weak reference by design, which is the whole reason this object
exists.
*/
void IoWeakLink_rawSetLink(IoWeakLink *self, IoObject *v) {
    IoWeakLink_rawStopListening(self);

    if (ISNIL(v)) {
        DATA(self)->link = NULL;
    } else {
        DATA(self)->link = v; // no IOREF needed since this is a weak link
        IoObject_addListener_(v, self);
    }
}

IO_METHOD(IoWeakLink, link) {
    /*doc WeakLink link
    Returns the link pointer or Nil if none is set.
*/

    IoObject *v = DATA(self)->link;
    return v ? v : IONIL(self);
}
