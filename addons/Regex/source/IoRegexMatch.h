/*
docCopyright("Daniel Rosengren", 2007)
docLicense("BSD revised")
*/

#ifndef IOREGEXMATCH_DEFINED
#define IOREGEXMATCH_DEFINED 1

#include "IoRegex.h"
#include "IoList.h"

#define ISREGEXMATCH(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoRegexMatch_rawClone)

typedef IoObject IoRegexMatch;

typedef struct
{
	IoObject *regex;
	IoSymbol *subject;
	IoList *ranges;
} IoRegexMatchData;

IoRegexMatch *IoRegexMatch_rawClone(IoRegexMatch *self);
IoRegexMatch *IoRegexMatch_proto(void *state);
IoRegexMatch *IoRegexMatch_newWithRegex_subject_captureRanges_(void *state, IoRegex *regex, IoSymbol *subject, IoList *ranges);
void IoRegexMatch_free(IoRegexMatch *self);
void IoRegexMatch_mark(IoRegexMatch *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoRegexMatch_regex(IoRegexMatch *self, IoObject *locals, IoMessage *m);
IoObject *IoRegexMatch_subject(IoRegexMatch *self, IoObject *locals, IoMessage *m);
IoObject *IoRegexMatch_ranges(IoRegexMatch *self, IoObject *locals, IoMessage *m);

#endif
