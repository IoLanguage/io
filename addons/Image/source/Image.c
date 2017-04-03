/*
 docCopyright("Steve Dekorte", 2002)
 docLicense("BSD revised")
 */
#include "Image.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#include <string.h>
#include <ctype.h>
#include <math.h>

#include <RandomGen.h>

#include "PNGImage.h"
#include "JPGImage.h"
#include "TIFFImage.h"


/*
#include <stdarg.h>
 typedef void (*TIFFErrorHandler)(const char* module, const char* fmt, va_list ap);
 TIFFErrorHandler TIFFSetErrorHandler(TIFFErrorHandler handler);
 */

Image *Image_new(void)
{
	Image *self = (Image *)io_calloc(1, sizeof(Image));
	Image_path_(self, "");
	Image_fileType_(self, "");
	self->byteArray = UArray_new();
	self->ownsUArray = 1;
	self->componentCount = 4;
	self->encodingQuality = 1.0;
	return self;
}

Image *Image_newWithPath_(char *path)
{
	Image *self = Image_new();
	Image_path_(self, path);
	Image_load(self);
	return self;
}

Image *Image_copyWithUArray_(Image *self, UArray *ba)
{
	Image *image = Image_new();

	Image_setExternalUArray_(image, ba);
	Image_path_(image, self->path);
	Image_error_(image, self->error);

	image->width = self->width;
	image->height = self->height;
	image->componentCount = self->componentCount;

	image->encodingQuality = self->encodingQuality;
	image->decodingWidthHint = self->decodingWidthHint;
	image->decodingHeightHint = self->decodingHeightHint;

	return image;
}

void Image_free(Image *self)
{
	if (self->ownsUArray)
	{
		UArray_free(self->byteArray);
	}

	if (self->error)
	{
		io_free(self->error);
	}

	io_free(self->fileType);
	io_free(self->path);

	io_free(self);
}

UArray *Image_byteArray(Image *self)
{
	return self->byteArray;
}

void Image_copyUArray_(Image *self, UArray *ba) /* private */
{
	UArray_copy_(self->byteArray, ba);
}

void Image_setExternalUArray_(Image *self, UArray *ba)
{
	if (self->ownsUArray) UArray_free(self->byteArray);
	self->byteArray = ba;
	self->ownsUArray = 0;
}

void Image_getFileType(Image *self) /* private */
{
	char *ext = strrchr(self->path, '.');
	char *e;
	if (!ext) { Image_fileType_(self, ""); return; }

	ext++;
	Image_fileType_(self, ext);
	e = self->fileType;

	while (*e) { *e = tolower(*e); e++; }
	if (strcmp(self->fileType, "jpeg")==0) Image_fileType_(self, "jpg");
}

void Image_path_(Image *self, const char *path)
{
	self->path = strcpy((char *)io_realloc(self->path, strlen(path)+1), path);
	Image_getFileType(self);
}

char *Image_path(Image *self)
{
	return self->path;
}

void Image_fileType_(Image *self, const char *fileType)
{
	self->fileType = strcpy((char *)io_realloc(self->fileType, strlen(fileType)+1), fileType);
}

char *Image_fileType(Image *self) { return self->fileType; }

void Image_error_(Image *self, const char *error)
{
	if (error && strlen(error))
	{
		/*printf("Image_error_(%s)\n", error);*/
		self->error = strcpy((char *)io_realloc(self->error, strlen(error)+1), error);
	}
	else
	{
		if (self->error) io_free(self->error);
		self->error = NULL;
	}
}

char *Image_error(Image *self) { return self->error; }


void Image_setData_width_height_componentCount_(Image *self, UArray *ba, int width, int height, int componentCount)
{
	int size = width * height * componentCount;

	if (size != UArray_size(ba))
	{
		printf("Image_setData_width_height_componentCount_() error - %i x %i x %i = %i, but buffer is %i\n",
			width, height, componentCount, size, (int)UArray_size(ba));
		return;
	}

	Image_copyUArray_(self, ba);
	self->width  = width;
	self->height = height;
	self->componentCount = componentCount;
}

void Image_load(Image *self)
{
	if (strcmp(self->fileType, "png")==0)
	{
		PNGImage *image = PNGImage_new();
		PNGImage_setExternalUArray_(image, self->byteArray);
		PNGImage_path_(image, self->path);
		PNGImage_load(image);
		Image_error_(self, PNGImage_error(image));
		self->width  = PNGImage_width(image);
		self->height = PNGImage_height(image);
		self->componentCount = PNGImage_components(image);
		PNGImage_free(image);
	}
	else if (strcmp(self->fileType, "jpg")==0)
	{
		JPGImage *image = JPGImage_new();
		JPGImage_setExternalUArray_(image, self->byteArray);
		JPGImage_path_(image, self->path);
		JPGImage_decodingWidthHint_(image, self->decodingWidthHint);
		JPGImage_decodingHeightHint_(image, self->decodingHeightHint);
		JPGImage_load(image);
		Image_error_(self, JPGImage_error(image));
		self->width  = JPGImage_width(image);
		self->height = JPGImage_height(image);
		self->componentCount = JPGImage_components(image);
		JPGImage_free(image);
	}
	else if (strcmp(self->fileType, "tif")==0 || strcmp(self->fileType, "tiff")==0)
	{
		TIFFImage *image = TIFFImage_new();
		TIFFImage_setExternalUArray_(image, self->byteArray);
		TIFFImage_path_(image, self->path);
		TIFFImage_load(image);
		Image_error_(self, TIFFImage_error(image));
		self->width  = TIFFImage_width(image);
		self->height = TIFFImage_height(image);
		self->componentCount = TIFFImage_components(image);
		TIFFImage_free(image);
	}
	else
	{
		Image_error_(self, "unknown file type");
	}

	if (UArray_size(self->byteArray) == 0)
	{
		Image_error_(self, "error reading file");
	}

	Image_makeRGBA(self);
}

