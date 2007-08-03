/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
		    */

#ifndef IOCAIROCONTEXT_DEFINED
#define IOCAIROCONTEXT_DEFINED 1

#include "IoObject.h"
#include "IoCairoSurfaceImage.h"
#include <cairo.h>

#define ISCAIROCONTEXT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoContext_rawClone)

typedef IoObject IoCairoContext;

typedef struct
{
	cairo_t *context;
} IoCairoContextData;

IoCairoContext *IoCairoContext_rawClone(IoCairoContext *self);
IoCairoContext *IoCairoContext_proto(void *state);
IoCairoContext *IoCairoContext_new(void *state);

void IoCairoContext_free(IoCairoContext *self);
void IoCairoContext_mark(IoCairoContext *self);

cairo_t *IoCairoContext_getRawContext(IoCairoContext *self);

/* ----------------------------------------------------------- */

IoObject *IoCairoContext_arc(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_closePath(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_create(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_fill(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_fontExtents(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_lineTo(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_mask(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_moveTo(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_paintWithAlpha(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_rectangle(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_relativeCurveTo(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_relativeLineTo(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_restore(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_save(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_scale(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_selectFontFace(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_setFontSize(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_setLineWidth(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_setSource(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_setSourceRGB(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_setSourceRGBA(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_showText(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_stroke(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_textExtents(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_translate(IoCairoContext *self, IoObject *locals, IoMessage *m);

#endif
