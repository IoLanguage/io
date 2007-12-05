/*
docCopyright("Daniel Rosengren", 2007)
*/

#ifndef IOCAIROGLYPH_DEFINED
#define IOCAIROGLYPH_DEFINED 1

#include "IoObject.h"
#include "IoList.h"
#include <cairo.h>

#define ISCAIROGLYPH(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoGlyph_rawClone)

typedef IoObject IoCairoGlyph;

IoCairoGlyph *IoCairoGlyph_proto(void *state);
IoCairoGlyph *IoCairoGlyph_rawClone(IoCairoGlyph *self);
IoCairoGlyph *IoCairoGlyph_newWithRawGlyph_(void *state, cairo_glyph_t *glyph);
void IoCairoGlyph_free(IoCairoGlyph *self);

cairo_glyph_t *IoCairoGlyph_rawGlyph(IoCairoGlyph *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoGlyph_setIndex(IoCairoGlyph *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoGlyph_index(IoCairoGlyph *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoGlyph_setX(IoCairoGlyph *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoGlyph_x(IoCairoGlyph *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoGlyph_setY(IoCairoGlyph *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoGlyph_y(IoCairoGlyph *self, IoObject *locals, IoMessage *m);

/* ------------------------------------------------------------------------------------------------*/

cairo_glyph_t *rawGlyphsFromList_count_(IoList *glyphList, int *count);

#endif
