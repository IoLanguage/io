/*#io
Image ioDoc(
		  docCopyright("Steve Dekorte", 2004)
		  docLicense("BSD revised")
		  docCategory("Graphics")
		  docDescription("The Image object can read and draw images and provide the image data as a buffer. Example use;
<pre>
image = Image clone open(\"curly.png\")
image draw
image scaleTo(image width / 2, image height / 2)
image save(\"curly.tiff\")
</pre>
When loading an attempt will be made to convert the image into whichever of the following formats it is closest to: L8, LA8, RGB8, RGBA8.
<p>
Currently supported formats include PNG(which supports alpha), JPG and TIFF. ")
*/

#include "IoImage.h"
#include "List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "GLImage.h"

#define DATA(self) ((IoImageData *)IoObject_dataPointer(self))

IoTag *IoImage_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Image");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoImage_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoImage_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoImage_mark);
	return tag;
}

IoImage *IoImage_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoImage_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoImageData)));
	
	DATA(self)->buffer = IoSeq_newWithCString_(IOSTATE, "");
	DATA(self)->glImage = GLImage_new();
	GLImage_setExternalUArray_(DATA(self)->glImage, IoSeq_rawUArray(DATA(self)->buffer));
	
	IoState_registerProtoWithFunc_(state, self, IoImage_proto);
	
	{
		IoMethodTable methodTable[] = {
		{"setDataWidthHeightComponentCount", IoImage_setDataWidthHeightComponentCount},
		{"setPath", IoImage_setPath},
		{"open", IoImage_open},
		{"save", IoImage_save},
		
		{"width", IoImage_width},
		{"height", IoImage_height},
		
		{"originalWidth", IoImage_originalWidth},
		{"originalHeight", IoImage_originalHeight},
		
		{"data", IoImage_data},
		{"isL8", IoImage_isL8},
		{"isLA8", IoImage_isLA8},
		{"isRGB8", IoImage_isRGB8},
		{"isRGBA8", IoImage_isRGBA8},
		{"error", IoImage_error},
		{"draw", IoImage_draw},
		{"scaleTo", IoImage_scaleTo},
		{"crop", IoImage_crop},
		{"compositeTo", IoImage_compositeTo},
		{"grab", IoImage_grab},
			
		// textures 
		
		{"grabTexture", IoImage_grabTexture},
		{"textureId", IoImage_textureId},
		{"updateTexture", IoImage_textureId},
		{"bindTexture", IoImage_bindTexture},
		{"closeTexture", IoImage_closeTexture},
		{"drawTexture", IoImage_drawTexture},
		{"drawScaledTexture", IoImage_drawScaledTexture},
			
		// extras 
		
		{"setEncodingQuality", IoImage_setEncodingQuality},  
		{"encodingQuality", IoImage_encodingQuality},  
			
		{"setDecodingWidthHint", IoImage_setDecodingWidthHint},  
		{"decodingWidthHint", IoImage_decodingWidthHint},  
			
		{"setDecodingHeightHint", IoImage_setDecodingHeightHint},  
		{"decodingHeightHint", IoImage_decodingHeightHint},  
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}  
	return self;
}

IoImage *IoImage_rawClone(IoImage *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoImageData)));
	DATA(self)->buffer = IOCLONE(DATA(proto)->buffer);
	DATA(self)->glImage = GLImage_new();
	GLImage_setExternalUArray_(DATA(self)->glImage, IoSeq_rawUArray(DATA(self)->buffer));
	GLImage_path_(DATA(self)->glImage, GLImage_path(DATA(proto)->glImage));
	return self; 
}

IoImage *IoImage_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoImage_proto);
	return IOCLONE(proto);
}

/* ----------------------------------------------------------- */

void IoImage_free(IoImage *self) 
{ 
	GLImage_free(DATA(self)->glImage);
	free(IoObject_dataPointer(self)); 
}

void IoImage_mark(IoImage *self) 
{ 
	IoObject_shouldMark(DATA(self)->buffer);
}

GLImage *IoImage_glImage(IoImage *self) 
{ 
	return DATA(self)->glImage; 
}

GLImage *IoImage_rawGLImage(IoImage *self)
{ 
	return DATA(self)->glImage; 
}

/* ----------------------------------------------------------- */

