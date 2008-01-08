/*#io
CairoImageSurface ioDoc(
	docCopyright("Trevor Fancher", 2007)
	docCopyright("Daniel Rosengren", 2007)
	docLicense("BSD revised")
	docCategory("Graphics")
*/

#include "IoCairoImageSurface.h"
#include "IoCairoSurface.h"
#include "IoCairoSurface_inline.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "tools.h"

#define DATA_SEQ(self) cairo_surface_get_user_data(SURFACE(self), &dataKey)

static const cairo_user_data_key_t dataKey;


static IoTag *IoCairoImageSurface_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoImageSurface");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoImageSurface_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoSurface_free);
	return tag;
}

IoCairoImageSurface *IoCairoImageSurface_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoImageSurface_newTag(state));

	IoState_registerProtoWithFunc_(state, self, IoCairoImageSurface_proto);

	IoCairoSurface_addMethods(self);
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoImageSurface_create},
			{"createForData", IoCairoImageSurface_createForData},
			#if CAIRO_HAS_PNG_FUNCTIONS
			{"createFromPNG", IoCairoImageSurface_createFromPNG},
			#endif

			{"getFormat", IoCairoImageSurface_getFormat},
			{"getWidth", IoCairoImageSurface_getWidth},
			{"getHeight", IoCairoImageSurface_getHeight},
			{"getStride", IoCairoImageSurface_getStride},
			{"getData", IoCairoImageSurface_getData},

			#if CAIRO_HAS_PNG_FUNCTIONS
			{"writeToPNG", IoCairoImageSurface_writeToPNG},
			#endif

			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoImageSurface *IoCairoImageSurface_rawClone(IoCairoImageSurface *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	if (SURFACE(proto))
		IoObject_setDataPointer_(self, cairo_surface_reference(SURFACE(proto)));
	return self;
}

void IoCairoImageSurface_mark(IoCairoImageSurface *self)
{
	IoSeq *data = DATA_SEQ(self);
	if (data)
		IoObject_shouldMark(data);
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoImageSurface_create(IoCairoImageSurface *self, IoObject *locals, IoMessage *m)
{
	cairo_format_t format = (cairo_format_t)IoMessage_locals_intArgAt_(m, locals, 0);
	int w = IoMessage_locals_intArgAt_(m, locals, 1);
	int h = IoMessage_locals_intArgAt_(m, locals, 2);
	return IoCairoSurface_newWithRawSurface_(IOSTATE, m, cairo_image_surface_create(format, w, h));
}

IoObject *IoCairoImageSurface_createForData(IoCairoImageSurface *self, IoObject *locals, IoMessage *m)
{
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 0);
	cairo_format_t format = (cairo_format_t)IoMessage_locals_intArgAt_(m, locals, 1);
	int w = IoMessage_locals_intArgAt_(m, locals, 2);
	int h = IoMessage_locals_intArgAt_(m, locals, 3);
	int stride = IoMessage_locals_intArgAt_(m, locals, stride);
	cairo_surface_t *surface = cairo_image_surface_create_for_data(IoSeq_rawBytes(data), format, w, h, stride);

	IoCairoImageSurface *new = IoCairoSurface_newWithRawSurface_(IOSTATE, m, surface);
	cairo_surface_set_user_data(SURFACE(new), &dataKey, data, 0);
	CHECK_STATUS(self);
	return new;
}

#if CAIRO_HAS_PNG_FUNCTIONS
IoObject *IoCairoImageSurface_createFromPNG(IoCairoImageSurface *self, IoObject *locals, IoMessage *m)
{
	char *filename = CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0));
	return IoCairoSurface_newWithRawSurface_(IOSTATE, m, cairo_image_surface_create_from_png(filename));
}
#endif


IoObject *IoCairoImageSurface_getFormat(IoCairoImageSurface *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_image_surface_get_format(SURFACE(self)));
}

IoObject *IoCairoImageSurface_getWidth(IoCairoImageSurface *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_image_surface_get_width(SURFACE(self)));
}

IoObject *IoCairoImageSurface_getHeight(IoCairoImageSurface *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_image_surface_get_height(SURFACE(self)));
}

IoObject *IoCairoImageSurface_getStride(IoCairoImageSurface *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(cairo_image_surface_get_stride(SURFACE(self)));
}

IoObject *IoCairoImageSurface_getData(IoCairoImageSurface *self, IoObject *locals, IoMessage *m)
{
	int h = 0, stride = 0;

	IoSeq *data = DATA_SEQ(self);
	if (data)
		return data;

	h = cairo_image_surface_get_height(SURFACE(self));
	stride = cairo_image_surface_get_stride(SURFACE(self));
	return IoSeq_newWithData_length_(IOSTATE, cairo_image_surface_get_data(SURFACE(self)), h * stride);
}


#if CAIRO_HAS_PNG_FUNCTIONS
IoObject *IoCairoImageSurface_writeToPNG(IoCairoImageSurface *self, IoObject *locals, IoMessage *m)
{
	char *filename = CSTRING(IoMessage_locals_valueArgAt_(m, locals, 0));

	cairo_surface_write_to_png(SURFACE(self), filename);
	CHECK_STATUS(self);
	return self;
}
#endif
