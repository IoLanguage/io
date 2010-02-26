//metadoc SecureServer copyright Joe Osborn, 2007
//metadoc SecureServer license BSD revised
//metadoc SecureServer category Networking
//metadoc SecureServer dependsOn Server
/*metadoc SecureServer description
Interface to secure network communication. 
A SecureServer is a wrapper on an OpenSSL SSL_CTX object
and supports both TLSv1 and DTLSv1.
Example:

<pre>
//...
</pre>
*/

#include "IoSecureServer.h"
#include "IoMessage.h"
#include "common.h"
#include "IPAddress.h"
#include "IoSocket.h"
#include "IoNumber.h"

IoSecureServer *IoMessage_locals_secureServerArgAt_(IoMessage *self, IoObject *locals, int n)
{
	IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);
	
	if (!ISSECURESERVER(v)) 
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "SecureServer");
	}
	
	return v;
}

typedef struct {
	SSL_CTX *ssl_ctx;

} SecureServerData;

#define DATA(self) ((SecureServerData *)IoObject_dataPointer(self))
#define OCTX(self) (DATA(self)->ssl_ctx)

IoTag *IoSecureServer_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("SecureServer");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSecureServer_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSecureServer_rawClone);	
	return tag;
}

IoSecureServer *IoSecureServer_proto(void *state)
{
	initSSL();
	
	IoObject *self = IoObject_new(state);
	
	IoObject_tag_(self, IoSecureServer_newTag(state));
	
	IoObject_setDataPointer_(self, (SecureServerData *)calloc(1, sizeof(SecureServerData)));
	SSL_CTX *ctx = SSL_CTX_new(TLSv1_server_method());
	DATA(self)->ssl_ctx = ctx;
	SSL_CTX_set_default_passwd_cb(ctx, IoSecureSockets_Password_Callback);
	SSL_CTX_set_default_passwd_cb_userdata(ctx, self);
	SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, IoSecureSockets_Verify_Callback);
	
	#ifdef DTLS_IMPLEMENTED
	IoObject_setSlot_to_(self, IOSYMBOL("supportsDTLS"), IOTRUE(self));
	#else
	IoObject_setSlot_to_(self, IOSYMBOL("supportsDTLS"), IOFALSE(self));
	#endif
	//doc SecureServer supportsDTLS Returns true if server supports DTLS, false otherwise.

	IoState_registerProtoWithFunc_((IoState *)state, self, IoSecureServer_proto);
	
	{
		IoMethodTable methodTable[] = {
		{"setKeyFile", IoSecureServer_setKeyFile},
		//doc SecureServer setKeyFile(path) Sets the key file. Returns self.
		
		{"setCertFile", IoSecureServer_setCertFile},
		//doc SecureServer setCertFile(path) Sets the certificate file. Returns self.
		
		{"setCAFile", IoSecureServer_setCAFile},
		//doc SecureServer setCAFile(path) Sets the CA file. Returns self.
		
		{"setCRLFile", IoSecureServer_setCRLFile},
		//doc SecureServer setCRLFile(path) Sets the CRL file. Returns self.
		
		{"useTLS", IoSecureServer_useTLS},
		//doc SecureServer useTLS Returns useTLS value.

		{"useDTLS", IoSecureServer_useDTLS},
		//doc SecureServer useDTLS Returns useDTLS value.

		{"setRequiresClientCertificate", IoSecureServer_setRequiresClientCertificate},
		//doc SecureServer setRequiresClientCertificate(aBool) Sets the requires client certificate attribute. Returns self.
		
		{"tlsWrap", IoSecureServer_tlsWrap},
		//doc SecureServer tlsWrap Returns tlsWrap value.
		
		#ifdef DTLS_IMPLEMENTED
		{"dtlsWrap", IoSecureServer_dtlsWrap},
		//doc SecureServer dtlsWrap Returns dtlsWrap value.
		
		{"udpRecvIP", IoSecureServer_udpRecvIP},
		//doc SecureServer udpRecvIP Returns udpRecvIP value.
		
		{"dispatchUdp", IoSecureServer_dispatchUDP},
		//doc SecureServer dispatchUdp Returns dispatchUdp value.
		
		#endif
		{NULL, NULL}
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	
	return self;
}

IoSecureServer *IoSecureServer_rawClone(IoSecureServer *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, (SecureServerData *)calloc(1, sizeof(SecureServerData)));
	SSL_CTX *ctx = SSL_CTX_new(TLSv1_server_method());
	DATA(self)->ssl_ctx = ctx;	
	SSL_CTX_set_default_passwd_cb(ctx, IoSecureSockets_Password_Callback);
	SSL_CTX_set_default_passwd_cb_userdata(ctx, self);
	SSL_CTX_set_verify(ctx, SSL_CTX_get_verify_mode(OCTX(proto)), SSL_CTX_get_verify_callback(OCTX(proto)));
	//need to do more work for the copy to be correct, also need to copy SSL method and certs.
	return self; 
}

