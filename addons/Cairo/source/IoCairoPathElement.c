//metadoc CairoPathElement copyright Daniel Rosengren, 2007
//metadoc CairoPathElement license BSD revised
//metadoc CairoPathElement category Graphics

#include "IoCairoPathElement.h"
#include "IoCairoPath.h"
#include "IoNumber.h"
#include "tools.h"
#include <stdlib.h>

#define DATA(self) ((IoCairoPathElementData *)IoObject_dataPointer(self))
#define PATH_DATA(self) (DATA(self)->pathData)

static int IoCairoPathElement_pointCount(IoCairoPathElement *self);


static IoTag *IoCairoPathElement_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoPathElement");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoPathElement_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoPathElement_free);
	IoTag_markFunc_(tag, (IoTagFreeFunc *)IoCairoPathElement_mark);
	return tag;
}

IoCairoPathElement *IoCairoPathElement_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoPathElement_newTag(state));

	IoState_registerProtoWithFunc_(state, self, IoCairoPathElement_proto);

	{
		IoMethodTable methodTable[] = {
			{"kind", IoCairoPathElement_kind},
			{"pointAt", IoCairoPathElement_pointAt},

			{NULL, NULL},
		};

		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoCairoPathElement *IoCairoPathElement_rawClone(IoCairoPathElement *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	return self;
}

IoCairoPathElement *IoCairoPathElement_newWithPath_dataOffset_(void *state, IoObject *path, int offset)
{
	IoCairoPathElement *self = IOCLONE(IoState_protoWithInitFunction_(state, IoCairoPathElement_proto));
	cairo_path_t *rawPath = ((IoCairoPathData *)IoObject_dataPointer(path))->path;

	IoObject_setDataPointer_(self, malloc(sizeof(IoCairoPathElementData)));
	DATA(self)->path = path;
	PATH_DATA(self) = rawPath->data + offset;
	return self;
}

void IoCairoPathElement_free(IoCairoPathElement *self)
{
	if (DATA(self))
		free(DATA(self));
}

void IoCairoPathElement_mark(IoCairoPathElement *self)
{
	if (DATA(self))
		IoObject_shouldMark(DATA(self)->path);
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoPathElement_kind(IoCairoPathElement *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(PATH_DATA(self)->header.type);
}

IoObject *IoCairoPathElement_pointAt(IoCairoPathElement *self, IoObject *locals, IoMessage *m)
{
	cairo_path_data_t *data = 0;
	int pointCount = 0;
	int i = 0;

	if (!DATA(self)) return IONIL(self);

	i = IoMessage_locals_intArgAt_(m, locals, 0);
	pointCount = IoCairoPathElement_pointCount(self);
	if (i < 0 || i >= pointCount) return IONIL(self);

	data = PATH_DATA(self) + i + 1;
	return IoSeq_newWithX_y_(IOSTATE, data->point.x, data->point.y);
}


/* ------------------------------------------------------------------------------------------------*/
/* Private */

static int IoCairoPathElement_pointCount(IoCairoPathElement *self)
{
	switch (PATH_DATA(self)->header.type)
	{
		case CAIRO_PATH_MOVE_TO:
			return 1;
		case CAIRO_PATH_LINE_TO:
			return 1;
		case CAIRO_PATH_CURVE_TO:
			return 3;
		case CAIRO_PATH_CLOSE_PATH:
			return 0;
	}
	return 0;
}
