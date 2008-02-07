
//metadoc Image copyright Steve Dekorte, 2004
//metadoc Image license BSD revised
//metadoc Image category Graphics
/*metadoc Image description
The Image object can read and draw images and provide the image 
data as a buffer. Example use;
<pre>	
image = Image clone open("curly.png")
image draw
image scaleTo(image width / 2, image height / 2)
image save("curly.tiff")
</pre>	
When loading an attempt will be made to convert the image into whichever 
of the following formats it is closest to: L8, LA8, RGB8, RGBA8.
<p>
Currently supported formats include PNG(which supports alpha), JPG and TIFF.
*/

#include "IoImage.h"
#include "List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "Image.h"

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
	DATA(self)->image = Image_new();
	Image_setExternalUArray_(DATA(self)->image, IoSeq_rawUArray(DATA(self)->buffer));

	IoState_registerProtoWithFunc_(state, self, IoImage_proto);

	{
		IoMethodTable methodTable[] = {
		{"setDataWidthHeightComponentCount", IoImage_setDataWidthHeightComponentCount},
		{"setPath", IoImage_setPath},
		{"open", IoImage_open},
		{"save", IoImage_save},

		{"width", IoImage_width},
		{"height", IoImage_height},

		{"data", IoImage_data},
		{"componentCount", IoImage_componentCount},
		{"isL8", IoImage_isL8},
		{"isLA8", IoImage_isLA8},
		{"isRGB8", IoImage_isRGB8},
		{"isRGBA8", IoImage_isRGBA8},
		{"error", IoImage_error},
		{"resizedTo", IoImage_resizedTo},
		{"crop", IoImage_crop},

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
	DATA(self)->image = Image_copyWithUArray_(DATA(proto)->image, IoSeq_rawUArray(DATA(self)->buffer));
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
	Image_free(DATA(self)->image);
	free(IoObject_dataPointer(self));
}

void IoImage_mark(IoImage *self)
{
	IoObject_shouldMark(DATA(self)->buffer);
}

Image *IoImage_image(IoImage *self)
{
	return DATA(self)->image;
}

Image *IoImage_rawImage(IoImage *self)
{
	return DATA(self)->image;
}

/* ----------------------------------------------------------- */

IoObject *IoImage_path(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image path
	Returns the image path.
	*/

	return IOSYMBOL(Image_path(DATA(self)->image));
}

IoObject *IoImage_setPath(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image setPath(aString)
	Sets the image path. Returns self.
	*/

	IoSymbol *s = IoMessage_locals_symbolArgAt_(m, locals, 0);
	Image_path_(DATA(self)->image, CSTRING(s));
	return self;
}

void IoImage_checkError(IoImage *self, IoObject *locals, IoMessage *m)
{
	const char *e = Image_error(DATA(self)->image);

	if (e != NULL)
	{
		IoState_error_(IOSTATE, m, "Image %s on %s", e, Image_path(DATA(self)->image));
	}
}


IoObject *IoImage_setDataWidthHeightComponentCount(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image setDataWidthHeightComponentCount(aSequence, width, height, componentCount)
	Sets the image data and it's parameters. Returns self.
	*/

	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 0);
	int w = IoMessage_locals_intArgAt_(m, locals, 1);
	int h = IoMessage_locals_intArgAt_(m, locals, 2);
	int c = IoMessage_locals_intArgAt_(m, locals, 3);

	printf("Image Image_setData_width_height_componentCount_\n");
	Image_setData_width_height_componentCount_(DATA(self)->image, IoSeq_rawUArray(data), w, h, c);
	printf("Image returning self\n");
	return self;
}

IoObject *IoImage_open(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image open(optionalPathString)
	Sets the path to optionalPathString if provided and opens the image file. 
	Returns self on success, Nil on failure.
	*/

	/*printf("opening Image %p %s\n", self, Image_path(DATA(self)->image));*/

	if (IoMessage_argCount(m) > 0)
	{
		IoSymbol *path = IoMessage_locals_symbolArgAt_(m, locals, 0);
		Image_path_(DATA(self)->image, CSTRING(path));
	}

	Image_load(DATA(self)->image);
	IoImage_checkError(self, locals, m);
	return self;
}

IoObject *IoImage_save(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image save(optionalPathString)
	Sets the path to optionalPathString if provided and saves the image 
	in the format specified by the path extension. Returns self on success, nil on failure.
	*/

	if (IoMessage_argCount(m) > 0)
	{
		IoSymbol *path = IoMessage_locals_symbolArgAt_(m, locals, 0);
		Image_path_(DATA(self)->image, CSTRING(path));
	}

	Image_save(DATA(self)->image);
	IoImage_checkError(self, locals, m);
	return self;
}

