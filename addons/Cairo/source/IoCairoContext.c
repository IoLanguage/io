//metadoc CairoContext copyright Daniel Rosengren, 2007
//metadoc CairoContext license BSD revised
//metadoc CairoContext category Graphics

#include "IoCairoContext.h"
#include "IoCairoSurface.h"
#include "IoCairoPattern.h"
#include "IoCairoMatrix.h"
#include "IoCairoPath.h"
#include "IoCairoRectangle.h"

#include "IoCairoFontFace.h"
#include "IoCairoScaledFont.h"
#include "IoCairoFontExtents.h"
#include "IoCairoFontOptions.h"
#include "IoCairoTextExtents.h"
#include "IoCairoExtents.h"
#include "IoCairoGlyph.h"

#include "IoNumber.h"
#include "IoList.h"
#include "tools.h"

#define CONTEXT(self) ((cairo_t *)IoObject_dataPointer(self))
#define CHECK_STATUS(self) checkStatus_(IOSTATE, m, cairo_status(CONTEXT(self)))

// MS Visual C doesn't seem to support C99's __func__ identifier
#if defined(_MSC_VER) 
#define __func__ __FUNCTION__
#endif

static IoTag *IoCairoContext_newTag(void *state)
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
			{"create", IoCairoContext_create},

			{"save", IoCairoContext_save},
			{"restore", IoCairoContext_restore},

			{"getTarget", IoCairoContext_getTarget},

			/* Groups */

			{"pushGroup", IoCairoContext_pushGroup},
			{"pushGroupWithContent", IoCairoContext_pushGroupWithContent},
			{"popGroup", IoCairoContext_popGroup},
			{"popGroupToSource", IoCairoContext_popGroupToSource},
			{"getGroupTarget", IoCairoContext_getGroupTarget},

			/* Source */

			{"setSource", IoCairoContext_setSource},
			{"setSourceRGB", IoCairoContext_setSourceRGB},
			{"setSourceRGBA", IoCairoContext_setSourceRGBA},
			{"setSourceSurface", IoCairoContext_setSourceSurface},
			{"getSource", IoCairoContext_getSource},

			/* Options / Properties */

			{"setAntialias", IoCairoContext_setAntialias},
			{"getAntialias", IoCairoContext_getAntialias},

			{"setDash", IoCairoContext_setDash},
			{"getDash", IoCairoContext_getDash},
			{"getDashCount", IoCairoContext_getDashCount},

			{"setFillRule", IoCairoContext_setFillRule},
			{"getFillRule", IoCairoContext_getFillRule},

			{"setLineCap", IoCairoContext_setLineCap},
			{"getLineCap", IoCairoContext_getLineCap},

			{"setLineJoin", IoCairoContext_setLineJoin},
			{"getLineJoin", IoCairoContext_getLineJoin},

			{"setLineWidth", IoCairoContext_setLineWidth},
			{"getLineWidth", IoCairoContext_getLineWidth},

			{"setMiterLimit", IoCairoContext_setMiterLimit},
			{"getMiterLimit", IoCairoContext_getMiterLimit},

			{"setOperator", IoCairoContext_setOperator},
			{"getOperator", IoCairoContext_getOperator},

			{"setTolerance", IoCairoContext_setTolerance},
			{"getTolerance", IoCairoContext_getTolerance},

			/* Paths */

			{"newPath", IoCairoContext_newPath},
			{"newSubPath", IoCairoContext_newSubPath},
			{"closePath", IoCairoContext_closePath},
			{"appendPath", IoCairoContext_appendPath},
			{"copyPath", IoCairoContext_copyPath},
			{"copyPathFlat", IoCairoContext_copyPathFlat},
			{"getCurrentPoint", IoCairoContext_getCurrentPoint},

			{"moveTo", IoCairoContext_moveTo},
			{"lineTo", IoCairoContext_lineTo},
			{"curveTo", IoCairoContext_curveTo},
			{"relMoveTo", IoCairoContext_relMoveTo},
			{"relCurveTo", IoCairoContext_relCurveTo},
			{"relLineTo", IoCairoContext_relLineTo},

			{"rectangle", IoCairoContext_rectangle},
			{"arc", IoCairoContext_arc},
			{"arcNegative", IoCairoContext_arcNegative},

			{"textPath", IoCairoContext_textPath},
			{"glyphPath", IoCairoContext_glyphPath},

			/* Clipping */

			{"clip", IoCairoContext_clip},
			{"clipPreserve", IoCairoContext_clipPreserve},
			{"clipExtents", IoCairoContext_clipExtents},
			{"resetClip", IoCairoContext_resetClip},
			{"copyClipRectangleList", IoCairoContext_copyClipRectangleList},

			/* Drawing */

			{"fill", IoCairoContext_fill},
			{"fillPreserve", IoCairoContext_fillPreserve},
			{"fillExtents", IoCairoContext_fillExtents},
			{"inFill", IoCairoContext_inFill},

			{"mask", IoCairoContext_mask},
			{"maskSurface", IoCairoContext_maskSurface},

			{"paint", IoCairoContext_paint},
			{"paintWithAlpha", IoCairoContext_paintWithAlpha},

			{"stroke", IoCairoContext_stroke},
			{"strokePreserve", IoCairoContext_strokePreserve},
			{"strokeExtents", IoCairoContext_strokeExtents},
			{"inStroke", IoCairoContext_inStroke},

			/* Transformations */

			{"translate", IoCairoContext_translate},
			{"scale", IoCairoContext_scale},
			{"rotate", IoCairoContext_rotate},
			{"transform", IoCairoContext_transform},

			{"setMatrix", IoCairoContext_setMatrix},
			{"getMatrix", IoCairoContext_getMatrix},
			{"identityMatrix", IoCairoContext_identityMatrix},

			{"userToDevice", IoCairoContext_userToDevice},
			{"userToDeviceDistance", IoCairoContext_userToDeviceDistance},
			{"deviceToUser", IoCairoContext_deviceToUser},
			{"deviceToUserDistance", IoCairoContext_deviceToUserDistance},

			/* Text */

			{"selectFontFace", IoCairoContext_selectFontFace},
			{"setFontFace", IoCairoContext_setFontFace},
			{"getFontFace", IoCairoContext_getFontFace},

			{"setFontSize", IoCairoContext_setFontSize},

			{"fontExtents", IoCairoContext_fontExtents},
			{"textExtents", IoCairoContext_textExtents},
			{"glyphExtents", IoCairoContext_glyphExtents},

			{"showText", IoCairoContext_showText},
			{"showGlyphs", IoCairoContext_showGlyphs},

			{"setScaledFont", IoCairoContext_setScaledFont},
			{"getScaledFont", IoCairoContext_getScaledFont},

			{"setFontOptions", IoCairoContext_setFontOptions},
			{"getFontOptions", IoCairoContext_getFontOptions},

			/* Pages */

			{"copyPage", IoCairoContext_copyPage},
			{"showPage", IoCairoContext_showPage},

			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoContext *IoCairoContext_rawClone(IoCairoContext *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	if (CONTEXT(proto)) {
		IoObject_setDataPointer_(self, cairo_reference(CONTEXT(proto)));
	}
	return self;
}

