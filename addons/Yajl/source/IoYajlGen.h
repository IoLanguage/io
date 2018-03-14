//metadoc YajlGenParser copyright Steve Dekorte 2002
//metadoc YajlGenParser license BSD revised

#ifndef IOYajlGenPARSER_DEFINED
#define IOYajlGenPARSER_DEFINED 1

#include "IoSeq.h"
#include <yajl/yajl_gen.h>

typedef IoObject IoYajlGen;

typedef struct
{
	yajl_gen yajl;
} IoYajlGenData;

IoYajlGen *IoYajlGen_proto(void *state);
void IoYajlGen_free(IoYajlGen *self);
IoYajlGen *IoYajlGen_rawClone(IoYajlGen *self);
void IoYajlGen_mark(IoYajlGen *self);
void IoYajlGen_free(IoYajlGen *self);

IoObject *IoYajlGen_pushString(IoYajlGen *self, IoObject *locals, IoMessage *m);
IoObject *IoYajlGen_generate(IoYajlGen *self, IoObject *locals, IoMessage *m);

#endif

