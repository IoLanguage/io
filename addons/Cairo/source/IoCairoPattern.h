/*
docCopyright("Trevor Fancher", 2007)
docCopyright("Daniel Rosengren", 2007)
*/

#ifndef IOCAIROPATTERN_DEFINED
#define IOCAIROPATTERN_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROPATTERN(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoPattern_rawClone)

typedef IoObject IoCairoPattern;

IoCairoPattern *IoCairoPattern_rawClone(IoCairoPattern *self);
IoCairoPattern *IoCairoPattern_proto(void *state);
void IoCairoPattern_addMethods(IoCairoPattern *self);
IoCairoPattern *IoCairoPattern_newWithRawPattern_(void *state, IoMessage *m, cairo_pattern_t *pattern);
void IoCairoPattern_free(IoCairoPattern *self);

cairo_pattern_t *IoCairoPattern_rawPattern(IoCairoPattern *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoPattern_setMatrix(IoCairoPattern *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoPattern_getMatrix(IoCairoPattern *self, IoObject *locals, IoMessage *m);

#endif