void Image_save(Image *self)
{
	//Image_flipY(self);

	if (strcmp(self->fileType, "png")==0)
	{
		PNGImage *image = PNGImage_new();
		PNGImage_setExternalUArray_(image, self->byteArray);
		PNGImage_path_(image, self->path);
		PNGImage_width_(image, self->width);
		PNGImage_height_(image, self->height);
		PNGImage_components_(image, Image_componentCount(self));
		PNGImage_save(image);
		Image_error_(self, PNGImage_error(image));
		PNGImage_free(image);
	}
	else if (strcmp(self->fileType, "jpg")==0)
	{
		JPGImage *image = JPGImage_new();
		JPGImage_setExternalUArray_(image, self->byteArray);
		JPGImage_path_(image, self->path);
		JPGImage_quality_(image, self->encodingQuality);
		JPGImage_width_(image, self->width);
		JPGImage_height_(image, self->height);
		
		if (Image_isRGBA8(self))
		{
			Image_removeAlpha(self);
		}
		
		if (!Image_isRGB8(self))
		{
			Image_error_(self, "can only save RGB images to JPEG");
			return;
		}
		JPGImage_components_(image, Image_componentCount(self));
		JPGImage_save(image);
		Image_error_(self, JPGImage_error(image));
		JPGImage_free(image);
	}
	else if (strcmp(self->fileType, "tiff")==0 || strcmp(self->fileType, "tif")==0)
	{
		TIFFImage *image = TIFFImage_new();
		TIFFImage_setExternalUArray_(image, self->byteArray);
		TIFFImage_path_(image, self->path);
		TIFFImage_width_(image, self->width);
		TIFFImage_height_(image, self->height);
		TIFFImage_components_(image, Image_componentCount(self));
		TIFFImage_save(image);
		Image_error_(self, TIFFImage_error(image));
		TIFFImage_free(image);
	}
	else
	{
		Image_error_(self, "unknown file type");
	}
	
	//Image_flipY(self);
}

int Image_width(Image *self)
{
	return self->width;
}

int Image_height(Image *self)
{
	return self->height;
}

int Image_componentCount(Image *self)
{
	return self->componentCount;
}

int Image_isRGB8(Image *self)
{
	return self->componentCount == 3;
}

int Image_isRGBA8(Image *self)
{
	return self->componentCount == 4;
}

int Image_isLA8(Image *self)
{
	return self->componentCount == 2;
}

int Image_isL8(Image *self)
{
	return self->componentCount == 1;
}

int Image_sizeInBytes(Image *self)
{
	return self->height * self->width * self->componentCount;
}

uint8_t *Image_data(Image *self)
{
	return (uint8_t *)UArray_bytes(self->byteArray);
}

void Image_data_length_(Image *self, unsigned char *data, size_t length)
{
	UArray_setData_type_size_copy_(self->byteArray, data, CTYPE_uint8_t, length, 1);
}


void Image_flipX(Image *self)
{
	size_t y;
	size_t w = self->width;
	size_t h = self->height;
	int componentCount = self->componentCount;
	uint8_t *bytes = UArray_mutableBytes(self->byteArray);
	uint8_t buf[4];

	for (y = 0; y < h; y ++)
	{
		size_t x;
		for (x = 0; x < w/2; x ++)
		{
			uint8_t *a = bytes + componentCount * (y * w) + x * componentCount;
			uint8_t *b = bytes + componentCount * (y * w) + (w - x) * componentCount;

			memcpy(buf, a, componentCount);
			memcpy(a,   b, componentCount);
			memcpy(b,   buf, componentCount);
		}
	}
}

void Image_flipY(Image *self)
{
	size_t y;
	size_t w = self->width;
	size_t h = self->height;
	int componentCount = self->componentCount;
	uint8_t *bytes = UArray_mutableBytes(self->byteArray);
	size_t bytesPerLine = componentCount * w;
	unsigned char *buf = io_malloc(bytesPerLine);

	for (y = 0; y < h/2; y ++)
	{
		uint8_t *a = bytes + componentCount * (y * w);
		uint8_t *b = bytes + componentCount * ((h-1-y) * w);

		memcpy(buf, a, bytesPerLine);
		memcpy(a,   b, bytesPerLine);
		memcpy(b,   buf, bytesPerLine);
	}
	
	io_free(buf);
}

void Image_resizeTo(Image *self, int w, int h, Image *outImage)
{
	int componentCount = self->componentCount;

	int inStride = self->width * componentCount;
	uint8_t *inPtr = Image_data(self);

	int outStride = w * componentCount;
	uint8_t *outPtr;
	int y;
	
	UArray *outUArray = UArray_new();
	UArray_setSize_(outUArray, h * outStride);
	outPtr = (uint8_t *)UArray_bytes(outUArray);

	for (y=0; y < self->height; y++, inPtr += inStride, outPtr += outStride)
		memcpy(outPtr, inPtr, inStride);

	Image_setData_width_height_componentCount_(outImage, outUArray, w, h, componentCount);
}

