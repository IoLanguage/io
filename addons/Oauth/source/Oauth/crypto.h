/*
 *	crypto.h
 *
 *	Cryptographic facilities
 */
#ifndef IO_CRYPTO_H
#define IO_CRYPTO_H
#include <stdint.h>

struct md5_context
{
	uint32_t buf[4];
	uint32_t bits[2];
	uint8_t in[64];
};

struct sha1_context
{
	unsigned long state[5];
	unsigned long count[2];
	unsigned char buffer[64];
};


void md5_init(struct md5_context *ctx);
void md5_update(struct md5_context *ctx, uint8_t *buf, unsigned int len);
void md5_finalize(struct md5_context *ctx, uint8_t *digest);
char *md5_string(char *digest, char *result);

void hmac(uint8_t *key, size_t keyLen, uint8_t *text, size_t textLen, uint8_t *md);

void sha1_init(struct sha1_context *context);
void sha1_update(struct sha1_context *self, const unsigned char *input, unsigned int inputLen);
void sha1_finalize(struct sha1_context *context, unsigned char digest[20]);
char *sha1_string(uint8_t *digest, char *result);
#endif
