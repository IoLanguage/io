/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
		    */

#include "IoCairoPatternGradientRadial.h"
#include "IoCairoPatternGradient.h"
#include "IoCairoPattern.h"
#include "IoCairoPattern_inline.h"

#define DATA(self) ((IoCairoPatternData *)IoObject_dataPointer(self))


IoTag *IoCairoPatternGradientRadial_newTag(void *state)
{
	IoTag *tag = IoObject_tag(IoState_protoWithInitFunction_(state, IoCairoPattern_proto));
	IoTag_reference(tag);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoPatternGradientRadial_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoPatternGradientRadial_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCairoPatternGradientRadial_mark);
	return tag;
}

IoCairoPatternGradientRadial *IoCairoPatternGradientRadial_proto(void *state) 
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoCairoPatternGradient_proto);
	IoObject *self = IoObject_rawClone(proto);
	IoObject_tag_(self, IoCairoPatternGradientRadial_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCairoPatternData)));
	
	IoState_registerProtoWithFunc_(state, self, IoCairoPatternGradientRadial_proto);
	
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoPatternGradientRadial_create},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoPatternGradientRadial *IoCairoPatternGradientRadial_rawClone(IoCairoPatternGradientRadial *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoCairoPatternData)));
	if (PATTERN(proto)) {
		cairo_pattern_reference(PATTERN(proto));
	}
	
	return self;
}

/* ----------------------------------------------------------- */

void IoCairoPatternGradientRadial_free(IoCairoPatternGradientRadial *self) 
{
	if (PATTERN(self)) {
		cairo_pattern_destroy(PATTERN(self));
	}
	
	free(IoObject_dataPointer(self)); 
}

void IoCairoPatternGradientRadial_mark(IoCairoPatternGradientRadial *self) 
{
}

/* ----------------------------------------------------------- */

IoObject *IoCairoPatternGradientRadial_create(IoCairoPatternGradientRadial *self, IoObject *locals, IoMessage *m)
{
	IoObject *proto = IoState_protoWithInitFunction_(IOSTATE, IoCairoPatternGradientRadial_proto);
	IoObject *pattern = IOCLONE(proto);
	double cx0 = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double cy0 = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double radius0 = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double cx1 = IoMessage_locals_doubleArgAt_(m, locals, 3);
	double cy1 = IoMessage_locals_doubleArgAt_(m, locals, 4);
	double radius1 = IoMessage_locals_doubleArgAt_(m, locals, 5);
	
	PATTERN(pattern) = cairo_pattern_create_radial(cx0, cy0, radius0, cx1, cy1, radius1);
	CHECK_STATUS(pattern);
	return pattern;
}