unsigned char *Image_pixelAt(Image *self, int x, int y)
{
	int bps = 8;
	int spp = Image_componentCount(self);
	int w = self->width;
	int h = self->height;
	uint8_t *p = (uint8_t *)UArray_bytes(self->byteArray);

	if (x < 0) { x = 0; } else if (x > w - 1) { x = w - 1; }
	if (y < 0) { y = 0; } else if (y > h - 1) { y = h - 1; }
	return p + (((x + (y * w)) * (spp * bps)) / 8);
}

void Image_crop(Image *self, int cx, int cy, int w, int h)
{
	int pixelSize = Image_componentCount(self);
	int x, y;

	if (cx > self->width)  { Image_error_(self, "crop x > width"); return; }
	if (cy > self->height) { Image_error_(self, "crop y > height"); return; }
	if (cx+w > self->width)  { w = self->width - cx; }
	if (cy+h > self->height) { h = self->height - cy; }

	for (x = 0; x < w; x ++)
	{
		for (y = 0; y < h; y ++)
		{
			unsigned char *ip = Image_pixelAt(self, cx+x, cy+y);
			unsigned char *op = Image_pixelAt(self, x, y);
			memcpy(op, ip, pixelSize);
		}
	}
	UArray_setSize_(self->byteArray, w*h*pixelSize);
	self->width  = w;
	self->height = h;
}

void Image_addAlpha(Image *self)
{
	if (Image_isRGB8(self))
	{
		uint8_t opaqueAlphaValue[1];
		UArray opaqueAlpha;

		opaqueAlphaValue[0] = 255;
		opaqueAlpha = UArray_stackAllocedWithData_type_size_(opaqueAlphaValue, CTYPE_uint8_t, 1);
		UArray_insert_every_(self->byteArray, &opaqueAlpha, 3);
		//UArray_stackFree(&opaqueAlpha);

		self->componentCount = 4;
	}
	if(Image_isL8(self))
	{
		uint8_t opaqueAlphaValue[1];
		UArray opaqueAlpha;

		opaqueAlphaValue[0] = 255;
		opaqueAlpha = UArray_stackAllocedWithData_type_size_(opaqueAlphaValue, CTYPE_uint8_t, 1);
		UArray_insert_every_(self->byteArray, &opaqueAlpha, 1);
		//UArray_stackFree(&opaqueAlpha);

		self->componentCount = 4;
	}
}

void Image_removeAlpha(Image *self)
{
	if (Image_isRGBA8(self))
	{
		UArray_leave_thenRemove_(self->byteArray, 3, 1);
		self->componentCount = 3;
	}
	if (Image_isLA8(self))
	{
		UArray_leave_thenRemove_(self->byteArray, 1, 1);
		self->componentCount = 1;
	}
}

/* --- extras --------------------------------------------------------- */

void Image_encodingQuality_(Image *self, float q)
{
	self->encodingQuality = q;
}

float Image_encodingQuality(Image *self)
{
	return self->encodingQuality;
}

void Image_decodingWidthHint_(Image *self, int v)
{
	self->decodingWidthHint = v;
}

int Image_decodingWidthHint(Image *self)
{
	return self->decodingWidthHint;
}

void Image_decodingHeightHint_(Image *self, int v)
{
	self->decodingHeightHint = v;
}

int Image_decodingHeightHint(Image *self)
{
	return self->decodingHeightHint;
}

void Image_makeRGBA(Image *self)
{
	if (self->componentCount == 3)
	{
		Image_addAlpha(self);
		//printf("converted component count from 3 to 4\n");
	} 
	else if (self->componentCount == 2)
	{
		size_t numPixels = self->width * self->height;
		size_t p1;
		size_t p2 = 0;
		uint8_t *outData;
		uint8_t *inData;
		UArray *outUArray = UArray_new();
		UArray_setItemType_(outUArray, CTYPE_int8_t);
		UArray_setEncoding_(outUArray, CENCODING_NUMBER);
		UArray_setSize_(outUArray, 4 * self->width * self->height);
		outData = (uint8_t *)UArray_mutableBytes(outUArray);
		inData  = (uint8_t *)UArray_bytes(self->byteArray);
		
		for (p1 = 0; p1 < numPixels; p1 ++)
		{
			outData[p2] = inData[p1 * 2];     p2 ++;
			outData[p2] = inData[p1 * 2];     p2 ++;
			outData[p2] = inData[p1 * 2];     p2 ++;
			outData[p2] = inData[p1 * 2 + 1]; p2 ++;
		}
		
		UArray *inUArray = self->byteArray;
		UArray_copyData_(inUArray, outUArray);
		UArray_free(outUArray);
		self->componentCount = 4;
		//printf("converted component count from 2 to 4\n");
	}
	else if (self->componentCount == 1)
	{
		size_t numPixels = self->width * self->height;
		size_t p1;
		size_t p2 = 0;
		uint8_t *outData;
		uint8_t *inData;
		UArray *outUArray = UArray_new();
		UArray_setItemType_(outUArray, CTYPE_int8_t);
		UArray_setEncoding_(outUArray, CENCODING_NUMBER);
		UArray_setSize_(outUArray, 4 * self->width * self->height);
		outData = (uint8_t *)UArray_mutableBytes(outUArray);
		inData  = (uint8_t *)UArray_bytes(self->byteArray);
		
		for (p1 = 0; p1 < numPixels; p1 ++)
		{
			outData[p2] = inData[p1]; p2 ++;
			outData[p2] = inData[p1]; p2 ++;
			outData[p2] = inData[p1]; p2 ++;
			outData[p2] = 255; p2 ++;
		}
		
		UArray *inUArray = self->byteArray;
		UArray_copyData_(inUArray, outUArray);
		UArray_free(outUArray);
		self->componentCount = 4;
		//printf("converted component count from 1 to 4\n");
	}
}

