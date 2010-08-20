#ifndef IO_TLS_H
#define IO_TLS_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/opensslconf.h>
#include <pthread.h>


SSL_CTX *tls_new_sslctx(SSL_METHOD *method);
void tls_destroy_sslctx(SSL_CTX *ctx);
int tls_init(void);

#define FreeSSL(s) do { SSL_set_fd(s, -1); SSL_free(s); } while (0);
#endif
