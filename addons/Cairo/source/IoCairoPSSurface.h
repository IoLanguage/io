/*
docCopyright("Daniel Rosengren", 2007)
*/

#ifndef IOCAIROPSSURFACE_DEFINED
#define IOCAIROPSSURFACE_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#if CAIRO_HAS_PS_SURFACE
#define ISCAIROPSSURFACE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoPSSurface_rawClone)

typedef IoObject IoCairoPSSurface;

IoCairoPSSurface *IoCairoPSSurface_proto(void *state);
IoCairoPSSurface *IoCairoPSSurface_rawClone(IoCairoPSSurface *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoPSSurface_create(IoCairoPSSurface *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoPSSurface_setSize(IoCairoPSSurface *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoPSSurface_dscBeginSetup(IoCairoPSSurface *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoPSSurface_dscBeginPageSetup(IoCairoPSSurface *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoPSSurface_dscComment(IoCairoPSSurface *self, IoObject *locals, IoMessage *m);

#endif	/* CAIRO_HAS_PS_SURFACE */

#endif
