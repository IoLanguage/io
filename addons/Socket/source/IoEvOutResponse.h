//metadoc Event copyright Steve Dekorte 2002
//metadoc Event license BSD revised
/*metadoc EvRequest description
*/

#ifndef IoEvOutResponse_DEFINED
#define IoEvOutResponse_DEFINED 1

#include "IoObject.h"
#include "Socket.h"
#include <event.h>
#include <evhttp.h>

#define ISEVOUTRESPONSE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoEvOutResponse_rawClone)

typedef IoObject IoEvOutResponse;

IoEvOutResponse *IoMessage_locals_eventArgAt_(IoMessage *self, IoObject *locals, int n);

IoEvOutResponse *IoEvOutResponse_proto(void *state);
IoEvOutResponse *IoEvOutResponse_new(void *state);
IoEvOutResponse *IoEvOutResponse_rawClone(IoEvOutResponse *self);
void IoEvOutResponse_free(IoEvOutResponse *self);

void IoEvOutResponse_rawSetRequest_(IoEvOutResponse *self, struct evhttp_request *req);

IoObject *IoEvOutResponse_asyncSend(IoEvOutResponse *self, IoObject *locals, IoMessage *m);

#endif
