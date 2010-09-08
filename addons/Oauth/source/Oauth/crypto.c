/*
 *	crypto.c
 *
 *	Cryptographic facilities
 */
#include <stdio.h>
#ifdef __FREEBSD
#include <machine/endian.h>
#elif defined (__LINUX)
#include <endian.h>
#elif defined (__SOLARIS)
#include <sys/isa_defs.h>
#endif
#include <sys/types.h>
#include <string.h>
#include "crypto.h"


#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 
#endif

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 
#else
#define HIGHFIRST  1
#endif

static char hexC[] = "0123456789abcdef";

#ifndef HIGHFIRST
#define ByteReverse(buf, len)
#else
static void ByteReverse(uint8_t *buf, unsigned int longs)
{
	u_int32_t t;

	do
	{
		t = (u_int32_t) ((unsigned) buf[3] << 8 | buf[2]) << 16 |
			((unsigned) buf[1] << 8 | buf[0]);
		*(u_int32_t *)buf = t;
		buf+=4;
	} while (--longs);
}
#endif





/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

/* This is the central step in the MD5 algorithm. */
#ifdef __PUREC__
#define MD5STEP(f, w, x, y, z, data, s) \
	( w += f /*(x, y, z)*/ + data,  w = w<<s | w>>(32-s),  w += x )
#else
#define MD5STEP(f, w, x, y, z, data, s) \
	( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )
#endif







/*
 * The core of the MD5 algorithm, this alters an existing MD5 hash to
 * reflect the addition of 16 longwords of new data.  MD5Update blocks
 * the data and converts uint8_ts into longwords for this routine.
 */

