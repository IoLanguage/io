/*
docCopyright("Daniel Rosengren", 2007)
docLicense("BSD revised")
*/

#ifndef IOCAIROFONTOPTIONS_DEFINED
#define IOCAIROFONTOPTIONS_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROFONTOPTIONS(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoFontOptions_rawClone)
void *IoMessage_locals_cairoFontOptionsArgAt_(IoMessage *self, void *locals, int n);

typedef IoObject IoCairoFontOptions;

IoCairoFontOptions *IoCairoFontOptions_proto(void *state);
IoCairoFontOptions *IoCairoFontOptions_rawClone(IoCairoFontOptions *proto);
IoCairoFontOptions *IoCairoFontOptions_new(void *state, IoMessage *m);
IoCairoFontOptions *IoCairoFontOptions_newWithRawFontOptions_(void *state, IoMessage *m, cairo_font_options_t *options);
void IoCairoFontOptions_free(IoCairoFontOptions *self);
int IoCairoFontOptions_compare(IoCairoFontOptions *self, IoCairoFontOptions *other);

cairo_font_options_t *IoCairoFontOptions_rawFontOptions(IoCairoFontOptions *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoFontOptions_create(IoCairoFontOptions *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoFontOptions_merge(IoCairoFontOptions *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoFontOptions_setAntialias(IoCairoFontOptions *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoFontOptions_getAntialias(IoCairoFontOptions *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoFontOptions_setSubpixelOrder(IoCairoFontOptions *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoFontOptions_getSubpixelOrder(IoCairoFontOptions *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoFontOptions_setHintStyle(IoCairoFontOptions *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoFontOptions_getHintStyle(IoCairoFontOptions *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoFontOptions_setHintMetrics(IoCairoFontOptions *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoFontOptions_getHintMetrics(IoCairoFontOptions *self, IoObject *locals, IoMessage *m);

#endif
