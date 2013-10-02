//metadoc CairoGradient copyright Trevor Fancher, 2007
//metadoc CairoGradient copyright Daniel Rosengren, 2007
//metadoc CairoGradient license BSD revised
//metadoc CairoGradient category Graphics

#include "IoCairoGradient.h"
#include "IoCairoPattern.h"
#include "IoCairoPattern_inline.h"
#include "IoNumber.h"
#include "tools.h"

static const char *protoId = "Gradient";

static IoTag *IoCairoGradient_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoGradient_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoPattern_free);
	return tag;
}

IoCairoGradient *IoCairoGradient_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoGradient_newTag(state));

	IoState_registerProtoWithId_(state, self, protoId);

	IoCairoGradient_addMethods(self);

	return self;
}

void IoCairoGradient_addMethods(IoCairoGradient *self)
{
	IoCairoPattern_addMethods(self);
	{
		IoMethodTable methodTable[] = {
			{"addColorStopRGB", IoCairoGradient_addColorStopRGB},
			{"addColorStopRGBA", IoCairoGradient_addColorStopRGBA},

			{"getColorStopCount", IoCairoGradient_getColorStopCount},
			{"getColorStopRGBA", IoCairoGradient_getColorStopRGBA},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
}

IoCairoGradient *IoCairoGradient_rawClone(IoCairoGradient *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	if (PATTERN(proto))
		IoObject_setDataPointer_(self, cairo_pattern_reference(PATTERN(proto)));
	return self;
}

/* ----------------------------------------------------------- */

IoObject *IoCairoGradient_addColorStopRGB(IoCairoGradient *self, IoObject *locals, IoMessage *m)
{
	double o = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double r = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double g = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double b = IoMessage_locals_doubleArgAt_(m, locals, 3);

	cairo_pattern_add_color_stop_rgb(PATTERN(self), o, r, g, b);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoGradient_addColorStopRGBA(IoCairoGradient *self, IoObject *locals, IoMessage *m)
{
	double o = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double r = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double g = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double b = IoMessage_locals_doubleArgAt_(m, locals, 3);
	double a = IoMessage_locals_doubleArgAt_(m, locals, 4);

	cairo_pattern_add_color_stop_rgba(PATTERN(self), o, r, g, b, a);
	CHECK_STATUS(self);
	return self;
}


IoObject *IoCairoGradient_getColorStopCount(IoCairoGradient *self, IoObject *locals, IoMessage *m)
{
	int count = 0;
	cairo_pattern_get_color_stop_count(PATTERN(self), &count);
	return IONUMBER(count);
}

IoObject *IoCairoGradient_getColorStopRGBA(IoCairoGradient *self, IoObject *locals, IoMessage *m)
{
	int index = IoMessage_locals_intArgAt_(m, locals, 0);
	double data[5];

	cairo_pattern_get_color_stop_rgba(PATTERN(self), index, &data[0], &data[1], &data[2], &data[3], &data[4]);
	return IoSeq_newWithDoubles_count_(IOSTATE, data, 5);
}
