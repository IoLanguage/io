/*
docCopyright("Daniel Rosengren", 2007)
*/

#ifndef IOCAIROPDFSURFACE_DEFINED
#define IOCAIROPDFSURFACE_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#if CAIRO_HAS_PDF_SURFACE
#define ISCAIROPDFSURFACE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoPDFSurface_rawClone)

typedef IoObject IoCairoPDFSurface;

IoCairoPDFSurface *IoCairoPDFSurface_proto(void *state);
IoCairoPDFSurface *IoCairoPDFSurface_rawClone(IoCairoPDFSurface *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoPDFSurface_create(IoCairoPDFSurface *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoPDFSurface_setSize(IoCairoPDFSurface *self, IoObject *locals, IoMessage *m);

#endif	/* CAIRO_HAS_PDF_SURFACE */

#endif