void Image_makeGrayscale(Image *self)
{
	int componentCount = self->componentCount;
	if(componentCount == 3)
	{
		size_t numPixels = self->width * self->height;
		size_t p1;
		uint8_t *outData;
		uint8_t *inData;
		UArray *outUArray = UArray_new();
		UArray_setItemType_(outUArray, CTYPE_int8_t);
		UArray_setEncoding_(outUArray, CENCODING_NUMBER);
		UArray_setSize_(outUArray, numPixels);
		outData = (uint8_t *)UArray_mutableBytes(outUArray);
		inData  = (uint8_t *)UArray_bytes(self->byteArray);
		
		for (p1 = 0; p1 < numPixels; p1 ++)
		{
			outData[p1] = inData[p1 * componentCount + 0] * 0.21 + 
			              inData[p1 * componentCount + 1] * 0.71 +
			              inData[p1 * componentCount + 2] * 0.07;
		}
		
		UArray *inUArray = self->byteArray;
		UArray_copyData_(inUArray, outUArray);
		UArray_free(outUArray);
		self->componentCount = 1;
	} 
	else if(componentCount == 4)
	{
		size_t numPixels = self->width * self->height;
		size_t p1;
		uint8_t *outData;
		uint8_t *inData;
		UArray *outUArray = UArray_new();
		UArray_setItemType_(outUArray, CTYPE_int8_t);
		UArray_setEncoding_(outUArray, CENCODING_NUMBER);
		UArray_setSize_(outUArray, numPixels * 2);
		outData = (uint8_t *)UArray_mutableBytes(outUArray);
		inData  = (uint8_t *)UArray_bytes(self->byteArray);
		
		for (p1 = 0; p1 < numPixels; p1 ++)
		{
			outData[p1 * 2] = inData[p1 * componentCount + 0] * 0.21 + 
			                  inData[p1 * componentCount + 1] * 0.71 +
			                  inData[p1 * componentCount + 2] * 0.07;
			outData[p1 * 2 + 1] = inData[p1 * componentCount + 3];
		}
		
		UArray *inUArray = self->byteArray;
		UArray_copyData_(inUArray, outUArray);
		UArray_free(outUArray);
		self->componentCount = 2;
	}
}

int Image_baselineHeight(Image *self)
{
	//size_t numPixels = self->width * self->height;
	/*
	printf("self->componentCount: %i\n", self->componentCount);
	printf("numPixels: %i\n", (int)numPixels);
	printf("height: %i\n", (int)self->height);
	printf("width: %i\n", (int)self->width);
	*/
	// optimize later
	int componentCount = self->componentCount;
	int base = 0;
	uint8_t *d = (uint8_t *)UArray_bytes(self->byteArray);
	int x, y;
	
	for (y = 0; y < self->height; y ++)
	//for (y = self->height; y > 0; y --)
	{
		for (x = 0; x < self->width; x ++)
		{
			int p = (x + (y * self->width))*componentCount;
			int c;
			
			for (c = 0; c < componentCount; c ++)
			{
				if (d[p + c] < 200)
				{
					base = y;
					break;
				}
			}
		}
	}
	
	//printf("base = %i\n", base);
	return self->height - base;
}

ImageBounds Image_bounds(Image *self, int cutoff)
{
	int componentCount = self->componentCount;
	uint8_t *d = (uint8_t *)UArray_bytes(self->byteArray);
	ImageBounds bounds;
	int x, y;
	
	bounds.xmin = self->width;
	bounds.xmax = 0;
	bounds.ymin = self->height;
	bounds.ymax = 0;

	for (y = 0; y < self->height; y ++)
	{
		for (x = 0; x < self->width; x ++)
		{
			int p = (x + (y * self->width)) * componentCount;
			int c;

			for (c = 0; c < componentCount; c ++)
			{
				if (d[p + c] < cutoff)
				{
					if(x < bounds.xmin) bounds.xmin = x;
					if(x > bounds.xmax) bounds.xmax = x;
					if(y < bounds.ymin) bounds.ymin = y;
					if(y > bounds.ymax) bounds.ymax = y;
					break;
				}
			}
		}
	}
	
	return bounds;
}

ColorStruct Image_averageColor(Image *self)
{
	int componentCount = self->componentCount;
	uint8_t *d = (uint8_t *)UArray_bytes(self->byteArray);
	int x, y, c;
	long cs[4];
	ColorStruct s;
	
	cs[0] = 0;
	cs[1] = 0;
	cs[2] = 0;
	cs[3] = 0;
	
	int p;
	int imageSizeInBytes = self->width * self->height * componentCount;
	for (p = 0; p < imageSizeInBytes; p++)
	{
		c = p % componentCount;
		cs[c] += d[p];
	}

	//printf("color %i %i %i\n", (int)cs[0], (int)cs[1], (int)cs[2]);


	for (c = 0; c < componentCount; c ++)
	{
		cs[c] /= (self->width * self->height);
	}
	
	//printf("color %i %i %i\n", (int)cs[0], (int)cs[1], (int)cs[2]);
	
	if (componentCount == 1)
	{
		s.r = cs[0];
		s.g = cs[0];
		s.b = cs[0];
		s.a = cs[0];
	}
	else
	{
		s.r = cs[0];
		s.g = cs[1];
		s.b = cs[2];
		s.a = cs[3];
	}
	
	//printf("color struct %i %i %i\n", s.r, s.g, s.b);
	
	return s;
}

