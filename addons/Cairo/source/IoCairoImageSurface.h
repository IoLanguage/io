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

typedef struct
{
	cairo_surface_t *surface;
} IoCairoImageSurfaceData;

IoCairoImageSurface *IoCairoImageSurface_rawClone(IoCairoImageSurface *self);
IoCairoImageSurface *IoCairoImageSurface_proto(void *state);
IoCairoImageSurface *IoCairoImageSurface_new(void *state);
void IoCairoImageSurface_free(IoCairoImageSurface *self);

cairo_surface_t *IoCairoImageSurface_rawSurface(IoCairoImageSurface *self);

/* ----------------------------------------------------------- */

IoObject *IoCairoImageSurface_create(IoCairoImageSurface *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoImageSurface_writeToPNG(IoCairoImageSurface *self, IoObject *locals, IoMessage *m);

#endif
