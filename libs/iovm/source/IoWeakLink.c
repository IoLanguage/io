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

#include "IoWeakLink.h"
#include "IoSeq.h"
#include "IoState.h"
#include "IoObject.h"
#include "IoNumber.h"

static const char *protoId = "WeakLink";
#define DATA(self) ((IoWeakLinkData *)IoObject_dataPointer(self))

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

void IoWeakLink_readFromStream_(IoWeakLink *self, BStream *stream) {
    PID_TYPE linkid = BStream_readTaggedInt32(stream);

    if (linkid != 0) {
        IoObject *link = IoState_objectWithPid_(IOSTATE, linkid);
        IoWeakLink_rawSetLink(self, link);
    }
}

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

IoObject *IoWeakLink_rawClone(IoWeakLink *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoWeakLinkData)));
    DATA(self)->link = NULL;
    return self;
}

IoObject *IoWeakLink_new(void *state) {
    IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

void IoWeakLink_rawStopListening(IoWeakLink *self) {
    if (DATA(self)->link)
        IoObject_removeListener_(DATA(self)->link, self);
}

void IoWeakLink_free(IoWeakLink *self) {
    IoWeakLink_rawStopListening(self);
    io_free(IoObject_dataPointer(self));
}

IoObject *IoWeakLink_newWithValue_(void *state, IoObject *v) {
    IoObject *self = IoWeakLink_new(state);
    DATA(self)->link = v;
    return self;
}

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
