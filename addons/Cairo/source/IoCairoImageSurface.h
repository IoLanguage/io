/*
docCopyright("Trevor Fancher", 2007)
docCopyright("Daniel Rosengren", 2007)
*/

#ifndef IOCAIROIMAGESURFACE_DEFINED
#define IOCAIROIMAGESURFACE_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROIMAGESURFACE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoImageSurface_rawClone)

typedef IoObject IoCairoImageSurface;

IoCairoImageSurface *IoCairoImageSurface_proto(void *state);
IoCairoImageSurface *IoCairoImageSurface_rawClone(IoCairoImageSurface *self);
void IoCairoImageSurface_mark(IoCairoImageSurface *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoImageSurface_create(IoCairoImageSurface *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoImageSurface_createForData(IoCairoImageSurface *self, IoObject *locals, IoMessage *m);
#if CAIRO_HAS_PNG_FUNCTIONS
IoObject *IoCairoImageSurface_createFromPNG(IoCairoImageSurface *self, IoObject *locals, IoMessage *m);
#endif

IoObject *IoCairoImageSurface_getFormat(IoCairoImageSurface *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoImageSurface_getWidth(IoCairoImageSurface *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoImageSurface_getHeight(IoCairoImageSurface *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoImageSurface_getStride(IoCairoImageSurface *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoImageSurface_getData(IoCairoImageSurface *self, IoObject *locals, IoMessage *m);

#if CAIRO_HAS_PNG_FUNCTIONS
IoObject *IoCairoImageSurface_writeToPNG(IoCairoImageSurface *self, IoObject *locals, IoMessage *m);
#endif

#endif
