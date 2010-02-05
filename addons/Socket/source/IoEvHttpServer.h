//metadoc Event copyright Steve Dekorte 2002
//metadoc Event license BSD revised
/*metadoc EvRequest description

*/

#ifndef IoEvHttpServer_DEFINED
#define IoEvHttpServer_DEFINED 1

#include "IoObject.h"
#include "Socket.h"
#include <sys/queue.h>
#include <event.h>
#include <evhttp.h>

#define ISEVCONNECTION(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoEvHttpServer_rawClone)
#define	TAILQ_FIRST(head)		((head)->tqh_first)
#define	TAILQ_END(head)			NULL
#define	TAILQ_NEXT(elm, field)		((elm)->field.tqe_next)
	
typedef IoObject IoEvHttpServer;

IoEvHttpServer *IoMessage_locals_eventArgAt_(IoMessage *self, IoObject *locals, int n);

IoEvHttpServer *IoEvHttpServer_proto(void *state);
IoEvHttpServer *IoEvHttpServer_new(void *state);
IoEvHttpServer *IoEvHttpServer_rawClone(IoEvHttpServer *self);
void IoEvHttpServer_free(IoEvHttpServer *self);

struct evhttp_connection *IoEvHttpServer_rawConnection(IoEvHttpServer *self);

IoObject *IoEvHttpServer_start(IoEvHttpServer *self, IoObject *locals, IoMessage *m);
IoObject *IoEvHttpServer_stop(IoEvHttpServer *self, IoObject *locals, IoMessage *m);

#endif
