
//metadoc TIFFImage copyright Steve Dekorte 2002
//metadoc TIFFImage license BSD revised

#include "TIFFImage.h"
#include <tiff.h>
#include <tiffio.h>

#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#include <string.h>


void MyTIFFErrorHandler(const char* module, const char* fmt, va_list ap)
{
	printf("%s, %s", module, fmt);
}

TIFFImage *TIFFImage_new(void)
{
	TIFFImage *self = (TIFFImage *)io_calloc(1, sizeof(TIFFImage));
	TIFFImage_path_(self, "");
	TIFFImage_error_(self, "");
	self->byteArray = UArray_new();
	self->ownsBuffer = 1;
	TIFFSetErrorHandler(MyTIFFErrorHandler);
	return self;
}

TIFFImage *TIFFImage_newWithPath_(char *path)
{
	TIFFImage *self = TIFFImage_new();
	TIFFImage_path_(self, path);
	TIFFImage_load(self);
	return self;
}

void TIFFImage_free(TIFFImage *self)
{
	if (self->ownsBuffer) UArray_free(self->byteArray);
	if (self->error) io_free(self->error);
	io_free(self->path);
	io_free(self);
}

void TIFFImage_path_(TIFFImage *self, const char *path)
{ 
	self->path = strcpy((char *)io_realloc(self->path, strlen(path)+1), path);  
}

char *TIFFImage_path(TIFFImage *self) 
{ 
	return self->path; 
}

void TIFFImage_error_(TIFFImage *self, const char *error)
{
	self->error = strcpy((char *)io_realloc(self->error, strlen(error)+1), error);
	/*if (strlen(self->error)) printf("TIFFImage error: %s\n", self->error);*/
}

char *TIFFImage_error(TIFFImage *self) { return self->error; }

void TIFFImage_load(TIFFImage *self)
{
	TIFF *in;
	uint16 samplesperpixel;
	uint16 bitspersample;
	uint16 photometric;

	printf("TIFFImage_load(%s)\n", self->path);

	in = TIFFOpen(self->path, "r");

	if (in == NULL)
	{
	TIFFImage_error_(self, "error opening file");
	return;
	}

	photometric = 0;
	TIFFGetField(in, TIFFTAG_PHOTOMETRIC, &photometric);
	if (photometric != PHOTOMETRIC_RGB && photometric != PHOTOMETRIC_PALETTE )
	{
	TIFFImage_error_(self, "Bad photometric; can only handle RGB and Palette images.\n");
	return;
	}
	printf("photometric = %i\n", photometric);

	TIFFGetField(in, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
	if (samplesperpixel != 1 && samplesperpixel != 3)
	{
	TIFFImage_error_(self, "Bad samples/pixel\n");
	return;
	}
	printf("samplesperpixel = %i\n", samplesperpixel);

	TIFFGetField(in, TIFFTAG_BITSPERSAMPLE, &bitspersample);
	if (bitspersample != 8)
	{
	TIFFImage_error_(self, "Sorry, only handle 8-bit samples.\n");
	return;
	}

	printf("bitspersample = %i\n", bitspersample);

	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &(self->width));
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &(self->height));
	printf("width  = %i\n", self->width);
	printf("height = %i\n", self->height);

	/* move to RGBA */

	self->components = 4;
	UArray_setSize_(self->byteArray, self->width * self->height * self->components);

	/* Read the image in one chunk into an RGBA array */
	if (!TIFFReadRGBAImage(in, self->width, self->height, (uint32 *)(UArray_bytes(self->byteArray)), 0))
	{
	TIFFImage_error_(self, "error converting to RGBA");
	return;
	}
	(void) TIFFClose(in);

	/* For some reason the TIFFReadRGBAImage() function chooses the
	lower left corner as the origin.  Vertically mirror scanlines. */

	{
	int row;
	uint32 *wrk_line = (uint32*)_TIFFmalloc(self->width * sizeof (uint32));
	if (wrk_line == 0)
	{
		TIFFImage_error_(self, "No space for raster scanline buffer");
		return;
	}

	for( row = 0; row < self->height / 2; row++ )
	{
		uint32 *raster = (uint32 *)(UArray_bytes(self->byteArray));
		uint32	*top_line, *bottom_line;

		top_line = raster + self->width * row;
		bottom_line = raster + self->width * (self->height-row-1);

		_TIFFmemcpy(wrk_line, top_line,    4*self->width);
		_TIFFmemcpy(top_line, bottom_line, 4*self->width);
		_TIFFmemcpy(bottom_line, wrk_line, 4*self->width);
	}

	_TIFFfree( wrk_line );
	}

	/* strip alpha */
	if( 1 )
	{
		uint32 *raster = (uint32 *)(UArray_bytes(self->byteArray));
		int pixel_count = self->width * self->height;
		unsigned char *src, *dst;

		src = (unsigned char *) raster;
		dst = (unsigned char *) raster;
		while( pixel_count > 0 )
		{
		// begin fix 22dec04 ------------------
		//
		//  RGBa has the alpha high and red low, i.e.  aBGR
		// so this code actually creates aBG pixels.
		// The alpha fills the red channel with 100% tone.
		//*(dst++) = *(src++);
		//*(dst++) = *(src++);
		//*(dst++) = *(src++);
		//src++;

		// Instead we convert aBGR >>> RGB
		//  (Rick Evans 22dec04)
		*(dst++) = *(src+3);    // move red high
		*(dst++) = *(src+2);    // green middle
		*(dst++) = *(src+1);    // blue low
		src+=4;				   // toss alpha
						   // end fix -----------------------------

			pixel_count--;
		}
		self->components = 3;
	}
	UArray_setSize_(self->byteArray, self->width * self->height *
			 self->components);
}

