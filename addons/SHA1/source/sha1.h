/*	$OpenBSD: sha1.h,v 1.23 2004/06/22 01:57:30 jfb Exp $	*/

/*
 * SHA-1 in C
 * By Steve Reid <steve@edmweb.com>
 * 100% Public Domain
 */

#ifndef _SHA1_H
#define _SHA1_H

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <sys/types.h>
typedef unsigned __int8 u_int8_t;
typedef unsigned __int16 u_int16_t;
typedef unsigned __int32 u_int32_t;
typedef unsigned __int64 u_int64_t;
typedef unsigned int u_int;
#define off_t _off_t
#endif

#define	SHA1_BLOCK_LENGTH		64
#define	SHA1_DIGEST_LENGTH		20
#define	SHA1_DIGEST_STRING_LENGTH	(SHA1_DIGEST_LENGTH * 2 + 1)

#include <sys/types.h>

typedef struct {
	u_int32_t state[5];
	u_int64_t count;
	u_int8_t buffer[SHA1_BLOCK_LENGTH];
} SHA1_CTX;

//#define sha1_ctx SHA1_CTX

#if !defined(_MSC_VER) && !defined(__MINGW32__)
#include <sys/cdefs.h>
#else
#define __BEGIN_DECLS
#define __END_DECLS
#endif

__BEGIN_DECLS
void SHA1Init(SHA1_CTX *);
void SHA1Pad(SHA1_CTX *);
void SHA1Transform(u_int32_t [5], const u_int8_t [SHA1_BLOCK_LENGTH]);
void SHA1Update(SHA1_CTX *, const u_int8_t *, size_t);
void SHA1Final(u_int8_t [SHA1_DIGEST_LENGTH], SHA1_CTX *);
char *SHA1End(SHA1_CTX *, char *);
char *SHA1File(const char *, char *);
char *SHA1FileChunk(const char *, char *, off_t, off_t);
char *SHA1Data(const u_int8_t *, size_t, char *);
__END_DECLS

#define HTONDIGEST(x) do {                                              \
		x[0] = htonl(x[0]);                                             \
		x[1] = htonl(x[1]);                                             \
		x[2] = htonl(x[2]);                                             \
		x[3] = htonl(x[3]);                                             \
		x[4] = htonl(x[4]); } while (0)

#define NTOHDIGEST(x) do {                                              \
		x[0] = ntohl(x[0]);                                             \
		x[1] = ntohl(x[1]);                                             \
		x[2] = ntohl(x[2]);                                             \
		x[3] = ntohl(x[3]);                                             \
		x[4] = ntohl(x[4]); } while (0)

#endif /* _SHA1_H */
