//metadoc SecureClient copyright Joe Osborn, 2007
//metadoc SecureClient license BSD revised
//metadoc SecureClient category Networking
//metadoc SecureClient dependsOn Client
/*metadoc SecureClient description
Interface to secure network communication. 
A SecureClient is a wrapper on an OpenSSL SSL_CTX object
and supports both TLSv1 and DTLSv1.
Example:

<pre>
//...
</pre>
*/

/*
TODO:
+ Nonblocking TLS client
+ Nonblocking DTLS client
- Server cert authentication & callback
- Nonblocking SSLv3 client
- Nonblocking SSLv3/TLS client
*/

#include "IoSecureClient.h"
#include "IoMessage.h"
#include "IoSecureSocket.h"
#include "common.h"

IoSecureClient *IoMessage_locals_secureClientArgAt_(IoMessage *self, IoObject *locals, int n)
{
	IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);
	
	if (!ISSECURECLIENT(v)) 
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "SecureClient");
	}
	
	return v;
}

#define OCTX(self) ((SSL_CTX *)IoObject_dataPointer(self))

IoTag *IoSecureClient_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("SecureClient");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSecureClient_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSecureClient_rawClone);
	
	return tag;
}

IoSecureClient *IoSecureClient_proto(void *state)
{
	initSSL();
	
	IoObject *self = IoObject_new(state);
	
	IoObject_tag_(self, IoSecureClient_newTag(state));

	SSL_CTX *ctx = SSL_CTX_new(TLSv1_client_method());
	if(ctx == NULL)
	{
		ERR_print_errors_fp(stderr);
	}
	IoObject_setDataPointer_(self, ctx);
	SSL_CTX_set_default_passwd_cb(ctx, IoSecureSockets_Password_Callback);
	SSL_CTX_set_default_passwd_cb_userdata(ctx, self);
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, IoSecureSockets_Verify_Callback);
	
			#ifdef DTLS_IMPLEMENTED
		IoObject_setSlot_to_(self, IOSYMBOL("supportsDTLS"), IOTRUE(self));
			#else
		IoObject_setSlot_to_(self, IOSYMBOL("supportsDTLS"), IOFALSE(self));
			#endif
	
	IoState_registerProtoWithFunc_((IoState *)state, self, IoSecureClient_proto);
	
	{
		IoMethodTable methodTable[] = {
		{"setKeyFile", IoSecureClient_setKeyFile},
		//doc SecureClient setKeyFile(aPath) Sets the key file. Returns self.
		
		{"setCertFile", IoSecureClient_setCertFile},
		//doc SecureClient setCertFile(aPath) Sets the Certificate file. Returns self.
		
		{"setCAFile", IoSecureClient_setCAFile},
		//doc SecureClient setCAFile(aPath) Sets the CA file. Returns self.
		
		{"setCRLFile", IoSecureClient_setCRLFile},
		//doc SecureClient setCRLFile(aPath) Sets the CRL file. Returns self.
		
		{"useTLS", IoSecureClient_useTLS},
		//doc SecureClient useTLS Returns useTLS value.
		
		{"useDTLS", IoSecureClient_useDTLS},
		//doc SecureClient useDTLS Returns useDTLS value.
		
		{"connectionToServer", IoSecureClient_connectionToServer},
		//doc SecureClient connectionToServer Returns connectionToServer.
		{NULL, NULL}
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	
	return self;
}

IoSecureClient *IoSecureClient_rawClone(IoSecureClient *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	SSL_CTX *ctx = SSL_CTX_new(TLSv1_client_method());
	if(ctx == NULL)
	{
		ERR_print_errors_fp(stderr);
	}
	IoObject_setDataPointer_(self, ctx);	
	SSL_CTX_set_default_passwd_cb(ctx, IoSecureSockets_Password_Callback);
	SSL_CTX_set_default_passwd_cb_userdata(ctx, self);
	SSL_CTX_set_verify(ctx, SSL_CTX_get_verify_mode(OCTX(proto)), SSL_CTX_get_verify_callback(OCTX(proto)));
	//need to do more work for the copy to be correct, also need to copy SSL method and certs.
	return self; 
}

void IoSecureClient_free(IoSecureClient *self) 
{
	SSL_CTX_free(OCTX(self));
}

IoSecureClient *IoSecureClient_setKeyFile(IoSecureClient *self, IoObject *locals, IoMessage *msg)
{
	SSL_CTX *ctx = OCTX(self);
	IoSeq *pathSeq = IoMessage_locals_seqArgAt_(msg, locals, 0);
	char *path = IoSeq_asCString(pathSeq);
	if(ctx == NULL)
	{
		IoState_error_(IOSTATE, msg, "SecureClient has no SSL_CTX");
		return IONIL(self);
	}
	if(SSL_CTX_use_PrivateKey_file(ctx, path, SSL_FILETYPE_PEM) != 1)
	{	
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "Error loading private key from file %s\n", path);
		return IONIL(self);
	}
	return self;
}

