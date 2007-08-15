/*#io
CairoContext ioDoc(
	docCopyright("Trevor Fancher", 2007)
	docCopyright("Daniel Rosengren", 2007)
	docLicense("BSD revised")
	docCategory("Graphics")
*/

#include "IoCairoContext.h"
#include "IoCairoImageSurface.h"
#include "IoCairoPattern.h"
#include "IoCairoFontExtents.h"
#include "IoCairoTextExtents.h"
#include "IoList.h"
#include "IoMessage.h"
#include "IoNumber.h"
#include <stdio.h>

#define CONTEXT(self) ((cairo_t *)IoObject_dataPointer(self))

#define STATUS(cairo_type) (cairo_status_to_string(cairo_status(cairo_type)))
#define CHECK_STATUS(obj) \
	cairo_status_t status = cairo_status(CONTEXT(obj)); \
	if (status != CAIRO_STATUS_SUCCESS) { \
		IoState_error_(IOSTATE, m, "%s: cairo: %s", __func__, STATUS(CONTEXT(obj))); \
	}


IoTag *IoCairoContext_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoContext");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoContext_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoContext_free);
	return tag;
}

IoCairoContext *IoCairoContext_proto(void *state) 
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoContext_newTag(state));
	
	IoState_registerProtoWithFunc_(state, self, IoCairoContext_proto);
	
	{
		IoMethodTable methodTable[] = {
			{"arc", IoCairoContext_arc},
			{"closePath", IoCairoContext_closePath},
			{"create", IoCairoContext_create},
			{"fill", IoCairoContext_fill},
			{"fontExtents", IoCairoContext_fontExtents},
			{"lineTo", IoCairoContext_lineTo},
			{"mask", IoCairoContext_mask},
			{"moveTo", IoCairoContext_moveTo},
			{"paintWithAlpha", IoCairoContext_paintWithAlpha},
			{"rectangle", IoCairoContext_rectangle},
			{"relativeCurveTo", IoCairoContext_relativeCurveTo},
			{"relativeLineTo", IoCairoContext_relativeLineTo},
			{"restore", IoCairoContext_restore},
			{"save", IoCairoContext_save},
			{"scale", IoCairoContext_scale},
			{"selectFontFace", IoCairoContext_selectFontFace},
			{"setFontSize", IoCairoContext_setFontSize},
			{"setLineWidth", IoCairoContext_setLineWidth},
			{"setSource", IoCairoContext_setSource},
			{"setSourceRGB", IoCairoContext_setSourceRGB},
			{"setSourceRGBA", IoCairoContext_setSourceRGBA},
			{"showText", IoCairoContext_showText},
			{"stroke", IoCairoContext_stroke},
			{"textExtents", IoCairoContext_textExtents},
			{"translate", IoCairoContext_translate},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoContext *IoCairoContext_rawClone(IoCairoContext *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	if (CONTEXT(proto))
		IoObject_setDataPointer_(self, cairo_reference(CONTEXT(proto)));
	return self;
}

IoCairoContext *IoCairoContext_newWithSurface_(void *state, IoCairoImageSurface *surface)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoCairoContext_proto);
	IoObject *self = IOCLONE(proto);
	cairo_surface_t *rawSurface = IoCairoImageSurface_rawSurface(surface);
	
	IoObject_setDataPointer_(self, cairo_create(rawSurface));
	return self;
}

void IoCairoContext_free(IoCairoContext *self) 
{
	if (CONTEXT(self))
		cairo_destroy(CONTEXT(self));
}


cairo_t *IoCairoContext_rawContext(IoCairoContext *self)
{
	return CONTEXT(self);
}

/* ----------------------------------------------------------- */

