/*
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IOMATCH_DEFINED
#define IOMATCH_DEFINED 1

#include "IoObject.h"
#include "IoList.h"
#include "IoMap.h"
#include <pcre.h>

#define ISMATCH(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoMatch_rawClone)

typedef IoObject IoMatch;

typedef struct
{
	IoSymbol *subject;
	IoList *captures;
	IoList *ranges;
	IoMap *nameToIndexMap;
} IoMatchData;

IoMatch *IoMatch_rawClone(IoMatch *self);
IoMatch *IoMatch_proto(void *state);
IoMatch *IoMatch_newFromRegex_(void *state, IoObject *regex);

void IoMatch_free(IoMatch *self);
void IoMatch_mark(IoMatch *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoMatch_subject(IoMatch *self, IoObject *locals, IoMessage *m);
IoObject *IoMatch_captures(IoMatch *self, IoObject *locals, IoMessage *m);
IoObject *IoMatch_ranges(IoMatch *self, IoObject *locals, IoMessage *m);
IoObject *IoMatch_nameToIndexMap(IoMatch *self, IoObject *locals, IoMessage *m);

#endif
