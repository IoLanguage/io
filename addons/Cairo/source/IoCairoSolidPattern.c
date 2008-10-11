//metadoc CairoSolidPattern copyright Trevor Fancher, 2007
//metadoc CairoSolidPattern copyright Daniel Rosengren, 2007
//metadoc CairoSolidPattern license BSD revised
//metadoc CairoSolidPattern category Graphics

#include "IoCairoSolidPattern.h"
#include "IoCairoPattern.h"
#include "IoCairoPattern_inline.h"
#include "tools.h"


static IoTag *IoCairoSolidPattern_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("SolidPattern");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoSolidPattern_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoPattern_free);
	return tag;
}

IoCairoSolidPattern *IoCairoSolidPattern_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoSolidPattern_newTag(state));

	IoState_registerProtoWithFunc_(state, self, IoCairoSolidPattern_proto);

	IoCairoPattern_addMethods(self);
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoSolidPattern_create},
			{"getRGBA", IoCairoSolidPattern_getRGBA},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoSolidPattern *IoCairoSolidPattern_rawClone(IoCairoSolidPattern *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	if (PATTERN(proto))
		IoObject_setDataPointer_(self, cairo_pattern_reference(PATTERN(proto)));
	return self;
}

/* ----------------------------------------------------------- */

IoObject *IoCairoSolidPattern_create(IoCairoSolidPattern *self, IoObject *locals, IoMessage *m)
{
	double r = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double g = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double b = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double a = 1.0;
	if (IoMessage_argCount(m) > 3)
		a = IoMessage_locals_doubleArgAt_(m, locals, 3);

	return IoCairoPattern_newWithRawPattern_(IOSTATE, m, cairo_pattern_create_rgba(r, g, b, a));
}

IoObject *IoCairoSolidPattern_getRGBA(IoCairoSolidPattern *self, IoObject *locals, IoMessage *m)
{
	double color[4];
	cairo_pattern_get_rgba(PATTERN(self), &color[0], &color[1], &color[2], &color[3]);
	return IoSeq_newWithDoubles_count_(IOSTATE, color, 4);
}
