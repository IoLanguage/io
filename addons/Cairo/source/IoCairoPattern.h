/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
		    */

#ifndef IOCAIROPATTERN_DEFINED
#define IOCAIROPATTERN_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROPATTERN(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoPattern_rawClone)

typedef IoObject IoCairoPattern;

typedef struct
{
	cairo_pattern_t *pattern;
} IoCairoPatternData;

IoCairoPattern *IoCairoPattern_rawClone(IoCairoPattern *self);
IoCairoPattern *IoCairoPattern_proto(void *state);
IoCairoPattern *IoCairoPattern_new(void *state);

void IoCairoPattern_free(IoCairoPattern *self);
void IoCairoPattern_mark(IoCairoPattern *self);

cairo_pattern_t *IoCairoPattern_getRawPattern(IoCairoPattern *self);

/* ----------------------------------------------------------- */

#endif
