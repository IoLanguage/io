/*#io
CairoGradient ioDoc(
	docCopyright("Trevor Fancher", 2007)
	docCopyright("Daniel Rosengren", 2007)
	docLicense("BSD revised")
	docCategory("Graphics")
*/

#include "IoCairoGradient.h"
#include "IoCairoPattern.h"
#include "IoCairoPattern_inline.h"

#define DATA(self) ((IoCairoPatternData *)IoObject_dataPointer(self))


IoTag *IoCairoGradient_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Gradient");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoGradient_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoPattern_free);
	return tag;
}

IoCairoGradient *IoCairoGradient_proto(void *state) 
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoGradient_newTag(state));
	
	IoState_registerProtoWithFunc_(state, self, IoCairoGradient_proto);

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
