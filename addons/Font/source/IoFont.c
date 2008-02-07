
//metadoc Font copyright Steve Dekorte, 2004
//metadoc Font license BSD revised
//metadoc Font category Graphics
/*metadoc Font description
The Font object can be used to load and render TypeTrype fonts. Example use;
<pre>	
// within a GLUT display callback...

timesFont = Font clone open(\"times.ttf\")
if (timesFont error, write(\"Error loading font: \", timesFont error, \"\n\"); return)
timesFont setPointSize(16)
glColor(0,0,0,1)
timesFont draw(\"This is a test.\")
</pre>	

<b>Rendering fonts using OpenGL textures</b>
<p>
Smaller fonts (those having a point size around 30 or smaller, depending on the font) will automatically be cached in and rendered from a texture. This technique is very fast and should support rendering speeds as fast (or faster than) those of typical desktop font rendering systems. Larger font sizes(due to texture memory constraints) will be rendered to a pixelmap when displayed. Thanks to Mike Austin for implementing the font texturing system.
*/


#include "IoFont.h"
#include "List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "GLFont.h"


#include "FreeTypeErrorCodes.h"
#include <freetype/ftglyph.h>
#include <math.h>

#define DATA(self) ((IoFontData *)IoObject_dataPointer(self))

IoTag *IoFont_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Font");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoFont_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoFont_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoFont_mark);
	return tag;
}

IoFont *IoFont_proto( void *state )
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoFont_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoFontData)));
	DATA(self)->path = IOSYMBOL(".");
	DATA(self)->font = GLFont_new();
	DATA(self)->isProto = 1;
	IoState_registerProtoWithFunc_(state, self, IoFont_proto);

	{
		IoMethodTable methodTable[] = {
		{"open", IoFont_open},

		{"setPath", IoFont_setPath},
		{"path", IoFont_path},

		{"setPixelSize", IoFont_setPixelSize},
		{"pixelSize", IoFont_pixelSize},

		{"drawString", IoFont_drawString},
		{"widthOfString", IoFont_lengthOfString},
		{"widthOfCharacter", IoFont_lengthOfCharacter},
		{"pixelHeight", IoFont_fontHeight},
		{"isTextured", IoFont_isTextured},
		{"error", IoFont_error},
		{"stringIndexAtWidth", IoFont_stringIndexAtWidth},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	GLFont_init();
	return self;
}

IoFont *IoFont_rawClone(IoFont *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoFontData)));
	DATA(self)->font = GLFont_new();
	DATA(self)->isProto = 0;
	return self;
}

IoFont *IoFont_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoFont_proto);
	return IOCLONE(proto);
}

/* -------------------------------------------------------------------------- */

void IoFont_free( IoFont *self )
{
	GLFont_free( DATA(self)->font );
	if (DATA(self)->isProto) { GLFont_done(); }
	free(DATA(self));
}

void IoFont_mark( IoFont *self )
{
	IoObject_shouldMark( (IoObject *)DATA(self)->path );
}

/* -------------------------------------------------------------------------- */

IoObject *IoFont_clone(IoFont *self, IoObject *locals, IoMessage *m)
{
	IoFont *newObject = IoFont_new(IOSTATE);
	DATA(newObject)->path = DATA(self)->path;
	return newObject;
}

IoObject *IoFont_setPath(IoFont *self, IoObject *locals, IoMessage *m)
{
	/*doc Font setPath(aString)
	Sets the Font path. Returns self.
	*/

	DATA(self)->path = IOREF(IoMessage_locals_seqArgAt_(m, locals, 0));
	return self;
}

IoObject *IoFont_path(IoFont *self, IoObject *locals, IoMessage *m)
{
	/*doc Font path
	Returns the Font path.
	*/
	return DATA(self)->path;
}

IoObject *IoFont_setPixelSize(IoFont *self, IoObject *locals, IoMessage *m)
{
	/*doc Font setPixelSize(aNumber)
	Sets the size of the font in pixels. Returns self.
	*/

	int size = IoMessage_locals_intArgAt_(m, locals, 0);
	GLFont_setPixelSize(DATA(self)->font, size);
	return self;
}

IoObject *IoFont_pixelSize(IoFont *self, IoObject *locals, IoMessage *m)
{
	/*doc Font pixelSize
	Returns the font's pixelSize.
	*/

	return IONUMBER(GLFont_pixelSize( DATA(self)->font));
}

void IoFont_checkError(IoFont *self, IoObject *locals, IoMessage *m)
{
	const char *e = GLFont_error(DATA(self)->font);

	if (e != NULL)
	{
		IoState_error_(IOSTATE, m, "Font %s", e);
	}
}

