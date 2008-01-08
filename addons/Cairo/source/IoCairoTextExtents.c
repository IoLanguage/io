/*#io
CairoTextExtents ioDoc(
	docCopyright("Trevor Fancher", 2007)
	docLicense("BSD revised")
	docCategory("Graphics")
*/

#include "IoCairoTextExtents.h"
#include "IoNumber.h"

#define EXTENTS(self) ((cairo_text_extents_t *)IoObject_dataPointer(self))


static IoTag *IoCairoTextExtents_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoTextExtents");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoTextExtents_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoTextExtents_free);
	return tag;
}

IoCairoTextExtents *IoCairoTextExtents_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoTextExtents_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(cairo_text_extents_t)));

	IoState_registerProtoWithFunc_(state, self, IoCairoTextExtents_proto);

	{
		IoMethodTable methodTable[] = {
			{"height", IoCairoTextExtents_height},
			{"width", IoCairoTextExtents_width},
			{"xAdvance", IoCairoTextExtents_xAdvance},
			{"xBearing", IoCairoTextExtents_xBearing},
			{"yAdvance", IoCairoTextExtents_yAdvance},
			{"yBearing", IoCairoTextExtents_yBearing},
			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoTextExtents *IoCairoTextExtents_rawClone(IoCairoTextExtents *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(EXTENTS(proto), sizeof(cairo_text_extents_t)));
	return self;
}

IoCairoTextExtents *IoCairoTextExtents_newWithRawTextExtents(void *state, cairo_text_extents_t *extents)
{
	IoCairoTextExtents *self = IOCLONE(IoState_protoWithInitFunction_(state, IoCairoTextExtents_proto));
	memcpy(EXTENTS(self), extents, sizeof(cairo_text_extents_t));
	return self;
}

void IoCairoTextExtents_free(IoCairoTextExtents *self)
{
	free(EXTENTS(self));
}


cairo_text_extents_t *IoCairoTextExtents_rawTextExtents(IoCairoTextExtents *self)
{
	return EXTENTS(self);
}


/* ------------------------------------------------------------------------------------------------*/

#define IoCairoTextExtents_makeGetFunc(funcName, dataName) \
	IoObject *IoCairoTextExtents_ ## funcName (IoCairoTextExtents *self, IoObject *locals, IoMessage *m) \
	{ \
		return IONUMBER(EXTENTS(self)->dataName); \
	}

IoCairoTextExtents_makeGetFunc(height, height)
IoCairoTextExtents_makeGetFunc(width, width)
IoCairoTextExtents_makeGetFunc(xAdvance, x_advance)
IoCairoTextExtents_makeGetFunc(xBearing, x_bearing)
IoCairoTextExtents_makeGetFunc(yAdvance, y_advance)
IoCairoTextExtents_makeGetFunc(yBearing, y_bearing)

#undef IoCairoTextExtents_makeGetFunc
