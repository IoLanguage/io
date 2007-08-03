/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
		    */

#ifndef IOCAIROPATTERNGRADIENTLINEAR_DEFINED
#define IOCAIROPATTERNGRADIENTLINEAR_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROPATTERNGRADIENTLINEAR(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoPatternGradientLinear_rawClone)

typedef IoObject IoCairoPatternGradientLinear;

IoCairoPatternGradientLinear *IoCairoPatternGradientLinear_rawClone(IoCairoPatternGradientLinear *self);
IoCairoPatternGradientLinear *IoCairoPatternGradientLinear_proto(void *state);
IoCairoPatternGradientLinear *IoCairoPatternGradientLinear_new(void *state);

void IoCairoPatternGradientLinear_free(IoCairoPatternGradientLinear *self);
void IoCairoPatternGradientLinear_mark(IoCairoPatternGradientLinear *self);

cairo_pattern_t *IoCairoPatternGradientLinear_getRawPatternGradientLinear(IoCairoPatternGradientLinear *self);

/* ----------------------------------------------------------- */

IoObject *IoCairoPatternGradientLinear_create(IoCairoPatternGradientLinear *self, IoObject *locals, IoMessage *m);

#endif
