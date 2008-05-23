//metadoc WeakLink copyright Steve Dekorte 2002
//metadoc WeakLink license BSD revised

#ifndef IOWEAKLINK_DEFINED
#define IOWEAKLINK_DEFINED 1

#include "Common.h"
#include "IoObject_struct.h"
#include "IoMessage.h"
#include "BStream.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISWEAKLINK(self) IOOBJECT_ISTYPE(self, WeakLink)

typedef IoObject IoWeakLink;

typedef struct
{
	IoObject *link;
} IoWeakLinkData;

IoObject *IoWeakLink_proto(void *state);
IoObject *IoWeakLink_new(void *state);

//void IoWeakLink_writeToStream_(IoWeakLink *self, BStream *stream);
//void IoWeakLink_readFromStream_(IoWeakLink *self, BStream *stream);

IoObject *IoWeakLink_rawClone(IoWeakLink *self);
void IoWeakLink_free(IoWeakLink *self);
void IoWeakLink_mark(IoWeakLink *self);

IoObject *IoWeakLink_rawLink(IoWeakLink *self);

void IoObject_collectorNotification(IoWeakLink *self);

IoObject *IoWeakLink_setLink(IoWeakLink *self, IoObject *locals, IoMessage *m);
void IoWeakLink_rawSetLink(IoWeakLink *self, IoObject *v);
IoObject *IoWeakLink_link(IoWeakLink *self, IoObject *locals, IoMessage *m);

void IoWeakLink_notification(IoWeakLink *self, void *notification);

#ifdef __cplusplus
}
#endif
#endif