static void MD5Transform(u_int32_t buf[4], u_int32_t const in[16])
{
	register u_int32_t a, b, c, d;

	a = buf[0];
	b = buf[1];
	c = buf[2];
	d = buf[3];

#ifdef __PUREC__        /* PureC Weirdness... (GG) */
	MD5STEP(F1(b,c,d), a, b, c, d, in[0] + 0xd76aa478L, 7);
	MD5STEP(F1(a,b,c), d, a, b, c, in[1] + 0xe8c7b756L, 12);
	MD5STEP(F1(d,a,b), c, d, a, b, in[2] + 0x242070dbL, 17);
	MD5STEP(F1(c,d,a), b, c, d, a, in[3] + 0xc1bdceeeL, 22);
	MD5STEP(F1(b,c,d), a, b, c, d, in[4] + 0xf57c0fafL, 7);
	MD5STEP(F1(a,b,c), d, a, b, c, in[5] + 0x4787c62aL, 12);
	MD5STEP(F1(d,a,b), c, d, a, b, in[6] + 0xa8304613L, 17);
	MD5STEP(F1(c,d,a), b, c, d, a, in[7] + 0xfd469501L, 22);
	MD5STEP(F1(b,c,d), a, b, c, d, in[8] + 0x698098d8L, 7);
	MD5STEP(F1(a,b,c), d, a, b, c, in[9] + 0x8b44f7afL, 12);
	MD5STEP(F1(d,a,b), c, d, a, b, in[10] + 0xffff5bb1L, 17);
	MD5STEP(F1(c,d,a), b, c, d, a, in[11] + 0x895cd7beL, 22);
	MD5STEP(F1(b,c,d), a, b, c, d, in[12] + 0x6b901122L, 7);
	MD5STEP(F1(a,b,c), d, a, b, c, in[13] + 0xfd987193L, 12);
	MD5STEP(F1(d,a,b), c, d, a, b, in[14] + 0xa679438eL, 17);
	MD5STEP(F1(c,d,a), b, c, d, a, in[15] + 0x49b40821L, 22);

	MD5STEP(F2(b,c,d), a, b, c, d, in[1] + 0xf61e2562L, 5);
	MD5STEP(F2(a,b,c), d, a, b, c, in[6] + 0xc040b340L, 9);
	MD5STEP(F2(d,a,b), c, d, a, b, in[11] + 0x265e5a51L, 14);
	MD5STEP(F2(c,d,a), b, c, d, a, in[0] + 0xe9b6c7aaL, 20);
	MD5STEP(F2(b,c,d), a, b, c, d, in[5] + 0xd62f105dL, 5);
	MD5STEP(F2(a,b,c), d, a, b, c, in[10] + 0x02441453L, 9);
	MD5STEP(F2(d,a,b), c, d, a, b, in[15] + 0xd8a1e681L, 14);
	MD5STEP(F2(c,d,a), b, c, d, a, in[4] + 0xe7d3fbc8L, 20);
	MD5STEP(F2(b,c,d), a, b, c, d, in[9] + 0x21e1cde6L, 5);
	MD5STEP(F2(a,b,c), d, a, b, c, in[14] + 0xc33707d6L, 9);
	MD5STEP(F2(d,a,b), c, d, a, b, in[3] + 0xf4d50d87L, 14);
	MD5STEP(F2(c,d,a), b, c, d, a, in[8] + 0x455a14edL, 20);
	MD5STEP(F2(b,c,d), a, b, c, d, in[13] + 0xa9e3e905L, 5);
	MD5STEP(F2(a,b,c), d, a, b, c, in[2] + 0xfcefa3f8L, 9);
	MD5STEP(F2(d,a,b), c, d, a, b, in[7] + 0x676f02d9L, 14);
	MD5STEP(F2(c,d,a), b, c, d, a, in[12] + 0x8d2a4c8aL, 20);

	MD5STEP(F3(b,c,d), a, b, c, d, in[5] + 0xfffa3942L, 4);
	MD5STEP(F3(a,b,c), d, a, b, c, in[8] + 0x8771f681L, 11);
	MD5STEP(F3(d,a,b), c, d, a, b, in[11] + 0x6d9d6122L, 16);
	MD5STEP(F3(c,d,a), b, c, d, a, in[14] + 0xfde5380cL, 23);
	MD5STEP(F3(b,c,d), a, b, c, d, in[1] + 0xa4beea44L, 4);
	MD5STEP(F3(a,b,c), d, a, b, c, in[4] + 0x4bdecfa9L, 11);
	MD5STEP(F3(d,a,b), c, d, a, b, in[7] + 0xf6bb4b60L, 16);
	MD5STEP(F3(c,d,a), b, c, d, a, in[10] + 0xbebfbc70L, 23);
	MD5STEP(F3(b,c,d), a, b, c, d, in[13] + 0x289b7ec6L, 4);
	MD5STEP(F3(a,b,c), d, a, b, c, in[0] + 0xeaa127faL, 11);
	MD5STEP(F3(d,a,b), c, d, a, b, in[3] + 0xd4ef3085L, 16);
	MD5STEP(F3(c,d,a), b, c, d, a, in[6] + 0x04881d05L, 23);
	MD5STEP(F3(b,c,d), a, b, c, d, in[9] + 0xd9d4d039L, 4);
	MD5STEP(F3(a,b,c), d, a, b, c, in[12] + 0xe6db99e5L, 11);
	MD5STEP(F3(d,a,b), c, d, a, b, in[15] + 0x1fa27cf8L, 16);
	MD5STEP(F3(c,d,a), b, c, d, a, in[2] + 0xc4ac5665L, 23);

	MD5STEP(F4(b,c,d), a, b, c, d, in[0] + 0xf4292244L, 6);
	MD5STEP(F4(a,b,c), d, a, b, c, in[7] + 0x432aff97L, 10);
	MD5STEP(F4(d,a,b), c, d, a, b, in[14] + 0xab9423a7L, 15);
	MD5STEP(F4(c,d,a), b, c, d, a, in[5] + 0xfc93a039L, 21);
	MD5STEP(F4(b,c,d), a, b, c, d, in[12] + 0x655b59c3L, 6);
	MD5STEP(F4(a,b,c), d, a, b, c, in[3] + 0x8f0ccc92L, 10);
	MD5STEP(F4(d,a,b), c, d, a, b, in[10] + 0xffeff47dL, 15);
	MD5STEP(F4(c,d,a), b, c, d, a, in[1] + 0x85845dd1L, 21);
	MD5STEP(F4(b,c,d), a, b, c, d, in[8] + 0x6fa87e4fL, 6);
	MD5STEP(F4(a,b,c), d, a, b, c, in[15] + 0xfe2ce6e0L, 10);
	MD5STEP(F4(d,a,b), c, d, a, b, in[6] + 0xa3014314L, 15);
	MD5STEP(F4(c,d,a), b, c, d, a, in[13] + 0x4e0811a1L, 21);
	MD5STEP(F4(b,c,d), a, b, c, d, in[4] + 0xf7537e82L, 6);
	MD5STEP(F4(a,b,c), d, a, b, c, in[11] + 0xbd3af235L, 10);
	MD5STEP(F4(d,a,b), c, d, a, b, in[2] + 0x2ad7d2bbL, 15);
	MD5STEP(F4(c,d,a), b, c, d, a, in[9] + 0xeb86d391L, 21);
#else
	MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
	MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
	MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
	MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
	MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
	MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
	MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
	MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
	MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
	MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
	MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
	MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
	MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
	MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
	MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
	MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

	MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
	MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
	MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
	MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
	MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
	MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
	MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
	MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
	MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
	MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
	MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
	MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
	MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
	MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
	MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
	MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

	MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
	MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
	MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
	MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
	MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
	MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
	MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
	MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
	MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
	MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
	MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
	MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
	MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
	MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
	MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
	MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

	MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
	MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
	MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
	MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
	MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
	MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
	MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
	MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
	MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
	MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
	MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
	MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
	MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
	MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
	MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
	MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);
