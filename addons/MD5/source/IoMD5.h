
//metadoc MD5 copyright Steve Dekorte 2002


#ifndef IOMD5_DEFINED
#define IOMD5_DEFINED 1

#include "IoObject.h"
#include "IoNumber.h"
#include "md5.h"

#define ISMD5(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoMD5_rawClone)

typedef IoObject IoMD5;

typedef struct
{
	md5_state_t mstate;
	md5_byte_t digest[16];
	char isDone;
} IoMD5Data;

IoMD5 *IoMD5_rawClone(IoMD5 *self);
IoMD5 *IoMD5_proto(void *state);
IoMD5 *IoMD5_new(void *state);

void IoMD5_free(IoMD5 *self);
void IoMD5_mark(IoMD5 *self);

/* ----------------------------------------------------------- */

IoObject *IoMD5_appendSeq(IoMD5 *self, IoObject *locals, IoMessage *m);
IoObject *IoMD5_md5(IoMD5 *self, IoObject *locals, IoMessage *m);
IoObject *IoMD5_md5String(IoMD5 *self, IoObject *locals, IoMessage *m);

#endif
