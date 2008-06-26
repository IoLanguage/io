#include "common.h"
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "IoObject.h"
#include "IoSeq.h"
#include <openssl/x509.h>

void initSSL()
{
	static int initialized = 0;
	if(initialized == 0) 
	{
		initialized = 1;
		SSL_library_init();
		SSL_load_error_strings();		
	}
}

void set_nonblocking(BIO *bio)
{
	if(BIO_method_type(bio) == BIO_TYPE_CONNECT)
	{
		BIO_set_nbio(bio, 1);
	}
	if(BIO_method_type(bio) == BIO_TYPE_ACCEPT)
	{
		BIO_set_nbio_accept(bio, 1);
	}
	#ifdef DTLS_IMPLEMENTED
	if(BIO_method_type(bio) == BIO_TYPE_DGRAM)
	{
		int fd = BIO_get_fd(bio, NULL);
#ifdef WIN32
	unsigned long nonzero = 1;
	
	SocketResetErrorStatus();
		ioctlsocket(fd, FIONBIO, &nonzero);
#else
		fcntl(fd, F_SETFL, FASYNC | O_NONBLOCK);
#endif
	}
	#endif
}

void set_blocking(BIO *bio)
{
	if(BIO_method_type(bio) == BIO_TYPE_CONNECT)
	{
		BIO_set_nbio(bio, 0);
	}
	if(BIO_method_type(bio) == BIO_TYPE_ACCEPT)
	{
		BIO_set_nbio_accept(bio, 0);
	}
#ifdef DTLS_IMPLEMENTED
	if(BIO_method_type(bio) == BIO_TYPE_DGRAM)
	{
  		int fd, flags;
  		if((fd = BIO_get_fd(bio, NULL)))       
  		{ 
    		flags = fcntl(fd, F_GETFL);
    		flags &= ~O_NONBLOCK;
    		fcntl(fd, F_SETFL, flags);
  		}
	}
#endif
}

int IoSecureSockets_Password_Callback(char *buf, int size, int flag, void *selfPtr)
{
	IoObject *self = (IoObject *)selfPtr;
	IoSeq *passSeq;
	if(flag == 0) //decryption
	{
		passSeq = IoState_on_doCString_withLabel_(IoObject_state(self), self, "fetchDecryptionPassword", "Decryption Password Callback");
	}
	else //encryption
	{
		passSeq = IoState_on_doCString_withLabel_(IoObject_state(self), self, "fetchEncryptionPassword", "Encryption Password Callback");
	}
	char *cpass = CSTRING(passSeq);
	strncpy(buf, cpass, size);
	buf[size-1] = '\0';
	return strlen(buf);	
}

int IoSecureSockets_Verify_Callback(int ok, X509_STORE_CTX *store)
{
    char data[256];
    if (!ok)
    {
        X509 *cert = X509_STORE_CTX_get_current_cert(store);
        //int  depth = X509_STORE_CTX_get_error_depth(store);
        //int  err = X509_STORE_CTX_get_error(store);
 
        //debugPrintf("-Error with certificate at depth: %i\n", depth);
        X509_NAME_oneline(X509_get_issuer_name(cert), data, 256);
        //debugPrintf("  issuer   = %s\n", data);
        X509_NAME_oneline(X509_get_subject_name(cert), data, 256);
        //debugPrintf("  subject  = %s\n", data);
        //debugPrintf("  err %i:%s\n", err, X509_verify_cert_error_string(err));
    }
 
    return ok;
}

int IoSecureSockets_Verify_CRL_Callback (int ok, X509_STORE_CTX * store)
{
  if (!ok)
	{
	    //debugPrintf("Error: %s\n",
		 //    X509_verify_cert_error_string (store->error));
	}
  return ok;
}

void shutdownSSL(SSL *ssl)
{
	//try to shut down, send the notification
	SSL_shutdown(ssl);
	//should I SSL_clear(ssl) or dispose of old sessions?
}

int SSL_isConnected(SSL *ssl)
{
	return SSL_isOpen(ssl) && SSL_is_init_finished(ssl);
}

int SSL_isOpen(SSL *ssl)
{
	if(SSL_get_shutdown(ssl) != 0)
	{
		return 0;
	}
	#ifdef WIN32
		return SSL_get_wfd(ssl) != INVALID_SOCKET ? 1 : 0; 
	#else
	    return SSL_get_wfd(ssl) > -1 ? 1 : 0; 
	#endif
}

#ifdef DTLS_IMPLEMENTED
int SSL_usesDatagramSockets(SSL *ssl)
{
	return SSL_version(ssl) == DTLS1_VERSION;
}
BIO *BIO_mem_dummy()
{
	static BIO *dummyBIO = NULL;
	if(!dummyBIO)
	{
		dummyBIO = BIO_new(BIO_s_mem());
		BIO_set_mem_eof_return(dummyBIO, -1);
	}
	return dummyBIO;
}
#endif

