//metadoc EvDNSRequestRequest copyright Steve Dekorte 2002
//metadoc EvDNSRequestRequest license BSD revised

#ifndef EvDNSRequest_DEFINED
#define EvDNSRequest_DEFINED 1

#include "IoSeq.h"
#include "IoError.h"

IoObject *IoEvDNSRequest_proto(void *state);

IoObject *IoEvDNSRequest_resolveIPv4(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoEvDNSRequest_resolveIPv6(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoEvDNSRequest_resolveReverseIPv4(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoEvDNSRequest_resolveReverseIPv6(IoObject *self, IoObject *locals, IoMessage *m);

#endif
