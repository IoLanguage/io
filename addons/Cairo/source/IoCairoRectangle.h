/*
docCopyright("Daniel Rosengren", 2007)
*/

#ifndef IOCAIRORECTANGLE_DEFINED
#define IOCAIRORECTANGLE_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIRORECTANGLE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoRectangle_rawClone)

typedef IoObject IoCairoRectangle;

IoCairoRectangle *IoCairoRectangle_proto(void *state);
IoCairoRectangle *IoCairoRectangle_rawClone(IoCairoRectangle *self);
IoCairoRectangle *IoCairoRectangle_newWithRawRectangle_(void *state, cairo_rectangle_t *rect);
void IoCairoRectangle_free(IoCairoRectangle *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoRectangle_x(IoCairoRectangle *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoRectangle_y(IoCairoRectangle *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoRectangle_width(IoCairoRectangle *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoRectangle_height(IoCairoRectangle *self, IoObject *locals, IoMessage *m);

#endif
