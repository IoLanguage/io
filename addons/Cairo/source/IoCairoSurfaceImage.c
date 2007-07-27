/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
		    */

#include "IoCairoSurfaceImage.h"

#define DATA(self) ((IoCairoSurfaceImageData *)IoObject_dataPointer(self))
#define SURFACE(srf) (DATA(srf)->surface)
#define STATUS(surface) (cairo_status_to_string(cairo_surface_status(surface)))

IoTag *IoCairoSurfaceImage_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoSurfaceImage");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoSurfaceImage_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoSurfaceImage_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCairoSurfaceImage_mark);
	return tag;
}

IoCairoSurfaceImage *IoCairoSurfaceImage_proto(void *state) 
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoSurfaceImage_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCairoSurfaceImageData)));	
	
	IoState_registerProtoWithFunc_(state, self, IoCairoSurfaceImage_proto);
	
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoSurfaceImage_create},
			{"writeToPNG", IoCairoSurfaceImage_writeToPNG},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoSurfaceImage *IoCairoSurfaceImage_rawClone(IoCairoSurfaceImage *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoCairoSurfaceImageData)));
	if (SURFACE(proto)) {
		cairo_surface_reference(SURFACE(proto));
	}
	
	return self;
}

/* ----------------------------------------------------------- */

IoCairoSurfaceImage *IoCairoSurfaceImage_newWithFormat_width_height_(void *state, cairo_format_t format, int width, int height)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoCairoSurfaceImage_proto);
	IoObject *self = IOCLONE(proto);
	
	DATA(self)->surface = cairo_image_surface_create(format, width, height);
	
	return self;
}

void IoCairoSurfaceImage_free(IoCairoSurfaceImage *self) 
{
	if (SURFACE(self)) {
		cairo_surface_destroy(SURFACE(self));
	}
		
	free(IoObject_dataPointer(self)); 
}

void IoCairoSurfaceImage_mark(IoCairoSurfaceImage *self) 
{
}

cairo_surface_t *IoCairoSurfaceImage_getRawSurface(IoCairoSurfaceImage *self)
{
	return SURFACE(self);
}

/* ----------------------------------------------------------- */

IoObject *IoCairoSurfaceImage_create(IoCairoSurfaceImage *self, IoObject *locals, IoMessage *m)
{
	cairo_format_t format = (cairo_format_t)IoMessage_locals_intArgAt_(m, locals, 0);
	int w = IoMessage_locals_intArgAt_(m, locals, 1);
	int h = IoMessage_locals_intArgAt_(m, locals, 2);
	
	return IoCairoSurfaceImage_newWithFormat_width_height_(IOSTATE, format, w, h);
}

IoObject *IoCairoSurfaceImage_writeToPNG(IoCairoSurfaceImage *self, IoObject *locals, IoMessage *m)
{
	char *filename = CSTRING(IoMessage_locals_valueArgAt_(m, locals, 0));
	
	cairo_surface_write_to_png(SURFACE(self), filename);
	return self;	
}
