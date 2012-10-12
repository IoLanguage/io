//metadoc SecureSocekt copyright Joe Osborn, 2007
//metadoc SecureSocket license BSD revised
//metadoc SecureSocket category Networking
//metadoc SecureSocket dependsOn Socket
/*metdoc SecureSocket description
Interface to secure network communication. 
Sockets will auto-yield to other coroutines while waiting on a request. 
All blocking operations use the timeout settings of the socket. 
Reads are appended to the socket's read buffer which can be accessed using the readBuffer method.
A SecureSocket is a wrapper on an OpenSSL SSL object and supports both TLSv1 and DTLSv1.
Example:

<pre>
//...
</pre>
*/

#include "IoSecureSocket.h"
#include "IoNumber.h"
#include <sys/uio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include "IPAddress.h"
#include "IoSocket.h"
#include "common.h"

static const char *protoId = "SecureSocket";

IoSecureSocket *IoMessage_locals_secureSocketArgAt_(IoMessage *self, IoObject *locals, int n)
{
	IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);
	
	if (!ISSECURESOCKET(v)) 
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "SecureSocket");
	}
	
	return v;
}

typedef struct {
	SSL *ssl;
} SecureSocket;

#define SSOCK(self) ((SecureSocket *)IoObject_dataPointer(self))
#define OSSL(self) (SSOCK(self)->ssl)
#define SIOP(self) (IoObject_getSlot_(self, IOSYMBOL("ipAddress")))


SSL *IoSecureSocket_SSL(IoSecureSocket *self)
{
	return OSSL(self);
}

IoTag *IoSecureSocket_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSecureSocket_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSecureSocket_rawClone);	
	return tag;
}

