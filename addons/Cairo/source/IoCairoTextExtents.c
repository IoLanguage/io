/*#io
Cairo ioDoc(
		    docCopyright("Trevor Fancher", 2007)
		    docLicense("BSD revised")
		    docObject("Cairo")
		    docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
		    docCategory("Graphics")
		    */

#include "IoCairoTextExtents.h"
#include "IoNumber.h"

#define DATA(self) ((IoCairoTextExtentsData *)IoObject_dataPointer(self))
#define TEXTEXTENTS(ctx) (DATA(ctx))

IoTag *IoCairoTextExtents_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoTextExtents");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoTextExtents_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoTextExtents_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCairoTextExtents_mark);
	return tag;
}

IoCairoTextExtents *IoCairoTextExtents_proto(void *state) 
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoTextExtents_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCairoTextExtentsData)));
	
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
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoCairoTextExtentsData)));	
	return self;
}

IoCairoTextExtents *IoCairoTextExtents_newWithRawTextExtents(void *state, cairo_text_extents_t *te)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoCairoTextExtents_proto);
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(te, sizeof(IoCairoTextExtentsData)));
	return self;
}

/* ----------------------------------------------------------- */

void IoCairoTextExtents_free(IoCairoTextExtents *self) 
{
	free(IoObject_dataPointer(self)); 
}

void IoCairoTextExtents_mark(IoCairoTextExtents *self) 
{
}

cairo_text_extents_t *IoCairoTextExtents_getRawTextExtents(IoCairoTextExtents *self)
{
	return TEXTEXTENTS(self);
}

/* ----------------------------------------------------------- */

#define IoCairoTextExtents_make_get_func(funName, dataName) \
	IoObject *IoCairoTextExtents_ ## funName (IoCairoTextExtents *self, IoObject *locals, IoMessage *m) \
	{ \
		return IONUMBER(DATA(self)->dataName); \
	}

IoCairoTextExtents_make_get_func(height, height)
IoCairoTextExtents_make_get_func(width, width)
IoCairoTextExtents_make_get_func(xAdvance, x_advance)
IoCairoTextExtents_make_get_func(xBearing, x_bearing)
IoCairoTextExtents_make_get_func(yAdvance, y_advance)
IoCairoTextExtents_make_get_func(yBearing, y_bearing)

#undef IoCairoTextExtents_make_get_func
