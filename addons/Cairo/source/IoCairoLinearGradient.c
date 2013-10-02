//metadoc CairoLinearGradient copyright Trevor Fancher, 2007
//metadoc CairoLinearGradient copyright Daniel Rosengren, 2007
//metadoc CairoLinearGradient license BSD revised
//metadoc CairoLinearGradient category Graphics

#include "IoCairoLinearGradient.h"
#include "IoCairoGradient.h"
#include "IoCairoPattern.h"
#include "IoCairoPattern_inline.h"
#include "tools.h"

static const char *protoId = "LinearGradient";

static IoTag *IoCairoLinearGradient_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoLinearGradient_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoPattern_free);
	return tag;
}

IoCairoLinearGradient *IoCairoLinearGradient_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoLinearGradient_newTag(state));

	IoState_registerProtoWithId_(state, self, protoId);

	IoCairoGradient_addMethods(self);
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoLinearGradient_create},
			{"getLinearPoints", IoCairoLinearGradient_getLinearPoints},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoCairoLinearGradient *IoCairoLinearGradient_rawClone(IoCairoLinearGradient *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	if (PATTERN(proto))
		IoObject_setDataPointer_(self, cairo_pattern_reference(PATTERN(proto)));
	return self;
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoLinearGradient_create(IoCairoLinearGradient *self, IoObject *locals, IoMessage *m)
{
	double x0 = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y0 = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double x1 = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double y1 = IoMessage_locals_doubleArgAt_(m, locals, 3);

	return IoCairoPattern_newWithRawPattern_(IOSTATE, m, cairo_pattern_create_linear(x0, y0, x1, y1));
}

IoObject *IoCairoLinearGradient_getLinearPoints(IoCairoLinearGradient *self, IoObject *locals, IoMessage *m)
{
	double data[4];
	cairo_pattern_get_linear_points(PATTERN(self), &data[0], &data[1], &data[2], &data[3]);
	return IoSeq_newWithDoubles_count_(IOSTATE, data, 4);
}

