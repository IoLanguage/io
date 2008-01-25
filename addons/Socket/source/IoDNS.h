/*
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IOSEQ_DNS_DEFINED
#define IOSEQ_DNS_DEFINED 1

#include "IoSeq.h"

#define DNSERROR(reason) IoState_setErrorDescription_(IOSTATE, "DNS Error: %s", reason)

IoObject *IoDNS_proto(void *state);

IoObject *IoObject_dnsQueryPacketForHostName(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_hostNameAndIPforDNSResponsePacket(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoObject_localNameServersIPs(IoObject *self, IoObject *locals, IoMessage *m);

#endif
