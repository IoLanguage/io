
//metadoc Oauth copyright Steve Dekorte 2002
//metadoc Oauth license BSD revised
//metadoc Oauth category Digests
/*metadoc Oauth description
An object for calculating Oauth hashes. Each hash calculation should instantiate its own Oauth instance.
<p>
Example:
<pre>
digest := Oauth clone
digest appendSeq("this is a message")
out := digest OauthString
</pre>
*/

#include "IoOauth.h"
#include "IoState.h"
#include "IoSeq.h"

#define DATA(self) ((IoOauthData *)IoObject_dataPointer(self))

IoTag *IoOauth_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Oauth");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoOauth_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoOauth_rawClone);
	return tag;
}

IoOauth *IoOauth_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoOauth_newTag(state));

	IoObject_setDataPointer_(self, Oauth_new());

	IoState_registerProtoWithFunc_(state, self, IoOauth_proto);

	{
		IoMethodTable methodTable[] = {
		{"setConsumerKey", IoOauth_setConsumerKey},
		{"setConsumerSecret", IoOauth_setConsumerSecret},
		{"getOauthTokenAndSecretFromUrl", IoOauth_getOauthTokenAndSecretFromUrl},
		{"getAccessKeyAndSecretFromUrl", IoOauth_getAccessKeyAndSecretFromUrl},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoOauth *IoOauth_rawClone(IoOauth *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, Oauth_new());
	return self;
}

IoOauth *IoOauth_new(void *state)
{
	IoOauth *proto = IoState_protoWithInitFunction_(state, IoOauth_proto);
	return IOCLONE(proto);
}

void IoOauth_free(IoOauth *self)
{
	Oauth_free(IoObject_dataPointer(self));
}

/* ----------------------------------------------------------- */

IoObject *IoOauth_setConsumerKey(IoOauth *self, IoObject *locals, IoMessage *m)
{
	/*doc Oauth setConsumerKey(aSequence)
	Sets the consumer key. Returns self.
	*/
	IoSeq *k = IoMessage_locals_seqArgAt_(m, locals, 0);

	Oauth_setConsumerKey_(IoObject_dataPointer(self), CSTRING(k));
	return self;
}

IoObject *IoOauth_setConsumerSecret(IoOauth *self, IoObject *locals, IoMessage *m)
{
	/*doc Oauth setConsumerKey(aSequence)
	Sets the consumer key. Returns self.
	*/
	IoSeq *k = IoMessage_locals_seqArgAt_(m, locals, 0);

	Oauth_setConsumerSecret_(IoObject_dataPointer(self), CSTRING(k));
	return self;
}

IoObject *IoOauth_getOauthTokenAndSecretFromUrl(IoOauth *self, IoObject *locals, IoMessage *m)
{
	/*doc Oauth getOauthTokenAndSecretFromUrl(aSequence)
	getOauthTokenAndSecretFromUrl Returns self.
	*/
	IoSeq *url = IoMessage_locals_seqArgAt_(m, locals, 0);

	Oauth_getOauthTokenAndSecretFromUrl_(IoObject_dataPointer(self), CSTRING(url));
	return self;
}

IoObject *IoOauth_getAccessKeyAndSecretFromUrl(IoOauth *self, IoObject *locals, IoMessage *m)
{
	/*doc Oauth getAccessKeyAndSecretFromUrl(aSequence)
	getAccessKeyAndSecretFromUrl Returns self.
	*/
	IoSeq *url = IoMessage_locals_seqArgAt_(m, locals, 0);

	Oauth_getAccessKeyAndSecretFromUrl_(IoObject_dataPointer(self), CSTRING(url));
	return self;
}

IoObject *IoOauth_requestUrl(IoOauth *self, IoObject *locals, IoMessage *m)
{
	/*doc Oauth setConsumerKey(aSequence)
	Sets the consumer key. Returns self.
	*/
	IoSeq *url = IoMessage_locals_seqArgAt_(m, locals, 0);

	Oauth_requestUrl_(IoObject_dataPointer(self), CSTRING(url));
	return self;
}

