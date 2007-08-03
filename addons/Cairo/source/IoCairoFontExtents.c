/*#io
CairoFontExtents ioDoc(
	docCopyright("Trevor Fancher", 2007)
	docLicense("BSD revised")
	docObject("Cairo")
	docDescription("Cairo is a 2D graphics library. http://cairographics.org/")
	docCategory("Graphics")
*/

#include "IoCairoFontExtents.h"
#include "IoNumber.h"

#define DATA(self) ((IoCairoFontExtentsData *)IoObject_dataPointer(self))
#define FONTEXTENTS(ctx) (DATA(ctx))

IoTag *IoCairoFontExtents_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoFontExtents");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoFontExtents_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoFontExtents_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoCairoFontExtents_mark);
	return tag;
}

IoCairoFontExtents *IoCairoFontExtents_proto(void *state) 
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoFontExtents_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCairoFontExtentsData)));
	
	IoState_registerProtoWithFunc_(state, self, IoCairoFontExtents_proto);
	
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
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoCairoFontExtentsData)));	
	return self;
}

IoCairoFontExtents *IoCairoFontExtents_newWithRawFontExtents(void *state, cairo_font_extents_t *te)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoCairoFontExtents_proto);
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(te, sizeof(IoCairoFontExtentsData)));
	return self;
}

/* ----------------------------------------------------------- */

void IoCairoFontExtents_free(IoCairoFontExtents *self) 
{
	free(IoObject_dataPointer(self)); 
}

void IoCairoFontExtents_mark(IoCairoFontExtents *self) 
{
}

cairo_font_extents_t *IoCairoFontExtents_getRawFontExtents(IoCairoFontExtents *self)
{
	return FONTEXTENTS(self);
}

/* ----------------------------------------------------------- */

#define IoCairoFontExtents_make_get_func(funName, dataName) \
	IoObject *IoCairoFontExtents_ ## funName (IoCairoFontExtents *self, IoObject *locals, IoMessage *m) \
	{ \
		return IONUMBER(DATA(self)->dataName); \
	}

IoCairoFontExtents_make_get_func(ascent, ascent)
IoCairoFontExtents_make_get_func(descent, descent)
IoCairoFontExtents_make_get_func(height, height)
IoCairoFontExtents_make_get_func(maxXAdvance, max_x_advance)
IoCairoFontExtents_make_get_func(maxYAdvance, max_y_advance)

#undef IoCairoFontExtents_make_get_func