IoCairoContext *IoCairoContext_newWithSurface_(void *state, IoCairoImageSurface *surface)
{
	IoCairoContext *self = IOCLONE(IoState_protoWithInitFunction_(state, IoCairoContext_proto));
	cairo_t *rawContext = cairo_create(IoCairoSurface_rawSurface(surface));
	checkStatus_(state, 0, cairo_status(rawContext));
	IoObject_setDataPointer_(self, rawContext);
	return self;
}

void IoCairoContext_free(IoCairoContext *self)
{
	if (CONTEXT(self))
		cairo_destroy(CONTEXT(self));
}

cairo_t *IoCairoContext_getRawContext(IoCairoContext *self)
{
	return CONTEXT(self);
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoContext_create(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	return IoCairoContext_newWithSurface_(IOSTATE, IoMessage_locals_valueArgAt_(m, locals, 0));
}


IoObject *IoCairoContext_save(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_save(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_restore(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_restore(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_getTarget(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_surface_t *target = cairo_get_target(CONTEXT(self));
	return IoCairoSurface_newWithRawSurface_(IOSTATE, m, cairo_surface_reference(target));
}


/* ------------------------------------------------------------------------------------------------*/
/* Source */

IoObject *IoCairoContext_setSource(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoCairoPattern *pattern = IoMessage_locals_valueArgAt_(m, locals, 0);
	cairo_set_source(CONTEXT(self), IoCairoPattern_rawPattern(pattern));
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

IoObject *IoCairoContext_setSourceSurface(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoCairoSurface *surface = IoMessage_locals_valueArgAt_(m, locals, 0);
	double x = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double y = IoMessage_locals_doubleArgAt_(m, locals, 2);

	cairo_set_source_surface(CONTEXT(self), IoCairoSurface_rawSurface(surface), x, y);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_getSource(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_pattern_t *pattern = cairo_get_source(CONTEXT(self));
	return IoCairoPattern_newWithRawPattern_(IOSTATE, m, cairo_pattern_reference(pattern));
}


/* ------------------------------------------------------------------------------------------------*/
/* Groups */

IoObject *IoCairoContext_pushGroup(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_push_group(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_pushGroupWithContent(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_push_group_with_content(CONTEXT(self), IoMessage_locals_intArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_popGroup(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_pattern_t *pattern = cairo_pop_group(CONTEXT(self));
	return IoCairoPattern_newWithRawPattern_(IOSTATE, m, cairo_pattern_reference(pattern));
}

IoObject *IoCairoContext_popGroupToSource(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_pop_group_to_source(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_getGroupTarget(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_surface_t *target = cairo_get_group_target(CONTEXT(self));
	return IoCairoSurface_newWithRawSurface_(IOSTATE, m, cairo_surface_reference(target));
}


/* ------------------------------------------------------------------------------------------------*/
/* Options/Properties */

IoObject *IoCairoContext_setAntialias(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_set_antialias(CONTEXT(self), IoMessage_locals_intArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_getAntialias(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_get_antialias(CONTEXT(self)));
}


IoObject *IoCairoContext_setDash(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoList *dashList = IoMessage_locals_valueArgAt_(m, locals, 0);
	double offset = IoMessage_locals_doubleArgAt_(m, locals, 1);
	List *list = 0;
	int dashCount = 0;
	double *dashes = 0;

	if (!ISNIL(dashList)) {
		list = IoList_rawList(dashList);
		dashCount = List_size(list);
	}

	if (dashCount > 0)
	{
		dashes = malloc(sizeof(double) * dashCount);
		LIST_FOREACH(list, i, number,
			dashes[i] = IoNumber_asDouble(number);
		);
	}

	cairo_set_dash(CONTEXT(self), dashes, dashCount, offset);
	if (dashes)
		free(dashes);

	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_getDash(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoList *list = IoList_new(IOSTATE);
	IoList *dashList = IoList_new(IOSTATE);
	int dashCount = cairo_get_dash_count(CONTEXT(self));
	double *dashes = 0;
	double offset = 0;
	int i;

	IoList_rawAppend_(list, dashList);

	if (dashCount == 0)
	{
		IoList_rawAppend_(list, IONUMBER(0));
		return list;
	}

	dashes = malloc(sizeof(double) * dashCount);
	cairo_get_dash(CONTEXT(self), dashes, &offset);
	for (i = 0; i < dashCount; i++)
		IoList_rawAppend_(dashList, IONUMBER(dashes[i]));
	free(dashes);

	CHECK_STATUS(self);
	IoList_rawAppend_(list, IONUMBER(offset));
	return list;
}

IoObject *IoCairoContext_getDashCount(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_get_dash_count(CONTEXT(self)));
}


IoObject *IoCairoContext_setFillRule(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_set_fill_rule(CONTEXT(self), IoMessage_locals_intArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_getFillRule(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_get_fill_rule(CONTEXT(self)));
}


IoObject *IoCairoContext_setLineCap(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_set_line_cap(CONTEXT(self), IoMessage_locals_intArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_getLineCap(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_get_line_cap(CONTEXT(self)));
}


IoObject *IoCairoContext_setLineJoin(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_set_line_join(CONTEXT(self), IoMessage_locals_intArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_getLineJoin(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_get_line_join(CONTEXT(self)));
}


IoObject *IoCairoContext_setLineWidth(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_set_line_width(CONTEXT(self), IoMessage_locals_doubleArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_getLineWidth(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_get_line_width(CONTEXT(self)));
}


IoObject *IoCairoContext_setMiterLimit(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_set_miter_limit(CONTEXT(self), IoMessage_locals_doubleArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_getMiterLimit(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_get_miter_limit(CONTEXT(self)));
}


IoObject *IoCairoContext_setOperator(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_set_operator(CONTEXT(self), IoMessage_locals_intArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_getOperator(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_get_operator(CONTEXT(self)));
}


IoObject *IoCairoContext_setTolerance(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_set_tolerance(CONTEXT(self), IoMessage_locals_doubleArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_getTolerance(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_get_tolerance(CONTEXT(self)));
}


/* ------------------------------------------------------------------------------------------------*/
/* Paths */

IoObject *IoCairoContext_newPath(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_new_path(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_newSubPath(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_new_sub_path(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_closePath(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_close_path(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_appendPath(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoCairoPath *path = IoMessage_locals_cairoPathArgAt_(m, locals, 0);
	cairo_append_path(CONTEXT(self), IoCairoPath_rawPath(path));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_copyPath(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	return IoCairoPath_newWithRawPath_(IOSTATE, cairo_copy_path(CONTEXT(self)));
}

IoObject *IoCairoContext_copyPathFlat(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	return IoCairoPath_newWithRawPath_(IOSTATE, cairo_copy_path_flat(CONTEXT(self)));
}

IoObject *IoCairoContext_getCurrentPoint(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double x = 0, y = 0;
	cairo_get_current_point(CONTEXT(self), &x, &y);
	return IoSeq_newWithX_y_(IOSTATE, x, y);
}


IoObject *IoCairoContext_moveTo(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y = IoMessage_locals_doubleArgAt_(m, locals, 1);

	cairo_move_to(CONTEXT(self), x, y);
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

IoObject *IoCairoContext_curveTo(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double x1 = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y1 = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double x2 = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double y2 = IoMessage_locals_doubleArgAt_(m, locals, 3);
	double x3 = IoMessage_locals_doubleArgAt_(m, locals, 4);
	double y3 = IoMessage_locals_doubleArgAt_(m, locals, 5);

	cairo_curve_to(CONTEXT(self), x1, y1, x2, y2, x3, y3);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_relMoveTo(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double dx = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double dy = IoMessage_locals_doubleArgAt_(m, locals, 1);

	cairo_rel_move_to(CONTEXT(self), dx, dy);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_relLineTo(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double dx = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double dy = IoMessage_locals_doubleArgAt_(m, locals, 1);

	cairo_rel_line_to(CONTEXT(self), dx, dy);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_relCurveTo(IoCairoContext *self, IoObject *locals, IoMessage *m)
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

IoObject *IoCairoContext_arcNegative(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double xc = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double yc = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double radius = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double angle1 = IoMessage_locals_doubleArgAt_(m, locals, 3);
	double angle2 = IoMessage_locals_doubleArgAt_(m, locals, 4);

	cairo_arc_negative(CONTEXT(self), xc, yc, radius, angle1, angle2);
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

IoObject *IoCairoContext_textPath(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_text_path(CONTEXT(self), IoMessage_locals_UTF8ArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_glyphPath(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoList *glyphList = IoMessage_locals_listArgAt_(m, locals, 0);
	int glyphCount = 0;
	cairo_glyph_t *glyphs = rawGlyphsFromList_count_(glyphList, &glyphCount);

	if (!glyphs)
		return self;

	cairo_glyph_path(CONTEXT(self), glyphs, glyphCount);
	free(glyphs);
	CHECK_STATUS(self);
	return self;
}


/* ------------------------------------------------------------------------------------------------*/
/* Clipping */

IoObject *IoCairoContext_clip(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_clip(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_clipPreserve(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_clip_preserve(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_clipExtents(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double x1, y1, x2, y2;
	cairo_clip_extents(CONTEXT(self), &x1, &y1, &x2, &y2);
	return IoCairoExtents_newSet(IOSTATE, x1, y1, x2, y2);
}

IoObject *IoCairoContext_resetClip(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_reset_clip(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_copyClipRectangleList(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoList *list = IoList_new(IOSTATE);
	cairo_rectangle_list_t *rectList = cairo_copy_clip_rectangle_list(CONTEXT(self));
	cairo_rectangle_t *rect = 0;
	int i;

	if (rectList->status != CAIRO_STATUS_SUCCESS)
		IoState_error_(IOSTATE, m, "%s: cairo: %s", __func__, cairo_status_to_string(rectList->status));

	rect = rectList->rectangles;
	for (i = 0; i < rectList->num_rectangles; i++)
	{
		IoList_rawAppend_(list, IoCairoRectangle_newWithRawRectangle_(IOSTATE, rect));
		rect++;
	}
	cairo_rectangle_list_destroy(rectList);
	return list;
}


/* ------------------------------------------------------------------------------------------------*/
/* Drawing */


IoObject *IoCairoContext_fill(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_fill(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_fillPreserve(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_fill_preserve(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_fillExtents(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double x1, y1, x2, y2;
	cairo_fill_extents(CONTEXT(self), &x1, &y1, &x2, &y2);
	return IoCairoExtents_newSet(IOSTATE, x1, y1, x2, y2);
}

IoObject *IoCairoContext_inFill(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	return IOBOOL(self, cairo_in_fill(CONTEXT(self), x, y));
}


IoObject *IoCairoContext_stroke(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_stroke(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_strokePreserve(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_stroke_preserve(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_strokeExtents(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double x1, y1, x2, y2;
	cairo_stroke_extents(CONTEXT(self), &x1, &y1, &x2, &y2);
	return IoCairoExtents_newSet(IOSTATE, x1, y1, x2, y2);
}

IoObject *IoCairoContext_inStroke(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	return IOBOOL(self, cairo_in_stroke(CONTEXT(self), x, y));
}


IoObject *IoCairoContext_mask(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoObject *pattern = IoMessage_locals_valueArgAt_(m, locals, 0);
	cairo_mask(CONTEXT(self), IoCairoPattern_rawPattern(pattern));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_maskSurface(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoCairoSurface *surface = IoMessage_locals_valueArgAt_(m, locals, 0);
	double x = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double y = IoMessage_locals_doubleArgAt_(m, locals, 2);

	cairo_mask_surface(CONTEXT(self), IoCairoSurface_rawSurface(surface), x, y);
	CHECK_STATUS(self);
	return self;
}


IoObject *IoCairoContext_paint(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_paint(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_paintWithAlpha(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_paint_with_alpha(CONTEXT(self), IoMessage_locals_doubleArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}


/* ------------------------------------------------------------------------------------------------*/
/* Transformations */

IoObject *IoCairoContext_translate(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double tx = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double ty = IoMessage_locals_doubleArgAt_(m, locals, 1);

	cairo_translate(CONTEXT(self), tx, ty);
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

IoObject *IoCairoContext_rotate(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_rotate(CONTEXT(self), IoMessage_locals_doubleArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_transform(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoCairoMatrix *matrix = IoMessage_locals_cairoMatrixArgAt_(m, locals, 0);
	cairo_transform(CONTEXT(self), IoCairoMatrix_rawMatrix(matrix));
	CHECK_STATUS(self);
	return self;
}


IoObject *IoCairoContext_setMatrix(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoCairoMatrix *matrix = IoMessage_locals_cairoMatrixArgAt_(m, locals, 0);
	cairo_set_matrix(CONTEXT(self), IoCairoMatrix_rawMatrix(matrix));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_getMatrix(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_matrix_t matrix;
	cairo_get_matrix(CONTEXT(self), &matrix);
	return IoCairoMatrix_newWithRawMatrix_(IOSTATE, &matrix);
}

IoObject *IoCairoContext_identityMatrix(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_identity_matrix(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}


IoObject *IoCairoContext_userToDevice(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y = IoMessage_locals_doubleArgAt_(m, locals, 1);

	cairo_user_to_device(CONTEXT(self), &x, &y);
	CHECK_STATUS(self);
	return IoSeq_newWithX_y_(IOSTATE, x, y);
}

IoObject *IoCairoContext_userToDeviceDistance(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y = IoMessage_locals_doubleArgAt_(m, locals, 1);

	cairo_user_to_device_distance(CONTEXT(self), &x, &y);
	return IoSeq_newWithX_y_(IOSTATE, x, y);
}

IoObject *IoCairoContext_deviceToUser(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y = IoMessage_locals_doubleArgAt_(m, locals, 1);

	cairo_device_to_user(CONTEXT(self), &x, &y);
	return IoSeq_newWithX_y_(IOSTATE, x, y);
}

IoObject *IoCairoContext_deviceToUserDistance(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y = IoMessage_locals_doubleArgAt_(m, locals, 1);

	cairo_device_to_user_distance(CONTEXT(self), &x, &y);
	CHECK_STATUS(self);
	return IoSeq_newWithX_y_(IOSTATE, x, y);
}


/* ------------------------------------------------------------------------------------------------*/
/* Text */

IoObject *IoCairoContext_setFontSize(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_set_font_size(CONTEXT(self), IoMessage_locals_doubleArgAt_(m, locals, 0));
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

IoObject *IoCairoContext_textExtents(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	const char *text = IoMessage_locals_UTF8ArgAt_(m, locals, 0);
	cairo_text_extents_t extents;

	cairo_text_extents(CONTEXT(self), text, &extents);
	CHECK_STATUS(self);
	return IoCairoTextExtents_newWithRawTextExtents(IOSTATE, &extents);
}

IoObject *IoCairoContext_glyphExtents(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoList *glyphList = IoMessage_locals_listArgAt_(m, locals, 0);
	int glyphCount = 0;
	cairo_glyph_t *glyphs = rawGlyphsFromList_count_(glyphList, &glyphCount);
	cairo_text_extents_t extents;

	if (!glyphs)
		return IONIL(self);

	cairo_glyph_extents(CONTEXT(self), glyphs, glyphCount, &extents);
	free(glyphs);
	CHECK_STATUS(self);
	return IoCairoTextExtents_newWithRawTextExtents(IOSTATE, &extents);
}


IoObject *IoCairoContext_showText(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_show_text(CONTEXT(self), IoMessage_locals_UTF8ArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_showGlyphs(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoList *glyphList = IoMessage_locals_listArgAt_(m, locals, 0);
	int glyphCount = 0;
	cairo_glyph_t *glyphs = rawGlyphsFromList_count_(glyphList, &glyphCount);

	if (!glyphs)
		return self;

	cairo_show_glyphs(CONTEXT(self), glyphs, glyphCount);
	free(glyphs);
	CHECK_STATUS(self);
	return self;
}


IoObject *IoCairoContext_selectFontFace(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	char *family = CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0));
	cairo_font_slant_t slant = IoMessage_locals_intArgAt_(m, locals, 1);
	cairo_font_weight_t weight = IoMessage_locals_intArgAt_(m, locals, 2);

	cairo_select_font_face(CONTEXT(self), family, slant, weight);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_setFontFace(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoCairoFontFace *face = IoMessage_locals_cairoFontFaceArgAt_(m, locals, 0);
	cairo_set_font_face(CONTEXT(self), IoCairoFontFace_rawFontFace(face));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_getFontFace(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_font_face_t *face = cairo_get_font_face(CONTEXT(self));
	return IoCairoFontFace_newWithRawFontFace_(self, cairo_font_face_reference(face));
}


IoObject *IoCairoContext_setScaledFont(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoCairoScaledFont *font = IoMessage_locals_cairoScaledFontArgAt_(m, locals, 0);
	cairo_set_scaled_font(CONTEXT(self), IoCairoScaledFont_rawScaledFont(font));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_getScaledFont(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_scaled_font_t *font = cairo_get_scaled_font(CONTEXT(self));
	return IoCairoScaledFont_newWithRawScaledFont_(IOSTATE, m, cairo_scaled_font_reference(font));
}


IoObject *IoCairoContext_setFontOptions(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	IoCairoFontOptions *options = IoMessage_locals_cairoFontOptionsArgAt_(m, locals, 0);
	cairo_set_font_options(CONTEXT(self), IoCairoFontOptions_rawFontOptions(options));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_getFontOptions(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_font_options_t *options = cairo_font_options_create();
	cairo_get_font_options(CONTEXT(self), options);
	return IoCairoFontOptions_newWithRawFontOptions_(IOSTATE, m, options);
}


/* ------------------------------------------------------------------------------------------------*/
/* Pages */

IoObject *IoCairoContext_copyPage(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_copy_page(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoContext_showPage(IoCairoContext *self, IoObject *locals, IoMessage *m)
{
	cairo_show_page(CONTEXT(self));
	CHECK_STATUS(self);
	return self;
}

