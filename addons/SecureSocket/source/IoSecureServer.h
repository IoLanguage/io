/*
docCopyright("Joe Osborn", 2007)
docLicense("BSD revised")
*/

#ifndef IOSECURESERVER_DEFINED
#define IOSECURESERVER_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoMessage.h"
#include <openssl/ssl.h>
#include "IoSecureSocket.h"
#include "IoIPAddress.h"
#include "IoCertificate.h"

#define ISSECURESERVER(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSecureServer_rawClone)

typedef IoObject IoSecureServer;

IoSecureServer *IoMessage_locals_secureServerArgAt_(IoMessage *self, IoObject *locals, int n);

IoTag *IoSecureServer_newTag(void *state);
IoSecureServer *IoSecureServer_proto(void *state);
IoSecureServer *IoSecureServer_rawClone(IoSecureServer *proto);
void IoSecureServer_free(IoSecureServer *self);

IoSecureServer *IoSecureServer_setRequiresClientCertificate(IoSecureServer *self, IoObject *locals, IoMessage *msg);
IoSecureServer *IoSecureServer_setKeyFile(IoSecureServer *self, IoObject *locals, IoMessage *msg);
IoSecureServer *IoSecureServer_setCertFile(IoSecureServer *self, IoObject *locals, IoMessage *msg);
IoSecureServer *IoSecureServer_setCAFile(IoSecureServer *self, IoObject *locals, IoMessage *msg);
IoSecureServer *IoSecureServer_setCRLFile(IoSecureServer *self, IoObject *locals, IoMessage *msg);
IoSecureServer *IoSecureServer_useTLS(IoSecureServer *self, IoObject *locals, IoMessage *msg);
IoSecureServer *IoSecureServer_useDTLS(IoSecureServer *self, IoObject *locals, IoMessage *msg);
IoSecureSocket *IoSecureServer_tlsWrap(IoSecureServer *self, IoObject *locals, IoMessage *msg);
	#ifdef DTLS_IMPLEMENTED
IoSecureSocket *IoSecureServer_dtlsWrap(IoSecureServer *self, IoObject *locals, IoMessage *msg);
IoIPAddress *IoSecureServer_udpRecvIP(IoSecureServer *self, IoObject *locals, IoMessage *msg);
IoSecureServer *IoSecureServer_dispatchUDP(IoSecureServer *self, IoObject *locals, IoMessage *msg);
	#endif
#endif
