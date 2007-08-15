/*
docCopyright("Trevor Fancher", 2007)
*/

#ifndef IOCAIROTEXTEXTENTS_DEFINED
#define IOCAIROTEXTEXTENTS_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROTEXTEXTENTS(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoTextExtents_rawClone)

typedef IoObject IoCairoTextExtents;

typedef cairo_text_extents_t IoCairoTextExtentsData;

IoCairoTextExtents *IoCairoTextExtents_rawClone(IoCairoTextExtents *self);
IoCairoTextExtents *IoCairoTextExtents_proto(void *state);
IoCairoTextExtents *IoCairoTextExtents_newWithRawTextExtents(void *state, cairo_text_extents_t *te);

void IoCairoTextExtents_free(IoCairoTextExtents *self);
void IoCairoTextExtents_mark(IoCairoTextExtents *self);

cairo_text_extents_t *IoCairoTextExtents_getRawTextExtents(IoCairoTextExtents *self);

/* ----------------------------------------------------------- */

IoObject *IoCairoTextExtents_height(IoCairoTextExtents *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoTextExtents_width(IoCairoTextExtents *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoTextExtents_xAdvance(IoCairoTextExtents *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoTextExtents_xBearing(IoCairoTextExtents *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoTextExtents_yAdvance(IoCairoTextExtents *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoTextExtents_yBearing(IoCairoTextExtents *self, IoObject *locals, IoMessage *m);

#endif