UArray* Image_histogram(Image *self)
{
	int componentCount = self->componentCount;
	UArray *toReturn = UArray_new();
	UArray_setItemType_(toReturn, CTYPE_int32_t);
	UArray_setEncoding_(toReturn, CENCODING_NUMBER);
	UArray_setSize_(toReturn, componentCount * 256);
	int32_t *outD = (int32_t *)UArray_mutableBytes(toReturn);
	uint8_t *inD = (uint8_t *)UArray_bytes(self->byteArray);
	int p, c;
	
	int imageSizeInBytes = self->width * self->height * componentCount;
	for (p = 0; p < imageSizeInBytes; p++)
	{
		c = p % componentCount;
		outD[inD[p] * componentCount + c] ++;
	}
	
	return toReturn;
}

void Image_equalizeHistogram(Image *self, int mode)
{
	int componentCount = self->componentCount;
	uint8_t *d = (uint8_t *)UArray_mutableBytes(self->byteArray);
	UArray* histogram = Image_histogram(self);
	int32_t* h = (int32_t*) UArray_bytes(histogram);
	uint32_t *left = io_calloc(256 * self->componentCount, sizeof(*left));
	uint32_t *right = io_calloc(256 * self->componentCount, sizeof(*right));
	int hAvg = (self->width * self->height + 255) / 256;
	int hInt, j, c, z, x, y;
	for(c = 0; c < componentCount; c++)
	{
		hInt = 0;
		z = 0;
		for(j = 0; j < 256; j++)
		{
			left[j * componentCount + c] = z;
			hInt += h[j * componentCount + c];
			while(hInt > hAvg)
			{
				hInt -= hAvg;
				z = (z < 255 ? z + 1 : 255);
			}
			right[j * componentCount + c] = z;
		}
	}
	
	RandomGen* random = RandomGen_new();
	
	for (y = 0; y < self->height; y ++)
	{
		for (x = 0; x < self->width; x ++)
		{
			int p = (x + (y * self->width))*componentCount;
			
			for (c = 0; c < componentCount; c ++)
			{
				int ppc = p + c;
				int dppc = d[ppc];
				int l = left[dppc * componentCount + c];
				int r = right[dppc * componentCount + c];
				if(mode == 0)
				{
					d[ppc] = (l + r) / 2;
				}
				else if(mode == 1)
				{
					d[ppc] = l + (r - l + 1) * RandomGen_randomDouble(random);
				}
				else if(mode == 2)
				{
					uint32_t avg = dppc +
					               (x > 0            ? d[ppc - componentCount]               : dppc) +
					               (y > 0            ? d[ppc - componentCount * self->width] : dppc) +
					               (x < self->width  ? d[ppc + componentCount]               : dppc) +
					               (y < self->height ? d[ppc + componentCount * self->width] : dppc);
					avg /= 5;
					d[ppc] = (avg > r ? r : (avg < l ? l : avg));
				}
				else if(mode == 3)
				{
					uint32_t avg = dppc +
					               (x > 0                                   ? d[ppc - componentCount]                     : dppc) +
					               (y > 0                                   ? d[ppc - componentCount * self->width]       : dppc) +
					               (x < self->width                         ? d[ppc + componentCount]                     : dppc) +
					               (y < self->height                        ? d[ppc + componentCount * self->width]       : dppc) +
					               ((x > 0) && (y > 0)                      ? d[ppc - componentCount * (self->width + 1)] : dppc) +
					               ((x > 0) && (y < self->height)           ? d[ppc + componentCount * (self->width - 1)] : dppc) +
					               ((x < self->width) && (y < self->height) ? d[ppc + componentCount * (self->width + 1)] : dppc) +
					               ((x < self->width) && (y > 0)            ? d[ppc - componentCount * (self->width - 1)] : dppc);
					avg /= 9;
					d[ppc] = (avg > r ? r : (avg < l ? l : avg));
				}
			}
		}
	}
	
	io_free(left);
	io_free(right);
	
	UArray_free(histogram);
	RandomGen_free(random);
}

void Image_linearContrast(Image *self)
{
	int componentCount = self->componentCount;
	uint8_t *left = io_malloc(componentCount * sizeof(*left));
	uint8_t *right = io_malloc(componentCount * sizeof(*right));
	memset(left, 0xFF, componentCount);
	memset(right, 0x00, componentCount);
	uint8_t *d = (uint8_t *)UArray_mutableBytes(self->byteArray);
	int p, c;
	
	int imageSizeInBytes = self->width * self->height * componentCount;
	for (p = 0; p < imageSizeInBytes; p++)
	{
		c = p % componentCount;
		left[c] = (left[c] > d[p] ? d[p] : left[c]);
		right[c] = (right[c] < d[p] ? d[p] : right[c]);
	}
	
	for (p = 0; p < imageSizeInBytes; p++)
	{
		c = p % componentCount;
		if(left[c] != right[c])
		{
			d[p] = ((double) d[p] - left[c]) / (right[c] - left[c]) * 255;
		}
	}
	
	io_free(left);
	io_free(right);
}