IoSecureSocket *IoSecureSocket_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	
	IoObject_tag_(self, IoSecureSocket_newTag(state));
	IoObject_setDataPointer_(self, NULL);
	IoObject_setSlot_to_(self, IOSYMBOL("ValidationOkay"), IONUMBER(X509_V_OK));
	//doc SecureSocket ValidationOkay Returns ValidationOkay attribute as a number.
	IoObject_setSlot_to_(self, IOSYMBOL("ValidationFail"), IONUMBER(X509_V_ERR_APPLICATION_VERIFICATION));
	//doc SecureSocket ValidationFail Returns ValidationFail attribute as a number.
	
	#ifdef DTLS_IMPLEMENTED
	IoObject_setSlot_to_(self, IOSYMBOL("supportsDTLS"), IOTRUE(self));
	#else
	IoObject_setSlot_to_(self, IOSYMBOL("supportsDTLS"), IOFALSE(self));
	#endif
	//doc SecureSocket supportsDTLS Returns true if server supports DTLS, false otherwise.
	
	IoState_registerProtoWithFunc_((IoState *)state, self, IoSecureSocket_proto);

	{
		IoMethodTable methodTable[] = {
		{"rawCertificate", IoSecureSocket_rawCertificate},
		//doc SecureSocket rawCertificate Returns rawCertificate.

		{"rawPeerCertificate", IoSecureSocket_rawPeerCertificate},
		//doc SecureSocket rawPeerCertificate Returns rawPeerCertificate.

		{"descriptorId", IoSecureSocket_descriptorId},
		//doc SecureSocket descriptorId Returns descriptorId.
		
		{"isConnected", IoSecureSocket_isConnected},
		//doc SecureSocket isConnected Returns true if socket is connected, false otherwise.
		
		{"isOpen", IoSecureSocket_isOpen},
		//doc SecureSocket isOpen Returns true if socket is open, false otherwise.
		
		{"asyncConnectBIO", IoSecureSocket_asyncConnectBIO},
		//doc SecureSocket asyncConnectBIO Performs asyncConnectBIO.

		{"asyncConnectSSL", IoSecureSocket_asyncConnectSSL},
		//doc SecureSocket asyncConnectSSL Performs asyncConnectSSL.

		{"rawValidate", IoSecureSocket_rawValidate},
		//doc SecureSocket rawValidate Performs rawValidate.
		
		{"wantsRead", IoSecureSocket_wantsRead},
		//doc SecureSocket wantsRead Returns wantsRead attribute.

		{"wantsWrite", IoSecureSocket_wantsWrite},
		//doc SecureSocket wantsWrite Returns IoSecureSocket_wantsWrite attribute.

		{"asyncAccept", IoSecureSocket_asyncAcceptSSL},
		//doc SecureSocket asyncAccept Accept a connection asynchronously.

		{"asyncStreamRead", IoSecureSocket_asyncStreamRead},
		//doc SecureSocket asyncStreamRead Read available data and return it.

		{"asyncWrite", IoSecureSocket_asyncWrite},
		//doc SecureSocket asyncWrite(aSeq) Writes aSeq to the socket. Returns self.
		
		{"shutdown", IoSecureSocket_shutdown},
		//doc SecureSocket shutdown close the socket connection. Returns self.

		{"setSocketReadBufferSize", IoSecureSocket_setSocketReadBufferSize},
		//doc SecureSocket setSocketReadBufferSize(numberOfBytes) Sets read buffer size. Returns self.
		
		{"setSocketWriteBufferSize", IoSecureSocket_setSocketWriteBufferSize},
		//doc SecureSocket setSocketWriteBufferSize(numberOfBytes)  Sets write buffer size. Returns self.
		
		{"getSocketReadLowWaterMark", IoSecureSocket_getSocketReadLowWaterMark},
		//doc SecureSocket getSocketReadLowWaterMark Returns ReadLowWaterMark attribute.
		
		{"getSocketWriteLowWaterMark", IoSecureSocket_getSocketWriteLowWaterMark},
		//doc SecureSocket getSocketWriteLowWaterMark Returns WriteLowWaterMark attribute.

		{"setSocketReadLowWaterMark", IoSecureSocket_setSocketReadLowWaterMark},
		//doc SecureSocket setSocketReadLowWaterMark(numberOfBytes) Sets read low water mark. Returns self.

		{"setSocketWriteLowWaterMark", IoSecureSocket_setSocketWriteLowWaterMark},
		//doc SecureSocket setSocketWriteLowWaterMark(numberOfBytes)  Sets write low water mark. Returns self.
		
		{"isDatagram", IoSecureSocket_isDatagram},
		//doc SecureSocket isDatagram Returns isDatagram attribute.
		#ifdef DTLS_IMPLEMENTED
		{"asyncUdpRead", IoSecureSocket_asyncUdpRead},
		//doc SecureSocket asyncUdpRead ?

		{"asyncUdpServerRead", IoSecureSocket_asyncUdpServerRead},
		//doc SecureSocket asyncUdpServerRead ?

		{"rbioReady", IoSecureSocket_rbioReady},
		//doc SecureSocket rbioReady Returns rbioReady attribute.

		#endif
		{NULL, NULL}
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	
	{
	SecureSocket *ssock = (SecureSocket *)calloc(1, sizeof(SecureSocket));
	IoObject_setDataPointer_(self, ssock);
	}
	
	return self;
}

IoSecureSocket *IoSecureSocket_rawClone(IoSecureSocket *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	//this behavior is probably wrong, this method should probably never be used
	IoObject_setDataPointer_(self, NULL);	
	SecureSocket *ssock = (SecureSocket *)calloc(1, sizeof(SecureSocket));
	IoObject_setDataPointer_(self, ssock);
	return self; 
}

// ----------------------------------------------------------- 

IoSecureSocket *IoSecureSocket_newWithCTX_(void *state, SSL_CTX *ctx)
{
	IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
	IoSecureSocket *self = IOCLONE(proto);
	SecureSocket *ssock = (SecureSocket *)calloc(1, sizeof(SecureSocket));
	IoObject_setDataPointer_(self, ssock);
	ssock->ssl = SSL_new(ctx);
//	SSL_set_verify(ssock->ssl, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, IoSecureSockets_Verify_Callback);
	
	if(!OSSL(self))
	{
		return IONIL(self);
	}
	
	return self;
}

IoSecureSocket *IoSecureSocket_newWithSSL_IP_(void *state, SSL *ssl, IoIPAddress *ip)
{
	IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
	IoSecureSocket *self = IOCLONE(proto);
	SecureSocket *ssock = (SecureSocket *)calloc(1, sizeof(SecureSocket));
	ssock->ssl = ssl;
//	SSL_set_verify(ssl, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, IoSecureSockets_Verify_Callback);
	IoObject_setSlot_to_(self, IOSYMBOL("ipAddress"), ip);
	IoObject_setDataPointer_(self, ssock);
	return self;
}

void IoSecureSocket_free(IoSecureSocket *self) 
{
	if(OSSL(self)) SSL_free(OSSL(self));
	free(SSOCK(self));
}

BIO *IoSecureSocket_createBIO(IoSecureSocket *self, char *server, char *port)
{
	IoIPAddress *ioaddr = IoIPAddress_new(IoObject_state(self));
	IPAddress *iaddr = IoIPAddress_rawIPAddress(ioaddr);
	IPAddress_setIp_(iaddr, server);
	IPAddress_setPort_(iaddr, strtol(port, NULL, 10));
	IoObject_setSlot_to_(self, IOSYMBOL("ipAddress"), ioaddr);
	SSL *ssl = OSSL(self);
	BIO *conn;
		#ifdef DTLS_IMPLEMENTED
		if(SSL_usesDatagramSockets(ssl)) //dgram bio
		{
			struct sockaddr *addr = IPAddress_sockaddr(iaddr);
			int udpsock = socket(AF_INET, SOCK_DGRAM, 0);
			if(udpsock == -1)
			{
				return NULL;
			}
			int status = connect(udpsock, (struct sockaddr *)addr, sizeof(struct sockaddr_in));
			if(status == -1)
			{
				return NULL;
			}
			//debugPrintf("UDP socket connected\n");
			conn = BIO_new_dgram(udpsock, BIO_CLOSE);
			BIO_dgram_set_peer(conn, addr);
			BIO_ctrl_set_connected(conn, 1, &addr);
			struct timeval timeout;
			timeout.tv_sec = 10;
			timeout.tv_usec = 0;
			BIO_ctrl(conn, BIO_CTRL_DGRAM_SET_RECV_TIMEOUT, 0, &timeout);
			BIO_ctrl(conn, BIO_CTRL_DGRAM_SET_SEND_TIMEOUT, 0, &timeout);
			BIO_ctrl(conn, BIO_CTRL_DGRAM_MTU_DISCOVER, 0, NULL);
			SSL_set_bio(ssl, conn, conn);
			SSL_set_mode(ssl, SSL_MODE_ENABLE_PARTIAL_WRITE |
			 	SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
			SSL_set_connect_state(ssl);
			set_nonblocking(conn);
			return conn;
		}
		#endif
		//connect bio
		char *sport = calloc(strlen(server) + strlen(":") + strlen(port) + 1, sizeof(char));
		sport = strncat(sport, server, strlen(server));
		sport = strncat(sport, ":", 1);
		sport = strncat(sport, port, 5);
		conn = BIO_new_connect(sport);
		set_nonblocking(conn);
		SSL_set_bio(ssl, conn, conn);
		SSL_set_mode(ssl, SSL_MODE_ENABLE_PARTIAL_WRITE |
			SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);
		SSL_set_connect_state(ssl);
		return conn;	
}

IoObject *IoSecureSocket_asyncConnectBIO(IoSecureSocket *self, IoObject *locals, IoMessage *msg)
{
	SSL *ssl = OSSL(self);
	if(!ssl)
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "SSL object is null, this should never happen");
		return IONUMBER(0);
	}
	BIO *conn = SSL_get_wbio(ssl);
	if(!conn)
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "SSL BIO is null, this should never happen");
		return IONUMBER(0);
	}
	int status = BIO_do_connect(conn);
	if(status <= 0)
	{
		if(!BIO_should_retry(conn))
		{
			ERR_print_errors_fp(stderr);
			IoState_error_(IOSTATE, msg, "Error %i connecting BIO", status);
		}
	}
	return IONUMBER(status);
}
IoObject *IoSecureSocket_asyncConnectSSL(IoSecureSocket *self, IoObject *locals, IoMessage *msg)
{
	SSL *ssl = OSSL(self);
	if(!ssl)
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "SSL object is null, this should never happen");
		return IONIL(self);
	}
	int result = SSL_connect(ssl);
	if(result == 0 || result < -1)
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "Error %i connecting SSL", result);
	}
	if((result == -1) && !(SSL_want_read(ssl) || SSL_want_write(ssl)))
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "Error -1 connecting SSL, but SSL neither wants read nor write");
	}
	return IONUMBER(result);
}
IoObject *IoSecureSocket_asyncAcceptSSL(IoSecureSocket *self, IoObject *locals, IoMessage *msg)
{
	SSL *ssl = OSSL(self);
	if(!ssl)
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "SSL object is null, this should never happen");
		return IONUMBER(0);
	}
	int result = SSL_accept(ssl);
	if(result == 0 || result < -1)
	{
		ERR_print_errors_fp(stderr);
		IoState_error_(IOSTATE, msg, "Error %i accepting SSL connection", result);
	}
	return IONUMBER(result);
}
IoObject *IoSecureSocket_rawValidate(IoSecureSocket *self, IoObject *locals, IoMessage *msg)
{
	SSL *ssl = OSSL(self);
	return IONUMBER(SSL_get_verify_result(ssl));
}
IoObject *IoSecureSocket_isConnected(IoSecureSocket *self, IoObject *locals, IoMessage *msg)
{
	return IOBOOL(self, SSL_isConnected(OSSL(self)));
}
IoObject *IoSecureSocket_descriptorId(IoSecureSocket *self, IoObject *locals, IoMessage *msg)
{
	return IONUMBER(SSL_get_wfd(OSSL(self)));
}
IoObject *IoSecureSocket_wantsRead(IoSecureSocket *self, IoObject *locals, IoMessage *msg)
{
	return IOBOOL(self, SSL_want_read(OSSL(self)));
}
IoObject *IoSecureSocket_wantsWrite(IoSecureSocket *self, IoObject *locals, IoMessage *msg)
{
	return IOBOOL(self, SSL_want_write(OSSL(self)));
}

