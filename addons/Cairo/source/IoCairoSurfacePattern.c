/*#io
CairoSurfacePattern ioDoc(
	docCopyright("Daniel Rosengren", 2007)
	docLicense("BSD revised")
	docCategory("Graphics")
*/

#include "IoCairoSurfacePattern.h"
#include "IoCairoPattern.h"
#include "IoCairoPattern_inline.h"
#include "IoCairoSurface.h"
#include "IoNumber.h"
#include "tools.h"


static IoTag *IoCairoSurfacePattern_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("SurfacePattern");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoSurfacePattern_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoPattern_free);
	return tag;
}

IoCairoSurfacePattern *IoCairoSurfacePattern_proto(void *state) 
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoSurfacePattern_newTag(state));
	
	IoState_registerProtoWithFunc_(state, self, IoCairoSurfacePattern_proto);
	
	IoCairoPattern_addMethods(self);
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoSurfacePattern_create},
				
			{"getSurface", IoCairoSurfacePattern_getSurface},
				
			{"setExtend", IoCairoSurfacePattern_setExtend},
			{"getExtend", IoCairoSurfacePattern_getExtend},

			{"setFilter", IoCairoSurfacePattern_setFilter},
			{"getFilter", IoCairoSurfacePattern_getFilter},

			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoSurfacePattern *IoCairoSurfacePattern_rawClone(IoCairoSurfacePattern *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	if (PATTERN(proto))
		IoObject_setDataPointer_(self, cairo_pattern_reference(PATTERN(proto)));
	return self;
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoSurfacePattern_create(IoCairoSurfacePattern *self, IoObject *locals, IoMessage *m)
{
	IoCairoSurface *surface = IoMessage_locals_valueArgAt_(m, locals, 0);
	cairo_surface_t *rawSurface = IoCairoSurface_rawSurface(surface);
	return IoCairoPattern_newWithRawPattern_(IOSTATE, m, cairo_pattern_create_for_surface(rawSurface));
}


IoObject *IoCairoSurfacePattern_getSurface(IoCairoSurfacePattern *self, IoObject *locals, IoMessage *m)
{
	cairo_surface_t *surface = 0;
	cairo_pattern_get_surface(PATTERN(self), &surface);
	return IoCairoSurface_newWithRawSurface_(IOSTATE, m, cairo_surface_reference(surface));
}


IoObject *IoCairoSurfacePattern_setExtend(IoCairoSurfacePattern *self, IoObject *locals, IoMessage *m)
{
	cairo_pattern_set_extend(PATTERN(self), IoMessage_locals_intArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoSurfacePattern_getExtend(IoCairoSurfacePattern *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_pattern_get_extend(PATTERN(self)));
}


IoObject *IoCairoSurfacePattern_setFilter(IoCairoSurfacePattern *self, IoObject *locals, IoMessage *m)
{
	cairo_pattern_set_filter(PATTERN(self), IoMessage_locals_intArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoSurfacePattern_getFilter(IoCairoSurfacePattern *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_pattern_get_filter(PATTERN(self)));
}
