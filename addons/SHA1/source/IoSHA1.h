/*   copyright: Steve Dekorte, 2002
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IOSHA1_DEFINED
#define IOSHA1_DEFINED 1

#include "IoObject.h"
#include "IoNumber.h"
#include "sha1.h"

#define ISSHA1(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoSHA1_rawClone)

typedef IoObject IoSHA1;

typedef struct
{
	SHA1_CTX context;
	u_int8_t digest[SHA1_DIGEST_LENGTH];
	char isDone;
} IoSHA1Data;

IoSHA1 *IoSHA1_rawClone(IoSHA1 *self);
IoSHA1 *IoSHA1_proto(void *state);
IoSHA1 *IoSHA1_new(void *state);

void IoSHA1_free(IoSHA1 *self);
void IoSHA1_mark(IoSHA1 *self);

/* ----------------------------------------------------------- */

IoObject *IoSHA1_appendSeq(IoSHA1 *self, IoObject *locals, IoMessage *m);
IoObject *IoSHA1_sha1(IoSHA1 *self, IoObject *locals, IoMessage *m);
IoObject *IoSHA1_sha1String(IoSHA1 *self, IoObject *locals, IoMessage *m);

IoObject *IoSHA1_hmac(IoSHA1 *self, IoObject *locals, IoMessage *m);

#endif
