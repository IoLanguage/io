/*
docCopyright("Trevor Fancher", 2007)
docCopyright("Daniel Rosengren", 2007)
*/

#ifndef IOCAIROGRADIENT_DEFINED
#define IOCAIROGRADIENT_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROGRADIENT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoGradient_rawClone)

typedef IoObject IoCairoGradient;

IoCairoGradient *IoCairoGradient_proto(void *state);
IoCairoGradient *IoCairoGradient_rawClone(IoCairoGradient *self);
void IoCairoGradient_addMethods(IoCairoGradient *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoGradient_addColorStopRGB(IoCairoGradient *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoGradient_addColorStopRGBA(IoCairoGradient *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoGradient_getColorStopCount(IoCairoGradient *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoGradient_getColorStopRGBA(IoCairoGradient *self, IoObject *locals, IoMessage *m);

#endif
