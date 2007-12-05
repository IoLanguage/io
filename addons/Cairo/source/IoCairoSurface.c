/*#io
CairoSurface ioDoc(
	docCopyright("Daniel Rosengren", 2007)
	docLicense("BSD revised")
	docCategory("Graphics")
*/

#include "IoCairoSurface.h"
#include "IoCairoSurface_inline.h"
#include "IoCairoFontOptions.h"
#include "IoNumber.h"
#include "tools.h"

#include "IoCairoImageSurface.h"
#include "IoCairoPSSurface.h"
#include "IoCairoPDFSurface.h"
#include "IoCairoSVGSurface.h"


static IoTag *IoCairoSurface_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoSurface");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoSurface_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoSurface_free);
	return tag;
}

IoCairoSurface *IoCairoSurface_proto(void *state) 
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoSurface_newTag(state));
	
	IoState_registerProtoWithFunc_(state, self, IoCairoSurface_proto);
	
	IoCairoSurface_addMethods(self);
	
	return self;
}

void IoCairoSurface_addMethods(IoCairoSurface *self)
{
	IoMethodTable methodTable[] = {
		{"createSimilar", IoCairoSurface_createSimilar},

		{"finish", IoCairoSurface_finish},
		{"flush", IoCairoSurface_flush},

		{"getFontOptions", IoCairoSurface_getFontOptions},
		{"getContent", IoCairoSurface_getContent},

		{"markDirty", IoCairoSurface_markDirty},
		{"markDirtyRectangle", IoCairoSurface_markDirtyRectangle},

		{"setDeviceOffset", IoCairoSurface_setDeviceOffset},
		{"getDeviceOffset", IoCairoSurface_getDeviceOffset},

		{"setFallbackResolution", IoCairoSurface_setFallbackResolution},

		{NULL, NULL},
	};
	IoObject_addMethodTable_(self, methodTable);
}

IoCairoSurface *IoCairoSurface_rawClone(IoCairoSurface *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	if (SURFACE(proto))
		IoObject_setDataPointer_(self, cairo_surface_reference(SURFACE(proto)));
	return self;
}

IoCairoSurface *IoCairoSurface_newWithRawSurface_(void *state, IoMessage *m, cairo_surface_t *surface)
{
	IoObject *self = 0;
	IoStateProtoFunc *initFunc = 0;
	
	checkStatus_(state, m, cairo_surface_status(surface)); 

	switch(cairo_surface_get_type(surface))
	{
		case CAIRO_SURFACE_TYPE_IMAGE:
			initFunc = IoCairoImageSurface_proto;
			break;
		case CAIRO_SURFACE_TYPE_PS:
			initFunc = IoCairoPSSurface_proto;
			break;
		case CAIRO_SURFACE_TYPE_PDF:
			initFunc = IoCairoPDFSurface_proto;
			break;
		case CAIRO_SURFACE_TYPE_SVG:
			initFunc = IoCairoSVGSurface_proto;
			break;
	default:
			IoState_error_(state, 0, "Unsupported surface type");
	}

	self = IOCLONE(IoState_protoWithInitFunction_(state, initFunc));
	IoObject_setDataPointer_(self, surface);
	return self;
}

void IoCairoSurface_free(IoCairoSurface *self) 
{
	if (SURFACE(self))
		cairo_surface_destroy(SURFACE(self));
}

cairo_surface_t *IoCairoSurface_rawSurface(IoCairoSurface *self)
{
	return SURFACE(self);
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoSurface_createSimilar(IoCairoSurface *self, IoObject *locals, IoMessage *m)
{
	cairo_content_t content = IoMessage_locals_intArgAt_(m, locals, 0);
	int w = IoMessage_locals_intArgAt_(m, locals, 1);
	int h = IoMessage_locals_intArgAt_(m, locals, 2);

	cairo_surface_t *surface = cairo_surface_create_similar(SURFACE(self), content, w, h);
	return IoCairoSurface_newWithRawSurface_(IOSTATE, m, surface);
}


IoObject *IoCairoSurface_finish(IoCairoSurface *self, IoObject *locals, IoMessage *m)
{
	cairo_surface_finish(SURFACE(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoSurface_flush(IoCairoSurface *self, IoObject *locals, IoMessage *m)
{
	cairo_surface_flush(SURFACE(self));
	CHECK_STATUS(self);
	return self;
}


IoObject *IoCairoSurface_getFontOptions(IoCairoSurface *self, IoObject *locals, IoMessage *m)
{
	cairo_font_options_t *options = cairo_font_options_create();
	cairo_surface_get_font_options(SURFACE(self), options);
	return IoCairoFontOptions_newWithRawFontOptions_(IOSTATE, m, options);
}

IoObject *IoCairoSurface_getContent(IoCairoSurface *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_surface_get_content(SURFACE(self)));
}


IoObject *IoCairoSurface_markDirty(IoCairoSurface *self, IoObject *locals, IoMessage *m)
{
	cairo_surface_mark_dirty(SURFACE(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoSurface_markDirtyRectangle(IoCairoSurface *self, IoObject *locals, IoMessage *m)
{
	double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double w = IoMessage_locals_doubleArgAt_(m, locals, 2);
	double h = IoMessage_locals_doubleArgAt_(m, locals, 3);
	
	cairo_surface_mark_dirty_rectangle(SURFACE(self), x, y, w, h);
	CHECK_STATUS(self);
	return self;
}


IoObject *IoCairoSurface_setDeviceOffset(IoCairoSurface *self, IoObject *locals, IoMessage *m)
{
	double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y = IoMessage_locals_doubleArgAt_(m, locals, 1);

	cairo_surface_set_device_offset(SURFACE(self), x, y);
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoSurface_getDeviceOffset(IoCairoSurface *self, IoObject *locals, IoMessage *m)
{
	double x = 0, y = 0;

	cairo_surface_get_device_offset(SURFACE(self), &x, &y);
	CHECK_STATUS(self);
	return IoSeq_newWithX_y_(IOSTATE, x, y);
}


IoObject *IoCairoSurface_setFallbackResolution(IoCairoSurface *self, IoObject *locals, IoMessage *m)
{
	double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y = IoMessage_locals_doubleArgAt_(m, locals, 1);

	cairo_surface_set_fallback_resolution(SURFACE(self), x, y);
	CHECK_STATUS(self);
	return self;
}
