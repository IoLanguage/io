//metadoc YajilParser copyright Steve Dekorte 2002
//metadoc YajilParser license BSD revised

#ifndef IOYajilPARSER_DEFINED
#define IOYajilPARSER_DEFINED 1

#include "IoSeq.h"
#include <yajl/yajl_parse.h>
#include <yajl/yajl_gen.h>

typedef IoObject IoYajil;

typedef struct
{
	IoMessage *addValueMessage;
	IoMessage *startArrayMessage;
	IoMessage *endArrayMessage;
	IoMessage *startMapMessage;
	IoMessage *endMapMessage;
	IoMessage *addMapKeyMessage;
} IoYajilData;

IoYajil *IoYajil_proto(void *state);
void IoYajil_free(IoYajil *self);
IoYajil *IoYajil_rawClone(IoYajil *self);
void IoYajil_mark(IoYajil *self);
void IoYajil_free(IoYajil *self);

IoObject *IoYajil_parse(IoYajil *self, IoObject *locals, IoMessage *m);

#endif

