//metadoc CairoPSSurface copyright Daniel Rosengren, 2007
//metadoc CairoPSSurface license BSD revised
//metadoc CairoPSSurface category Graphics

#include "IoCairoPSSurface.h"

#if CAIRO_HAS_PS_SURFACE
#include "IoCairoSurface.h"
#include "IoCairoSurface_inline.h"
#include <cairo-ps.h>

static const char *protoId = "CairoPSSurface";

static IoTag *IoCairoPSSurface_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoPSSurface_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoSurface_free);
	return tag;
}

IoCairoPSSurface *IoCairoPSSurface_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoPSSurface_newTag(state));

	IoState_registerProtoWithId_(state, self, protoId);

	IoCairoSurface_addMethods(self);
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoPSSurface_create},

			{"setSize", IoCairoPSSurface_setSize},
			{"dscBeginSetup", IoCairoPSSurface_dscBeginSetup},
			{"dscBeginPageSetup", IoCairoPSSurface_dscBeginPageSetup},
			{"dscComment", IoCairoPSSurface_dscComment},

			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoPSSurface *IoCairoPSSurface_rawClone(IoCairoPSSurface *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	if (SURFACE(proto))
		IoObject_setDataPointer_(self, cairo_surface_reference(SURFACE(proto)));
	return self;
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoPSSurface_create(IoCairoPSSurface *self, IoObject *locals, IoMessage *m)
{
	char *filename = CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0));
	double w = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double h = IoMessage_locals_doubleArgAt_(m, locals, 2);

	return IoCairoSurface_newWithRawSurface_(IOSTATE, m, cairo_ps_surface_create(filename, w, h));
}


IoObject *IoCairoPSSurface_setSize(IoCairoPSSurface *self, IoObject *locals, IoMessage *m)
{
	double w = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double h = IoMessage_locals_doubleArgAt_(m, locals, 1);

	cairo_ps_surface_set_size(SURFACE(self), w, h);
	CHECK_STATUS(self);
	return self;
}


IoObject *IoCairoPSSurface_dscBeginSetup(IoCairoPSSurface *self, IoObject *locals, IoMessage *m)
{
	cairo_ps_surface_dsc_begin_setup(SURFACE(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoPSSurface_dscBeginPageSetup(IoCairoPSSurface *self, IoObject *locals, IoMessage *m)
{
	cairo_ps_surface_dsc_begin_page_setup(SURFACE(self));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoPSSurface_dscComment(IoCairoPSSurface *self, IoObject *locals, IoMessage *m)
{
	char *comment = CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0));

	cairo_ps_surface_dsc_comment(SURFACE(self), comment);
	CHECK_STATUS(self);
	return self;
}

#endif
