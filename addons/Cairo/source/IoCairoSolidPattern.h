/*
docCopyright("Trevor Fancher", 2007)
docCopyright("Daniel Rosengren", 2007)
*/

#ifndef IOCAIROSOLIDPATTERN_DEFINED
#define IOCAIROSOLIDPATTERN_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROSOLIDPATTERN(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoSolidPattern_rawClone)

typedef IoObject IoCairoSolidPattern;

IoCairoSolidPattern *IoCairoSolidPattern_rawClone(IoCairoSolidPattern *self);
IoCairoSolidPattern *IoCairoSolidPattern_proto(void *state);
IoCairoSolidPattern *IoCairoSolidPattern_new(void *state);

void IoCairoSolidPattern_free(IoCairoSolidPattern *self);

/* ----------------------------------------------------------- */

IoObject *IoCairoSolidPattern_create(IoCairoSolidPattern *self, IoObject *locals, IoMessage *m);

#endif
