/*#io
CairoLinearGradient ioDoc(
	docCopyright("Trevor Fancher", 2007)
	docCopyright("Daniel Rosengren", 2007)
	docLicense("BSD revised")
	docCategory("Graphics")
*/

#include "IoCairoLinearGradient.h"
#include "IoCairoGradient.h"
#include "IoCairoPattern.h"
#include "IoCairoPattern_inline.h"


IoTag *IoCairoLinearGradient_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("LinearGradient");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoLinearGradient_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoPattern_free);
	return tag;
}

IoCairoLinearGradient *IoCairoLinearGradient_proto(void *state) 
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoLinearGradient_newTag(state));
	
	IoState_registerProtoWithFunc_(state, self, IoCairoLinearGradient_proto);
	
	IoCairoGradient_addMethods(self);
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoLinearGradient_create},
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

 
/* ----------------------------------------------------------- */

IoObject *IoCairoLinearGradient_create(IoCairoLinearGradient *self, IoObject *locals, IoMessage *m)
{
	IoObject *proto = IoState_protoWithInitFunction_(IOSTATE, IoCairoLinearGradient_proto);
	IoObject *pattern = IOCLONE(proto);
	double x0 = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y0 = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double x1 = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double y1 = IoMessage_locals_doubleArgAt_(m, locals, 3);
	
	IoObject_setDataPointer_(pattern, cairo_pattern_create_linear(x0, y0, x1, y1));
	CHECK_STATUS(pattern);
	return pattern;
}
