/*#io
CairoPattern ioDoc(
	docCopyright("Trevor Fancher", 2007)
	docCopyright("Daniel Rosengren", 2007)
	docLicense("BSD revised")
	docCategory("Graphics")
*/

#include "IoCairoPattern.h"
#include "IoCairoPattern_inline.h"

#define DATA(self) ((IoCairoPatternData *)IoObject_dataPointer(self))

IoTag *IoCairoPattern_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoPattern");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoPattern_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoPattern_free);
	return tag;
}

IoCairoPattern *IoCairoPattern_proto(void *state) 
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoPattern_newTag(state));
	
	IoState_registerProtoWithFunc_(state, self, IoCairoPattern_proto);
	
	IoCairoPattern_addMethods(self);
	return self;
}

void IoCairoPattern_addMethods(IoCairoPattern *self)
{
	IoMethodTable methodTable[] = {
		//{"create", IoCairoPattern_create},
		{NULL, NULL},
	};
	IoObject_addMethodTable_(self, methodTable);
}

IoCairoPattern *IoCairoPattern_rawClone(IoCairoPattern *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	if (PATTERN(proto))
		IoObject_setDataPointer_(self, cairo_pattern_reference(PATTERN(proto)));
	return self;
}

void IoCairoPattern_free(IoCairoPattern *self) 
{
	if (PATTERN(self))
		cairo_pattern_destroy(PATTERN(self));
}

cairo_pattern_t *IoCairoPattern_rawPattern(IoCairoPattern *self)
{
	return PATTERN(self);
}

/* ----------------------------------------------------------- */
