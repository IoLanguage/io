//metadoc CairoFontFace copyright Daniel Rosengren, 2007
//metadoc CairoFontFace license BSD revised
//metadoc CairoFontFace category Graphics

#include "IoCairoFontFace.h"
#include "IoNumber.h"

#define FACE(self) ((cairo_font_face_t *)IoObject_dataPointer(self))

static const char *protoId = "CairoFontFace";

void *IoMessage_locals_cairoFontFaceArgAt_(IoMessage *self, void *locals, int n)
{
	IoObject *arg = IoMessage_locals_valueArgAt_(self, locals, n);
	if (!ISCAIROFONTFACE(arg))
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "CairoFontFace");
	return arg;
}


static IoTag *IoCairoFontFace_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoFontFace_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoFontFace_free);
	return tag;
}

IoCairoFontFace *IoCairoFontFace_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoFontFace_newTag(state));

	IoState_registerProtoWithId_(state, self, protoId);

	return self;
}

IoCairoFontFace *IoCairoFontFace_rawClone(IoCairoFontFace *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	if (FACE(proto))
		IoObject_setDataPointer_(self, cairo_font_face_reference(FACE(proto)));
	return self;
}

IoCairoFontFace *IoCairoFontFace_newWithRawFontFace_(void *state, cairo_font_face_t *face)
{
	//IoCairoFontFace *self = IOCLONE(IoState_protoWithInitId_(state, protoId));
	IoCairoFontFace *self = IOCLONE(IoState_protoWithId_(state, protoId));
	IoObject_setDataPointer_(self, face);
	return self;
}

void IoCairoFontFace_free(IoCairoFontFace *self)
{
	if (FACE(self))
		cairo_font_face_destroy(FACE(self));
}


cairo_font_face_t *IoCairoFontFace_rawFontFace(IoCairoFontFace *self)
{
	return FACE(self);
}
