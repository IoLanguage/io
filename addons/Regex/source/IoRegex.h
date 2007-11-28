/*
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IOREGEX_DEFINED
#define IOREGEX_DEFINED 1

#include "IoObject.h"
#include "IoMap.h"
#include "Regex.h"

#define ISREGEX(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoRegex_rawClone)

typedef IoObject IoRegex;

typedef struct
{
	IoSymbol *pattern;
	int options;
	IoMap *nameToIndexMap;
	Regex *regex;
} IoRegexData;

IoRegex *IoRegex_rawClone(IoRegex *self);
IoRegex *IoRegex_proto(void *state);
IoRegex *IoRegex_newWithPattern_(void *state, IoSymbol *pattern);
void IoRegex_free(IoRegex *self);
void IoRegex_mark(IoRegex *self);

Regex *IoRegex_rawRegex(IoRegex *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoRegex_with(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_pattern(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_captureCount(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_nameToIndexMap(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_version(IoRegex *self, IoObject *locals, IoMessage *m);

/* ------------------------------------------------------------------------------------------------*/
/* Options */

IoObject *IoRegex_caseless(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_notCaseless(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_isCaseless(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_dotAll(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_notDotAll(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_isDotAll(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_extended(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_notExtended(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_isExtended(IoRegex *self, IoObject *locals, IoMessage *m);

IoObject *IoRegex_multiline(IoRegex *self, IoObject *locals, IoMessage *m);
IoRegex *IoRegex_notMultiline(IoRegex *self, IoObject *locals, IoMessage *m);
IoObject *IoRegex_isMultiline(IoRegex *self, IoObject *locals, IoMessage *m);

#endif