#endif

	buf[0] += a;
	buf[1] += b;
	buf[2] += c;
	buf[3] += d;
}














void md5_init(struct md5_context *ctx)
{
	ctx->buf[0] = 0x67452301;
	ctx->buf[1] = 0xefcdab89;
	ctx->buf[2] = 0x98badcfe;
	ctx->buf[3] = 0x10325476;


	ctx->bits[0] = 0;
	ctx->bits[1] = 0;
}

void md5_finalize(struct md5_context *ctx, uint8_t *digest)
{
	unsigned int count;
	uint8_t *p;

	count = (ctx->bits[0] >> 3) & 0x3F;

	p = ctx->in + count;
	*p++ = 0x80;

	count = 64 - 1 - count;

	if (count < 8)
	{
		memset(p, 0, count);
		ByteReverse(ctx->in, 16);
		MD5Transform(ctx->buf, (u_int32_t *)ctx->in);

		memset(ctx->in, 0, 56);
	}
	else
	{
		memset(p, 0, count - 8);
	}

	ByteReverse(ctx->in, 14);

	((u_int32_t *)ctx->in)[14] = ctx->bits[0];
	((u_int32_t *)ctx->in)[15] = ctx->bits[1];

	MD5Transform(ctx->buf, (u_int32_t *)ctx->in);
	ByteReverse((uint8_t *)ctx->buf, 4);
	memcpy(digest, ctx->buf, 16);
	memset(&ctx, 0, sizeof(ctx));

}

void md5_update(struct md5_context *ctx, uint8_t *buf, unsigned int len)
{
	u_int32_t t;

	/* Update bitcount */

	t = ctx->bits[0];
	if ((ctx->bits[0] = t + ((u_int32_t) len << 3)) < t)
		ctx->bits[1]++;         /* Carry from low to high */
	ctx->bits[1] += len >> 29;

	t = (t >> 3) & 0x3f;        	/* Bytes already in shsInfo->data */


	/* Handle any leading odd-sized chunks */

	if (t)
	{
		uint8_t *p = (uint8_t *) ctx->in + t;

		t = 64 - t;
		if (len < t)
		{
			memcpy(p, buf, len);
			return;
		}	

		memcpy(p, buf, t);
		ByteReverse(ctx->in, 16);
		MD5Transform(ctx->buf, (u_int32_t *)ctx->in);
		buf+=t;
		len-=t;
	}

	/* Process data in 64-uint8_t chunks */

	while (len >= 64)
	{
		memcpy(ctx->in, buf, 64);
		ByteReverse(ctx->in, 16);
		MD5Transform(ctx->buf, (u_int32_t *)ctx->in);
		buf+=64;
		len-=64;
	}

	/* Handle any remaining uint8_ts of data */

	memcpy(ctx->in, buf, len);
}

char *md5_string(char *digest, char *result)
{
	int i;

	for (i = 0 ; i < 16 ; ++i)
	{
		result[2 * i] 	  = hexC[(digest[i] >> 4) & 0xf];
		result[2 * i + 1] = hexC[digest[i] & 0xf];
	}

	result[2 * i] = 0;
	return result;
}


