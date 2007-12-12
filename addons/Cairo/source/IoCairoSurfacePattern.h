/*
docCopyright("Daniel Rosengren", 2007)
*/

#ifndef IOCAIROSURFACEPATTERN_DEFINED
#define IOCAIROSURFACEPATTERN_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROSURFACEPATTERN(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoSurfacePattern_rawClone)

typedef IoObject IoCairoSurfacePattern;

IoCairoSurfacePattern *IoCairoSurfacePattern_proto(void *state);
IoCairoSurfacePattern *IoCairoSurfacePattern_rawClone(IoCairoSurfacePattern *self);

/* ----------------------------------------------------------- */

IoObject *IoCairoSurfacePattern_create(IoCairoSurfacePattern *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoSurfacePattern_getSurface(IoCairoSurfacePattern *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoSurfacePattern_setExtend(IoCairoSurfacePattern *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoSurfacePattern_getExtend(IoCairoSurfacePattern *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoSurfacePattern_setFilter(IoCairoSurfacePattern *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoSurfacePattern_getFilter(IoCairoSurfacePattern *self, IoObject *locals, IoMessage *m);

#endif
