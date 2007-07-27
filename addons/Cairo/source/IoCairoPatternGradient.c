/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
		    */

#include "IoCairoPatternGradient.h"
#include "IoCairoPattern.h"
#include "IoCairoPattern_inline.h"

#define DATA(self) ((IoCairoPatternData *)IoObject_dataPointer(self))


IoTag *IoCairoPatternGradient_newTag(void *state)
{
	IoTag *tag = IoObject_tag(IoState_protoWithInitFunction_(state, IoCairoPattern_proto));
	IoTag_reference(tag);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoPatternGradient_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoPatternGradient_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCairoPatternGradient_mark);
	return tag;
}

IoCairoPatternGradient *IoCairoPatternGradient_proto(void *state) 
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoCairoPattern_proto);
	IoObject *self = IoObject_rawClone(proto);
	IoObject_tag_(self, IoCairoPatternGradient_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCairoPatternData)));
	
	IoState_registerProtoWithFunc_(state, self, IoCairoPatternGradient_proto);
	
	{
		IoMethodTable methodTable[] = {
			{"addColorStopRGB", IoCairoPatternGradient_addColorStopRGB},
			{"addColorStopRGBA", IoCairoPatternGradient_addColorStopRGBA},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoPatternGradient *IoCairoPatternGradient_rawClone(IoCairoPatternGradient *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoCairoPatternData)));
	if (PATTERN(proto)) {
		cairo_pattern_reference(PATTERN(proto));
	}
	
	return self;
}

/* ----------------------------------------------------------- */

void IoCairoPatternGradient_free(IoCairoPatternGradient *self) 
{
	if (PATTERN(self)) {
		cairo_pattern_destroy(PATTERN(self));
	}
	
	free(IoObject_dataPointer(self)); 
}

void IoCairoPatternGradient_mark(IoCairoPatternGradient *self) 
{
}

/* ----------------------------------------------------------- */

IoObject *IoCairoPatternGradient_addColorStopRGB(IoCairoPatternGradient *self, IoObject *locals, IoMessage *m)
{
	double o = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double r = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double g = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double b = IoMessage_locals_doubleArgAt_(m, locals, 3);
	
	cairo_pattern_add_color_stop_rgb(PATTERN(self), o, r, g, b);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoPatternGradient_addColorStopRGBA(IoCairoPatternGradient *self, IoObject *locals, IoMessage *m)
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
