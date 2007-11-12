/*
docCopyright("Daniel Rosengren", 2007)
*/

#ifndef IOCAIROSVGSURFACE_DEFINED
#define IOCAIROSVGSURFACE_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#if CAIRO_HAS_SVG_SURFACE
#define ISCAIROSVGSURFACE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoSVGSurface_rawClone)

typedef IoObject IoCairoSVGSurface;

IoCairoSVGSurface *IoCairoSVGSurface_proto(void *state);
IoCairoSVGSurface *IoCairoSVGSurface_rawClone(IoCairoSVGSurface *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoSVGSurface_create(IoCairoSVGSurface *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoSVGSurface_restrictToVersion(IoCairoSVGSurface *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoSVGSurface_getVersions(IoCairoSVGSurface *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoSVGSurface_versionToString(IoCairoSVGSurface *self, IoObject *locals, IoMessage *m);

#endif	/* CAIRO_HAS_SVG_SURFACE */

#endif
