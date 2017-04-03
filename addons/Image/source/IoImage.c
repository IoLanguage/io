
//metadoc Image copyright Steve Dekorte, 2004
//metadoc Image license BSD revised
//metadoc Image category Graphics
/*metadoc Image description
The Image object can read and draw images and provide the image 
data as a buffer. Example use:
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
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "Image.h"
#include <math.h>

#define DATA(self) ((IoImageData *)IoObject_dataPointer(self))

static const char *protoId = "Image";

IoTag *IoImage_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
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

	IoObject_setDataPointer_(self, io_calloc(1, sizeof(IoImageData)));

	DATA(self)->buffer = IoSeq_newWithCString_(IOSTATE, "");
	DATA(self)->image = Image_new();
	Image_setExternalUArray_(DATA(self)->image, IoSeq_rawUArray(DATA(self)->buffer));

	IoState_registerProtoWithId_(state, self, protoId);

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
		{"isGrayscale", IoImage_isL8},
		{"isL8", IoImage_isL8},
		{"isLA8", IoImage_isLA8},
		{"isRGB8", IoImage_isRGB8},
		{"isRGBA8", IoImage_isRGBA8},
		{"error", IoImage_error},
		{"resizedTo", IoImage_resizedTo},
		{"crop", IoImage_crop},
		{"addAlpha", IoImage_addAlpha},
		{"removeAlpha", IoImage_removeAlpha},
		
		{"makeRGBA8", IoImage_makeRGBA8},
		{"makeL8", IoImage_makeL8},
		{"makeGrayscale", IoImage_makeGrayscale},

		// extras

		{"setEncodingQuality", IoImage_setEncodingQuality},
		{"encodingQuality", IoImage_encodingQuality},

		{"setDecodingWidthHint", IoImage_setDecodingWidthHint},
		{"decodingWidthHint", IoImage_decodingWidthHint},

		{"setDecodingHeightHint", IoImage_setDecodingHeightHint},
		{"decodingHeightHint", IoImage_decodingHeightHint},

		{"flipX", IoImage_flipX},
		{"flipY", IoImage_flipY},
		
		{"baselineHeight", IoImage_baselineHeight},
		{"averageColor", IoImage_averageColor},
		{"histogram", IoImage_histogram},
		{"equalizeHistogram", IoImage_equalizeHistogram},
		{"linearContrast", IoImage_linearContrast},
		{"bitPlain", IoImage_bitPlain},
		{"componentPlain", IoImage_componentPlain},
		
		{"thresholdByGradient", IoImage_thresholdByGradient},
		{"thresholdByHistogram", IoImage_thresholdByHistogram},
		{"thresholdByOtsu", IoImage_thresholdByOtsu},
		
		{"filterLinear", IoImage_filterLinear},
		{"filterUniformAverage", IoImage_filterUniformAverage},
		{"filterGauss", IoImage_filterGauss},
		{"filterKirsch", IoImage_filterKirsch},
		{"filterSobel", IoImage_filterSobel},
		{"filterUnsharpMask", IoImage_filterUnsharpMask},
		{"filterMin", IoImage_filterMin},
		{"filterMax", IoImage_filterMax},
		{"filterMedian", IoImage_filterMedian},
		{"filterWeightedMedian", IoImage_filterWeightedMedian},

		{"bounds", IoImage_bounds},

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
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

IOIMAGE_API IoImage *IoImage_newWithImage_(void *state, Image* image)
{
	IoImage* self = IoImage_new(state);
	DATA(self)->image = image;
	DATA(self)->buffer = IoSeq_newWithData_length_(state, Image_data(image), Image_sizeInBytes(image));
	Image_setExternalUArray_(image, IoSeq_rawUArray(DATA(self)->buffer));
	return self;
}

/* ----------------------------------------------------------- */

void IoImage_free(IoImage *self)
{
	Image_free(DATA(self)->image);
	io_free(IoObject_dataPointer(self));
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
	Sets the image data and its parameters. Returns self.
	*/

	IoSeq *data = IoMessage_locals_seqArgAt_(m, locals, 0);
	int w = IoMessage_locals_intArgAt_(m, locals, 1);
	int h = IoMessage_locals_intArgAt_(m, locals, 2);
	int c = IoMessage_locals_intArgAt_(m, locals, 3);

	//printf("Image Image_setData_width_height_componentCount_\n");
	Image_setData_width_height_componentCount_(DATA(self)->image, IoSeq_rawUArray(data), w, h, c);
	//printf("Image returning self\n");
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
	Manipulating this data will affect what is drawn when the receiver's draw method is called.
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

	return IOBOOL(self, Image_isRGB8(DATA(self)->image));
}

