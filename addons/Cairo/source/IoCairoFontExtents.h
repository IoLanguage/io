/*
docCopyright("Trevor Fancher", 2007)
*/

#ifndef IOCAIROFONTEXTENTS_DEFINED
#define IOCAIROFONTEXTENTS_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROFONTEXTENTS(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoFontExtents_rawClone)

typedef IoObject IoCairoFontExtents;

IoCairoFontExtents *IoCairoFontExtents_proto(void *state);
IoCairoFontExtents *IoCairoFontExtents_rawClone(IoCairoFontExtents *self);
IoCairoFontExtents *IoCairoFontExtents_newWithRawFontExtents(void *state, cairo_font_extents_t *extents);
void IoCairoFontExtents_free(IoCairoFontExtents *self);

cairo_font_extents_t *IoCairoFontExtents_rawFontExtents(IoCairoFontExtents *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoFontExtents_ascent(IoCairoFontExtents *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoFontExtents_descent(IoCairoFontExtents *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoFontExtents_height(IoCairoFontExtents *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoFontExtents_maxXAdvance(IoCairoFontExtents *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoFontExtents_maxYAdvance(IoCairoFontExtents *self, IoObject *locals, IoMessage *m);

#endif