IOIMAGE_API void Image_bitMask(Image *self, int component, int bitMask)
{
	int componentCount = self->componentCount;
	uint8_t *d = (uint8_t *)UArray_mutableBytes(self->byteArray);
	int p = component;
	
	int imageSizeInBytes = self->width * self->height * componentCount;
	while(p < imageSizeInBytes)
	{
		d[p / componentCount] = d[p] & bitMask;
		p += componentCount;
	}
	self->componentCount = 1;
	UArray_setSize_(self->byteArray, self->width * self->height);
}

IOIMAGE_API void Image_setIntensityInRangeTo_(Image *self, int component, int left, int right, int newValue)
{
	
}

IOIMAGE_API void Image_thresholdByHistogram(Image* self)
{
	Image_removeAlpha(self);
	Image_makeGrayscale(self);
	UArray* histogram = Image_histogram(self);
	int32_t *h = (int32_t*) UArray_bytes(histogram);
	uint8_t *d = (uint8_t *)UArray_mutableBytes(self->byteArray);
	int imageSizeInBytes = self->width * self->height;
	int p;
	int t = 128, mu1, mu2, muCount1, muCount2;
	int tVals[256];
	
	for(p = 0; p < 256; p++)
	{
		tVals[p] = -1;
	}
	
	while(1)
	{
		mu1 = muCount1 = mu2 = muCount2 = 0;
		for(p = 0; p < t; p++)
		{
			mu1 += h[p] * p;
			muCount1 += h[p];
		}
		for(p = t; p < 256; p++)
		{
			mu2 += h[p] * p;
			muCount2 += h[p];
		}
		if(muCount1 > 0) mu1 /= muCount1;
		if(muCount2 > 0) mu2 /= muCount2;
		int newT = (mu1 + mu2) / 2;
		int oldTVals = tVals[newT];
		tVals[newT] = t;
		t = newT;
		if(oldTVals >= 0) break;
	}
	
	int initialT = t;
	int currentT = t;
	int countT = 1;
	do {
		currentT = tVals[currentT];
		t += currentT;
		countT++;
	} while(currentT != initialT);
	
	t /= countT;
	
	for(p = 0; p < imageSizeInBytes; p++)
	{
		if(d[p] >= t)
		{
			d[p] = 255;
		} else {
			d[p] = 0;
		}
	}
	
	UArray_free(histogram);
}

IOIMAGE_API void Image_thresholdByOtsu(Image* self)
{
	Image_removeAlpha(self);
	Image_makeGrayscale(self);
	UArray* histogram = Image_histogram(self);
	int32_t *h = (int32_t*) UArray_bytes(histogram);
	uint8_t *d = (uint8_t *)UArray_mutableBytes(self->byteArray);
	int imageSizeInBytes = self->width * self->height;
	int p;
	int t = 0, curT;
	float sigmaBT = 0; 
	float mu1, mu2, muCount1, muCount2;
	
	for(curT = 0; curT < 256; curT++)
	{
		mu1 = muCount1 = mu2 = muCount2 = 0;
		for(p = 0; p < curT; p++)
		{
			mu1 += h[p] * p;
			muCount1 += h[p];
		}
		for(p = curT; p < 256; p++)
		{
			mu2 += h[p] * p;
			muCount2 += h[p];
		}
		if(muCount1 > 0) mu1 /= muCount1;
		if(muCount2 > 0) mu2 /= muCount2;
		muCount1 /= imageSizeInBytes;
		muCount2 /= imageSizeInBytes;
		float curSigma = (mu1 - mu2) * (mu1 - mu2) * muCount1 * muCount2;
		if(curSigma > sigmaBT)
		{
			t = curT;
			sigmaBT = curSigma;
		}
	}
	
	for(p = 0; p < imageSizeInBytes; p++)
	{
		if(d[p] >= t)
		{
			d[p] = 255;
		} else {
			d[p] = 0;
		}
	}
	
	UArray_free(histogram);
}

IOIMAGE_API void Image_thresholdByGradient(Image* self)
{
	Image_removeAlpha(self);
	Image_makeGrayscale(self);
	uint8_t *d = (uint8_t *)UArray_mutableBytes(self->byteArray);
	int imageSizeInBytes = self->width * self->height;
	int x, y, p, t;
	long sumG = 0;
	long sumGf = 0;
	
	for (y = 1; y < self->height - 1; y ++)
	{
		for (x = 1; x < self->width - 1; x ++)
		{
			int p = (x + (y * self->width));
			int gx = d[p + 1] - d[p - 1];
			int gy = d[p + self->width] - d[p - self->width];
			int g = (gx > gy ? gx : gy);
			sumG  += g;
			sumGf += d[p] * g;
		}
	}
	
	t = sumGf / sumG;
	
	for(p = 0; p < imageSizeInBytes; p++)
	{
		if(d[p] >= t)
		{
			d[p] = 255;
		} else {
			d[p] = 0;
		}
	}
}

