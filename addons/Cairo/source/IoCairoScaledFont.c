/*#io
CairoScaledFont ioDoc(
  docCopyright("Daniel Rosengren", 2007)
  docLicense("BSD revised")
  docCategory("Graphics")
*/

#include "IoCairoScaledFont.h"
#include "IoCairoFontFace.h"
#include "IoCairoFontExtents.h"
#include "IoCairoTextExtents.h"
#include "IoCairoFontOptions.h"
#include "IoCairoGlyph.h"
#include "IoCairoMatrix.h"
#include "IoList.h"
#include "IoNumber.h"
#include "tools.h"
#include <stdlib.h>

#define FONT(self) ((cairo_scaled_font_t *)IoObject_dataPointer(self))
#define CHECK_STATUS(self) checkStatus_(IOSTATE, m, cairo_scaled_font_status(FONT(self)))


void *IoMessage_locals_cairoScaledFontArgAt_(IoMessage *self, void *locals, int n)
{
	IoObject *arg = IoMessage_locals_valueArgAt_(self, locals, n);
	if (!ISCAIROSCALEDFONT(arg)) 
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "CairoScaledFont");
	return arg;
}


static IoTag *IoCairoScaledFont_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoScaledFont");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoScaledFont_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoScaledFont_rawClone);
	return tag;
}

IoCairoScaledFont *IoCairoScaledFont_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoScaledFont_newTag(state));

	IoState_registerProtoWithFunc_(state, self, IoCairoScaledFont_proto);
	
	{
		IoMethodTable methodTable[] = {
			{"create", IoCairoScaledFont_create},

			{"extents", IoCairoScaledFont_extents},
			{"textExtents", IoCairoScaledFont_textExtents},
			{"glyphExtents", IoCairoScaledFont_glyphExtents},

			{"getFontFace", IoCairoScaledFont_getFontFace},
			{"getFontOptions", IoCairoScaledFont_getFontOptions},
			{"getFontMatrix", IoCairoScaledFont_getFontMatrix},
			{"getCTM", IoCairoScaledFont_getCTM},

			{NULL, NULL},
		};
		
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoCairoScaledFont *IoCairoScaledFont_rawClone(IoCairoScaledFont *proto)
{
	IoCairoScaledFont *self = IoObject_rawClonePrimitive(proto);
	if (FONT(proto))
		IoObject_setDataPointer_(self, cairo_scaled_font_reference(FONT(proto)));
	return self;
}

IoCairoScaledFont *IoCairoScaledFont_newWithRawScaledFont_(void *state, IoMessage *m, cairo_scaled_font_t *font)
{
	IoCairoScaledFont *self = IOCLONE(IoState_protoWithInitFunction_(state, IoCairoScaledFont_proto));
	IoObject_setDataPointer_(self, font);
	CHECK_STATUS(self);
	return self;
}

void IoCairoScaledFont_free(IoCairoScaledFont *self)
{
	if (FONT(self))
		cairo_scaled_font_destroy(FONT(self));
}

cairo_scaled_font_t *IoCairoScaledFont_rawScaledFont(IoCairoScaledFont *self)
{
	return FONT(self);
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoScaledFont_create(IoCairoScaledFont *self, IoObject *locals, IoMessage *m)
{
	IoCairoFontFace *face = IoMessage_locals_cairoFontFaceArgAt_(m, locals, 0);
	IoCairoMatrix *matrix = IoMessage_locals_cairoMatrixArgAt_(m, locals, 1);
	IoCairoMatrix *ctm = IoMessage_locals_cairoMatrixArgAt_(m, locals, 2);
	IoCairoFontOptions *options = IoMessage_locals_cairoFontOptionsArgAt_(m, locals, 3);
	
	cairo_scaled_font_t *font = cairo_scaled_font_create(
		IoCairoFontFace_rawFontFace(face),
		IoCairoMatrix_rawMatrix(matrix),
		IoCairoMatrix_rawMatrix(ctm),
		IoCairoFontOptions_rawFontOptions(options)
	);
	return IoCairoScaledFont_newWithRawScaledFont_(IOSTATE, m, font);
}


IoObject *IoCairoScaledFont_extents(IoCairoScaledFont *self, IoObject *locals, IoMessage *m)
{
	cairo_font_extents_t extents;
	cairo_scaled_font_extents(FONT(self), &extents);
	CHECK_STATUS(self);
	return IoCairoFontExtents_newWithRawFontExtents(IOSTATE, &extents);
}

IoObject *IoCairoScaledFont_textExtents(IoCairoScaledFont *self, IoObject *locals, IoMessage *m)
{
	const char *text = IoMessage_locals_UTF8ArgAt_(m, locals, 0);
	cairo_text_extents_t extents;
	cairo_scaled_font_text_extents(FONT(self), text, &extents);
	CHECK_STATUS(self);	
	return IoCairoTextExtents_newWithRawTextExtents(IOSTATE, &extents);
}

IoObject *IoCairoScaledFont_glyphExtents(IoCairoScaledFont *self, IoObject *locals, IoMessage *m)
{
	IoList *glyphList = IoMessage_locals_listArgAt_(m, locals, 0);
	int glyphCount = 0;
	cairo_glyph_t *glyphs = rawGlyphsFromList_count_(glyphList, &glyphCount);
	cairo_text_extents_t extents;

	if (!glyphs)
		return IONIL(self);
	
	cairo_scaled_font_glyph_extents(FONT(self), glyphs, glyphCount, &extents);
	free(glyphs);
	CHECK_STATUS(self);
	return IoCairoTextExtents_newWithRawTextExtents(IOSTATE, &extents);
}


IoObject *IoCairoScaledFont_getFontFace(IoCairoScaledFont *self, IoObject *locals, IoMessage *m)
{
	cairo_font_face_t *face = cairo_scaled_font_get_font_face(FONT(self));
	return IoCairoFontFace_newWithRawFontFace_(self, cairo_font_face_reference(face));
}

IoObject *IoCairoScaledFont_getFontOptions(IoCairoScaledFont *self, IoObject *locals, IoMessage *m)
{
	cairo_font_options_t *options = cairo_font_options_create();
	cairo_scaled_font_get_font_options(FONT(self), options);
	return IoCairoFontOptions_newWithRawFontOptions_(IOSTATE, m, options);
}

IoObject *IoCairoScaledFont_getFontMatrix(IoCairoScaledFont *self, IoObject *locals, IoMessage *m)
{
	cairo_matrix_t matrix;
	cairo_scaled_font_get_font_matrix(FONT(self), &matrix);
	return IoCairoMatrix_newWithRawMatrix_(IOSTATE, &matrix);
}

IoObject *IoCairoScaledFont_getCTM(IoCairoScaledFont *self, IoObject *locals, IoMessage *m)
{
	cairo_matrix_t matrix;
	cairo_scaled_font_get_ctm(FONT(self), &matrix);
	return IoCairoMatrix_newWithRawMatrix_(IOSTATE, &matrix);
}