void IoSecureServer_free(IoSecureServer *self) 
{
	SSL_CTX_free(OCTX(self));
	free(DATA(self));
}

IoSecureServer *IoSecureServer_setKeyFile(IoSecureServer *self, IoObject *locals, IoMessage *msg)
{
	SSL_CTX *ctx = OCTX(self);
	IoSeq *pathSeq = IoMessage_locals_seqArgAt_(msg, locals, 0);
	char *path = IoSeq_asCString(pathSeq);
	if(ctx == NULL)
	{
		IoState_error_(IOSTATE, msg, "SecureServer has no SSL_CTX");
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

IoSecureServer *IoSecureServer_setCertFile(IoSecureServer *self, IoObject *locals, IoMessage *msg)
{
	SSL_CTX *ctx = OCTX(self);
	IoSeq *pathSeq = IoMessage_locals_seqArgAt_(msg, locals, 0);
	char *path = IoSeq_asCString(pathSeq);
	if(ctx == NULL)
	{
		IoState_error_(IOSTATE, msg, "SecureServer has no SSL_CTX");
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

IoSecureServer *IoSecureServer_setCAFile(IoSecureServer *self, IoObject *locals, IoMessage *msg)
{
	SSL_CTX *ctx = OCTX(self);
	IoSeq *pathSeq = IoMessage_locals_seqArgAt_(msg, locals, 0);
	char *path = IoSeq_asCString(pathSeq);
	if(ctx == NULL)
	{
		IoState_error_(IOSTATE, msg, "SecureServer has no SSL_CTX");
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


IoSecureServer *IoSecureServer_setCRLFile(IoSecureServer *self, IoObject *locals, IoMessage *msg)
{
	SSL_CTX *ctx = OCTX(self);
	IoSeq *pathSeq = IoMessage_locals_seqArgAt_(msg, locals, 0);
	char *path = IoSeq_asCString(pathSeq);
	if(ctx == NULL)
	{
		IoState_error_(IOSTATE, msg, "SecureServer has no SSL_CTX");
		return IONIL(self);
	}
	X509_STORE *store = SSL_CTX_get_cert_store(ctx);
	X509_STORE_set_verify_cb_func(store, IoSecureSockets_Verify_CRL_Callback);
	X509_STORE_set_flags (store, X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL);
	X509_LOOKUP *lookup;
	if (!(lookup = X509_STORE_add_lookup (store, X509_LOOKUP_file ())))
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "Error creating X509_LOOKUP object.");
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

IoSecureServer *IoSecureServer_setRequiresClientCertificate(IoSecureServer *self, IoObject *locals, IoMessage *msg)
{
	SSL_CTX *ctx = OCTX(self);
	if(IoMessage_locals_valueArgAt_(msg, locals, 0) == IOTRUE(self))
	{
		SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, IoSecureSockets_Verify_Callback);
	}
	else
	{
		SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, IoSecureSockets_Verify_Callback);
	}
	return self;
}

IoSecureServer *IoSecureServer_useTLS(IoSecureServer *self, IoObject *locals, IoMessage *msg)
{
	SSL_CTX *ctx = OCTX(self);
	if(SSL_CTX_set_ssl_version(ctx, TLSv1_server_method()) != 1)
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "Error using TLS");
		return IONIL(self);
	}
	return self;
}
IoSecureServer *IoSecureServer_useDTLS(IoSecureServer *self, IoObject *locals, IoMessage *msg)
{
	#ifdef DTLS_IMPLEMENTED
	SSL_CTX *ctx = OCTX(self);
	if(SSL_CTX_set_ssl_version(ctx, DTLSv1_server_method()) != 1)
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
IoSecureSocket *IoSecureServer_tlsWrap(IoSecureServer *self, IoObject *locals, IoMessage *msg)
{
	SSL_CTX *ctx = OCTX(self);
	IoSocket *sock = IoMessage_locals_socketArgAt_(msg, locals, 0);
	IoNumber *port = IoObject_getSlot_(sock, IOSYMBOL("port"));
	SSL *ssl = SSL_new(ctx);
	SSL_set_fd(ssl, IoSocket_rawDescriptor(sock));
	set_nonblocking(SSL_get_rbio(ssl));
	set_nonblocking(SSL_get_wbio(ssl));
	SSL_set_accept_state(ssl);
	SSL_set_mode(ssl, SSL_MODE_ENABLE_PARTIAL_WRITE |
		SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
	IoIPAddress *ioaddr = IoIPAddress_new(IoObject_state(self));
	IPAddress *iaddr = IoIPAddress_rawIPAddress(ioaddr);
	IPAddress_setIp_(iaddr, "0.0.0.0");
	IPAddress_setPort_(iaddr, IoNumber_asLong(port));
	IoSecureSocket *ssock = IoSecureSocket_newWithSSL_IP_(IoObject_state(self), ssl, ioaddr);
	return ssock;
}

	#ifdef DTLS_IMPLEMENTED
IoIPAddress *IoSecureServer_udpRecvIP(IoSecureServer *self, IoObject *locals, IoMessage *msg)
{
	SSL_CTX *ctx = OCTX(self);
	IoSocket *sock = IoObject_getSlot_(self, IOSYMBOL("socket"));
	IoIPAddress *ioaddress = IoObject_getSlot_(sock, IOSYMBOL("ipAddress"));
	IPAddress *address = IoIPAddress_rawIPAddress(ioaddress);
	socklen_t addressSize = IPAddress_size(address);
	int fd = IoSocket_rawDescriptor(sock);
	int bytesPerRead = 1;
	char *buffer = calloc(1, sizeof(char));
	int bytesRead = recvfrom(fd, buffer, bytesPerRead, MSG_PEEK, IPAddress_sockaddr(address), &addressSize);
	if (bytesRead > 0)
	{
		IPAddress_setSize_(address, addressSize);
	}
	free(buffer);
	return ioaddress;
}
IoSecureSocket *IoSecureServer_dtlsWrap(IoSecureServer *self, IoObject *locals, IoMessage *msg)
{
	SSL_CTX *ctx = OCTX(self);
	IoSocket *sock = IoObject_getSlot_(self, IOSYMBOL("socket"));
	IoIPAddress *ioip = IoMessage_locals_addressArgAt_(msg, locals, 0);
	IPAddress *ip = IoIPAddress_rawIPAddress(ioip);
	struct sockaddr *addr = IPAddress_sockaddr(ip);
	IoNumber *port = IoObject_getSlot_(sock, IOSYMBOL("port"));
	int fd = IoSocket_rawDescriptor(sock);
	SSL *ssl = SSL_new(ctx);
	BIO *rbio = BIO_new(BIO_s_mem());
	BIO_set_mem_eof_return(rbio, -1);
	BIO *wbio = BIO_new_dgram(fd, BIO_NOCLOSE);
	BIO_dgram_set_peer(wbio, addr);
	SSL_set_bio(ssl, rbio, wbio);
	SSL_set_accept_state(ssl);
	set_nonblocking(wbio);
	SSL_set_mode(ssl, SSL_MODE_ENABLE_PARTIAL_WRITE |
		SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
	IoSecureSocket *ssock = IoSecureSocket_newWithSSL_IP_(IoObject_state(self), ssl, ioip);
	return ssock;
}
IoSecureServer *IoSecureServer_dispatchUDP(IoSecureServer *self, IoObject *locals, IoMessage *msg)
{
	SSL_CTX *ctx = OCTX(self);
	IoSocket *sock = IoObject_getSlot_(self, IOSYMBOL("socket"));
	IoIPAddress *ioaddress = IoObject_getSlot_(sock, IOSYMBOL("ipAddress"));
	IPAddress *address = IoIPAddress_rawIPAddress(ioaddress);
	socklen_t addressSize = IPAddress_size(address);
	IoSecureSocket *ssock = IoMessage_locals_secureSocketArgAt_(msg, locals, 0);
	SSL *ssl = IoSecureSocket_SSL(ssock);
	int fd = IoSocket_rawDescriptor(sock);
	int bytesPerRead = IoNumber_asLong(IoObject_getSlot_(sock, IOSYMBOL("bytesPerRead")));
	char *buffer = calloc(bytesPerRead, sizeof(char));
	//debugPrintf("errno going in: %i\n", errno);
	int bytesRead = recvfrom(fd, buffer, bytesPerRead, 0, IPAddress_sockaddr(address), &addressSize);
	//debugPrintf("Read %i/%i : %i\n", bytesRead, bytesPerRead, errno);
	if(bytesRead > 0)
	{
		BIO_write(ssl->rbio, buffer, bytesRead);
	}
	free(buffer);
	return IOBOOL(self, bytesRead > 0);
}
	#endif