/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
		    */

#ifndef IOCAIROPATTERNSOLID_DEFINED
#define IOCAIROPATTERNSOLID_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROPATTERNSOLID(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoPatternSolid_rawClone)

typedef IoObject IoCairoPatternSolid;

IoCairoPatternSolid *IoCairoPatternSolid_rawClone(IoCairoPatternSolid *self);
IoCairoPatternSolid *IoCairoPatternSolid_proto(void *state);
IoCairoPatternSolid *IoCairoPatternSolid_new(void *state);

void IoCairoPatternSolid_free(IoCairoPatternSolid *self);
void IoCairoPatternSolid_mark(IoCairoPatternSolid *self);

cairo_pattern_t *IoCairoPatternSolid_getRawPatternSolid(IoCairoPatternSolid *self);

/* ----------------------------------------------------------- */

IoObject *IoCairoPatternSolid_create(IoCairoPatternSolid *self, IoObject *locals, IoMessage *m);

#endif
