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
	Image *self = (Image *)calloc(1, sizeof(Image));
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
		free(self->error);
	}

	free(self->fileType);
	free(self->path);

	free(self);
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
	self->path = strcpy((char *)realloc(self->path, strlen(path)+1), path);
	Image_getFileType(self);
}

char *Image_path(Image *self)
{
	return self->path;
}

void Image_fileType_(Image *self, const char *fileType)
{
	self->fileType = strcpy((char *)realloc(self->fileType, strlen(fileType)+1), fileType);
}

char *Image_fileType(Image *self) { return self->fileType; }

void Image_error_(Image *self, const char *error)
{
	if (error && strlen(error))
	{
		/*printf("Image_error_(%s)\n", error);*/
		self->error = strcpy((char *)realloc(self->error, strlen(error)+1), error);
	}
	else
	{
		if (self->error) free(self->error);
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
	unsigned char *buf = malloc(bytesPerLine);

	for (y = 0; y < h/2; y ++)
	{
		uint8_t *a = bytes + componentCount * (y * w);
		uint8_t *b = bytes + componentCount * ((h-1-y) * w);

		memcpy(buf, a, bytesPerLine);
		memcpy(a,   b, bytesPerLine);
		memcpy(b,   buf, bytesPerLine);
	}
	
	free(buf);
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

inline unsigned char *Image_pixelAt(Image *self, int x, int y)
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
}

void Image_removeAlpha(Image *self)
{
	if (Image_isRGBA8(self))
	{
		UArray_leave_thenRemove_(self->byteArray, 3, 1);
		self->componentCount = 3;
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
		//Image_addAlpha(self);
		//printf("converted component count from 3 to 4\n");
	} 
	else if (self->componentCount == 1)
	{
		UArray *outUArray = UArray_new();
		UArray_setSize_(outUArray, 4 * self->width * self->height);
		uint8_t *outData = (uint8_t *)UArray_bytes(outUArray);
		uint8_t *inData  = (uint8_t *)UArray_bytes(self->byteArray);
		size_t numPixels = self->width * self->height;
		size_t p1;
		size_t p2 = 0;
		
		for (p1 = 0; p1 < numPixels; p1 ++)
		{
			outData[p2] = inData[p1]; p2 ++;
			outData[p2] = inData[p1]; p2 ++;
			outData[p2] = inData[p1]; p2 ++;
			outData[p2] = 255; p2 ++;
		}
		
		UArray_copy_(self->byteArray, outUArray);
		UArray_free(outUArray);

		self->componentCount = 4;
		//printf("converted component count from 1 to 4\n");
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
	
	for (y = 0; y < self->height; y ++)
	{
		for (x = 0; x < self->width; x ++)
		{
			int p = (x + (y * self->width))*componentCount;
			int c;
			
			for (c = 0; c < componentCount; c ++)
			{
				cs[c] += d[p + c];
			}
		}
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