IOIMAGE_API Image* Image_applyLinearFilter(Image* self, int filterWidth, int filterHeight, UArray* filter)
{
	int componentCount = self->componentCount;
	
	Image* toReturn = Image_new();
	toReturn->componentCount = componentCount;
	if((self->width < filterWidth) || (self->height < filterHeight) || (filterWidth < 1) || (filterHeight < 1)) return toReturn;
	
	int newWidth = self->width - filterWidth + 1;
	int newHeight = self->height - filterHeight + 1;
	toReturn->width = newWidth;
	toReturn->height = newHeight;

	UArray *toReturnArray = UArray_new();
	UArray_free(toReturn->byteArray);
	toReturn->byteArray = toReturnArray;
	UArray_setItemType_(toReturnArray, CTYPE_int8_t);
	UArray_setEncoding_(toReturnArray, CENCODING_NUMBER);
	UArray_setSize_(toReturnArray, newWidth * newHeight * componentCount);
	uint8_t *outD = (uint8_t *)UArray_mutableBytes(toReturnArray);
	uint8_t *inD = (uint8_t *)UArray_bytes(self->byteArray);
	
	double sum = 0;
	int p;
	for(p = 0; p < filterHeight * filterWidth; p++)
	{
		sum += UArray_doubleAt_(filter, p);
	}
	if(sum == 0)
	{
		sum = 1;
	}
	
	int x, y, c;
	int filterX, filterY;
	
	for(x = 0; x < newWidth; x++)
	{
		for(y = 0; y < newHeight; y++)
		{
			for(c = 0; c < componentCount; c++)
			{
				p = (x + y * self->width) * componentCount + c; 
				double newValue = 0;
				for(filterX = 0; filterX < filterWidth; filterX++)
				{
					for(filterY = 0; filterY < filterHeight; filterY++)
					{
						newValue += UArray_doubleAt_(filter, filterX + filterY * filterWidth) *
						            inD[p + (filterX + filterY * self->width) * componentCount];
					}
				}
				newValue /= sum;
				outD[(x + y * newWidth) * componentCount + c] = (newValue < 256 ? (newValue > 0 ? newValue : 0) : 255);
			}
		}
	}
	
	return toReturn;
}

static uint8_t medianByQSort(uint8_t* buffer, int size)
{
	int leftBound = 0, rightBound = size - 1;
	while(1) {
		if(leftBound == rightBound) return buffer[leftBound];
		int left = leftBound, right = rightBound;
		uint8_t mid = buffer[(left + right) / 2];
		do {
			while(buffer[left] < mid) left ++;
			while(buffer[right] > mid) right --;
			if(left <= right)
			{
				uint8_t temp = buffer[left];
				buffer[left] = buffer[right];
				buffer[right] = temp;
				left++; right--;
			}
		} while(left <= right);
		if(right >= size / 2)
		{
			rightBound = right;
		} else {
			leftBound = left;
		}
	}
}

static uint8_t medianBySelectSort(uint8_t* buffer, int size)
{
	int i, j, k;
	for(i = 0; i <= size / 2; i++)
	{
		k = i;
		for(j = i + 1; j < size; j++)
		{
			if(buffer[k] > buffer[j]) k = j;
		}
		if(k != i)
		{
			uint8_t swap = buffer[i];
			buffer[i] = buffer[k];
			buffer[k] = swap;
		}
	}
	return buffer[size / 2];
}

IOIMAGE_API Image* Image_applyWeightedMedianFilter(Image* self, int filterWidth, int filterHeight, UArray* filter)
{
	int componentCount = self->componentCount;
	
	Image* toReturn = Image_new();
	toReturn->componentCount = componentCount;
	if((self->width < filterWidth) || (self->height < filterHeight) || (filterWidth < 1) || (filterHeight < 1)) return toReturn;
	
	int newWidth = self->width - filterWidth + 1;
	int newHeight = self->height - filterHeight + 1;
	toReturn->width = newWidth;
	toReturn->height = newHeight;

	UArray *toReturnArray = UArray_new();
	UArray_free(toReturn->byteArray);
	toReturn->byteArray = toReturnArray;
	UArray_setItemType_(toReturnArray, CTYPE_int8_t);
	UArray_setEncoding_(toReturnArray, CENCODING_NUMBER);
	UArray_setSize_(toReturnArray, newWidth * newHeight * componentCount);
	uint8_t *outD = (uint8_t *)UArray_mutableBytes(toReturnArray);
	uint8_t *inD = (uint8_t *)UArray_bytes(self->byteArray);
	
	int sum = 0;
	int p;
	for(p = 0; p < filterHeight * filterWidth; p++)
	{
		sum += UArray_longAt_(filter, p);
	}
	if(sum % 2 == 0)
	{
		return toReturn;
	}
	
	uint8_t *buffer = io_malloc(sum);
	int positionInBuffer, weight, positionInWeight;
	
	int x, y, c;
	int filterX, filterY;
	
	for(x = 0; x < newWidth; x++)
	{
		for(y = 0; y < newHeight; y++)
		{
			for(c = 0; c < componentCount; c++)
			{
				p = (x + y * self->width) * componentCount + c; 
				positionInBuffer = 0;
				for(filterX = 0; filterX < filterWidth; filterX++)
				{
					for(filterY = 0; filterY < filterHeight; filterY++)
					{
						weight = UArray_longAt_(filter, filterX + filterY * filterWidth);
						for(positionInWeight = 0; positionInWeight < weight; positionInWeight++)
						{
							buffer[positionInBuffer++] = inD[p + (filterX + filterY * self->width) * componentCount];
						}
					}
				}
				outD[(x + y * newWidth) * componentCount + c] = (sum > 64 ? medianByQSort(buffer, sum) : medianBySelectSort(buffer, sum));
			}
		}
	}
	
	io_free(buffer);	
	return toReturn;
}

