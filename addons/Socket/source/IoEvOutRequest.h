//metadoc Event copyright Steve Dekorte 2002
//metadoc Event license BSD revised
/*metadoc EvRequest description
 
*/

#ifndef IoEvOutRequest_DEFINED
#define IoEvOutRequest_DEFINED 1

#include "IoObject.h"
#include "Socket.h"
#include <sys/queue.h>
#include <event.h>
#include <evhttp.h>

#define ISEVENT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoEvOutRequest_rawClone)

typedef IoObject IoEvOutRequest;

IoEvOutRequest *IoMessage_locals_eventArgAt_(IoMessage *self, IoObject *locals, int n);

IoEvOutRequest *IoEvOutRequest_proto(void *state);
IoEvOutRequest *IoEvOutRequest_new(void *state);
IoEvOutRequest *IoEvOutRequest_rawClone(IoEvOutRequest *self);
void IoEvOutRequest_free(IoEvOutRequest *self);

struct event *IoEvOutRequest_rawEvent(IoEvOutRequest *self);

IoObject *IoEvOutRequest_send(IoEvOutRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoEvOutRequest_cancel(IoEvOutRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoEvOutRequest_encodeUri(IoEvOutRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoEvOutRequest_decodeUri(IoEvOutRequest *self, IoObject *locals, IoMessage *m);
IoObject *IoEvOutRequest_htmlEscape(IoEvOutRequest *self, IoObject *locals, IoMessage *m);

/*
//TODO: Cheap hack, clean this shit
#ifndef __linux__
struct evbuffer {
    u_char *buffer;
    u_char *orig_buffer;
    
    size_t misalign;
    size_t totallen;
    size_t off;

    void (*cb)(struct evbuffer *, size_t, size_t, void *);
    void *cbarg;
}; 
#endif // ndef __linux__ 
*/

#endif