void hmac(uint8_t *key, size_t keyLen, uint8_t *text, size_t textLen, uint8_t *md)
{
#define DIGESTSIZE 20
	struct sha1_context ctx;
	uint8_t mdKey[128], kIpad[64], kOpad[64], dummy[64];
	uint32_t i;

	if (keyLen > 64)
	{
		sha1_init(&ctx);
		sha1_update(&ctx, key, keyLen);
		sha1_finalize(&ctx, mdKey);

		keyLen 	= DIGESTSIZE;
		key 	= mdKey;
	}
	else
	{
		memcpy(mdKey, key, keyLen);
		key = mdKey;
	}


	if (keyLen < 64)
		memset(key + keyLen, 0, 64 - keyLen);

	for (i = 0; i < 64; ++i)
	{
		kIpad[i] = key[i] ^ 0x36;
		kOpad[i] = key[i] ^ 0x5c;
	}


	sha1_init(&ctx);
	sha1_update(&ctx, kIpad, 64);
	sha1_update(&ctx, text, textLen);
	sha1_finalize(&ctx, dummy);

	sha1_init(&ctx);
	sha1_update(&ctx, kOpad, 64);
	sha1_update(&ctx, dummy, DIGESTSIZE);
	sha1_finalize(&ctx, md);
}












static void SHA1Decode(uint32_t *output, const unsigned char *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((uint32_t) input[j + 3]) | (((uint32_t) input[j + 2]) << 8) |
			(((uint32_t) input[j + 1]) << 16) | (((uint32_t) input[j]) << 24);
}

/* F, G, H and I are basic SHA1 functions.
 */
#define F(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z) ((x) ^ (y) ^ (z))
#define H(x, y, z) (((x) & (y)) | ((z) & ((x) | (y))))
#define I(x, y, z) ((x) ^ (y) ^ (z))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* W[i]
 */
#define W(i) ( tmp=x[(i-3)&15]^x[(i-8)&15]^x[(i-14)&15]^x[i&15], \
		(x[i&15]=ROTATE_LEFT(tmp, 1)) )  

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
 */
#define FF(a, b, c, d, e, w) { \
	(e) += F ((b), (c), (d)) + (w) + (uint32_t)(0x5A827999); \
	(e) += ROTATE_LEFT ((a), 5); \
	(b) = ROTATE_LEFT((b), 30); \
}
#define GG(a, b, c, d, e, w) { \
	(e) += G ((b), (c), (d)) + (w) + (uint32_t)(0x6ED9EBA1); \
	(e) += ROTATE_LEFT ((a), 5); \
	(b) = ROTATE_LEFT((b), 30); \
}
#define HH(a, b, c, d, e, w) { \
	(e) += H ((b), (c), (d)) + (w) + (uint32_t)(0x8F1BBCDC); \
	(e) += ROTATE_LEFT ((a), 5); \
	(b) = ROTATE_LEFT((b), 30); \
}
#define II(a, b, c, d, e, w) { \
	(e) += I ((b), (c), (d)) + (w) + (uint32_t)(0xCA62C1D6); \
	(e) += ROTATE_LEFT ((a), 5); \
	(b) = ROTATE_LEFT((b), 30); \
}

