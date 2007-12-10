/*#io
CairoGlyph ioDoc(
	docCopyright("Daniel Rosengren", 2007)
	docLicense("BSD revised")
	docCategory("Graphics")
*/

#include "IoCairoGlyph.h"
#include "IoNumber.h"

#define GLYPH(self) ((cairo_glyph_t *)IoObject_dataPointer(self))


static IoTag *IoCairoGlyph_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoGlyph");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoGlyph_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoGlyph_free);
	return tag;
}

IoCairoGlyph *IoCairoGlyph_proto(void *state) 
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoGlyph_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(cairo_glyph_t)));
	
	IoState_registerProtoWithFunc_(state, self, IoCairoGlyph_proto);
	
	{
		IoMethodTable methodTable[] = {
			{"setIndex", IoCairoGlyph_setIndex},
			{"index", IoCairoGlyph_index},

			{"setX", IoCairoGlyph_setX},
			{"x", IoCairoGlyph_x},

			{"setY", IoCairoGlyph_setY},
			{"y", IoCairoGlyph_y},

			{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCairoGlyph *IoCairoGlyph_rawClone(IoCairoGlyph *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);	
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(cairo_glyph_t)));	
	return self;
}

IoCairoGlyph *IoCairoGlyph_newWithRawGlyph_(void *state, cairo_glyph_t *glyph)
{
	IoCairoGlyph *self = IOCLONE(IoState_protoWithInitFunction_(state, IoCairoGlyph_proto));
	memcpy(GLYPH(self), glyph, sizeof(cairo_glyph_t));
	return self;
}

void IoCairoGlyph_free(IoCairoGlyph *self) 
{
	free(IoObject_dataPointer(self));
}


cairo_glyph_t *IoCairoGlyph_rawGlyph(IoCairoGlyph *self)
{
	return GLYPH(self);
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoGlyph_setIndex(IoCairoGlyph *self, IoObject *locals, IoMessage *m)
{
	GLYPH(self)->index = IoMessage_locals_intArgAt_(m, locals, 0);
	return self;
}

IoObject *IoCairoGlyph_index(IoCairoGlyph *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(GLYPH(self)->index);
}


IoObject *IoCairoGlyph_setX(IoCairoGlyph *self, IoObject *locals, IoMessage *m)
{
	GLYPH(self)->x = IoMessage_locals_doubleArgAt_(m, locals, 1);
	return self;
}

IoObject *IoCairoGlyph_x(IoCairoGlyph *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(GLYPH(self)->x);
}


IoObject *IoCairoGlyph_setY(IoCairoGlyph *self, IoObject *locals, IoMessage *m)
{
	GLYPH(self)->y = IoMessage_locals_doubleArgAt_(m, locals, 2);
	return self;
}

IoObject *IoCairoGlyph_y(IoCairoGlyph *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(GLYPH(self)->y);
}


/* ------------------------------------------------------------------------------------------------*/

cairo_glyph_t *rawGlyphsFromList_count_(IoList *glyphList, int *count)
{
	int glyphCount = IoList_rawSize(glyphList);
	cairo_glyph_t *glyphs = 0;
	int i;
	
	if (count)
		*count = glyphCount;
	
	if (glyphCount == 0)
		return 0;
	
	glyphs = malloc(sizeof(cairo_glyph_t) * glyphCount);
	for (i = 0; i < glyphCount; i++)
	{
		cairo_glyph_t *glyph = IoCairoGlyph_rawGlyph(IoList_rawAt_(glyphList, i));
		memcpy(glyphs + i, glyph, sizeof(cairo_glyph_t));
	}
	
	return glyphs;
}