IoSecureClient *IoSecureClient_setCertFile(IoSecureClient *self, IoObject *locals, IoMessage *msg)
{
	SSL_CTX *ctx = OCTX(self);
	IoSeq *pathSeq = IoMessage_locals_seqArgAt_(msg, locals, 0);
	char *path = IoSeq_asCString(pathSeq);
	if(ctx == NULL)
	{
		IoState_error_(IOSTATE, msg, "SecureClient has no SSL_CTX");
		return IONIL(self);
	}
	if(SSL_CTX_use_certificate_chain_file(ctx, path) != 1)
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "Error loading certificate from file %s\n", path);
		return IONIL(self);
	}
	return self;
}

IoSecureClient *IoSecureClient_setCAFile(IoSecureClient *self, IoObject *locals, IoMessage *msg)
{
	SSL_CTX *ctx = OCTX(self);
	IoSeq *pathSeq = IoMessage_locals_seqArgAt_(msg, locals, 0);
	char *path = IoSeq_asCString(pathSeq);
	if(ctx == NULL)
	{
		IoState_error_(IOSTATE, msg, "SecureClient has no SSL_CTX");
		return IONIL(self);
	}
	if(SSL_CTX_load_verify_locations(ctx, path, NULL) != 1)
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "Error loading CA certificate from file %s\n", path);
		return IONIL(self);
	}
	return self;
}


IoSecureClient *IoSecureClient_setCRLFile(IoSecureClient *self, IoObject *locals, IoMessage *msg)
{
	SSL_CTX *ctx = OCTX(self);
	IoSeq *pathSeq = IoMessage_locals_seqArgAt_(msg, locals, 0);
	char *path = IoSeq_asCString(pathSeq);
	if(ctx == NULL)
	{
		IoState_error_(IOSTATE, msg, "SecureClient has no SSL_CTX");
		return IONIL(self);
	}
	X509_STORE *store = SSL_CTX_get_cert_store(ctx);
	X509_STORE_set_verify_cb_func(store, IoSecureSockets_Verify_CRL_Callback);
	X509_STORE_set_flags (store, X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL);
	X509_LOOKUP *lookup;
	if (!(lookup = X509_STORE_add_lookup (store, X509_LOOKUP_file ())))
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "Error creating X509_LOOKUP");
	  	return IONIL(self);
	}
	if (X509_load_crl_file(lookup, path, X509_FILETYPE_PEM) != 1)
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "Error loading CRL from file %s\n", path);
	  	return IONIL(self);
	}
	
	return self;
}

IoSecureClient *IoSecureClient_useTLS(IoSecureClient *self, IoObject *locals, IoMessage *msg)
{
	SSL_CTX *ctx = OCTX(self);
	if(SSL_CTX_set_ssl_version(ctx, TLSv1_client_method()) != 1)
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "Error using TLS");
		return IONIL(self);
	}
	return self;
}
IoSecureClient *IoSecureClient_useDTLS(IoSecureClient *self, IoObject *locals, IoMessage *msg)
{
	#ifdef DTLS_IMPLEMENTED
	SSL_CTX *ctx = OCTX(self);
	if(SSL_CTX_set_ssl_version(ctx, DTLSv1_client_method()) != 1)
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "Error using DTLS");
		return IONIL(self);
	}
	SSL_CTX_set_read_ahead(ctx, 1);
	return self;
	#else
	IoState_error_(IOSTATE, msg, "Addon built against OpenSSL older than 0.9.8; DTLS is not supported.");
	return IONIL(self);
	#endif
}

IoSecureSocket *IoSecureClient_connectionToServer(IoSecureClient *self, IoObject *locals, IoMessage *msg)
{
	SSL_CTX *ctx = OCTX(self);
	char *serverIP = CSTRING(IoMessage_locals_seqArgAt_(msg, locals, 0));
	char *port = CSTRING(IoMessage_locals_seqArgAt_(msg, locals, 1));
	IoSecureSocket *sock = IoSecureSocket_newWithCTX_(IoObject_state(self), ctx);
	if(sock == IONIL(self))
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "Error creating SecureSocket");
		return sock;
	}
	if(!IoSecureSocket_createBIO(sock, serverIP, port))
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "Error creating BIO for SecureSocket");
		return IONIL(self);
	}
	return sock;
}