static void SHA1Transform(state, block)
	uint32_t state[5];
	const unsigned char block[64];
{
	uint32_t a = state[0], b = state[1], c = state[2];
	uint32_t d = state[3], e = state[4], x[16], tmp;

	SHA1Decode(x, block, 64);

	/* Round 1 */
	FF(a, b, c, d, e, x[0]);   /* 1 */
	FF(e, a, b, c, d, x[1]);   /* 2 */
	FF(d, e, a, b, c, x[2]);   /* 3 */
	FF(c, d, e, a, b, x[3]);   /* 4 */
	FF(b, c, d, e, a, x[4]);   /* 5 */
	FF(a, b, c, d, e, x[5]);   /* 6 */
	FF(e, a, b, c, d, x[6]);   /* 7 */
	FF(d, e, a, b, c, x[7]);   /* 8 */
	FF(c, d, e, a, b, x[8]);   /* 9 */
	FF(b, c, d, e, a, x[9]);   /* 10 */
	FF(a, b, c, d, e, x[10]);  /* 11 */
	FF(e, a, b, c, d, x[11]);  /* 12 */
	FF(d, e, a, b, c, x[12]);  /* 13 */
	FF(c, d, e, a, b, x[13]);  /* 14 */
	FF(b, c, d, e, a, x[14]);  /* 15 */
	FF(a, b, c, d, e, x[15]);  /* 16 */
	FF(e, a, b, c, d, W(16));  /* 17 */
	FF(d, e, a, b, c, W(17));  /* 18 */
	FF(c, d, e, a, b, W(18));  /* 19 */
	FF(b, c, d, e, a, W(19));  /* 20 */

	/* Round 2 */
	GG(a, b, c, d, e, W(20));  /* 21 */
	GG(e, a, b, c, d, W(21));  /* 22 */
	GG(d, e, a, b, c, W(22));  /* 23 */
	GG(c, d, e, a, b, W(23));  /* 24 */
	GG(b, c, d, e, a, W(24));  /* 25 */
	GG(a, b, c, d, e, W(25));  /* 26 */
	GG(e, a, b, c, d, W(26));  /* 27 */
	GG(d, e, a, b, c, W(27));  /* 28 */
	GG(c, d, e, a, b, W(28));  /* 29 */
	GG(b, c, d, e, a, W(29));  /* 30 */
	GG(a, b, c, d, e, W(30));  /* 31 */
	GG(e, a, b, c, d, W(31));  /* 32 */
	GG(d, e, a, b, c, W(32));  /* 33 */
	GG(c, d, e, a, b, W(33));  /* 34 */
	GG(b, c, d, e, a, W(34));  /* 35 */
	GG(a, b, c, d, e, W(35));  /* 36 */
	GG(e, a, b, c, d, W(36));  /* 37 */
	GG(d, e, a, b, c, W(37));  /* 38 */
	GG(c, d, e, a, b, W(38));  /* 39 */
	GG(b, c, d, e, a, W(39));  /* 40 */

	/* Round 3 */
	HH(a, b, c, d, e, W(40));  /* 41 */
	HH(e, a, b, c, d, W(41));  /* 42 */
	HH(d, e, a, b, c, W(42));  /* 43 */
	HH(c, d, e, a, b, W(43));  /* 44 */
	HH(b, c, d, e, a, W(44));  /* 45 */
	HH(a, b, c, d, e, W(45));  /* 46 */
	HH(e, a, b, c, d, W(46));  /* 47 */
	HH(d, e, a, b, c, W(47));  /* 48 */
	HH(c, d, e, a, b, W(48));  /* 49 */
	HH(b, c, d, e, a, W(49));  /* 50 */
	HH(a, b, c, d, e, W(50));  /* 51 */
	HH(e, a, b, c, d, W(51));  /* 52 */
	HH(d, e, a, b, c, W(52));  /* 53 */
	HH(c, d, e, a, b, W(53));  /* 54 */
	HH(b, c, d, e, a, W(54));  /* 55 */
	HH(a, b, c, d, e, W(55));  /* 56 */
	HH(e, a, b, c, d, W(56));  /* 57 */
	HH(d, e, a, b, c, W(57));  /* 58 */
	HH(c, d, e, a, b, W(58));  /* 59 */
	HH(b, c, d, e, a, W(59));  /* 60 */

	/* Round 4 */
	II(a, b, c, d, e, W(60));  /* 61 */
	II(e, a, b, c, d, W(61));  /* 62 */
	II(d, e, a, b, c, W(62));  /* 63 */
	II(c, d, e, a, b, W(63));  /* 64 */
	II(b, c, d, e, a, W(64));  /* 65 */
	II(a, b, c, d, e, W(65));  /* 66 */
	II(e, a, b, c, d, W(66));  /* 67 */
	II(d, e, a, b, c, W(67));  /* 68 */
	II(c, d, e, a, b, W(68));  /* 69 */
	II(b, c, d, e, a, W(69));  /* 70 */
	II(a, b, c, d, e, W(70));  /* 71 */
	II(e, a, b, c, d, W(71));  /* 72 */
	II(d, e, a, b, c, W(72));  /* 73 */
	II(c, d, e, a, b, W(73));  /* 74 */
	II(b, c, d, e, a, W(74));  /* 75 */
	II(a, b, c, d, e, W(75));  /* 76 */
	II(e, a, b, c, d, W(76));  /* 77 */
	II(d, e, a, b, c, W(77));  /* 78 */
	II(c, d, e, a, b, W(78));  /* 79 */
	II(b, c, d, e, a, W(79));  /* 80 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;

	/* Zeroize sensitive information. */
	memset((unsigned char*) x, 0, sizeof(x));
}

