/*
docCopyright("Daniel Rosengren", 2007)
docLicense("BSD revised")
*/

#ifndef IOCAIROPATHELEMENT_DEFINED
#define IOCAIROPATHELEMENT_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

typedef IoObject IoCairoPathElement;

typedef struct
{
	IoObject *path;
	cairo_path_data_t *pathData;
} IoCairoPathElementData;

IoCairoPathElement *IoCairoPathElement_proto(void *state);
IoCairoPathElement *IoCairoPathElement_rawClone(IoCairoPathElement *proto);
IoCairoPathElement *IoCairoPathElement_newWithPath_dataOffset_(void *state, IoObject *path, int offset);
void IoCairoPathElement_free(IoCairoPathElement *self);
void IoCairoPathElement_mark(IoCairoPathElement *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoPathElement_kind(IoCairoPathElement *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoPathElement_pointAt(IoCairoPathElement *self, IoObject *locals, IoMessage *m);

#endif
