//metadoc GLE copyright: Steve Dekorte, 2002
//metadoc GLE license BSD Revised

#ifndef IOGLE_DEFINED
#define IOGLE_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#define ISGLE(v) (IoObject_tag(v)->cloneFunc == (IoTagCloneFunc *)IoGLE_rawClone)

#include <gle.h>

typedef IoObject IoGLE;

// WTF? empty structure is a syntax error
/*typedef struct
{
} IoGLEData;
*/
IoObject *IoGLE_rawClone(IoGLE *self);
IoGLE *IoGLE_proto(void *state);
IoGLE *IoGLE_new(void *state);
/*
void IoGLE_free(IoGLE *self);
void IoGLE_mark(IoGLE *self);
*/
void IoGLE_protoInit(IoGLE *self);

#endif
