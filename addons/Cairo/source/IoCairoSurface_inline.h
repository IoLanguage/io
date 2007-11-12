#ifndef IOCAIROSURFACE_INLINE
#define IOCAIROSURFACE_INLINE 1

#include "tools.h"

#define SURFACE(self) ((cairo_surface_t *)IoObject_dataPointer(self))
#define CHECK_STATUS(self) checkStatus_(IOSTATE, m, cairo_surface_status(SURFACE(self)))

#endif