IoObject *IoImage_width(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image width
	Returns the image width.
	*/

	return IONUMBER(Image_width(DATA(self)->image));
}

IoObject *IoImage_height(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image height
	Returns the image hieght.
	*/

	return IONUMBER(Image_height(DATA(self)->image));
}

IoObject *IoImage_data(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image data
	Returns a Buffer primitive containing the image data (loading it first if needed). 
	Manipulating this data will effect what is drawn when the receiver's draw method is called.
	*/

	return DATA(self)->buffer;
}

IoObject *IoImage_componentCount(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image componentCount
	Returns the number of color components in the receiver as a Number.
	*/
	
	return IONUMBER(Image_componentCount(DATA(self)->image));
}

IoObject *IoImage_error(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image error
	Returns a String containing the current error or nil if there is no error.
	*/

	char *s = (char *)Image_error(DATA(self)->image);

	if ((!s) || (!strlen(s)))
	{
		return IONIL(self);
	}

	return IOSYMBOL(s);
}

IoObject *IoImage_isRGB8(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image isRGB8
	Returns true if the receiver is in RGB8 format, false otherwise.
	*/

	return IOBOOL(self, Image_componentCount(DATA(self)->image) == 3);
}

IoObject *IoImage_isRGBA8(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image isRGBA8
	Returns true if the receiver is in RGBA8 format, false otherwise.
	*/

	return IOBOOL(self, Image_componentCount(DATA(self)->image) == 4);
}

IoObject *IoImage_isL8(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image isL8
	Returns true if the receiver is in L8 (8bit Luminance) format, false otherwise.
	*/

	return IOBOOL(self, Image_componentCount(DATA(self)->image) == 1);
}

IoObject *IoImage_isLA8(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image isLA8
	Returns true if the receiver is in LA8 (8bit Luminance-Alpha) format, false otherwise.
	*/

	return IOBOOL(self, Image_componentCount(DATA(self)->image) == 2);
}

IoObject *IoImage_crop(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image crop(x, y, width, height)
	Crops the image to the specified values. Returns self.
	Raises an exception on error.
	*/

	int cx = IoMessage_locals_intArgAt_(m, locals, 0);
	int cy = IoMessage_locals_intArgAt_(m, locals, 1);
	int width = IoMessage_locals_intArgAt_(m, locals, 2);
	int height = IoMessage_locals_intArgAt_(m, locals, 3);

	Image_crop(DATA(self)->image, cx, cy, width, height);
	IoImage_checkError(self, locals, m);
	return self;
}

IoObject *IoImage_resizedTo(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image resizedTo(width, height)
	Returns a new image of the receiver resized to the given width and height.
	Raises an exception on error.
	*/
	int w = IoMessage_locals_intArgAt_(m, locals, 0);
	int h = IoMessage_locals_intArgAt_(m, locals, 1);

	IoImage *outImage = IoImage_new(IOSTATE);

	IoState_error_(IOSTATE, m, "Image resizeTo");

	Image_resizeTo(DATA(self)->image, w, h, DATA(outImage)->image);
	IoImage_checkError(self, locals, m);
	return outImage;
}


/* --- extras -------------------------------------------------------- */

IoObject *IoImage_setEncodingQuality(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image setEncodingQuality(aNumber)
	Sets the image encoding quality (range is 0.0 - 1.0, 1.0 with being the highest).
	*/

	Image_encodingQuality_(DATA(self)->image, IoMessage_locals_doubleArgAt_(m, locals, 0));
	return self;
}

IoObject *IoImage_encodingQuality(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image encodingQuality
	Returns the encodingQuality setting.
	*/

	return IONUMBER(Image_encodingQuality(DATA(self)->image));
}

IoObject *IoImage_setDecodingWidthHint(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image setDecodingWidthHint(width)
	Sets the decoding width hint. Returns self.
	*/

	Image_decodingWidthHint_(DATA(self)->image, IoMessage_locals_intArgAt_(m, locals, 0));
	return self;
}

IoObject *IoImage_decodingWidthHint(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image decodingWidthHint
	Returns the decoding width hint.
	*/
	
	return IONUMBER(Image_decodingWidthHint(DATA(self)->image));
}

IoObject *IoImage_setDecodingHeightHint(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image setDecodingHeightHint(width)
	Sets the decoding height hint. Returns self.
	*/
	
	Image_decodingHeightHint_(DATA(self)->image, IoMessage_locals_intArgAt_(m, locals, 0));
	return self;
}

IoObject *IoImage_decodingHeightHint(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image decodingHeightHint
	Returns the decoding height hint.
	*/
	return IONUMBER(Image_decodingHeightHint(DATA(self)->image));
}

