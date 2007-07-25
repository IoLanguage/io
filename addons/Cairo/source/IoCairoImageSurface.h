/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
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
void IoCairoImageSurface_mark(IoCairoImageSurface *self);

cairo_surface_t *IoCairoImageSurface_getRawSurface(IoCairoImageSurface *self);

/* ----------------------------------------------------------- */

IoObject *IoCairoImageSurface_create(IoCairoImageSurface *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoImageSurface_writeToPNG(IoCairoImageSurface *self, IoObject *locals, IoMessage *m);

#endif