IOIMAGE_API Image* Image_applyMinFilter(Image* self, int filterWidth, int filterHeight)
{
	int componentCount = self->componentCount;
	
	Image* toReturn = Image_new();
	toReturn->componentCount = componentCount;
	if((self->width < filterWidth) || (self->height < filterHeight) || (filterWidth < 1) || (filterHeight < 1)) return toReturn;
	
	int newWidth = self->width - filterWidth + 1;
	int newHeight = self->height - filterHeight + 1;
	toReturn->width = newWidth;
	toReturn->height = newHeight;

	UArray *toReturnArray = UArray_new();
	UArray_free(toReturn->byteArray);
	toReturn->byteArray = toReturnArray;
	UArray_setItemType_(toReturnArray, CTYPE_int8_t);
	UArray_setEncoding_(toReturnArray, CENCODING_NUMBER);
	UArray_setSize_(toReturnArray, newWidth * newHeight * componentCount);
	uint8_t *outD = (uint8_t *)UArray_mutableBytes(toReturnArray);
	uint8_t *inD = (uint8_t *)UArray_bytes(self->byteArray);
	
	int p;
	int x, y, c;
	int filterX, filterY;
	uint8_t value, candidate;
	
	for(x = 0; x < newWidth; x++)
	{
		for(y = 0; y < newHeight; y++)
		{
			for(c = 0; c < componentCount; c++)
			{
				p = (x + y * self->width) * componentCount + c; 
				value = inD[p];
				for(filterX = 0; filterX < filterWidth; filterX++)
				{
					for(filterY = 0; filterY < filterHeight; filterY++)
					{
						candidate = inD[p + (filterX + filterY * self->width) * componentCount];
						if(candidate < value) value = candidate;
					}
				}
				outD[(x + y * newWidth) * componentCount + c] = value;
			}
		}
	}
	
	return toReturn;
}

IOIMAGE_API Image* Image_applyMaxFilter(Image* self, int filterWidth, int filterHeight)
{
	int componentCount = self->componentCount;
	
	Image* toReturn = Image_new();
	toReturn->componentCount = componentCount;
	if((self->width < filterWidth) || (self->height < filterHeight) || (filterWidth < 1) || (filterHeight < 1)) return toReturn;
	
	int newWidth = self->width - filterWidth + 1;
	int newHeight = self->height - filterHeight + 1;
	toReturn->width = newWidth;
	toReturn->height = newHeight;

	UArray *toReturnArray = UArray_new();
	UArray_free(toReturn->byteArray);
	toReturn->byteArray = toReturnArray;
	UArray_setItemType_(toReturnArray, CTYPE_int8_t);
	UArray_setEncoding_(toReturnArray, CENCODING_NUMBER);
	UArray_setSize_(toReturnArray, newWidth * newHeight * componentCount);
	uint8_t *outD = (uint8_t *)UArray_mutableBytes(toReturnArray);
	uint8_t *inD = (uint8_t *)UArray_bytes(self->byteArray);
	
	int p;
	int x, y, c;
	int filterX, filterY;
	uint8_t value, candidate;
	
	for(x = 0; x < newWidth; x++)
	{
		for(y = 0; y < newHeight; y++)
		{
			for(c = 0; c < componentCount; c++)
			{
				p = (x + y * self->width) * componentCount + c; 
				value = inD[p];
				for(filterX = 0; filterX < filterWidth; filterX++)
				{
					for(filterY = 0; filterY < filterHeight; filterY++)
					{
						candidate = inD[p + (filterX + filterY * self->width) * componentCount];
						if(candidate > value) value = candidate;
					}
				}
				outD[(x + y * newWidth) * componentCount + c] = value;
			}
		}
	}
	
	return toReturn;
}

IOIMAGE_API Image* Image_applyNonlinearGradientsFilter(Image* self)
{
	int componentCount = self->componentCount;
	
	Image* toReturn = Image_new();
	toReturn->componentCount = componentCount;
	
	int width = self->width;
	int height = self->height;
	toReturn->width = width - 2;
	toReturn->height = height - 2;

	UArray *toReturnArray = UArray_new();
	UArray_free(toReturn->byteArray);
	toReturn->byteArray = toReturnArray;
	UArray_setItemType_(toReturnArray, CTYPE_int8_t);
	UArray_setEncoding_(toReturnArray, CENCODING_NUMBER);
	UArray_setSize_(toReturnArray, (width - 2) * (height - 2) * componentCount);
	uint8_t *outD = (uint8_t *)UArray_mutableBytes(toReturnArray);
	uint8_t *inD = (uint8_t *)UArray_bytes(self->byteArray);
	
	int p;
	int x, y, c;
	int value;
	
	for(x = 1; x < width - 1; x++)
	{
		for(y = 1; y < height - 1; y++)
		{
			for(c = 0; c < componentCount; c++)
			{
				int value = abs((int) inD[((x) + (y - 1) * width) * componentCount + c] - (int) inD[((x) + (y + 1) * width) * componentCount + c]) +
					    abs((int) inD[((x - 1) + (y) * width) * componentCount + c] - (int) inD[((x + 1) + (y) * width) * componentCount + c]);
				outD[((x - 1) + (y - 1) * (width - 2)) * componentCount + c] = (value < 256) ? value : 255;
			}
		}
	}
	
	return toReturn;
}


