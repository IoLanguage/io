/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
		    */

#ifndef IOCAIROSURFACEIMAGE_DEFINED
#define IOCAIROSURFACEIMAGE_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROSURFACEIMAGE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoSurfaceImage_rawClone)

typedef IoObject IoCairoSurfaceImage;

typedef struct
{
	cairo_surface_t *surface;
} IoCairoSurfaceImageData;

IoCairoSurfaceImage *IoCairoSurfaceImage_rawClone(IoCairoSurfaceImage *self);
IoCairoSurfaceImage *IoCairoSurfaceImage_proto(void *state);
IoCairoSurfaceImage *IoCairoSurfaceImage_new(void *state);

void IoCairoSurfaceImage_free(IoCairoSurfaceImage *self);
void IoCairoSurfaceImage_mark(IoCairoSurfaceImage *self);

cairo_surface_t *IoCairoSurfaceImage_getRawSurface(IoCairoSurfaceImage *self);

/* ----------------------------------------------------------- */

IoObject *IoCairoSurfaceImage_create(IoCairoSurfaceImage *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoSurfaceImage_writeToPNG(IoCairoSurfaceImage *self, IoObject *locals, IoMessage *m);

#endif
