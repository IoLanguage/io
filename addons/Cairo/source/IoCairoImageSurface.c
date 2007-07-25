/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
		    */

#include "IoCairoImageSurface.h"

#define DATA(self) ((IoCairoImageSurfaceData *)IoObject_dataPointer(self))
#define SURFACE(srf) (DATA(srf)->surface)
#define STATUS(surface) (cairo_status_to_string(cairo_surface_status(surface)))

IoTag *IoCairoImageSurface_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoImageSurface");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoImageSurface_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoImageSurface_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCairoImageSurface_mark);
	return tag;
}

IoCairoImageSurface *IoCairoImageSurface_proto(void *state) 
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoImageSurface_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCairoImageSurfaceData)));	
	
	IoState_registerProtoWithFunc_(state, self, IoCairoImageSurface_proto);
	
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoImageSurface_create},
			{"writeToPNG", IoCairoImageSurface_writeToPNG},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoImageSurface *IoCairoImageSurface_rawClone(IoCairoImageSurface *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoCairoImageSurfaceData)));
	if (SURFACE(proto)) {
		cairo_surface_reference(SURFACE(proto));
	}
	
	return self;
}

/* ----------------------------------------------------------- */

IoCairoImageSurface *IoCairoImageSurface_newWithFormat_width_height_(void *state, cairo_format_t format, int width, int height)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoCairoImageSurface_proto);
	IoObject *self = IOCLONE(proto);
	
	DATA(self)->surface = cairo_image_surface_create(format, width, height);
	
	return self;
}

void IoCairoImageSurface_free(IoCairoImageSurface *self) 
{
	if (SURFACE(self)) {
		cairo_surface_destroy(SURFACE(self));
	}
		
	free(IoObject_dataPointer(self)); 
}

void IoCairoImageSurface_mark(IoCairoImageSurface *self) 
{
}

cairo_surface_t *IoCairoImageSurface_getRawSurface(IoCairoImageSurface *self)
{
	return SURFACE(self);
}

/* ----------------------------------------------------------- */

IoObject *IoCairoImageSurface_create(IoCairoImageSurface *self, IoObject *locals, IoMessage *m)
{
	cairo_format_t format = (cairo_format_t)IoMessage_locals_intArgAt_(m, locals, 0);
	int w = IoMessage_locals_intArgAt_(m, locals, 1);
	int h = IoMessage_locals_intArgAt_(m, locals, 2);
	
	return IoCairoImageSurface_newWithFormat_width_height_(IOSTATE, format, w, h);
}

IoObject *IoCairoImageSurface_writeToPNG(IoCairoImageSurface *self, IoObject *locals, IoMessage *m)
{
	char *filename = CSTRING(IoMessage_locals_valueArgAt_(m, locals, 0));
	
	cairo_surface_write_to_png(SURFACE(self), filename);
	return self;	
}
