//metadoc EvDNS copyright Steve Dekorte 2002
//metadoc EvDNS license BSD revised

#ifndef EVDNS_DEFINED
#define EVDNS_DEFINED 1

#include "IoSeq.h"
#include "IoError.h"

#define ISEVDNS(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoEvDNS_rawClone)

IoObject *IoEvDNS_proto(void *state);
IoObject *IoEvDNS_newRequest(IoObject *self, IoObject *locals, IoMessage *m);

#endif
