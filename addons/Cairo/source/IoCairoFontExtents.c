//metadoc CairoFontExtents copyright Trevor Fancher, 2007
//metadoc CairoFontExtents license BSD revised
//metadoc CairoFontExtents category Graphics

#include "IoCairoFontExtents.h"
#include "IoNumber.h"

#define EXTENTS(self) ((cairo_font_extents_t *)IoObject_dataPointer(self))

static const char *protoId = "CairoFontExtents";

static IoTag *IoCairoFontExtents_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoFontExtents_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoFontExtents_free);
	return tag;
}

IoCairoFontExtents *IoCairoFontExtents_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoFontExtents_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(cairo_font_extents_t)));

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
			{"ascent", IoCairoFontExtents_ascent},
			{"descent", IoCairoFontExtents_descent},
			{"height", IoCairoFontExtents_height},
			{"maxXAdvance", IoCairoFontExtents_maxXAdvance},
			{"maxYAdvance", IoCairoFontExtents_maxYAdvance},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoFontExtents *IoCairoFontExtents_rawClone(IoCairoFontExtents *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(EXTENTS(proto), sizeof(cairo_font_extents_t)));
	return self;
}

IoCairoFontExtents *IoCairoFontExtents_newWithRawFontExtents(void *state, cairo_font_extents_t *extents)
{
	//IoCairoFontExtents *self = IOCLONE(IoState_protoWithInitId_(state, protoId));
	IoCairoFontExtents *self = IOCLONE(IoState_protoWithId_(state, protoId));
	memcpy(EXTENTS(self), extents, sizeof(cairo_font_extents_t));
	return self;
}

void IoCairoFontExtents_free(IoCairoFontExtents *self)
{
	free(EXTENTS(self));
}


cairo_font_extents_t *IoCairoFontExtents_rawFontExtents(IoCairoFontExtents *self)
{
	return EXTENTS(self);
}

/* ------------------------------------------------------------------------------------------------*/

#define IoCairoFontExtents_make_get_func(funcName, dataName) \
	IoObject *IoCairoFontExtents_ ## funcName (IoCairoFontExtents *self, IoObject *locals, IoMessage *m) \
	{ \
		return IONUMBER(EXTENTS(self)->dataName); \
	}

IoCairoFontExtents_make_get_func(ascent, ascent)
IoCairoFontExtents_make_get_func(descent, descent)
IoCairoFontExtents_make_get_func(height, height)
IoCairoFontExtents_make_get_func(maxXAdvance, max_x_advance)
IoCairoFontExtents_make_get_func(maxYAdvance, max_y_advance)

#undef IoCairoFontExtents_make_get_func
