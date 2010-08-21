
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

#define DATA(self) ((Oauth *)IoObject_dataPointer(self))

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
			{"consumerKey", IoOauth_consumerKey},
			
			{"setConsumerSecret", IoOauth_setConsumerSecret},
			{"consumerSecret", IoOauth_consumerSecret},

			{"setOauthToken", IoOauth_setOauthToken},
			{"oauthToken", IoOauth_oauthToken},
			
			{"setOauthSecret", IoOauth_setOauthSecret},
			{"oauthSecret", IoOauth_oauthSecret},
			
			{"setAccessKey", IoOauth_setAccessKey},
			{"accessKey", IoOauth_accessKey},
			
			{"setAccessSecret", IoOauth_setAccessSecret},
			{"accessSecret", IoOauth_accessSecret},
			
			{"getOauthTokenAndSecretFromUrl", IoOauth_getOauthTokenAndSecretFromUrl},
			{"getAccessKeyAndSecretFromUrl", IoOauth_getAccessKeyAndSecretFromUrl},
			
			{"requestUrl", IoOauth_requestUrl},
			
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
	printf("setting consumer key 1\n");
	Oauth_setConsumerKey_(IoObject_dataPointer(self), CSTRING(k));
	printf("setting consumer key 2\n");
	return self;
}

IoObject *IoOauth_consumerKey(IoOauth *self, IoObject *locals, IoMessage *m)
{
	/*doc Oauth consumerKey
	Returns consumerKey.
	*/

	return IOSYMBOL(string_data(Oauth_consumerKey(IoObject_dataPointer(self))));
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

IoObject *IoOauth_consumerSecret(IoOauth *self, IoObject *locals, IoMessage *m)
{
	/*doc Oauth consumerSecret
	Returns consumerSecret.
	*/

	return IOSYMBOL(string_data(Oauth_consumerSecret(IoObject_dataPointer(self))));
}

/* ----------------------------------------------------------- */

IoObject *IoOauth_setOauthToken(IoOauth *self, IoObject *locals, IoMessage *m)
{
	/*doc Oauth setOauthToken(aSequence)
	Sets the oauth key. Returns self.
	*/
	IoSeq *k = IoMessage_locals_seqArgAt_(m, locals, 0);

	Oauth_setOauthToken_(IoObject_dataPointer(self), CSTRING(k));
	return self;
}

IoObject *IoOauth_oauthToken(IoOauth *self, IoObject *locals, IoMessage *m)
{
	/*doc Oauth oauthToken
	Returns oauthToken.
	*/

	return IOSYMBOL(string_data(Oauth_oauthToken(IoObject_dataPointer(self))));
}

IoObject *IoOauth_setOauthSecret(IoOauth *self, IoObject *locals, IoMessage *m)
{
	/*doc Oauth setOauthSecret(aSequence)
	Sets the consumer key. Returns self.
	*/
	IoSeq *k = IoMessage_locals_seqArgAt_(m, locals, 0);

	Oauth_setOauthSecret_(IoObject_dataPointer(self), CSTRING(k));
	return self;
}

IoObject *IoOauth_oauthSecret(IoOauth *self, IoObject *locals, IoMessage *m)
{
	/*doc Oauth oauthSecret
	Returns oauthSecret.
	*/

	return IOSYMBOL(string_data(Oauth_oauthSecret(IoObject_dataPointer(self))));
}


/* ----------------------------------------------------------- */

IoObject *IoOauth_setAccessKey(IoOauth *self, IoObject *locals, IoMessage *m)
{
	/*doc Oauth setAccessKey(aSequence)
	Sets the access key. Returns self.
	*/
	IoSeq *k = IoMessage_locals_seqArgAt_(m, locals, 0);

	Oauth_setAccessKey_(IoObject_dataPointer(self), CSTRING(k));
	return self;
}

IoObject *IoOauth_accessKey(IoOauth *self, IoObject *locals, IoMessage *m)
{
	/*doc Oauth accessKey
	Returns accessKey.
	*/

	return IOSYMBOL(string_data(Oauth_accessKey(IoObject_dataPointer(self))));
}


IoObject *IoOauth_setAccessSecret(IoOauth *self, IoObject *locals, IoMessage *m)
{
	/*doc Oauth setAccessSecret(aSequence)
	Sets the access secret. Returns self.
	*/
	IoSeq *k = IoMessage_locals_seqArgAt_(m, locals, 0);

	Oauth_setAccessSecret_(IoObject_dataPointer(self), CSTRING(k));
	return self;
}

IoObject *IoOauth_accessSecret(IoOauth *self, IoObject *locals, IoMessage *m)
{
	/*doc Oauth accessSecret
	Returns accessSecret.
	*/

	return IOSYMBOL(string_data(Oauth_accessSecret(IoObject_dataPointer(self))));
}

/* ----------------------------------------------------------- */

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
	/*doc Oauth requestUrl(aUrlString, postContent)
	Request the given URL. If postContent is not provided, a GET request is sent. Returns self.
	*/
	IoSeq *url = IoMessage_locals_seqArgAt_(m, locals, 0);
	Oauth *oa = IoObject_dataPointer(self);
	
	if (IoMessage_argCount(m) > 1)
	{
		IoSeq *postContent = IoMessage_locals_seqArgAt_(m, locals, 1);
		Oauth_requestUrl_(oa, 
			CSTRING(url), 
			CSTRING(postContent), 
			IOSEQ_LENGTH(postContent));
	}
	else 
	{
		Oauth_requestUrl_(oa, 
			CSTRING(url), 
			NULL, 
			0);
	}

	struct string *s = Oauth_responseData(oa);
	return IOSEQ(string_data(s), string_len(s));
}

