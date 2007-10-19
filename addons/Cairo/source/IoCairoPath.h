/*
docCopyright("Daniel Rosengren", 2007)
docLicense("BSD revised")
*/

#ifndef IOCAIROPATH_DEFINED
#define IOCAIROPATH_DEFINED 1

#include "IoObject.h"
#include "IoList.h"
#include <cairo.h>

#define ISCAIROPATH(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoPath_rawClone)
void *IoMessage_locals_cairoPathArgAt_(IoMessage *self, void *locals, int n);

typedef IoObject IoCairoPath;

typedef struct
{
	cairo_path_t *path;
	int refCount;
} IoCairoPathData;

IoCairoPath *IoCairoPath_proto(void *state);
IoCairoPath *IoCairoPath_rawClone(IoCairoPath *proto);
IoCairoPath *IoCairoPath_newWithRawPath_(void *state, cairo_path_t *path);
void IoCairoPath_free(IoCairoPath *self);

cairo_path_t *IoCairoPath_rawPath(IoCairoPath *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoPath_foreach(IoCairoPath *self, IoObject *locals, IoMessage *m);

#endif
