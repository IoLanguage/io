/*
docCopyright("Daniel Rosengren", 2007)
docLicense("BSD revised")
*/

#ifndef IOCAIROSCALEDFONT_DEFINED
#define IOCAIROSCALEDFONT_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROSCALEDFONT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoScaledFont_rawClone)
void *IoMessage_locals_cairoScaledFontArgAt_(IoMessage *self, void *locals, int n);

typedef IoObject IoCairoScaledFont;

IoCairoScaledFont *IoCairoScaledFont_proto(void *state);
IoCairoScaledFont *IoCairoScaledFont_rawClone(IoCairoScaledFont *proto);
IoCairoScaledFont *IoCairoScaledFont_newWithRawScaledFont_(void *state, IoMessage *m, cairo_scaled_font_t *font);
void IoCairoScaledFont_free(IoCairoScaledFont *self);
void IoCairoScaledFont_mark(IoCairoScaledFont *self);

cairo_scaled_font_t *IoCairoScaledFont_rawScaledFont(IoCairoScaledFont *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoScaledFont_create(IoCairoScaledFont *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoScaledFont_extents(IoCairoScaledFont *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoScaledFont_textExtents(IoCairoScaledFont *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoScaledFont_glyphExtents(IoCairoScaledFont *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoScaledFont_getFontFace(IoCairoScaledFont *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoScaledFont_getFontOptions(IoCairoScaledFont *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoScaledFont_getFontMatrix(IoCairoScaledFont *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoScaledFont_getCTM(IoCairoScaledFont *self, IoObject *locals, IoMessage *m);

#endif
