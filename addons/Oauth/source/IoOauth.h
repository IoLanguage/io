
//metadoc Oauth copyright Steve Dekorte 2002


#ifndef IOOauth_DEFINED
#define IOOauth_DEFINED 1

#include "IoObject.h"
#include "IoNumber.h"
#include "Oauth/Oauth.h"

#define ISOauth(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoOauth_rawClone)

typedef IoObject IoOauth;

typedef struct
{
	Oauth *oauth;
} IoOauthData;

IoOauth *IoOauth_rawClone(IoOauth *self);
IoOauth *IoOauth_proto(void *state);
IoOauth *IoOauth_new(void *state);

void IoOauth_free(IoOauth *self);
void IoOauth_mark(IoOauth *self);

/* ----------------------------------------------------------- */

IoObject *IoOauth_setConsumerKey(IoOauth *self, IoObject *locals, IoMessage *m);
IoObject *IoOauth_setConsumerSecret(IoOauth *self, IoObject *locals, IoMessage *m);
IoObject *IoOauth_getOauthTokenAndSecretFromUrl(IoOauth *self, IoObject *locals, IoMessage *m);
IoObject *IoOauth_getAccessKeyAndSecretFromUrl(IoOauth *self, IoObject *locals, IoMessage *m);
IoObject *IoOauth_requestUrl(IoOauth *self, IoObject *locals, IoMessage *m);

#endif