ssize_t streamRead(SSL *ssl, UArray *buffer, size_t readSize)
{ 
	ssize_t bytesRead;
	size_t originalSize = UArray_sizeInBytes(buffer);
		
	UArray_sizeTo_(buffer, originalSize + readSize + 1);
	
	if (SSL_get_wfd(ssl) == -1) 
	{ 
		//debugPrintf("Shutting down, fd invalid\n");
		shutdownSSL(ssl);
		UArray_setSize_(buffer, originalSize); 
		//should this throw an exception?
		return 0; 
	}
	bytesRead = (size_t)SSL_read(ssl, (uint8_t *)UArray_bytes(buffer) + originalSize, readSize);
	
	if (bytesRead > 0)
	{
		UArray_setSize_(buffer, originalSize + bytesRead);
	}
	else
	{
		UArray_setSize_(buffer, originalSize);
	}
	
	return bytesRead;
}

IoObject *IoSecureSocket_asyncStreamRead(IoSecureSocket *self, IoObject *locals, IoMessage *msg)
{
	IoSeq *bufferSeq = IoMessage_locals_mutableSeqArgAt_(msg, locals, 0);
	UArray *buffer = IoSeq_rawUArray(bufferSeq);
	size_t readSize = IoMessage_locals_intArgAt_(msg, locals, 1);
	
	ssize_t bytesRead = streamRead(OSSL(self), buffer, readSize);

	//debugPrintf("bytesRead = %i; %i, %i, %i\n", bytesRead, SSL_want_read(OSSL(self)), SSL_want_write(OSSL(self)), errno);
	if (bytesRead == -1 && (SSL_want_read(OSSL(self)) || SSL_want_write(OSSL(self)))) 
	{
		return IOFALSE(self);
	}
	
	if (bytesRead == 0)
	{
		//debugPrintf("Shutting down, no bytes read\n");
		ERR_print_errors_fp(stderr);
		shutdownSSL(OSSL(self));
		//should this throw an exception?
		return IOFALSE(self);
	}
	return IOBOOL(self, bytesRead > 0);
}

