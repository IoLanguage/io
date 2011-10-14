//metadoc Event copyright Steve Dekorte 2002
//metadoc Event license BSD revised
/*metadoc EvRequest description
*/

#ifndef IOEVCONNECTION_DEFINED
#define IOEVCONNECTION_DEFINED 1

#include "IoObject.h"
#include "Socket.h"
#include <sys/queue.h>
#include "event.h"
//#include <event.h>
#include <evhttp.h>

#define ISEVCONNECTION(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoEvConnection_rawClone)

typedef IoObject IoEvConnection;

IoEvConnection *IoMessage_locals_eventArgAt_(IoMessage *self, IoObject *locals, int n);

IoEvConnection *IoEvConnection_proto(void *state);
IoEvConnection *IoEvConnection_new(void *state);
IoEvConnection *IoEvConnection_rawClone(IoEvConnection *self);
void IoEvConnection_free(IoEvConnection *self);

struct evhttp_connection *IoEvConnection_rawConnection(IoEvConnection *self);

IoObject *IoEvConnection_setTimeout_(IoEvConnection *self, IoObject *locals, IoMessage *m);
IoObject *IoEvConnection_setRetries_(IoEvConnection *self, IoObject *locals, IoMessage *m);
IoObject *IoEvConnection_setLocalAddress_(IoEvConnection *self, IoObject *locals, IoMessage *m);
IoObject *IoEvConnection_connect(IoEvConnection *self, IoObject *locals, IoMessage *m);
IoObject *IoEvConnection_disconnect(IoEvConnection *self, IoObject *locals, IoMessage *m);

#endif
