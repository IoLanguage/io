/*#io
CairoPDFSurface ioDoc(
	docCopyright("Daniel Rosengren", 2007)
	docLicense("BSD revised")
	docCategory("Graphics")
*/

#include "IoCairoPDFSurface.h"

#if CAIRO_HAS_PDF_SURFACE
#include "IoCairoSurface.h"
#include "IoCairoSurface_inline.h"
#include <cairo-pdf.h>


static IoTag *IoCairoPDFSurface_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoPDFSurface");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoPDFSurface_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoSurface_free);
	return tag;
}

IoCairoPDFSurface *IoCairoPDFSurface_proto(void *state) 
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoPDFSurface_newTag(state));
	
	IoState_registerProtoWithFunc_(state, self, IoCairoPDFSurface_proto);

	IoCairoSurface_addMethods(self);
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoPDFSurface_create},
			{"setSize", IoCairoPDFSurface_setSize},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoPDFSurface *IoCairoPDFSurface_rawClone(IoCairoPDFSurface *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	if (SURFACE(proto))
		IoObject_setDataPointer_(self, cairo_surface_reference(SURFACE(proto)));
	return self;
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoPDFSurface_create(IoCairoPDFSurface *self, IoObject *locals, IoMessage *m)
{
	char *filename = CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0));
	double w = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double h = IoMessage_locals_doubleArgAt_(m, locals, 2);

	return IoCairoSurface_newWithRawSurface_(IOSTATE, m, cairo_pdf_surface_create(filename, w, h));
}

IoObject *IoCairoPDFSurface_setSize(IoCairoPDFSurface *self, IoObject *locals, IoMessage *m)
{
	double w = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double h = IoMessage_locals_doubleArgAt_(m, locals, 1);
	
	cairo_ps_surface_set_size(SURFACE(self), w, h);
	CHECK_STATUS(self);
	return self;
}

#endif