IoObject *IoImage_path(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("path", 
		   "Returns the image path. ")
	*/
	
	return IOSYMBOL(GLImage_path(DATA(self)->glImage)); 
}

IoObject *IoImage_setPath(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("setPath(aString)", 
		   "Sets the image path. Returns self. ")
	*/
	
	IoSymbol *s = IoMessage_locals_symbolArgAt_(m, locals, 0);
	GLImage_path_(DATA(self)->glImage, CSTRING(s));
	return self;
}

void IoImage_checkError(IoImage *self, IoObject *locals, IoMessage *m)
{
	const char *e = GLImage_error(DATA(self)->glImage);
	
	if (e != NULL)
	{
		IoState_error_(IOSTATE, m, "Image %s on %s", e, GLImage_path(DATA(self)->glImage));
	}
}


IoObject *IoImage_setDataWidthHeightComponentCount(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("setDataWidthHeightComponentCount(aSequence, width, height, componentCount)", 
		   "Sets the image data and it's parameters. Returns self.")
	*/
	
	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 0); 
	int w = IoMessage_locals_intArgAt_(m, locals, 1); 
	int h = IoMessage_locals_intArgAt_(m, locals, 2); 
	int c = IoMessage_locals_intArgAt_(m, locals, 3); 
	
	GLImage_setData_width_height_componentCount_(DATA(self)->glImage, IoSeq_rawUArray(data), w, h, c);
	
	return self;
}

IoObject *IoImage_open(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("open(optionalPathString)", 
		   "Sets the path to optionalPathString if provided and opens the image file. Returns self on success, Nil on failure. ")
	*/
	
	/*printf("opening Image %p %s\n", self, GLImage_path(DATA(self)->glImage));*/
	
	if (IoMessage_argCount(m) > 0)
	{ 
		IoSymbol *path = IoMessage_locals_symbolArgAt_(m, locals, 0); 
		GLImage_path_(DATA(self)->glImage, CSTRING(path));
	}
	
	GLImage_load(DATA(self)->glImage);
	IoImage_checkError(self, locals, m);
	return self; 
}

IoObject *IoImage_save(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("save(optionalPathString)", 
		   "Sets the path to optionalPathString if provided and saves the image in the format specified by the path extension. Returns self on success, Nil on failure. ")
	*/
	
	if (IoMessage_argCount(m) > 0)
	{ 
		IoSymbol *path = IoMessage_locals_symbolArgAt_(m, locals, 0); 
		GLImage_path_(DATA(self)->glImage, CSTRING(path));
	}
	
	GLImage_save(DATA(self)->glImage);
	IoImage_checkError(self, locals, m);
	return self; 
}

IoObject *IoImage_width(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("width", "Returns the image width. ")
	*/
	
	return IONUMBER(GLImage_width(DATA(self)->glImage)); 
}

IoObject *IoImage_height(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("height", "Returns the image hieght. ")
	*/
	
	return IONUMBER(GLImage_height(DATA(self)->glImage)); 
}

IoObject *IoImage_originalWidth(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("originalWidth", "Returns the original image width (before it was resized to make a texture). ")
	*/
	
	return IONUMBER(GLImage_originalWidth(DATA(self)->glImage)); 
}

IoObject *IoImage_originalHeight(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("originalHeight", "Returns the original image height (before it was resized to make a texture). ")
	*/
	
	return IONUMBER(GLImage_originalHeight(DATA(self)->glImage)); 
}

IoObject *IoImage_data(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("data", 
		   "Returns a Buffer primitive containing the image data(loading it first if needed). Manipulating this data will effect what is drawn when the receiver's draw method is called. ")
	*/
	
	return DATA(self)->buffer; 
}

IoObject *IoImage_error(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("error", 
		   "Returns a String containing the current error or Nil if there is no error. ")
	*/
	
	char *s = (char *)GLImage_error(DATA(self)->glImage);
	
	if ((!s) || (!strlen(s))) 
	{
		return IONIL(self);
	}
	
	return IOSYMBOL(s);
}

IoObject *IoImage_isRGB8(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("isRGB8", 
		   "Returns true if the receiver is in RGB8 format, false otherwise.")
	*/
	
	return IOBOOL(self, GLImage_format(DATA(self)->glImage) == GL_RGB8); 
}