ssize_t streamWrite(SSL *ssl, UArray *buffer, size_t start, size_t writeSize)
{
	size_t bufferSize = UArray_sizeInBytes(buffer);
	ssize_t bytesSent;
	
	if (start > bufferSize) 
	{
		return 0;
	}
	
	if (start + writeSize > bufferSize) 
	{
		writeSize = bufferSize - start;
	}

	if (SSL_get_wfd(ssl) == -1) 
	{ 
		shutdownSSL(ssl);
		//should this throw an exception?
		return 0;
	}
	bytesSent = SSL_write(ssl, UArray_bytes(buffer) + start, writeSize);
	if (bytesSent == 0)
	{
		//debugPrintf("Shutting down, no bytes sent\n");
		shutdownSSL(ssl);
		//should this throw an exception?
	}
	
	return bytesSent;
}

IoObject *IoSecureSocket_asyncWrite(IoSecureSocket *self, IoObject *locals, IoMessage *m)
{
	IoSeq *bufferSeq = IoMessage_locals_seqArgAt_(m, locals, 0);
	UArray *buffer = IoSeq_rawUArray(bufferSeq);
	//size_t bufSize = UArray_size(buffer);
	size_t start = IoMessage_locals_intArgAt_(m, locals, 1);
	size_t writeSize = IoMessage_locals_intArgAt_(m, locals, 2);
	ssize_t bytesWritten;
	bytesWritten = streamWrite(OSSL(self), buffer, start, writeSize);
	//debugPrintf("wrote %i/%i, %i %i, %i : %i\n", bytesWritten, writeSize, SSL_want_read(OSSL(self)), SSL_want_write(OSSL(self)), SSL_get_error(OSSL(self), bytesWritten), errno);
	return IOBOOL(self, bytesWritten > 0);
	//return IOBOOL(self, bytesWritten == writeSize);
	//return IONUMBER(bytesWritten);
}


