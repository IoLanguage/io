//metadoc Event copyright Steve Dekorte 2002
//metadoc Event license BSD revised
/*metadoc Event description
 e := Event clone
 EventManager addEvent(e, fd, type, timeout)

 Event handleReadEvent := method()
 Event handleWriteEvent := method()
 Event handleTimeoutEvent := method()
*/

#ifndef IOEVENT_DEFINED
#define IOEVENT_DEFINED 1

#include "IoObject.h"
#include "Socket.h"
#include <event.h>

#define ISEVENT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoEvent_rawClone)

typedef IoObject IoEvent;

IoEvent *IoMessage_locals_eventArgAt_(IoMessage *self, IoObject *locals, int n);

IoEvent *IoEvent_proto(void *state);
IoEvent *IoEvent_new(void *state);
IoEvent *IoEvent_rawClone(IoEvent *self);
void IoEvent_free(IoEvent *self);

struct event *IoEvent_rawEvent(IoEvent *self);

IoObject *IoEvent_isPending(IoEvent *self, IoObject *locals, IoMessage *m);
IoObject *IoEvent_isValid(IoEvent *self, IoObject *locals, IoMessage *m);

#endif
