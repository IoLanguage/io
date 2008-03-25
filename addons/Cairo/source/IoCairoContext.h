/*
docCopyright("Trevor Fancher", 2007)
docCopyright("Daniel Rosengren", 2007)
*/

#ifndef IOCAIROCONTEXT_DEFINED
#define IOCAIROCONTEXT_DEFINED 1

#include "IoObject.h"
#include "IoCairoImageSurface.h"
#include <cairo.h>

#define ISCAIROCONTEXT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoContext_rawClone)
#ifdef WIN32
#define __func__ __FUNCTION__
#endif

typedef IoObject IoCairoContext;

IoCairoContext *IoCairoContext_proto(void *state);
IoCairoContext *IoCairoContext_rawClone(IoCairoContext *self);
IoCairoContext *IoCairoContext_newWithSurface_(void *state, IoCairoImageSurface *surface);
void IoCairoContext_free(IoCairoContext *self);

cairo_t *IoCairoContext_rawContext(IoCairoContext *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoContext_create(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_save(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_restore(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_getTarget(IoCairoContext *self, IoObject *locals, IoMessage *m);

/* ------------------------------------------------------------------------------------------------*/
/* Source */

IoObject *IoCairoContext_setSource(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_setSourceRGB(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_setSourceRGBA(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_setSourceSurface(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getSource(IoCairoContext *self, IoObject *locals, IoMessage *m);

/* ------------------------------------------------------------------------------------------------*/
/* Groups */

IoObject *IoCairoContext_pushGroup(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_pushGroupWithContent(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_popGroup(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_popGroupToSource(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getGroupTarget(IoCairoContext *self, IoObject *locals, IoMessage *m);

/* ------------------------------------------------------------------------------------------------*/
/* Options/Properties */

IoObject *IoCairoContext_setAntialias(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getAntialias(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_setDash(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getDash(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getDashCount(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_setFillRule(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getFillRule(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_setLineCap(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getLineCap(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_setLineJoin(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getLineJoin(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_setMiterLimit(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getMiterLimit(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_setOperator(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getOperator(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_setTolerance(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getTolerance(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_setLineWidth(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getLineWidth(IoCairoContext *self, IoObject *locals, IoMessage *m);

/* ------------------------------------------------------------------------------------------------*/
/* Paths */

IoObject *IoCairoContext_newPath(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_newSubPath(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_closePath(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_appendPath(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_copyPath(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_copyPathFlat(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getCurrentPoint(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_moveTo(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_lineTo(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_curveTo(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_relMoveTo(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_relLineTo(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_relCurveTo(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_arc(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_arcNegative(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_rectangle(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_textPath(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_glyphPath(IoCairoContext *self, IoObject *locals, IoMessage *m);

/* ------------------------------------------------------------------------------------------------*/
/* Clipping */

IoObject *IoCairoContext_clip(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_clipPreserve(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_clipExtents(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_resetClip(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_copyClipRectangleList(IoCairoContext *self, IoObject *locals, IoMessage *m);

/* ------------------------------------------------------------------------------------------------*/
/* Drawing */

IoObject *IoCairoContext_fill(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_fillPreserve(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_fillExtents(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_inFill(IoCairoContext *self, IoObject *locals, IoMessage *m);


IoObject *IoCairoContext_stroke(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_strokePreserve(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_strokeExtents(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_inStroke(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_mask(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_maskSurface(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_paint(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_paintWithAlpha(IoCairoContext *self, IoObject *locals, IoMessage *m);

/* ------------------------------------------------------------------------------------------------*/
/* Transformations */

IoObject *IoCairoContext_translate(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_scale(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_rotate(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_transform(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_setMatrix(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getMatrix(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_identityMatrix(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_userToDevice(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_userToDeviceDistance(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_deviceToUser(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_deviceToUserDistance(IoCairoContext *self, IoObject *locals, IoMessage *m);

/* ------------------------------------------------------------------------------------------------*/
/* Text */

IoObject *IoCairoContext_selectFontFace(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_setFontSize(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_fontExtents(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_textExtents(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_glyphExtents(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_showText(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_showGlyphs(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_setFontOptions(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getFontOptions(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_setFontFace(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getFontFace(IoCairoContext *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoContext_setScaledFont(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_getScaledFont(IoCairoContext *self, IoObject *locals, IoMessage *m);

/* ------------------------------------------------------------------------------------------------*/
/* Pages */

IoObject *IoCairoContext_copyPage(IoCairoContext *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoContext_showPage(IoCairoContext *self, IoObject *locals, IoMessage *m);

#endif
