/*
docCopyright("Daniel Rosengren", 2007)
*/

#ifndef IOCAIROFONTFACE_DEFINED
#define IOCAIROFONTFACE_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROFONTFACE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoFontFace_rawClone)
void *IoMessage_locals_cairoFontFaceArgAt_(IoMessage *self, void *locals, int n);
	
typedef IoObject IoCairoFontFace;

IoCairoFontFace *IoCairoFontFace_proto(void *state);
IoCairoFontFace *IoCairoFontFace_rawClone(IoCairoFontFace *self);
IoCairoFontFace *IoCairoFontFace_newWithRawFontFace_(void *state, cairo_font_face_t *face);
void IoCairoFontFace_free(IoCairoFontFace *self);

cairo_font_face_t *IoCairoFontFace_rawFontFace(IoCairoFontFace *self);

#endif
