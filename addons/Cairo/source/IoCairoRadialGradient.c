/*#io
CairoRadialGradient ioDoc(
	docCopyright("Trevor Fancher", 2007)
	docCopyright("Daniel Rosengren", 2007)
	docLicense("BSD revised")
	docCategory("Graphics")
*/

#include "IoCairoRadialGradient.h"
#include "IoCairoGradient.h"
#include "IoCairoPattern.h"
#include "IoCairoPattern_inline.h"

#define DATA(self) ((IoCairoPatternData *)IoObject_dataPointer(self))


IoTag *IoCairoRadialGradient_newTag(void *state)
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
	
	IoState_registerProtoWithFunc_(state, self, IoCairoRadialGradient_proto);
	
	IoCairoGradient_addMethods(self);
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoRadialGradient_create},
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

/* ----------------------------------------------------------- */

IoObject *IoCairoRadialGradient_create(IoCairoRadialGradient *self, IoObject *locals, IoMessage *m)
{
	IoObject *proto = IoState_protoWithInitFunction_(IOSTATE, IoCairoRadialGradient_proto);
	IoObject *pattern = IOCLONE(proto);
	double cx0 = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double cy0 = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double radius0 = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double cx1 = IoMessage_locals_doubleArgAt_(m, locals, 3);
	double cy1 = IoMessage_locals_doubleArgAt_(m, locals, 4);
	double radius1 = IoMessage_locals_doubleArgAt_(m, locals, 5);
	
	IoObject_setDataPointer_(pattern, cairo_pattern_create_radial(cx0, cy0, radius0, cx1, cy1, radius1));
	CHECK_STATUS(self);
	return pattern;
}