IoObject *IoSecureSocket_shutdown(IoSecureSocket *self, IoObject *locals, IoMessage *msg)
{
	shutdownSSL(OSSL(self));
	return self;
}

IoObject *IoSecureSocket_setSocketReadBufferSize(IoSecureSocket *self, IoObject *locals, IoMessage *m)
{
	int size = IoMessage_locals_intArgAt_(m, locals, 0);
	int r = setsockopt(SSL_get_wfd(OSSL(self)), SOL_SOCKET, SO_RCVBUF, &size, sizeof(int));
	return IONUMBER(r);
}

IoObject *IoSecureSocket_setSocketWriteBufferSize(IoSecureSocket *self, IoObject *locals, IoMessage *m)
{
	int size = IoMessage_locals_intArgAt_(m, locals, 0);
	int r = setsockopt(SSL_get_wfd(OSSL(self)), SOL_SOCKET, SO_SNDBUF, &size, sizeof(int));
	return IONUMBER(r);
}

IoObject *IoSecureSocket_setSocketReadLowWaterMark(IoSecureSocket *self, IoObject *locals, IoMessage *m)
{
	int size = IoMessage_locals_intArgAt_(m, locals, 0);
	int r = setsockopt(SSL_get_wfd(OSSL(self)), SOL_SOCKET, SO_RCVLOWAT, &size, sizeof(int));
	return IONUMBER(r);
}

IoObject *IoSecureSocket_setSocketWriteLowWaterMark(IoSecureSocket *self, IoObject *locals, IoMessage *m)
{
	int size = IoMessage_locals_intArgAt_(m, locals, 0);
	int r = setsockopt(SSL_get_wfd(OSSL(self)), SOL_SOCKET, SO_SNDLOWAT, &size, sizeof(int));
	return IONUMBER(r);
}

IoObject *IoSecureSocket_getSocketReadLowWaterMark(IoSecureSocket *self, IoObject *locals, IoMessage *m)
{
	int size = 0;
	socklen_t length = sizeof(int);
	//int r = 
	getsockopt(SSL_get_wfd(OSSL(self)), SOL_SOCKET, SO_RCVLOWAT, &size, &length);
	
	return IONUMBER(size);
}

IoObject *IoSecureSocket_getSocketWriteLowWaterMark(IoSecureSocket *self, IoObject *locals, IoMessage *m)
{
	int size = 0;
	socklen_t length = sizeof(int);
	//int r = 
	getsockopt(SSL_get_wfd(OSSL(self)), SOL_SOCKET, SO_SNDLOWAT, &size, &length);
	return IONUMBER(size);
}

IoObject *IoSecureSocket_isOpen(IoSecureSocket *self, IoObject *locals, IoMessage *m)
{
	return IOBOOL(self, SSL_isOpen(OSSL(self)));
}

IoObject *IoSecureSocket_rawCertificate(IoSecureSocket *self, IoObject *locals, IoMessage *m)
{
	X509 *x = SSL_get_certificate(OSSL(self));
	if(!x)
	{
		return IONIL(self);
	}
	return IoCertificate_newWithX509_shouldFree(IoObject_state(self), x, 0);
}
IoObject *IoSecureSocket_rawPeerCertificate(IoSecureSocket *self, IoObject *locals, IoMessage *m)
{
	X509 *x = SSL_get_peer_certificate(OSSL(self));
	if(!x)
	{
		return IONIL(self);
	}
	return IoCertificate_newWithX509_shouldFree(IoObject_state(self), x, 1);
}

