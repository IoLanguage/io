//metadoc CairoRectangle copyright Daniel Rosengren, 2007
//metadoc CairoRectangle license BSD revised
//metadoc CairoRectangle category Graphics

#include "IoCairoRectangle.h"
#include "IoNumber.h"

#define RECT(self) ((cairo_rectangle_t *)IoObject_dataPointer(self))

static const char *protoId = "CairoRectangle";

static IoTag *IoCairoRectangle_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoRectangle_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoRectangle_free);
	return tag;
}

IoCairoRectangle *IoCairoRectangle_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoRectangle_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(cairo_rectangle_t)));

	IoState_registerProtoWithFunc_(state, self, IoCairoRectangle_proto);

	{
		IoMethodTable methodTable[] = {
			{"x", IoCairoRectangle_x},
			{"y", IoCairoRectangle_y},
			{"width", IoCairoRectangle_width},
			{"height", IoCairoRectangle_height},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoRectangle *IoCairoRectangle_rawClone(IoCairoRectangle *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(cairo_rectangle_t)));
	return self;
}

IoCairoRectangle *IoCairoRectangle_newWithRawRectangle_(void *state, cairo_rectangle_t *rect)
{
	IoCairoRectangle *self = IOCLONE(IoState_protoWithId_(state, protoId));
	memcpy(RECT(self), rect, sizeof(cairo_rectangle_t));
	return self;
}

void IoCairoRectangle_free(IoCairoRectangle *self)
{
	free(IoObject_dataPointer(self));
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoRectangle_x(IoCairoRectangle *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(RECT(self)->x);
}

IoObject *IoCairoRectangle_y(IoCairoRectangle *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(RECT(self)->y);
}

IoObject *IoCairoRectangle_width(IoCairoRectangle *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(RECT(self)->width);
}

IoObject *IoCairoRectangle_height(IoCairoRectangle *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(RECT(self)->height);
}
