/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
		    */

#include "IoCairoContext.h"
#include "IoCairoSurfaceImage.h"
#include "IoCairoPattern.h"
#include "IoList.h"
#include "IoMessage.h"
#include "IoNumber.h"

#define DATA(self) ((IoCairoContextData *)IoObject_dataPointer(self))
#define CONTEXT(ctx) (DATA(ctx)->context)

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
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCairoContext_mark);
	return tag;
}

IoCairoContext *IoCairoContext_proto(void *state) 
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoContext_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCairoContextData)));
	
	IoState_registerProtoWithFunc_(state, self, IoCairoContext_proto);
	
	{
		IoMethodTable methodTable[] = {
			{"arc", IoCairoContext_arc},
			{"closePath", IoCairoContext_closePath},
			{"create", IoCairoContext_create},
			{"fill", IoCairoContext_fill},		
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
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoContext *IoCairoContext_rawClone(IoCairoContext *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoCairoContextData)));
	if (CONTEXT(proto)) {
		cairo_reference(CONTEXT(proto));
	}
	
	return self;
}

/* ----------------------------------------------------------- */

IoCairoContext *IoCairoContext_newWithSurface_(void *state, IoCairoSurfaceImage *surface)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoCairoContext_proto);
	IoObject *self = IOCLONE(proto);
	
	CONTEXT(self) = cairo_create(IoCairoSurfaceImage_getRawSurface(surface));
	return self;
}

void IoCairoContext_free(IoCairoContext *self) 
{
	if (CONTEXT(self)) {
		cairo_destroy(CONTEXT(self));
	}
	
	free(IoObject_dataPointer(self)); 
}

void IoCairoContext_mark(IoCairoContext *self) 
{
}

cairo_t *IoCairoContext_getRawContext(IoCairoContext *self)
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
	
	cairo_mask(CONTEXT(self), IoCairoPattern_getRawPattern(pattern));
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
	cairo_pattern_t *pattern = IoCairoPattern_getRawPattern(o);
	
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
	
	IoList *list = IoList_new(IOSTATE);
	IoList_rawAppend_(list, IONUMBER(extents.x_bearing));
	IoList_rawAppend_(list, IONUMBER(extents.y_bearing));
	IoList_rawAppend_(list, IONUMBER(extents.width));
	IoList_rawAppend_(list, IONUMBER(extents.height));
	IoList_rawAppend_(list, IONUMBER(extents.x_advance));
	IoList_rawAppend_(list, IONUMBER(extents.x_advance));
	
	return IoState_on_doCString_withLabel_(IOSTATE, list, "asCairoTextExtents", "IoCairoContext_textExtents");
}
