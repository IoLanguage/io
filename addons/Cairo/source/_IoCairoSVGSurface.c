/*#io
CairoSVGSurface ioDoc(
	docCopyright("Daniel Rosengren", 2007)
	docLicense("BSD revised")
	docCategory("Graphics")
*/

#include "IoCairoSVGSurface.h"

#if CAIRO_HAS_SVG_SURFACE
#include "IoCairoSurface.h"
#include "IoCairoSurface_inline.h"
#include "IoNumber.h"
#include "IoList.h"
#include <cairo-svg.h>


static IoTag *IoCairoSVGSurface_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoSVGSurface");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoSVGSurface_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoSurface_free);
	return tag;
}

IoCairoSVGSurface *IoCairoSVGSurface_proto(void *state) 
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoSVGSurface_newTag(state));
	
	IoState_registerProtoWithFunc_(state, self, IoCairoSVGSurface_proto);

	IoCairoSurface_addMethods(self);
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoSVGSurface_create},

			{"restrictToVersion", IoCairoSVGSurface_restrictToVersion},
			{"getVersions", IoCairoSVGSurface_getVersions},
			{"versionToString", IoCairoSVGSurface_versionToString},

			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoSVGSurface *IoCairoSVGSurface_rawClone(IoCairoSVGSurface *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	if (SURFACE(proto))
		IoObject_setDataPointer_(self, cairo_surface_reference(SURFACE(proto)));
	return self;
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoSVGSurface_create(IoCairoSVGSurface *self, IoObject *locals, IoMessage *m)
{
	char *filename = CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0));
	double w = IoMessage_locals_doubleArgAt_(m, locals, 1);
	double h = IoMessage_locals_doubleArgAt_(m, locals, 2);

	return IoCairoSurface_newWithRawSurface_(IOSTATE, m, cairo_svg_surface_create(filename, w, h));
}


IoObject *IoCairoSVGSurface_restrictToVersion(IoCairoSVGSurface *self, IoObject *locals, IoMessage *m)
{
	cairo_svg_surface_restrict_to_version(SURFACE(self), IoMessage_locals_intArgAt_(m, locals, 0));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoSVGSurface_getVersions(IoCairoSVGSurface *self, IoObject *locals, IoMessage *m)
{
	IoList *versionList = IoList_new(IOSTATE);
	const cairo_svg_version_t *versions = 0;
	int versionCount = 0;
	int i = 0;
	
	cairo_svg_get_versions(&versions, &versionCount);
	for (i = 0; i < versionCount; i++)
		IoList_rawAppend_(versionList, IONUMBER(versions[i]));
	return versionList;
}

IoObject *IoCairoSVGSurface_versionToString(IoCairoSVGSurface *self, IoObject *locals, IoMessage *m)
{
	cairo_svg_version_t version = IoMessage_locals_intArgAt_(m, locals, 0);
	return IOSYMBOL(cairo_svg_version_to_string(version));
}

#endif
