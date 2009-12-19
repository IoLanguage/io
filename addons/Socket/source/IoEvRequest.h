//metadoc Event copyright Steve Dekorte 2002
//metadoc Event license BSD revised
/*metadoc EvRequest description
 
*/

#ifndef IOEVREQUEST_DEFINED
#define IOEVREQUEST_DEFINED 1

#include "IoObject.h"
#include "Socket.h"
#include <event.h>
#include <evhttp.h>

#define ISEVENT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoEvRequest_rawClone)

typedef IoObject IoEvRequest;

IoEvRequest *IoMessage_locals_eventArgAt_(IoMessage *self, IoObject *locals, int n);

IoEvRequest *IoEvRequest_proto(void *state);
IoEvRequest *IoEvRequest_new(void *state);
IoEvRequest *IoEvRequest_rawClone(IoEvRequest *self);
void IoEvRequest_free(IoEvRequest *self);

struct event *IoEvRequest_rawEvent(IoEvRequest *self);

IoObject *IoEvRequest_send(IoEvRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoEvRequest_cancel(IoEvRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoEvRequest_encodeUri(IoEvRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoEvRequest_decodeUri(IoEvRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoEvRequest_htmlEscape(IoEvRequest *self, IoObject *locals, IoMessage *m);

#endif
