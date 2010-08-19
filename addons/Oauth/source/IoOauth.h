
//metadoc Oauth copyright Steve Dekorte 2002


#ifndef IOOauth_DEFINED
#define IOOauth_DEFINED 1

#include "IoObject.h"
#include "IoNumber.h"
#include "Oauth/Oauth.h"

#define ISOauth(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoOauth_rawClone)

typedef IoObject IoOauth;


IoOauth *IoOauth_rawClone(IoOauth *self);
IoOauth *IoOauth_proto(void *state);
IoOauth *IoOauth_new(void *state);

void IoOauth_free(IoOauth *self);
void IoOauth_mark(IoOauth *self);

/* ----------------------------------------------------------- */

IoObject *IoOauth_setConsumerKey(IoOauth *self, IoObject *locals, IoMessage *m);
IoObject *IoOauth_consumerKey(IoOauth *self, IoObject *locals, IoMessage *m);

IoObject *IoOauth_setConsumerSecret(IoOauth *self, IoObject *locals, IoMessage *m);
IoObject *IoOauth_consumerSecret(IoOauth *self, IoObject *locals, IoMessage *m);

/* ----------------------------------------------------------- */

IoObject *IoOauth_setOauthToken(IoOauth *self, IoObject *locals, IoMessage *m);
IoObject *IoOauth_oauthToken(IoOauth *self, IoObject *locals, IoMessage *m);

IoObject *IoOauth_setOauthSecret(IoOauth *self, IoObject *locals, IoMessage *m);
IoObject *IoOauth_oauthSecret(IoOauth *self, IoObject *locals, IoMessage *m);

/* ----------------------------------------------------------- */

IoObject *IoOauth_setAccessKey(IoOauth *self, IoObject *locals, IoMessage *m);
IoObject *IoOauth_accessKey(IoOauth *self, IoObject *locals, IoMessage *m);

IoObject *IoOauth_setAccessSecret(IoOauth *self, IoObject *locals, IoMessage *m);
IoObject *IoOauth_accessSecret(IoOauth *self, IoObject *locals, IoMessage *m);

/* ----------------------------------------------------------- */

IoObject *IoOauth_getOauthTokenAndSecretFromUrl(IoOauth *self, IoObject *locals, IoMessage *m);
IoObject *IoOauth_getAccessKeyAndSecretFromUrl(IoOauth *self, IoObject *locals, IoMessage *m);

IoObject *IoOauth_requestUrl(IoOauth *self, IoObject *locals, IoMessage *m);

#endif