IoObject *IoImage_isRGBA8(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image isRGBA8
	Returns true if the receiver is in RGBA8 format, false otherwise.
	*/

	return IOBOOL(self, Image_isRGBA8(DATA(self)->image));
}

IoObject *IoImage_isL8(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image isL8
	Returns true if the receiver is in L8 (8bit Luminance) format, false otherwise.
	*/

	return IOBOOL(self, Image_isL8(DATA(self)->image));
}

IoObject *IoImage_isLA8(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image isLA8
	Returns true if the receiver is in LA8 (8bit Luminance-Alpha) format, false otherwise.
	*/

	return IOBOOL(self, Image_componentCount(DATA(self)->image) == 2);
}

IoObject *IoImage_makeRGBA8(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image makeRGBA8
	Converts color model to RGBA 8bit. Returns self.
	*/
	Image_makeRGBA(DATA(self)->image);
	return self;
}

IoObject *IoImage_makeL8(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image makeL8
	Converts color model to Luminance 8bit (grayscale). Returns self.
	*/
	Image_removeAlpha(DATA(self)->image);
	Image_makeGrayscale(DATA(self)->image);
	return self;
}

IoObject *IoImage_makeGrayscale(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image makeGrayscale
	Converts color model to Luminance or Luminance-Alpha 8bit (grayscale). Returns self.
	*/
	Image_makeGrayscale(DATA(self)->image);
	return self;
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

IoObject *IoImage_addAlpha(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image addAlpha
	Adds an opaque alpha component if the image is in RGB format and does not already contain one.  Returns self.
	*/

	Image_addAlpha(DATA(self)->image);
	return self;
}

IoObject *IoImage_removeAlpha(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image removeAlpha
	Removes the alpha component if the image contains one.  Returns self.
	*/

	Image_removeAlpha(DATA(self)->image);
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

	IoState_error_(IOSTATE, m, "Image resizedTo is broken - use Texture for scaling");

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


IoObject *IoImage_flipX(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image flipX
	Flips the image on the horizonal plane (left/right mirror). Returns self.
	*/
	
	Image_flipX(DATA(self)->image);
	return self;
}

IoObject *IoImage_flipY(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image flipY
	Flips the image on the vertical plane (top/bottom mirror). Returns self.
	*/
	
	Image_flipY(DATA(self)->image);
	return self;
}

IoObject *IoImage_baselineHeight(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image baselineHeight
	Returns the pixel height (relative to the bottom of the image) which first contains a non-white pixel or -1 if
	no baseline is found.
	*/
	
	return IONUMBER(Image_baselineHeight(DATA(self)->image));
}

IOIMAGE_API IoObject *IoImage_bounds(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image bounds(cutoff)
	Returns an object continaing the bounds of the image. Cutoff is max bound color value for any color component.
	If it is negative, it is the min bound color value.
	*/
	int cutoff = IoMessage_locals_intArgAt_(m, locals, 0);
	ImageBounds b = Image_bounds(DATA(self)->image, cutoff);
	IoObject *bounds = IoObject_new(IOSTATE);
	
	IoObject_setSlot_to_(bounds, IOSYMBOL("xmin"), IONUMBER(b.xmin));
	IoObject_setSlot_to_(bounds, IOSYMBOL("ymin"), IONUMBER(b.ymin));
	IoObject_setSlot_to_(bounds, IOSYMBOL("xmax"), IONUMBER(b.xmax));
	IoObject_setSlot_to_(bounds, IOSYMBOL("ymax"), IONUMBER(b.ymax));
	
	return bounds;
}


IoObject *IoImage_averageColor(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image averageColor
	Returns the average RGB color of all pixels in the image. 
	*/
	
	ColorStruct c = Image_averageColor(DATA(self)->image);
	vec3f v;
	
	v.x = c.r;
	v.y = c.g;
	v.z = c.b;
	
	return IoSeq_newVec3f(IOSTATE, v);
}

IoObject *IoImage_histogram(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image histogram
	Returns array with histogram values interleaved by channels.
	*/
	return IoSeq_newWithUArray_copy_(IOSTATE, Image_histogram(DATA(self)->image), 0);
}

IoObject *IoImage_equalizeHistogram(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image equalizeHistogram(mode)
	Performs histogram equalization. Mode denotes quality factor and processing speed. Mode should be in range [0..3].
	Returns self.
	*/
	int arg = IoMessage_locals_intArgAt_(m, locals, 0);
	Image_equalizeHistogram(DATA(self)->image, (arg > 3 ? 3 : (arg < 0 ? 0 : arg)));
	return self;
}

IoObject *IoImage_linearContrast(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image linearContrast
	Performs contrast linarization. Per-pixel per-channel transformation that extends intensity to its full range.
	Returns self.
	*/
	Image_linearContrast(DATA(self)->image);
	return self;
}

IOIMAGE_API IoObject *IoImage_componentPlain(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image componentPlain(component)
	Removes other components except specified from the image.
	Returns self.
	*/
	int component = IoMessage_locals_intArgAt_(m, locals, 0) % Image_componentCount(DATA(self)->image);
	Image_bitMask(DATA(self)->image, component, 255); 
	return self;
}

IOIMAGE_API IoObject *IoImage_bitPlain(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image bitPlain(component, bit)
	Replaces contents with black-and-white image where white appears if and only if specified component contained 1 on the specified bit plain.
	Returns self.
	*/
	int component = IoMessage_locals_intArgAt_(m, locals, 0) % Image_componentCount(DATA(self)->image);
	int bit = IoMessage_locals_intArgAt_(m, locals, 0) % 8;
	Image_bitMask(DATA(self)->image, component, 1 << bit);
	UArray* imageByteArray = Image_byteArray(DATA(self)->image);
	UArray_multiplyScalarDouble_(imageByteArray, (1 << (7 - bit)));
	return self;
}

IoObject *IoImage_thresholdByGradient(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image thresholdByGradient
	Performs thresholding. Threshold value is finded using gradients method.
	Returns self.
	*/
	Image_thresholdByGradient(DATA(self)->image);
	return self;
}

IoObject *IoImage_thresholdByHistogram(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image thresholdByGradient
	Performs thresholding. Threshold value is finded using histogram splitting method.
	Returns self.
	*/
	Image_thresholdByHistogram(DATA(self)->image);
	return self;
}

IoObject *IoImage_thresholdByOtsu(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image thresholdByGradient
	Performs thresholding. Threshold value is finded using Otsu's method.
	Returns self.
	*/
	Image_thresholdByOtsu(DATA(self)->image);
	return self;
}


IOIMAGE_API IoObject *IoImage_filterLinear(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image filterLinear(filterSizeX, filterSizeY, filter)
	Returns new image as a result of applying filter.
	*/
	int filterSizeX = IoMessage_locals_intArgAt_(m, locals, 0);
	int filterSizeY = IoMessage_locals_intArgAt_(m, locals, 1);
	IoObject *ioFilter = IoMessage_locals_seqArgAt_(m, locals, 2);
	if(IoSeq_rawSize(ioFilter) < filterSizeX * filterSizeY)
	{
		IoState_error_(IOSTATE, m, "filter should be a Sequence with size >= filterSizeX * filterSizeY");
		return self;
	}
	UArray* filter = IoSeq_rawUArray(ioFilter);
	return IoImage_newWithImage_(IOSTATE, Image_applyLinearFilter(DATA(self)->image, filterSizeX, filterSizeY, filter));
}

IOIMAGE_API IoObject *IoImage_filterUniformAverage(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image filterUniformAverage(filterSizeX, filterSizeY)
	Returns new image as a result of applying filter. Implements low pass filtering.
	*/
	int filterSizeX = IoMessage_locals_intArgAt_(m, locals, 0);
	int filterSizeY = IoMessage_locals_intArgAt_(m, locals, 1);
	UArray* filter = UArray_new();
	UArray_setItemType_(filter, CTYPE_int8_t);
	UArray_setEncoding_(filter, CENCODING_NUMBER);
	UArray_setSize_(filter, filterSizeX * filterSizeY);
	memset(UArray_mutableBytes(filter), 1, filterSizeX * filterSizeY);
	IoImage* toReturn = IoImage_newWithImage_(IOSTATE, Image_applyLinearFilter(DATA(self)->image, filterSizeX, filterSizeY, filter));
	UArray_free(filter);
	return toReturn;
}

IOIMAGE_API IoObject *IoImage_filterGauss(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image filterGauss(sigma)
	Returns new image as a result of applying filter. Implements Gauss smoothing filtering with parameter sigma.
	*/
	double sigma = IoMessage_locals_doubleArgAt_(m, locals, 0);
	int filterSize = round(sigma * 2.5) * 2 + 1;
	UArray* filter = UArray_new();
	UArray_setItemType_(filter, CTYPE_int8_t);
	UArray_setEncoding_(filter, CENCODING_NUMBER);
	UArray_setSize_(filter, filterSize * filterSize);
	int8_t *filterBytes = UArray_mutableBytes(filter);
	int x, y, x1, y1;
	for(y = 0; y < filterSize; y++)
	{
		y1 = y - filterSize / 2;
		for(x = 0; x < filterSize; x++)
		{
			x1 = x - filterSize / 2;
			filterBytes[x + y * filterSize] = exp(-(x1*x1 + y1*y1)/2/sigma) * filterSize * filterSize * 2;
		}
	}
	IoImage* toReturn = IoImage_newWithImage_(IOSTATE, Image_applyLinearFilter(DATA(self)->image, filterSize, filterSize, filter));
	UArray_free(filter);
	return toReturn;
}

IOIMAGE_API IoObject *IoImage_filterUnsharpMask(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image filterUnsharpMask(a)
	Returns new image as a result of applying filter. Implements unsharp mask filtering. 
	The result is sharpened image.
	The parameter value may by any but it makes sense if it is > 0.
	*/
	int a = IoMessage_locals_intArgAt_(m, locals, 0);
	UArray* filter = UArray_new();
	UArray_setItemType_(filter, CTYPE_int8_t);
	UArray_setEncoding_(filter, CENCODING_NUMBER);
	UArray_setSize_(filter, 9);
	int8_t *filterBytes = UArray_mutableBytes(filter);
	
	filterBytes[0] = -1; filterBytes[1] = -1;    filterBytes[2] = -1;
	filterBytes[3] = -1; filterBytes[4] = a + 8; filterBytes[5] = -1;
	filterBytes[6] = -1; filterBytes[7] = -1;    filterBytes[8] = -1;
	IoImage* toReturn = IoImage_newWithImage_(IOSTATE, Image_applyLinearFilter(DATA(self)->image, 3, 3, filter));
	UArray_free(filter);
	return toReturn;
}

IOIMAGE_API IoObject *IoImage_filterSobel(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image filterSobel(a)
	Returns new image as a result of applying Sobel filter.
	The argument denotes direction: 0, 1, 2, ... -> 0, pi / 4, pi / 2, ...
	*/
	int a = IoMessage_locals_intArgAt_(m, locals, 0);
	a = ((a % 8) + 8) % 8;
	static int mapOfPixels[8] = {0, 1, 2, 5, 8, 7, 6, 3};
	static int contentsOfPixels[8] = {-1, -2, -1, 0, 1, 2, 1, 0};
	UArray* filter = UArray_new();
	UArray_setItemType_(filter, CTYPE_int8_t);
	UArray_setEncoding_(filter, CENCODING_NUMBER);
	UArray_setSize_(filter, 9);
	int8_t *filterBytes = UArray_mutableBytes(filter);
	int i;
	for(i = 0; i < 8; i++)
	{
		filterBytes[(i + a) % 8] = contentsOfPixels[i];
	}
	IoImage* toReturn = IoImage_newWithImage_(IOSTATE, Image_applyLinearFilter(DATA(self)->image, 3, 3, filter));
	UArray_free(filter);
	return toReturn;
}

IOIMAGE_API IoObject *IoImage_filterKirsch(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image filterKirsch(a)
	Returns new image as a result of applying Kirsch filter.
	The argument denotes direction: 0, 1, 2, ... -> 0, pi / 4, pi / 2, ...
	*/
	int a = IoMessage_locals_intArgAt_(m, locals, 0);
	a = ((a % 8) + 8) % 8;
	static int mapOfPixels[8] = {0, 1, 2, 5, 8, 7, 6, 3};
	static int contentsOfPixels[8] = {3, 3, 3, 3, -5, -5, -5, 3};
	UArray* filter = UArray_new();
	UArray_setItemType_(filter, CTYPE_int8_t);
	UArray_setEncoding_(filter, CENCODING_NUMBER);
	UArray_setSize_(filter, 9);
	int8_t *filterBytes = UArray_mutableBytes(filter);
	int i;
	for(i = 0; i < 8; i++)
	{
		filterBytes[(i + a) % 8] = contentsOfPixels[i];
	}
	IoImage* toReturn = IoImage_newWithImage_(IOSTATE, Image_applyLinearFilter(DATA(self)->image, 3, 3, filter));
	UArray_free(filter);
	return toReturn;
}

IOIMAGE_API IoObject *IoImage_filterMin(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image filterMin(filterSizeX, filterSizeY)
	Returns new image as a result of applying filter.
	*/
	int filterSizeX = IoMessage_locals_intArgAt_(m, locals, 0);
	int filterSizeY = IoMessage_locals_intArgAt_(m, locals, 1);
	return IoImage_newWithImage_(IOSTATE, Image_applyMinFilter(DATA(self)->image, filterSizeX, filterSizeY));
}

IOIMAGE_API IoObject *IoImage_filterMax(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image filterMax(filterSizeX, filterSizeY)
	Returns new image as a result of applying filter.
	*/
	int filterSizeX = IoMessage_locals_intArgAt_(m, locals, 0);
	int filterSizeY = IoMessage_locals_intArgAt_(m, locals, 1);
	return IoImage_newWithImage_(IOSTATE, Image_applyMaxFilter(DATA(self)->image, filterSizeX, filterSizeY));
}

IOIMAGE_API IoObject *IoImage_filterMedian(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image filterMedian(filterSizeX, filterSizeY)
	Returns new image as a result of applying filter.
	*/
	int filterSizeX = IoMessage_locals_intArgAt_(m, locals, 0);
	int filterSizeY = IoMessage_locals_intArgAt_(m, locals, 1);
	UArray* filter = UArray_new();
	UArray_setItemType_(filter, CTYPE_int8_t);
	UArray_setEncoding_(filter, CENCODING_NUMBER);
	UArray_setSize_(filter, filterSizeX * filterSizeY);
	memset(UArray_mutableBytes(filter), 1, filterSizeX * filterSizeY);
	IoImage* toReturn = IoImage_newWithImage_(IOSTATE, Image_applyWeightedMedianFilter(DATA(self)->image, filterSizeX, filterSizeY, filter));
	UArray_free(filter);
	return toReturn;
}

IOIMAGE_API IoObject *IoImage_filterWeightedMedian(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image filterWeightedMedian(filterSizeX, filterSizeY, filter)
	Returns new image as a result of applying filter.
	*/
	int filterSizeX = IoMessage_locals_intArgAt_(m, locals, 0);
	int filterSizeY = IoMessage_locals_intArgAt_(m, locals, 1);
	IoObject *ioFilter = IoMessage_locals_seqArgAt_(m, locals, 2);
	if(IoSeq_rawSize(ioFilter) < filterSizeX * filterSizeY)
	{
		IoState_error_(IOSTATE, m, "filter should be a Sequence with size >= filterSizeX * filterSizeY");
		return self;
	}
	UArray* filter = IoSeq_rawUArray(ioFilter);
	return IoImage_newWithImage_(IOSTATE, Image_applyWeightedMedianFilter(DATA(self)->image, filterSizeX, filterSizeY, filter));
}

IOIMAGE_API IoObject *IoImage_filterNonlinearGradients(IoImage *self, IoObject *locals, IoMessage *m)
{
	/*doc Image filterNonlinearGradients
	Returns new image as a result of applying filter. Calculates abs(f'x) + abs(f'y).
	*/
	return IoImage_newWithImage_(IOSTATE, Image_applyNonlinearGradientsFilter(DATA(self)->image));
}

