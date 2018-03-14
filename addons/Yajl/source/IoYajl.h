//metadoc YajlParser copyright Steve Dekorte 2002
//metadoc YajlParser license BSD revised

#ifndef IOYajlPARSER_DEFINED
#define IOYajlPARSER_DEFINED 1

#include "IoSeq.h"
#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>

typedef IoObject IoYajl;

typedef struct
{
	IoMessage *addValueMessage;
	IoMessage *startArrayMessage;
	IoMessage *endArrayMessage;
	IoMessage *startMapMessage;
	IoMessage *endMapMessage;
	IoMessage *addMapKeyMessage;
} IoYajlData;

IoYajl *IoYajl_proto(void *state);
void IoYajl_free(IoYajl *self);
IoYajl *IoYajl_rawClone(IoYajl *self);
void IoYajl_mark(IoYajl *self);
void IoYajl_free(IoYajl *self);

IoObject *IoYajl_parse(IoYajl *self, IoObject *locals, IoMessage *m);

#endif

