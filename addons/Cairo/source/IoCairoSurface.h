/*
docCopyright("Trevor Fancher", 2007)
docCopyright("Daniel Rosengren", 2007)
*/

#ifndef IOCAIROSURFACE_DEFINED
#define IOCAIROSURFACE_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROSURFACE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoSurface_rawClone)

typedef IoObject IoCairoSurface;

IoCairoSurface *IoCairoSurface_proto(void *state);
void IoCairoSurface_addMethods(IoCairoSurface *self);
IoCairoSurface *IoCairoSurface_rawClone(IoCairoSurface *self);
IoCairoSurface *IoCairoSurface_newWithRawSurface_(void *state, IoMessage *m, cairo_surface_t *surface);
void IoCairoSurface_free(IoCairoSurface *self);

cairo_surface_t *IoCairoSurface_rawSurface(IoCairoSurface *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoSurface_createSimilar(IoCairoSurface *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoSurface_finish(IoCairoSurface *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoSurface_flush(IoCairoSurface *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoSurface_getFontOptions(IoCairoSurface *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoSurface_getContent(IoCairoSurface *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoSurface_markDirty(IoCairoSurface *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoSurface_markDirtyRectangle(IoCairoSurface *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoSurface_setDeviceOffset(IoCairoSurface *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoSurface_getDeviceOffset(IoCairoSurface *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoSurface_setFallbackResolution(IoCairoSurface *self, IoObject *locals, IoMessage *m);

#endif
