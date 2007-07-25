/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
		    */

#include "IoCairoPattern.h"

#define DATA(self) ((IoCairoPatternData *)IoObject_dataPointer(self))
#define PATTERN(ctx) (DATA(ctx)->pattern)

#define STATUS(cairo_type) (cairo_status_to_string(cairo_pattern_status(cairo_type)))
#define CHECK_STATUS(obj) \
				cairo_status_t status = cairo_status(PATTERN(obj)); \
				if (status != CAIRO_STATUS_SUCCESS) { \
					IoState_error_(IOSTATE, m, "%s: cairo: %s", __func__, STATUS(PATTERN(obj))); \
				}


IoTag *IoCairoPattern_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoPattern");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoPattern_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoPattern_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCairoPattern_mark);
	return tag;
}

IoCairoPattern *IoCairoPattern_proto(void *state) 
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoPattern_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCairoPatternData)));
	
	IoState_registerProtoWithFunc_(state, self, IoCairoPattern_proto);
	
	{
		IoMethodTable methodTable[] = {
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoPattern *IoCairoPattern_rawClone(IoCairoPattern *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoCairoPatternData)));
	if (PATTERN(proto)) {
		cairo_pattern_reference(PATTERN(proto));
	}
	
	return self;
}

/* ----------------------------------------------------------- */

void IoCairoPattern_free(IoCairoPattern *self) 
{
	if (PATTERN(self)) {
		cairo_pattern_destroy(PATTERN(self));
	}
	
	free(IoObject_dataPointer(self)); 
}

void IoCairoPattern_mark(IoCairoPattern *self) 
{
}

cairo_pattern_t *IoCairoPattern_getRawPattern(IoCairoPattern *self)
{
	return PATTERN(self);
}

/* ----------------------------------------------------------- */
