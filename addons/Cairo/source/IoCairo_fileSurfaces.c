#include "IoCairoPDFSurface.h"
#include "IoCairoPSSurface.h"
#include "IoCairoSVGSurface.h"

void IoCairo_fileSurfacesInit(IoObject *context)
{
	IoState *self = IoObject_state((IoObject *)context);

	#if CAIRO_HAS_PDF_SURFACE
	IoObject_setSlot_to_(context, SIOSYMBOL("CairoPDFSurface"), IoCairoPDFSurface_proto(self));
	#endif

	#if CAIRO_HAS_PS_SURFACE
	IoObject_setSlot_to_(context, SIOSYMBOL("CairoPSSurface"), IoCairoPSSurface_proto(self));
	#endif

	#if CAIRO_HAS_SVG_SURFACE
	IoObject_setSlot_to_(context, SIOSYMBOL("CairoSVGSurface"), IoCairoSVGSurface_proto(self));
	#endif
}