void TIFFImage_save(TIFFImage *self)
{
	TIFF *out = TIFFOpen(self->path, "w");

	if (out == NULL)
	{ TIFFImage_error_(self, "error opening tiff for writing"); }

	/*TIFFSetField(out, TIFFTAG_SUBFILETYPE, FILETYPE_REDUCEDIMAGE);*/
	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, (uint32) self->width);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, (uint32) self->height); /* ????? */
	TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, (uint32) self->height); /* ????? */
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, (uint16) 8);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, (uint16) self->components);
	TIFFSetField(out, TIFFTAG_COMPRESSION, COMPRESSION_PACKBITS);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	/*
	 cpTag(in, out, TIFFTAG_SOFTWARE,		(uint16) -1, TIFF_ASCII);
	 cpTag(in, out, TIFFTAG_IMAGEDESCRIPTION,	(uint16) -1, TIFF_ASCII);
	 cpTag(in, out, TIFFTAG_DATETIME,		(uint16) -1, TIFF_ASCII);
	 cpTag(in, out, TIFFTAG_HOSTCOMPUTER,	(uint16) -1, TIFF_ASCII);
	 */
	{
	uint32 diroff[1];
	diroff[0] = 0;
	TIFFSetField(out, TIFFTAG_SUBIFD, 1, diroff);
	}

	if (TIFFWriteEncodedStrip(out, 0, (tdata_t)(UArray_bytes(self->byteArray)), self->width*self->height*self->components) != -1)
	{ TIFFImage_error_(self, "error writing tiff"); }
	/*
	 else
	 if (TIFFWriteDirectory(out) != -1);
	 { TIFFImage_error_(self, "error writing tiff directory"); }
	 */

	(void) TIFFClose(out);
}

int TIFFImage_width(TIFFImage *self)
{
	return self->width;
}

int TIFFImage_height(TIFFImage *self)
{
	return self->height;
}

void TIFFImage_width_(TIFFImage *self, int w)  { self->width = w; }
void TIFFImage_height_(TIFFImage *self, int h) { self->height = h; }
void TIFFImage_components_(TIFFImage *self, int c) { self->components = c; }

unsigned char TIFFImage_isL8(TIFFImage *self)    { return (self->components == 1); }
unsigned char TIFFImage_isLA8(TIFFImage *self)   { return (self->components == 2); }
unsigned char TIFFImage_isRGB8(TIFFImage *self)  { return (self->components == 3); }
unsigned char TIFFImage_isRGBA8(TIFFImage *self) { return (self->components == 4); }

int TIFFImage_components(TIFFImage *self)
{
	return self->components;
}

int TIFFImage_sizeInBytes(TIFFImage *self)
{
	return self->height * self->width * self->components;
}

UArray *TIFFImage_byteArray(TIFFImage *self)
{
	return self->byteArray;
}

void TIFFImage_setExternalUArray_(TIFFImage *self, UArray *ba)
{
	if (self->ownsBuffer) UArray_free(self->byteArray);
	self->byteArray = ba;
	self->ownsBuffer = 0;
}


