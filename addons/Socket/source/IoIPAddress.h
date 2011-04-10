//metadoc IPAddress copyright Steve Dekorte 2002
//metadoc IPAddress license BSD revised

#ifndef IOIPADDRESS_DEFINED
#define IOIPADDRESS_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IPAddress.h"

#define ISIPADDRESS(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoIPAddress_rawClone)

typedef IoObject IoIPAddress;

IoIPAddress *IoMessage_locals_ipAddressArgAt_(IoMessage *self, IoObject *locals, int n);
IPAddress *IoMessage_locals_rawIPAddressArgAt_(IoMessage *self, IoObject *locals, int n);

IoIPAddress *IoIPAddress_proto(void *state);
IoIPAddress *IoIPAddress_new(void *state);
IoIPAddress *IoIPAddress_rawClone(IoIPAddress *self);
void IoIPAddress_free(IoIPAddress *self);

IPAddress *IoIPAddress_rawIPAddress(IoIPAddress *self);

// ip

IoObject *IoIPAddress_setIp(IoIPAddress *self, IoObject *locals, IoMessage *m);
IoObject *IoIPAddress_ip(IoIPAddress *self, IoObject *locals, IoMessage *m);

// port

IoObject *IoIPAddress_setPort(IoIPAddress *self, IoObject *locals, IoMessage *m);
IoObject *IoIPAddress_port(IoIPAddress *self, IoObject *locals, IoMessage *m);

#endif
