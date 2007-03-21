/*
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IOREGULAREXPRESSION_DEFINED
#define IOREGULAREXPRESSION_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include <pcre.h>

#define ISREGULAREXPRESSION(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoRegex_rawClone)

typedef IoObject IoRegex;

typedef struct
{
    pcre *re;
    pcre_extra *pe;
    int *outputVector;
    int outputVectorSize;
    int numberOfCaptures;
    IoSymbol *pattern;
    IoSymbol *string;
	IoMessage *getMatchProtoMessage;
	IoMessage *cloneMatchMessage;
	IoMessage *addCaptureRangeMessage;
	IoObject *currentMatch;
    int lastIndex;
    int compileOptions;
    int execOptions;
    int matchGroupSize;
} IoRegexData;

IoRegex *IoRegex_rawClone(IoRegex *self);
IoRegex *IoRegex_proto(void *state);
IoRegex *IoRegex_new(void *state);

void IoRegex_free(IoRegex *self);
void IoRegex_mark(IoRegex *self);

/* ----------------------------------------------------------- */
IoObject *IoRegex_setPattern(IoRegex *self, IoObject *locals, IoMessage *m);
void IoRegex_justUpdatePattern(IoRegex *self, IoMessage *m);
IoObject *IoRegex_pattern(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_setString(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_string(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_nextMatch(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_currentMatch(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_resetSearch(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_numberOfCaptures(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_version(IoRegex *self, IoObject *locals, IoMessage *m);

/* --- compileOptions ----------------------------------- */

IoObject *IoRegex_isCaseInsensitive(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_setIsCaseInsensitive(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_isDollarEndOnly(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_setIsDollarEndOnly(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_isDotAll(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_setIsDotAll(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_isExtended(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_setIsExtended(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_isMultiline(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_setIsMultiline(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_isGreedy(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_setIsGreedy(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_isUTF8(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_setIsUTF8(IoRegex *self, IoObject *locals, IoMessage *m);

/* --- execOptions ----------------------------------- */

IoObject *IoRegex_canMatchEmpty(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_setCanMatchEmpty(IoRegex *self, IoObject *locals, IoMessage *m);

#endif
