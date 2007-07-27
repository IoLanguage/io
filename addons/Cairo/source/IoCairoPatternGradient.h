/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
		    */

#ifndef IOCAIROPATTERNGRADIENT_DEFINED
#define IOCAIROPATTERNGRADIENT_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROPATTERNGRADIENT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoPatternGradient_rawClone)

typedef IoObject IoCairoPatternGradient;

IoCairoPatternGradient *IoCairoPatternGradient_rawClone(IoCairoPatternGradient *self);
IoCairoPatternGradient *IoCairoPatternGradient_proto(void *state);
IoCairoPatternGradient *IoCairoPatternGradient_new(void *state);

void IoCairoPatternGradient_free(IoCairoPatternGradient *self);
void IoCairoPatternGradient_mark(IoCairoPatternGradient *self);

cairo_pattern_t *IoCairoPatternGradient_getRawPatternGradient(IoCairoPatternGradient *self);

/* ----------------------------------------------------------- */

IoObject *IoCairoPatternGradient_addColorStopRGB(IoCairoPatternGradient *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoPatternGradient_addColorStopRGBA(IoCairoPatternGradient *self, IoObject *locals, IoMessage *m);

#endif