IoObject *IoFont_open(IoFont *self, IoObject *locals, IoMessage *m)
{
	/*doc Font open(optionalPath)
	Opens the font. Sets path using optionalPath if supplied. Returns self.
	*/

	if (IoMessage_argCount(m) > 0)
	{
		DATA(self)->path = IOREF(IoMessage_locals_seqArgAt_(m, locals, 0));
	}

	GLFont_loadFont( DATA(self)->font, CSTRING(DATA(self)->path) );
	IoFont_checkError(self, locals, m);
	return self;
}

IoObject *IoFont_lengthOfString(IoFont *self, IoObject *locals, IoMessage *m)
{
	/*doc Font widthOfString(aString)
	Returns a Number with the width that aString would render 
	to with the receiver's current settings.
	*/

	IoSymbol *text = IoMessage_locals_seqArgAt_(m, locals, 0);
	int startIndex = 0;
	int max = IoSeq_rawSize(text);
	int endIndex = max;

	if (IoMessage_argCount(m) == 2)
	{
		startIndex = IoNumber_asInt(IoMessage_locals_numberArgAt_(m, locals, 1));
		if (startIndex > max) startIndex = max;
	}

	if (IoMessage_argCount(m) > 2)
	{
		endIndex = IoNumber_asInt(IoMessage_locals_numberArgAt_(m, locals, 2));
		if (startIndex > max) endIndex = max;
	}

	return IONUMBER( GLFont_lengthOfString( DATA(self)->font, CSTRING(text), startIndex, endIndex) );
}

IoObject *IoFont_lengthOfCharacter(IoFont *self, IoObject *locals, IoMessage *m)
{
	/*doc Font widthOfCharacter(aNumber)
	Returns the width of the character specified by aNumber in the receiver's font.
	*/

	unsigned char c = IoNumber_asInt(IoMessage_locals_numberArgAt_(m, locals, 0));
	return IONUMBER( GLFont_lengthOfCharacter_( DATA(self)->font, c) );
}

IoObject *IoFont_fontHeight(IoFont *self, IoObject *locals, IoMessage *m)
{
	/*doc Font pixelHeight
	Returns the height of the font measured in pixels.
	*/

	return (IoObject*)IONUMBER( GLFont_fontHeight( DATA(self)->font ) );
}

IoObject *IoFont_stringIndexAtWidth(IoFont *self, IoObject *locals, IoMessage *m)
{
	/*doc Font stringIndexAtWidth(aString, startIndex, width)
	Returns the max index of the character in String (starting at startIndex) 
	that fits within width.
	*/

	IoSymbol *text = IoMessage_locals_seqArgAt_(m, locals, 0);
	int startIndex;
	int width;
	//IOASSERT(IoMessage_argCount(m) == 2, "requires 3 arguments");
	startIndex = IoNumber_asInt(IoMessage_locals_numberArgAt_(m, locals, 1));
	if (startIndex > (int)IoSeq_rawSize(text)) startIndex = (int)IoSeq_rawSize(text);

	width = IoNumber_asInt(IoMessage_locals_numberArgAt_(m, locals, 2));

	return IONUMBER(GLFont_stringIndexAtWidth(DATA(self)->font, CSTRING(text), startIndex, width));
}

IoObject *IoFont_drawString(IoFont *self, IoObject *locals, IoMessage *m)
{
	/*doc Font drawString(aString, optionalStartIndex, optionalEndIndex)
	Draws aString using the optional start and end indexes, if supplied. Returns self.
<p>
Note; Fonts are draw as RGBA pixel maps. These blending options are recommended:
<pre>	
glEnable(GL_BLEND)
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)
</pre>	
	*/

	IoSymbol *textString = IoMessage_locals_seqArgAt_(m, locals, 0);
	int startIndex = 0;
	int endIndex;

	if (IoMessage_argCount(m) > 1)
	{
		startIndex = IoNumber_asInt(IoMessage_locals_numberArgAt_(m, locals, 1));
		if (startIndex > (int)IoSeq_rawSize(textString)) startIndex = (int)IoSeq_rawSize(textString);
	}

	if (IoMessage_argCount(m) > 2)
	{
		endIndex = IoNumber_asInt(IoMessage_locals_numberArgAt_(m, locals, 2));
	}
	else
	{
		endIndex = IoSeq_rawSize(textString);
	}

	GLFont_drawString(DATA(self)->font, CSTRING(textString) , startIndex, endIndex);
	IoFont_checkError(self, locals, m);
	return self;
}

IoObject *IoFont_isTextured(IoFont *self, IoObject *locals, IoMessage *m )
{
	/*doc Font isTextured
	Returns true if the font is being cached in and rendered from a texture, false otherwise.
	*/

	return IOBOOL(self, GLFont_isTextured(DATA(self)->font));
}

IoObject *IoFont_error(IoFont *self, IoObject *locals, IoMessage *m)
{
	/*doc Font error
	Returns the current error string or nil if there is no error.
	*/

	const char *e = GLFont_error(DATA(self)->font);
	return strlen(e) ? (IoObject *)IOSYMBOL((char *)e) : IONIL(self);
}
