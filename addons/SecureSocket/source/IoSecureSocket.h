/*
docCopyright("Joe Osborn", 2007)
docLicense("BSD revised")
*/

#ifndef IOSECURESOCKET_DEFINED
#define IOSECURESOCKET_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoMessage.h"
#include <openssl/ssl.h>
#include "IoIPAddress.h"
#include "IoCertificate.h"

#define ISSECURESOCKET(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSecureSocket_rawClone)

typedef IoObject IoSecureSocket;

IoSecureSocket *IoMessage_locals_secureSocketArgAt_(IoMessage *self, IoObject *locals, int n);

SSL *IoSecureSocket_SSL(IoSecureSocket *self);

IoSecureSocket *IoSecureSocket_rawClone(IoSecureSocket *self);
IoSecureSocket *IoSecureSocket_proto(void *state);
IoSecureSocket *IoSecureSocket_newWithCTX_(void *state, SSL_CTX *ctx);
IoSecureSocket *IoSecureSocket_newWithSSL_IP_(void *state, SSL *ssl, IoIPAddress *ip);
void IoSecureSocket_free(IoSecureSocket *self);

BIO *IoSecureSocket_createBIO(IoSecureSocket *self, char *server, char *port);
IoObject *IoSecureSocket_asyncConnectBIO(IoSecureSocket *self, IoObject *locals, IoMessage *msg);
IoObject *IoSecureSocket_asyncConnectSSL(IoSecureSocket *self, IoObject *locals, IoMessage *msg);
IoObject *IoSecureSocket_asyncAcceptSSL(IoSecureSocket *self, IoObject *locals, IoMessage *msg);
IoObject *IoSecureSocket_rawValidate(IoSecureSocket *self, IoObject *locals, IoMessage *msg);
IoObject *IoSecureSocket_isConnected(IoSecureSocket *self, IoObject *locals, IoMessage *msg);
IoObject *IoSecureSocket_descriptorId(IoSecureSocket *self, IoObject *locals, IoMessage *msg);
IoObject *IoSecureSocket_wantsRead(IoSecureSocket *self, IoObject *locals, IoMessage *msg);
IoObject *IoSecureSocket_wantsWrite(IoSecureSocket *self, IoObject *locals, IoMessage *msg);
IoObject *IoSecureSocket_asyncStreamRead(IoSecureSocket *self, IoObject *locals, IoMessage *msg);
IoObject *IoSecureSocket_asyncWrite(IoSecureSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSecureSocket_shutdown(IoSecureSocket *self, IoObject *locals, IoMessage *msg);

IoObject *IoSecureSocket_setSocketReadBufferSize(IoSecureSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSecureSocket_setSocketWriteBufferSize(IoSecureSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSecureSocket_setSocketReadLowWaterMark(IoSecureSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSecureSocket_setSocketWriteLowWaterMark(IoSecureSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSecureSocket_getSocketReadLowWaterMark(IoSecureSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSecureSocket_getSocketWriteLowWaterMark(IoSecureSocket *self, IoObject *locals, IoMessage *m);

IoObject *IoSecureSocket_isOpen(IoSecureSocket *self, IoObject *locals, IoMessage *m);

IoObject *IoSecureSocket_rawCertificate(IoSecureSocket *self, IoObject *locals, IoMessage *m);
IoObject *IoSecureSocket_rawPeerCertificate(IoSecureSocket *self, IoObject *locals, IoMessage *m);

IoSecureSocket *IoSecureSocket_isDatagram(IoSecureSocket *self, IoObject *locals, IoMessage *msg);
				#ifdef DTLS_IMPLEMENTED
		IoObject *IoSecureSocket_rbioReady(IoSecureSocket *self, IoObject *locals, IoMessage *m);
		IoObject *IoSecureSocket_asyncUdpRead(IoSecureSocket *self, IoObject *locals, IoMessage *msg);
		IoObject *IoSecureSocket_asyncUdpServerRead(IoSecureSocket *self, IoObject *locals, IoMessage *msg);
				#endif

#endif
