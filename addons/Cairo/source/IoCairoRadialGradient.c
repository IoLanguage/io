//metadoc CairoRadialGradient copyright Trevor Fancher, 2007
//metadoc CairoRadialGradient copyright Daniel Rosengren, 2007
//metadoc CairoRadialGradient license BSD revised
//metadoc CairoRadialGradient category Graphics

#include "IoCairoRadialGradient.h"
#include "IoCairoGradient.h"
#include "IoCairoPattern.h"
#include "IoCairoPattern_inline.h"
#include "tools.h"
static const char *protoId = "RadialGradient";

static IoTag *IoCairoRadialGradient_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("RadialGradient");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoRadialGradient_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoPattern_free);
	return tag;
}

IoCairoRadialGradient *IoCairoRadialGradient_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoRadialGradient_newTag(state));

	IoState_registerProtoWithId_(state, self, protoId);

	IoCairoGradient_addMethods(self);
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoRadialGradient_create},
			{"getRadialCircles", IoCairoRadialGradient_getRadialCircles},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoRadialGradient *IoCairoRadialGradient_rawClone(IoCairoRadialGradient *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	if (PATTERN(proto))
		IoObject_setDataPointer_(self, cairo_pattern_reference(PATTERN(proto)));
	return self;
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoRadialGradient_create(IoCairoRadialGradient *self, IoObject *locals, IoMessage *m)
{
	double cx0 = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double cy0 = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double radius0 = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double cx1 = IoMessage_locals_doubleArgAt_(m, locals, 3);
	double cy1 = IoMessage_locals_doubleArgAt_(m, locals, 4);
	double radius1 = IoMessage_locals_doubleArgAt_(m, locals, 5);

	cairo_pattern_t *pattern = cairo_pattern_create_radial(cx0, cy0, radius0, cx1, cy1, radius1);
	return IoCairoPattern_newWithRawPattern_(IOSTATE, m, pattern);
}

IoObject *IoCairoRadialGradient_getRadialCircles(IoCairoRadialGradient *self, IoObject *locals, IoMessage *m)
{
	double data[6];
	cairo_pattern_get_radial_circles(PATTERN(self), &data[0], &data[1], &data[2], &data[3], &data[4], &data[5]);
	return IoSeq_newWithDoubles_count_(IOSTATE, data, 6);
}
