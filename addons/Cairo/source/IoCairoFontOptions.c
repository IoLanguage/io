//metadoc CairoFontOptions copyright Daniel Rosengren, 2007
//metadoc CairoFontOptions license BSD revised
//metadoc CairoFontOptions category Graphics

#include "IoCairoFontOptions.h"
#include "IoNumber.h"
#include "tools.h"
#include <stdlib.h>

#define OPTIONS(self) ((cairo_font_options_t *)IoObject_dataPointer(self))
#define CHECK_STATUS(self) checkStatus_(IOSTATE, m, cairo_font_options_status(OPTIONS(self)))

static const char *protoId = "CairoFontOptions";

void *IoMessage_locals_cairoFontOptionsArgAt_(IoMessage *self, void *locals, int n)
{
	IoObject *arg = IoMessage_locals_valueArgAt_(self, locals, n);
	if (!ISCAIROFONTOPTIONS(arg))
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "CairoFontOptions");
	return arg;
}


static IoTag *IoCairoFontOptions_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoFontOptions_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoFontOptions_rawClone);
	IoTag_compareFunc_(tag, (IoTagCompareFunc *)IoCairoFontOptions_compare);
	return tag;
}

IoCairoFontOptions *IoCairoFontOptions_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoFontOptions_newTag(state));

	IoState_registerProtoWithFunc_(state, self, IoCairoFontOptions_proto);

	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoFontOptions_create},
			{"merge", IoCairoFontOptions_merge},

			{"setAntialias", IoCairoFontOptions_setAntialias},
			{"getAntialias", IoCairoFontOptions_getAntialias},

			{"getSubpixelOrder", IoCairoFontOptions_getSubpixelOrder},
			{"setSubpixelOrder", IoCairoFontOptions_setSubpixelOrder},

			{"setHintStyle", IoCairoFontOptions_setHintStyle},
			{"getHintStyle", IoCairoFontOptions_getHintStyle},

			{"setHintMetrics", IoCairoFontOptions_setHintMetrics},
			{"getHintMetrics", IoCairoFontOptions_getHintMetrics},

			{NULL, NULL},
		};

		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoCairoFontOptions *IoCairoFontOptions_rawClone(IoCairoFontOptions *proto)
{
	IoCairoFontOptions *self = IoObject_rawClonePrimitive(proto);
	if (OPTIONS(proto))
		IoObject_setDataPointer_(self, cairo_font_options_copy(OPTIONS(proto)));
	return self;
}

IoCairoFontOptions *IoCairoFontOptions_new(void *state, IoMessage *m)
{
	return IoCairoFontOptions_newWithRawFontOptions_(state, m, cairo_font_options_create());
}

IoCairoFontOptions *IoCairoFontOptions_newWithRawFontOptions_(void *state, IoMessage *m, cairo_font_options_t *options)
{
	IoCairoFontOptions *self = IOCLONE(IoState_protoWithId_(state, proto));
	IoObject_setDataPointer_(self, options);
	CHECK_STATUS(self);
	return self;
}

void IoCairoFontOptions_free(IoCairoFontOptions *self)
{
	if (OPTIONS(self))
		cairo_font_options_destroy(OPTIONS(self));
}

int IoCairoFontOptions_compare(IoCairoFontOptions *self, IoCairoFontOptions *other)
{
	cairo_font_options_t *options = OPTIONS(self);
	cairo_font_options_t *otherOptions = OPTIONS(other);
	if (!options && !otherOptions)
		return 1;
	if (!options || !otherOptions)
		return 0;
	return cairo_font_options_equal(options, otherOptions);
}


cairo_font_options_t *IoCairoFontOptions_rawFontOptions(IoCairoFontOptions *self)
{
	return OPTIONS(self);
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoFontOptions_create(IoCairoFontOptions *self, IoObject *locals, IoMessage *m)
{
	return IoCairoFontOptions_new(IOSTATE, m);
}

IoObject *IoCairoFontOptions_merge(IoCairoFontOptions *self, IoObject *locals, IoMessage *m)
{
	IoCairoFontOptions *other = IoMessage_locals_cairoFontOptionsArgAt_(m, locals, 0);
	cairo_font_options_merge(OPTIONS(self), IoCairoFontOptions_rawFontOptions(other));
	CHECK_STATUS(self);
	return self;
}


IoObject *IoCairoFontOptions_setAntialias(IoCairoFontOptions *self, IoObject *locals, IoMessage *m)
{
	cairo_font_options_set_antialias(OPTIONS(self), IoMessage_locals_intArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoFontOptions_getAntialias(IoCairoFontOptions *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_font_options_get_antialias(OPTIONS(self)));
}


IoObject *IoCairoFontOptions_setSubpixelOrder(IoCairoFontOptions *self, IoObject *locals, IoMessage *m)
{
	cairo_font_options_set_subpixel_order(OPTIONS(self), IoMessage_locals_intArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoFontOptions_getSubpixelOrder(IoCairoFontOptions *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_font_options_get_subpixel_order(OPTIONS(self)));
}


IoObject *IoCairoFontOptions_setHintStyle(IoCairoFontOptions *self, IoObject *locals, IoMessage *m)
{
	cairo_font_options_set_subpixel_order(OPTIONS(self), IoMessage_locals_intArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoFontOptions_getHintStyle(IoCairoFontOptions *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_font_options_get_hint_style(OPTIONS(self)));
}


IoObject *IoCairoFontOptions_setHintMetrics(IoCairoFontOptions *self, IoObject *locals, IoMessage *m)
{
	cairo_font_options_set_hint_metrics(OPTIONS(self), IoMessage_locals_intArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoFontOptions_getHintMetrics(IoCairoFontOptions *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_font_options_get_hint_metrics(OPTIONS(self)));
}