IoObject *IoImage_isRGBA8(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("isRGBA8", 
		   "Returns true if the receiver is in RGBA8 format, false otherwise.")
	*/
	
	return IOBOOL(self, GLImage_format(DATA(self)->glImage) == GL_RGBA8); 
}

IoObject *IoImage_isL8(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("isL8", 
		   "Returns true if the receiver is in L8 (8bit Luminance) format, false otherwise.")
	*/
	
	return IOBOOL(self, GLImage_format(DATA(self)->glImage) == GL_LUMINANCE); 
}

IoObject *IoImage_isLA8(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("isLA8", 
		   "Returns true if the receiver is in LA8 (8bit Luminance-Alpha) format, false otherwise.")
	*/
	
	return IOBOOL(self, GLImage_format(DATA(self)->glImage) == GL_LUMINANCE_ALPHA); 
}

IoObject *IoImage_draw(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("draw", 
		   "Draws the image. (Sets the glRaster position to 0,0)")
	*/
	
	glRasterPos2d(0,0);
	GLImage_draw(DATA(self)->glImage);
	IoImage_checkError(self, locals, m);
	return self;
}

IoObject *IoImage_scaleTo(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("scaleTo(width, height)", 
		   "Scale the image to the specified size. Returns self. ")
	*/
	
	int width = IoMessage_locals_intArgAt_(m, locals, 0);
	int height = IoMessage_locals_intArgAt_(m, locals, 1);
	GLImage_scaleTo(DATA(self)->glImage, width, height);
	IoImage_checkError(self, locals, m);
	return self;
}

IoObject *IoImage_crop(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("crop(x, y, width, height)", 
		   "Crops the image to the specified values. Returns self.")
	*/
	
	int cx = IoMessage_locals_intArgAt_(m, locals, 0);
	int cy = IoMessage_locals_intArgAt_(m, locals, 1);
	int width = IoMessage_locals_intArgAt_(m, locals, 2);
	int height = IoMessage_locals_intArgAt_(m, locals, 3);
	
	GLImage_crop(DATA(self)->glImage, cx, cy, width, height);
	IoImage_checkError(self, locals, m);
	return self;
}

IoObject *IoImage_compositeTo(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("compositeTo(destinationImage, x, y)", 
		   "Write the image onto the destinationImage at the specified position. Returns self. ")
	*/
	
	IoImage *image = IoMessage_locals_valueArgAt_(m, locals, 0);
	int x = IoMessage_locals_intArgAt_(m, locals, 1);
	int y = IoMessage_locals_intArgAt_(m, locals, 2);
	IOASSERT(ISIMAGE(image), "first argument must be an image");
	GLImage_composite_to_(DATA(self)->glImage, DATA(image)->glImage, x, y);
	IoImage_checkError(self, locals, m);
	return self;
}

