/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
		    */

#ifndef IOCAIROPATTERNGRADIENTRADIAL_DEFINED
#define IOCAIROPATTERNGRADIENTRADIAL_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROPATTERNGRADIENTRADIAL(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoPatternGradientRadial_rawClone)

typedef IoObject IoCairoPatternGradientRadial;

IoCairoPatternGradientRadial *IoCairoPatternGradientRadial_rawClone(IoCairoPatternGradientRadial *self);
IoCairoPatternGradientRadial *IoCairoPatternGradientRadial_proto(void *state);
IoCairoPatternGradientRadial *IoCairoPatternGradientRadial_new(void *state);

void IoCairoPatternGradientRadial_free(IoCairoPatternGradientRadial *self);
void IoCairoPatternGradientRadial_mark(IoCairoPatternGradientRadial *self);

cairo_pattern_t *IoCairoPatternGradientRadial_getRawPatternGradientRadial(IoCairoPatternGradientRadial *self);

/* ----------------------------------------------------------- */

IoObject *IoCairoPatternGradientRadial_create(IoCairoPatternGradientRadial *self, IoObject *locals, IoMessage *m);

#endif
