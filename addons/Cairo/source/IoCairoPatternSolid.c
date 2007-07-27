/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
		    */

#include "IoCairoPatternSolid.h"
#include "IoCairoPattern.h"
#include "IoCairoPattern_inline.h"

#define DATA(self) ((IoCairoPatternData *)IoObject_dataPointer(self))


IoTag *IoCairoPatternSolid_newTag(void *state)
{
	IoTag *tag = IoObject_tag(IoState_protoWithInitFunction_(state, IoCairoPattern_proto));
	IoTag_reference(tag);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoPatternSolid_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoPatternSolid_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCairoPatternSolid_mark);
	return tag;
}

IoCairoPatternSolid *IoCairoPatternSolid_proto(void *state) 
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoCairoPattern_proto);
	IoObject *self = IoObject_rawClone(proto);
	IoObject_tag_(self, IoCairoPatternSolid_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCairoPatternData)));
	
	IoState_registerProtoWithFunc_(state, self, IoCairoPatternSolid_proto);
	
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoPatternSolid_create},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoPatternSolid *IoCairoPatternSolid_rawClone(IoCairoPatternSolid *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoCairoPatternData)));
	if (PATTERN(proto)) {
		cairo_pattern_reference(PATTERN(proto));
	}
	
	return self;
}

/* ----------------------------------------------------------- */

void IoCairoPatternSolid_free(IoCairoPatternSolid *self) 
{
	if (PATTERN(self)) {
		cairo_pattern_destroy(PATTERN(self));
	}
	
	free(IoObject_dataPointer(self)); 
}

void IoCairoPatternSolid_mark(IoCairoPatternSolid *self) 
{
}

cairo_pattern_t *IoCairoPatternSolid_getRawPatternSolid(IoCairoPatternSolid *self)
{
	return PATTERN(self);
}

/* ----------------------------------------------------------- */

IoObject *IoCairoPatternSolid_create(IoCairoPatternSolid *self, IoObject *locals, IoMessage *m)
{
	IoObject *proto = IoState_protoWithInitFunction_(IOSTATE, IoCairoPatternSolid_proto);
	IoObject *pattern = IOCLONE(proto);
	double r = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double g = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double b = IoMessage_locals_doubleArgAt_(m, locals, 2);
	
	int hasAlpha = IoMessage_argCount(m) > 3;
	if (hasAlpha) {
		double a = IoMessage_locals_doubleArgAt_(m, locals, 3);
		PATTERN(pattern) = cairo_pattern_create_rgba(r, g, b, a);
	} else {
		PATTERN(pattern) = cairo_pattern_create_rgb(r, g, b);
	}
	
	CHECK_STATUS(pattern);
	return pattern;	
}
