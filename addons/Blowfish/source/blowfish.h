/*
 * Author     :  Paul Kocher
 * E-mail     :  pck@netcom.com
 * Date       :  1997
 * Description:  C implementation of the Blowfish algorithm.
 */

#define BLOWFISH_MAX_KEY_BYTES 56          /* 448 bits */


typedef struct
{
	unsigned long   P[16 + 2];
	unsigned long   S[4][256];
} blowfish_ctx;


void blowfish_init( blowfish_ctx *ctx, unsigned char *key, int keyLen);
void blowfish_encrypt( blowfish_ctx *ctx, unsigned long *xl, unsigned long *xr);
void blowfish_decrypt( blowfish_ctx *ctx, unsigned long *xl, unsigned long *xr);