IoObject *IoCairoContext_arc(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double xc = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double yc = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double radius = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double angle1 = IoMessage_locals_doubleArgAt_(m, locals, 3);
	double angle2 = IoMessage_locals_doubleArgAt_(m, locals, 4);
		
	cairo_arc(CONTEXT(self), xc, yc, radius, angle1, angle2);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_closePath(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_close_path(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_create(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoObject *surface = IoMessage_locals_valueArgAt_(m, locals, 0);
	IoObject *context = IoCairoContext_newWithSurface_(IOSTATE, surface);

	CHECK_STATUS(context);
	return context;
}

IoObject *IoCairoContext_fill(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_fill(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_fontExtents(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_font_extents_t extents;
	
	cairo_font_extents(CONTEXT(self), &extents);
	CHECK_STATUS(self);
	return IoCairoFontExtents_newWithRawFontExtents(IOSTATE, &extents);
}

IoObject *IoCairoContext_lineTo(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	
	cairo_line_to(CONTEXT(self), x, y);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_mask(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoObject *pattern = IoMessage_locals_valueArgAt_(m, locals, 0);
	
	cairo_mask(CONTEXT(self), IoCairoPattern_rawPattern(pattern));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_moveTo(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	
	cairo_move_to(CONTEXT(self), x, y);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_paintWithAlpha(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double alpha = IoMessage_locals_doubleArgAt_(m, locals, 0);
	
	cairo_paint_with_alpha(CONTEXT(self), alpha);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_rectangle(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double w = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double h = IoMessage_locals_doubleArgAt_(m, locals, 3);
	
	cairo_rectangle(CONTEXT(self), x, y, w, h);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_relativeCurveTo(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double dx1 = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double dy1 = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double dx2 = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double dy2 = IoMessage_locals_doubleArgAt_(m, locals, 3);
	double dx3 = IoMessage_locals_doubleArgAt_(m, locals, 4);
	double dy3 = IoMessage_locals_doubleArgAt_(m, locals, 5);
	
	cairo_rel_curve_to(CONTEXT(self), dx1, dy1, dx2, dy2, dx3, dy3);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_relativeLineTo(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double dx = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double dy = IoMessage_locals_doubleArgAt_(m, locals, 1);
	
	cairo_rel_line_to(CONTEXT(self), dx, dy);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_restore(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_restore(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_save(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_save(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_scale(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double sx = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double sy = IoMessage_locals_doubleArgAt_(m, locals, 1);
	
	cairo_scale(CONTEXT(self), sx, sy);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_selectFontFace(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	char *family = CSTRING(IoMessage_locals_valueArgAt_(m, locals, 0));
	cairo_font_slant_t slant = (cairo_font_slant_t)IoMessage_locals_intArgAt_(m, locals, 1);
	cairo_font_weight_t weight = (cairo_font_weight_t)IoMessage_locals_intArgAt_(m, locals, 2);
	
	cairo_select_font_face(CONTEXT(self), family, slant, weight);
	CHECK_STATUS(self);
	return self;	
}

IoObject *IoCairoContext_setFontSize(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double size = IoMessage_locals_doubleArgAt_(m, locals, 0);
	
	cairo_set_font_size(CONTEXT(self), size);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_setLineWidth(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double width = IoMessage_locals_doubleArgAt_(m, locals, 0);
	
	cairo_set_line_width(CONTEXT(self), width);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_setSource(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoObject *o = IoMessage_locals_valueArgAt_(m, locals, 0);
	cairo_pattern_t *pattern = IoCairoPattern_rawPattern(o);
	
	cairo_set_source(CONTEXT(self), pattern);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_setSourceRGB(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double r = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double g = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double b = IoMessage_locals_doubleArgAt_(m, locals, 2);
	
	cairo_set_source_rgb(CONTEXT(self), r, g, b);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_setSourceRGBA(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double r = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double g = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double b = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double a = IoMessage_locals_doubleArgAt_(m, locals, 3);
	
	cairo_set_source_rgba(CONTEXT(self), r, g, b, a);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_showText(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	char *text = CSTRING(IoMessage_locals_valueArgAt_(m, locals, 0));
	
	cairo_show_text(CONTEXT(self), text);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_stroke(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_stroke(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_textExtents(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	char *text = CSTRING(IoMessage_locals_valueArgAt_(m, locals, 0));
	cairo_text_extents_t extents;
	
	cairo_text_extents(CONTEXT(self), text, &extents);
	CHECK_STATUS(self);	
	return IoCairoTextExtents_newWithRawTextExtents(IOSTATE, &extents);
}

IoObject *IoCairoContext_translate(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double tx = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double ty = IoMessage_locals_doubleArgAt_(m, locals, 1);
	
	cairo_translate(CONTEXT(self), tx, ty);
	CHECK_STATUS(self);
	return self;
}
