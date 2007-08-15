/*
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IOREGEX_DEFINED
#define IOREGEX_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include <pcre.h>

#define ISREGEX(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoRegex_rawClone)

typedef IoObject IoRegex;

typedef struct
{
	pcre *regex;
	pcre_extra *studyData;
	int *outputVector;
	int outputVectorSize;
	int captureCount;
	IoSymbol *pattern;
	IoSymbol *string;
	IoObject *currentMatch;
	int lastIndex;
	int previousCompileOptions;
	int compileOptions;
	int execOptions;
	int matchGroupSize;
} IoRegexData;

IoRegex *IoRegex_rawClone(IoRegex *self);
IoRegex *IoRegex_proto(void *state);
IoRegex *IoRegex_new(void *state);

void IoRegex_free(IoRegex *self);
void IoRegex_mark(IoRegex *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoRegex_setPattern(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_pattern(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_setString(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_string(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_nextMatch(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_currentMatch(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_resetSearch(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_captureCount(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_version(IoRegex *self, IoObject *locals, IoMessage *m);

/* ------------------------------------------------------------------------------------------------*/
/* Compile options */

IoObject *IoRegex_anchoredOn(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_anchoredOff(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_caselessOn(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_caselessOff(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_dotAllOn(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_dotAllOff(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_isDotAll(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_setIsDotAll(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_extendedOn(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_extendedOff(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_multilineOn(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_multilineOff(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_UTF8On(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_UTF8Off(IoRegex *self, IoObject *locals, IoMessage *m);

/* ------------------------------------------------------------------------------------------------*/
/* Match options */

IoObject *IoRegex_matchEmptyOn(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_matchEmptyOff(IoRegex *self, IoObject *locals, IoMessage *m);

#endif
