/*
docCopyright("Trevor Fancher", 2007)
docCopyright("Daniel Rosengren", 2007)
*/

#ifndef IOCAIRORADIALGRADIENT_DEFINED
#define IOCAIRORADIALGRADIENT_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIRORADIALGRADIENT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoRadialGradient_rawClone)

typedef IoObject IoCairoRadialGradient;

IoCairoRadialGradient *IoCairoRadialGradient_rawClone(IoCairoRadialGradient *self);
IoCairoRadialGradient *IoCairoRadialGradient_proto(void *state);
IoCairoRadialGradient *IoCairoRadialGradient_new(void *state);

/* ----------------------------------------------------------- */

IoObject *IoCairoRadialGradient_create(IoCairoRadialGradient *self, IoObject *locals, IoMessage *m);

#endif
