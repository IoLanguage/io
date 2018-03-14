/*
	Written by Daniel Rosengren
	danne.rosengren@gmail.com
*/

#ifndef IOREGEXMATCHES_DEFINED
#define IOREGEXMATCHES_DEFINED 1

#include "IoRegex.h"
#include "IoList.h"
#include "UArray.h"

#define ISREGEXMATCHES(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoRegexMatches_rawClone)

typedef IoObject IoRegexMatches;

typedef struct
{
	IoRegex *regex;
	IoSymbol *string;

	int options;
	int position;
	int endPosition;
	int currentMatchIsEmpty;

	UArray *captureArray;
} IoRegexMatchesData;

IoRegexMatches *IoRegexMatches_rawClone(IoRegexMatches *self);
IoRegexMatches *IoRegexMatches_proto(void *state);
IoRegexMatches *IoRegexMatches_new(void *state);
void IoRegexMatches_free(IoRegexMatches *self);
void IoRegexMatches_mark(IoRegexMatches *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoRegexMatches_setRegex(IoRegexMatches *self, IoObject *locals, IoMessage *m);
IoObject *IoRegexMatches_regex(IoRegexMatches *self, IoObject *locals, IoMessage *m);

IoObject *IoRegexMatches_setString(IoRegexMatches *self, IoObject *locals, IoMessage *m);
IoObject *IoRegexMatches_string(IoRegexMatches *self, IoObject *locals, IoMessage *m);

IoObject *IoRegexMatches_setPosition(IoRegexMatches *self, IoObject *locals, IoMessage *m);
IoObject *IoRegexMatches_position(IoRegexMatches *self, IoObject *locals, IoMessage *m);
IoObject *IoRegexMatches_setEndPosition(IoRegexMatches *self, IoObject *locals, IoMessage *m);
IoObject *IoRegexMatches_endPosition(IoRegexMatches *self, IoObject *locals, IoMessage *m);

IoObject *IoRegexMatches_next(IoRegexMatches *self, IoObject *locals, IoMessage *m);
IoObject *IoRegexMatches_anchored(IoRegexMatches *self, IoObject *locals, IoMessage *m);

IoObject *IoRegexMatches_allowEmptyMatches(IoRegexMatches *self, IoObject *locals, IoMessage *m);
IoObject *IoRegexMatches_disallowEmptyMatches(IoRegexMatches *self, IoObject *locals, IoMessage *m);
IoObject *IoRegexMatches_allowsEmptyMatches(IoRegexMatches *self, IoObject *locals, IoMessage *m);

#endif