IoObject *IoImage_grab(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("grab(x, y, width, height, format)", 
		   "Grab an image from the current read buffer of the specified position and size. 
Valid formats are GL_LUMINANCE, GL_RGB, GL_RGBA, GL_LUMINANCE_ALPHA. Returns self. ")
	*/
	
	int x = IoMessage_locals_intArgAt_(m, locals, 0);
	int y = IoMessage_locals_intArgAt_(m, locals, 1);
	int w = IoMessage_locals_intArgAt_(m, locals, 2);
	int h = IoMessage_locals_intArgAt_(m, locals, 3);
	GLenum format = IoMessage_locals_intArgAt_(m, locals, 4);
	IOASSERT( 
			format == GL_LUMINANCE ||
			format == GL_RGB ||
			format == GL_RGBA ||
			format == GL_LUMINANCE_ALPHA 
			, "invalid format");
	GLImage_grab(DATA(self)->glImage, x, y, w, h, format);
	IoImage_checkError(self, locals, m);
	return self;
}

IoObject *IoImage_grabTexture(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("grabTexture(x, y, width, height, format)", 
		   "Grab an image from the current read buffer of the specified position and size. 
Valid formats are GL_LUMINANCE, GL_RGB, GL_RGBA, GL_LUMINANCE_ALPHA. Returns self. ")
	*/
	
	int x = IoMessage_locals_intArgAt_(m, locals, 0);
	int y = IoMessage_locals_intArgAt_(m, locals, 1);
	int w = IoMessage_locals_intArgAt_(m, locals, 2);
	int h = IoMessage_locals_intArgAt_(m, locals, 3);
	GLImage_grabTexture(DATA(self)->glImage, x, y, w, h);
	IoImage_checkError(self, locals, m);
	return self;
}

/* --- textures ---------------------- */

IoObject *IoImage_textureId(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("textureId", 
		   "Returns the receiver's texture id. If the image has no texture, it creates one 
and scales the image dimensions to powers of 2 and sets the texture to the content of 
the image data. Returns self ")
	*/
	
	return IONUMBER(GLImage_textureId(DATA(self)->glImage)); 
}

IoObject *IoImage_bindTexture(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("bindTexture", 
		   "Bind the image's texture. If the image has no texture, it creates one and 
scales the image dimensions to powers of 2 and sets the texture to the content of the 
image data. Returns self. ")
	*/
	
	GLImage_bindTexture(DATA(self)->glImage);
	IoImage_checkError(self, locals, m);
	return self;
}

IoObject *IoImage_updateTexture(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("updateTexture", 
		   "Updates the receiver's texture to match it's image data. ")
	*/
	
	GLImage_updateTexture(DATA(self)->glImage);
	IoImage_checkError(self, locals, m);
	return self;
}

IoObject *IoImage_closeTexture(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("closeTexture", 
		   "Deletes the receiver's texture if it has one. Returns self. ")
	*/
	
	GLImage_closeTexture(DATA(self)->glImage);
	IoImage_checkError(self, locals, m);
	return self;
}

IoObject *IoImage_drawTexture(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("drawTexture(optionalWidth, optionalHeight)", 
		   "Draws the receiver as a texture area. Returns self.")
	*/
	
	if (IoMessage_argCount(m) == 0)
	{
		GLImage_drawTexture(DATA(self)->glImage);
	}
	else
	{
		int w = IoMessage_locals_intArgAt_(m, locals, 0);
		int h = IoMessage_locals_intArgAt_(m, locals, 1); 
		GLImage_drawTextureArea(DATA(self)->glImage, w, h);
	}
	IoImage_checkError(self, locals, m);
	return self;
}

IoObject *IoImage_drawScaledTexture(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("drawTexture(optionalWidth, optionalHeight)", 
		   "Draws the receiver as a texture area. Returns self.")
	*/
	
	if (IoMessage_argCount(m) == 0)
	{
		GLImage_drawTexture(DATA(self)->glImage);
	}
	else
	{
		int w = IoMessage_locals_intArgAt_(m, locals, 0);
		int h = IoMessage_locals_intArgAt_(m, locals, 1); 
		GLImage_drawScaledTextureArea(DATA(self)->glImage, w, h);
	}
	IoImage_checkError(self, locals, m);
	return self;
}

/* --- extras -------------------------------------------------------- */

IoObject *IoImage_setEncodingQuality(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("setEncodingQuality(aNumber)", 
		   "Sets the image encoding quality (range is 0.0 - 1.0, 1.0 with being the highest).")
	*/
	
	GLImage_encodingQuality_(DATA(self)->glImage, IoMessage_locals_doubleArgAt_(m, locals, 0));
	return self;
}

IoObject *IoImage_encodingQuality(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("encodingQuality", 
		   "Returns the encodingQuality setting.")
	*/
	
	return IONUMBER(GLImage_encodingQuality(DATA(self)->glImage)); 
}

IoObject *IoImage_setDecodingWidthHint(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	
	
	GLImage_decodingWidthHint_(DATA(self)->glImage, IoMessage_locals_intArgAt_(m, locals, 0));
	return self;
}

IoObject *IoImage_decodingWidthHint(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	
	return IONUMBER(GLImage_decodingWidthHint(DATA(self)->glImage)); 
}

IoObject *IoImage_setDecodingHeightHint(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	
	GLImage_decodingHeightHint_(DATA(self)->glImage, IoMessage_locals_intArgAt_(m, locals, 0));
	return self;
}

IoObject *IoImage_decodingHeightHint(IoImage *self, IoObject *locals, IoMessage *m)
{ 
	
	return IONUMBER(GLImage_decodingHeightHint(DATA(self)->glImage)); 
}