IoSecureSocket *IoSecureSocket_isDatagram(IoSecureSocket *self, IoObject *locals, IoMessage *msg)
{
	#ifdef DTLS_IMPLEMENTED
	return IOBOOL(self, SSL_usesDatagramSockets(OSSL(self)));
	#else
	return IOFALSE(self);
	#endif
}

		#ifdef DTLS_IMPLEMENTED

	ssize_t udpRead(SSL *ssl, UArray *buffer, size_t readSize)
	{
		ssize_t bytesRead;
		size_t originalSize = UArray_sizeInBytes(buffer);

		char *tempBuf = (char *)calloc(readSize+1, sizeof(char));
		//debugPrintf("udp reading %i from %i\n", readSize, BIO_get_fd(ssl->wbio, NULL));
		int len = recvfrom(SSL_get_wfd(ssl), 
							tempBuf,
							readSize,
							0,
							NULL,
							0);
		//debugPrintf("read raw %d\n", len);
		if(len == 0 || len == -1)
		{
			free(tempBuf);
			return -1;
		}
		if(len + 1 >= readSize)
		{
			free(tempBuf);
			return -1;
		}
		BIO *tmp = ssl->rbio;
		BIO *rbio = BIO_new_mem_buf(tempBuf, len);
		BIO_set_mem_eof_return(rbio, -1);
		ssl->rbio = rbio;
		UArray_sizeTo_(buffer, originalSize + len + 1);
		len = SSL_read(ssl, (uint8_t *)UArray_bytes(buffer) + originalSize, readSize);
		//debugPrintf("read clean %i\n", len);
		UArray_sizeTo_(buffer, originalSize + len + 1);
		BIO_free(ssl->rbio);
		ssl->rbio = tmp;
		free(tempBuf);
		return len;
	}


	IoObject *IoSecureSocket_asyncUdpRead(IoSecureSocket *self, IoObject *locals, IoMessage *msg)
	{
		IoSeq *bufferSeq = IoMessage_locals_mutableSeqArgAt_(msg, locals, 0);
		UArray *buffer = IoSeq_rawUArray(bufferSeq);
		size_t readSize = IoMessage_locals_intArgAt_(msg, locals, 1);

		ssize_t bytesRead = udpRead(OSSL(self), buffer, readSize);

		//debugPrintf("bytesRead = %i; %i, %i, %i\n", bytesRead, SSL_want_read(OSSL(self)), SSL_want_write(OSSL(self)), errno);
		if (bytesRead == -1 && (SSL_want_read(OSSL(self)) || SSL_want_write(OSSL(self)))) 
		{
			return IOFALSE(self);
		}
		if(bytesRead == -1)
		{
//			IoState_error_(IOSTATE, msg, "Read error; errno=%i", errno);
			return IOFALSE(self);
		}
		if (bytesRead == 0)
		{
			//debugPrintf("Shutting down, no bytes read\n");
			ERR_print_errors_fp(stderr);
			shutdownSSL(OSSL(self));
			return IOFALSE(self);
		}
		return IOBOOL(self, bytesRead > 0);
	}

	IoObject *IoSecureSocket_asyncUdpServerRead(IoSecureSocket *self, IoObject *locals, IoMessage *msg)
	{
		IoSeq *bufferSeq = IoMessage_locals_mutableSeqArgAt_(msg, locals, 0);
		UArray *buffer = IoSeq_rawUArray(bufferSeq);
		size_t readSize = IoMessage_locals_intArgAt_(msg, locals, 1);

		ssize_t bytesRead = streamRead(OSSL(self), buffer, readSize);
		//debugPrintf("bytesRead = %i; %i, %i, %i\n", bytesRead, SSL_want_read(OSSL(self)), SSL_want_write(OSSL(self)), errno);
		if (bytesRead == -1 && (SSL_want_read(OSSL(self)) || SSL_want_write(OSSL(self)))) 
		{
			return IOFALSE(self);
		}

		if (bytesRead == 0)
		{
			//debugPrintf("server shutting down, no bytes read\n");
			ERR_print_errors_fp(stderr);
			shutdownSSL(OSSL(self));
			return IOFALSE(self);
		}
		return IOBOOL(self, bytesRead > 0);
	}

	IoObject *IoSecureSocket_rbioReady(IoSecureSocket *self, IoObject *locals, IoMessage *m)
	{
		return IOBOOL(self, !BIO_eof(OSSL(self)->rbio));
	}

		#endif
