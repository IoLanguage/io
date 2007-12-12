/*
docCopyright("Daniel Rosengren", 2007)
*/

#ifndef IOCAIROEXTENTS_DEFINED
#define IOCAIROEXTENTS_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROEXTENTS(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoExtents_rawClone)

typedef IoObject IoCairoExtents;

typedef struct
{
	double x1, y1;
	double x2, y2;
} IoCairoExtentsData;

IoCairoExtents *IoCairoExtents_proto(void *state);
IoCairoExtents *IoCairoExtents_rawClone(IoCairoExtents *self);
IoCairoExtents *IoCairoExtents_newSet(void *state, double x1, double y1, double x2, double y2);
void IoCairoExtents_free(IoCairoExtents *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoExtents_x1(IoCairoExtents *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoExtents_y1(IoCairoExtents *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoExtents_x2(IoCairoExtents *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoExtents_y2(IoCairoExtents *self, IoObject *locals, IoMessage *m);

#endif
