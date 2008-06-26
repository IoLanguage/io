#ifndef SECURE_SOCKET_COMMON_H
#define SECURE_SOCKET_COMMON_H 1
#include <openssl/ssl.h>
#include <openssl/err.h>

//#define debugPrintf printf

void initSSL();
void set_nonblocking(BIO *bio);
void set_blocking(BIO *bio);
int IoSecureSockets_Password_Callback(char *buf, int size, int flag, void *selfPtr);
int IoSecureSockets_Verify_Callback(int ok, X509_STORE_CTX *store);
int IoSecureSockets_Verify_CRL_Callback(int ok, X509_STORE_CTX * store);
int SSL_isConnected(SSL *ssl);
int SSL_isOpen(SSL *ssl);
void shutdownSSL(SSL *ssl);
#ifdef DTLS_IMPLEMENTED
int SSL_usesDatagramSockets(SSL *ssl);
BIO *BIO_mem_dummy();
#endif
#endif
