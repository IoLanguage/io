/*   Copyright (c) 2003, Steve Dekorte
docLicense("BSD revised")
 */

#ifndef IOFNMATCH_DEFINED
#define IOFNMATCH_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#define ISFnmatch(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoFnmatch_rawClone)

typedef IoObject IoFnmatch;

typedef struct
{
	IoSymbol *pattern;
	IoSymbol *string;
	int flags;
} IoFnmatchData;

IoFnmatch *IoFnmatch_rawClone(IoFnmatch *self);
IoFnmatch *IoFnmatch_proto(void *state);
IoFnmatch *IoFnmatch_new(void *state);
void IoFnmatch_free(IoFnmatch *self);
void IoFnmatch_mark(IoFnmatch *self);

/* ----------------------------------------------------------- */
IoObject *IoFnmatch_string(IoFnmatch *self, IoObject *locals, IoMessage *m);
IoObject *IoFnmatch_setString(IoFnmatch *self, IoObject *locals, IoMessage *m);

IoObject *IoFnmatch_pattern(IoFnmatch *self, IoObject *locals, IoMessage *m);
IoObject *IoFnmatch_setPattern(IoFnmatch *self, IoObject *locals, IoMessage *m);

IoObject *IoFnmatch_hasMatch(IoFnmatch *self, IoObject *locals, IoMessage *m);
IoObject *IoFnmatch_matchFor(IoFnmatch *self, IoObject *locals, IoMessage *m);

IoObject *IoFnmatch_noEscapeOn(IoFnmatch *self, IoObject *locals, IoMessage *m);
IoObject *IoFnmatch_noEscapeOff(IoFnmatch *self, IoObject *locals, IoMessage *m);

IoObject *IoFnmatch_pathNameOn(IoFnmatch *self, IoObject *locals, IoMessage *m);
IoObject *IoFnmatch_pathNameOff(IoFnmatch *self, IoObject *locals, IoMessage *m);

IoObject *IoFnmatch_periodOn(IoFnmatch *self, IoObject *locals, IoMessage *m);
IoObject *IoFnmatch_periodOff(IoFnmatch *self, IoObject *locals, IoMessage *m);

IoObject *IoFnmatch_leadingDirOn(IoFnmatch *self, IoObject *locals, IoMessage *m);
IoObject *IoFnmatch_leadingDirOff(IoFnmatch *self, IoObject *locals, IoMessage *m);

IoObject *IoFnmatch_caseFoldOn(IoFnmatch *self, IoObject *locals, IoMessage *m);
IoObject *IoFnmatch_caseFoldOff(IoFnmatch *self, IoObject *locals, IoMessage *m);



#endif
