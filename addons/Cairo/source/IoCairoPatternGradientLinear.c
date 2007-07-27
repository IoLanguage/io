/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
		    */

#include "IoCairoPatternGradientLinear.h"
#include "IoCairoPatternGradient.h"
#include "IoCairoPattern.h"
#include "IoCairoPattern_inline.h"

#define DATA(self) ((IoCairoPatternData *)IoObject_dataPointer(self))


IoTag *IoCairoPatternGradientLinear_newTag(void *state)
{
	IoTag *tag = IoObject_tag(IoState_protoWithInitFunction_(state, IoCairoPattern_proto));
	IoTag_reference(tag);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoPatternGradientLinear_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoPatternGradientLinear_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCairoPatternGradientLinear_mark);
	return tag;
}

IoCairoPatternGradientLinear *IoCairoPatternGradientLinear_proto(void *state) 
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoCairoPatternGradient_proto);
	IoObject *self = IoObject_rawClone(proto);
	IoObject_tag_(self, IoCairoPatternGradientLinear_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCairoPatternData)));
	
	IoState_registerProtoWithFunc_(state, self, IoCairoPatternGradientLinear_proto);
	
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoPatternGradientLinear_create},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoPatternGradientLinear *IoCairoPatternGradientLinear_rawClone(IoCairoPatternGradientLinear *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoCairoPatternData)));
	if (PATTERN(proto)) {
		cairo_pattern_reference(PATTERN(proto));
	}
	
	return self;
}

/* ----------------------------------------------------------- */

void IoCairoPatternGradientLinear_free(IoCairoPatternGradientLinear *self) 
{
	if (PATTERN(self)) {
		cairo_pattern_destroy(PATTERN(self));
	}
	
	free(IoObject_dataPointer(self)); 
}

void IoCairoPatternGradientLinear_mark(IoCairoPatternGradientLinear *self) 
{
}

/* ----------------------------------------------------------- */

IoObject *IoCairoPatternGradientLinear_create(IoCairoPatternGradientLinear *self, IoObject *locals, IoMessage *m)
{
	IoObject *proto = IoState_protoWithInitFunction_(IOSTATE, IoCairoPatternGradientLinear_proto);
	IoObject *pattern = IOCLONE(proto);
	double x0 = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y0 = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double x1 = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double y1 = IoMessage_locals_doubleArgAt_(m, locals, 3);
	
	PATTERN(pattern) = cairo_pattern_create_linear(x0, y0, x1, y1);
	CHECK_STATUS(pattern);
	return pattern;
}
