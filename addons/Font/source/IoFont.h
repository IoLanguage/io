/*
//metadoc copyright Steve Dekorte", 2002, "Mike Austin (Modified for textures)", 2003)
*/

#ifndef IOFONT_DEFINED
#define IOFONT_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "GLFont.h"

#define ISFONT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoFont_rawClone)

typedef IoObject IoFont;

typedef struct
{
	GLFont *font;
	IoSymbol *path;
	char isProto;
} IoFontData;

IoFont *IoFont_proto(void *state);
IoFont *IoFont_new(void *state);
IoFont *IoFont_rawClone(IoFont *self);

void IoFont_free(IoFont *self);
void IoFont_mark(IoFont *self);

/* -------------------------------------------------------------------------- */
IoObject *IoFont_open(IoFont *self, IoObject *locals, IoMessage *m);

IoObject *IoFont_setPath(IoFont *self, IoObject *locals, IoMessage *m);
IoObject *IoFont_path(IoFont *self, IoObject *locals, IoMessage *m);

IoObject *IoFont_setPixelSize(IoFont *self, IoObject *locals, IoMessage *m);
IoObject *IoFont_pixelSize(IoFont *self, IoObject *locals, IoMessage *m);

IoObject *IoFont_drawString(IoFont *self, IoObject *locals, IoMessage *m);
IoObject *IoFont_lengthOfCharacter(IoFont *self, IoObject *locals, IoMessage *m);
IoObject *IoFont_lengthOfString(IoFont *self, IoObject *locals, IoMessage *m);
IoObject *IoFont_stringIndexAtWidth(IoFont *self, IoObject *locals, IoMessage *m);
IoObject *IoFont_fontHeight(IoFont *self, IoObject *locals, IoMessage *m);
IoObject *IoFont_isTextured(IoFont *self, IoObject *locals, IoMessage *m);
IoObject *IoFont_error(IoFont *self, IoObject *locals, IoMessage *m);

#endif

