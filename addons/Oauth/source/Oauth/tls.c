/*
 *	tls.c
 *
 *	Transport Layer Security
 */
#include "tls.h"

int tls_init(void)
{
	SSL_library_init();
	SSL_load_error_strings();
	SSLeay_add_all_algorithms();
	SSLeay_add_ssl_algorithms();
	OpenSSL_add_all_algorithms();

	SSL_load_error_strings();
	ERR_load_BIO_strings();

	const char seed[] = "io Programming Language / Steve Dekorte";
	RAND_seed(seed, sizeof(seed) / sizeof(seed[0]));

#if 0
	// Thread safety work postponed

	self->locks = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * (CRYPTO_num_locks() + 1));
	for (unsigned i = 0; i < CRYPTO_num_locks() + 1; ++i)
		pthread_mutex_init(&self->locks[i]);

	CRYPTO_set_locking_callback(lockProc);
	CRYPTO_set_id_callback(idProc);

	CRYPTO_set_dynlock_create_callback(createProc);
	CRYPTO_set_dynlock_lock_callback(lockProc);
	CRYPTO_set_dynlock_destroy_callback(destroyProc);
#endif

	return 0;
}

SSL_CTX *tls_new_sslctx(SSL_METHOD *method)
{
	SSL_CTX *ctx = SSL_CTX_new(method);

	SSL_CTX_set_mode(ctx,           SSL_MODE_AUTO_RETRY);
	SSL_CTX_set_mode(ctx,           SSL_MODE_ENABLE_PARTIAL_WRITE);
	SSL_CTX_set_options(ctx,        SSL_OP_ALL);
	SSL_CTX_set_options(ctx,        SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION);
	SSL_CTX_set_verify(ctx,         SSL_VERIFY_NONE, NULL);

	return ctx;
}

void tls_destroy_sslctx(SSL_CTX *ctx)
{
	SSL_CTX_free(ctx);
}