static void SHA1Encode(unsigned char *output, uint32_t *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (unsigned char) ((input[i] >> 24) & 0xff);
		output[j + 1] = (unsigned char) ((input[i] >> 16) & 0xff);
		output[j + 2] = (unsigned char) ((input[i] >> 8) & 0xff);
		output[j + 3] = (unsigned char) (input[i] & 0xff);
	}
}

static unsigned char PADDING[64] =
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


void sha1_init(struct sha1_context *self)
{
	self->count[0] = self->count[1] = 0;
	/* Load magic initialization constants.
	 */
	self->state[0] = 0x67452301;
	self->state[1] = 0xefcdab89;
	self->state[2] = 0x98badcfe;
	self->state[3] = 0x10325476;
	self->state[4] = 0xc3d2e1f0;
}

void sha1_update(struct sha1_context *self, const unsigned char *input, unsigned int inputLen)
{
	unsigned int i, index, partLen;

	/* Compute number of bytes mod 64 */
	index = (unsigned int) ((self->count[0] >> 3) & 0x3F);

	/* Update number of bits */
	if ((self->count[0] += ((uint32_t) inputLen << 3))
			< ((uint32_t) inputLen << 3))
		self->count[1]++;
	self->count[1] += ((uint32_t) inputLen >> 29);

	partLen = 64 - index;

	/* Transform as many times as possible.
	 */
	if (inputLen >= partLen) {
		memcpy
			((unsigned char*) & self->buffer[index], (unsigned char*) input, partLen);
		SHA1Transform(self->state, self->buffer);

		for (i = partLen; i + 63 < inputLen; i += 64)
			SHA1Transform(self->state, &input[i]);

		index = 0;
	} else
		i = 0;

	/* Buffer remaining input */
	memcpy
		((unsigned char*) & self->buffer[index], (unsigned char*) & input[i],
		 inputLen - i);
}

void sha1_finalize(struct sha1_context *self, unsigned char digest[20])
{
	unsigned char bits[8];
	unsigned int index, padLen;

	/* Save number of bits */
	bits[7] = self->count[0] & 0xFF;
	bits[6] = (self->count[0] >> 8) & 0xFF;
	bits[5] = (self->count[0] >> 16) & 0xFF;
	bits[4] = (self->count[0] >> 24) & 0xFF;
	bits[3] = self->count[1] & 0xFF;
	bits[2] = (self->count[1] >> 8) & 0xFF;
	bits[1] = (self->count[1] >> 16) & 0xFF;
	bits[0] = (self->count[1] >> 24) & 0xFF;

	/* Pad out to 56 mod 64.
	 */
	index = (unsigned int) ((self->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	sha1_update(self, PADDING, padLen);

	/* Append length (before padding) */
	sha1_update(self, bits, 8);

	/* Store state in digest */
	SHA1Encode(digest, (uint32_t *)self->state, 20);

	/* Zeroize sensitive information.
	 */
	memset((unsigned char*) self, 0, sizeof(*self));
}




static inline char dec2hex(const int dec)
{
	switch (dec)
	{
		case 0 ... 9:
			return '0' + dec;

		default:
			return dec - 10 + 'a';
	}
}

char *sha1_string(uint8_t *digest, char *result)
{
	char *out = result;
	uint v, n;

	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));
	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));
	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));
	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));

	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));
	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));
	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));
	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));

	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));
	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));
	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));
	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));

	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));
	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));
	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));
	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));

	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));
	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));
	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));
	n = (v = *digest++) >> 4; *out++=dec2hex(n); *out++=dec2hex(v - (n << 4));

	*out = '\0';

	return result;
}
