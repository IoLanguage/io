/*#io
CairoSolidPattern ioDoc(
	docCopyright("Trevor Fancher", 2007)
	docCopyright("Daniel Rosengren", 2007)
	docLicense("BSD revised")
	docCategory("Graphics")
*/

#include "IoCairoSolidPattern.h"
#include "IoCairoPattern.h"
#include "IoCairoPattern_inline.h"

#define DATA(self) ((IoCairoPatternData *)IoObject_dataPointer(self))


IoTag *IoCairoSolidPattern_newTag(void *state)
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
	
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoSolidPattern_create},
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
	IoObject *proto = IoState_protoWithInitFunction_(IOSTATE, IoCairoSolidPattern_proto);
	IoObject *pattern = IOCLONE(proto);
	double r = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double g = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double b = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double a = 1.0;
	
	if (IoMessage_argCount(m) > 3)
		a = IoMessage_locals_doubleArgAt_(m, locals, 3);
	
	IoObject_setDataPointer_(pattern, cairo_pattern_create_rgba(r, g, b, a));
	CHECK_STATUS(pattern);
	return pattern;	
}
