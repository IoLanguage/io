/*
docCopyright("Joe Osborn", 2007)
docLicense("BSD revised")
*/

#ifndef IOSECURECLIENT_DEFINED
#define IOSECURECLIENT_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoMessage.h"
#include "IoCertificate.h"
#include <openssl/ssl.h>

#define ISSECURECLIENT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSecureClient_rawClone)

typedef IoObject IoSecureClient;

IoSecureClient *IoMessage_locals_secureClientArgAt_(IoMessage *self, IoObject *locals, int n);

IoSecureClient *IoSecureClient_rawClone(IoSecureClient *self);
IoSecureClient *IoSecureClient_proto(void *state);
IoSecureClient *IoSecureClient_newWithCTX_(void *state, SSL_CTX *ctx);
void IoSecureClient_free(IoSecureClient *self);

IoSecureClient *IoSecureClient_setKeyFile(IoSecureClient *self, IoObject *locals, IoMessage *msg);
IoSecureClient *IoSecureClient_setCertFile(IoSecureClient *self, IoObject *locals, IoMessage *msg);
IoSecureClient *IoSecureClient_setCAFile(IoSecureClient *self, IoObject *locals, IoMessage *msg);
IoSecureClient *IoSecureClient_setCRLFile(IoSecureClient *self, IoObject *locals, IoMessage *msg);

IoSecureClient *IoSecureClient_useTLS(IoSecureClient *self, IoObject *locals, IoMessage *msg);
IoSecureClient *IoSecureClient_useDTLS(IoSecureClient *self, IoObject *locals, IoMessage *msg);
IoSecureClient *IoSecureClient_connectionToServer(IoSecureClient *self, IoObject *locals, IoMessage *msg);
#endif
