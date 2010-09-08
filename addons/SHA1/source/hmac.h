#include <stddef.h>

//resbuf is prealloc 20 bytes min
int hmac_sha1 (const void *key, size_t keylen, const void *in, size_t inlen, void *resbuf);

