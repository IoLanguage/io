/*  copyright: Steve Dekorte, 2002
 *  All rights reserved. See _BSDLicense.txt.
 *
 *  This is an object wrapper for libjpeg.
 *
 *  It's possible to use an external UArray for loading by calling
 *  JPGImage_setExternalUArray_(), but be carefull not to use the
 *  JPGImage instance after freeing its external byte array.
 */

#ifndef JPGIMAGE_DEFINED
#define JPGIMAGE_DEFINED 1

#include "IoImageApi.h"

#include <UArray.h>
#include <jpeglib.h>

typedef struct
{
	char *path;
	int width;
	int height;
	int components;
	UArray *byteArray;
	unsigned char ownsUArray;
	char *error;
	float quality; /* 0.0 - 1.0 */
	/*
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	*/
	int decodingWidthHint;
	int decodingHeightHint;
} JPGImage;

IOIMAGE_API JPGImage *JPGImage_new(void);
IOIMAGE_API JPGImage *JPGImage_newWithPath_(char *fname);
IOIMAGE_API void JPGImage_free(JPGImage *self);

IOIMAGE_API void JPGImage_path_(JPGImage *self, const char *path);
IOIMAGE_API char *JPGImage_path(JPGImage *self);

IOIMAGE_API void JPGImage_quality_(JPGImage *self, float q);
IOIMAGE_API float JPGImage_quality(JPGImage *self);

IOIMAGE_API void JPGImage_decodingWidthHint_(JPGImage *self, int w);
IOIMAGE_API int JPGImage_decodingWidthHint(JPGImage *self);

IOIMAGE_API void JPGImage_decodingHeightHint_(JPGImage *self, int h);
IOIMAGE_API int JPGImage_decodingHeightHint(JPGImage *self);

IOIMAGE_API void JPGImage_error_(JPGImage *self, const char *path);
IOIMAGE_API char *JPGImage_error(JPGImage *self);

IOIMAGE_API void JPGImage_load(JPGImage *self);
IOIMAGE_API void JPGImage_save(JPGImage *self);

IOIMAGE_API int JPGImage_width(JPGImage *self);
IOIMAGE_API int JPGImage_height(JPGImage *self);

IOIMAGE_API void JPGImage_width_(JPGImage *self, int w);
IOIMAGE_API void JPGImage_height_(JPGImage *self, int h);
IOIMAGE_API void JPGImage_components_(JPGImage *self, int c);

IOIMAGE_API unsigned char JPGImage_isRGB8(JPGImage *self);
IOIMAGE_API unsigned char JPGImage_isRGBA8(JPGImage *self);
IOIMAGE_API unsigned char JPGImage_isL8(JPGImage *self);
IOIMAGE_API unsigned char JPGImage_isLA8(JPGImage *self);
IOIMAGE_API int JPGImage_components(JPGImage *self);

IOIMAGE_API UArray *JPGImage_byteArray(JPGImage *self);

/* Whomever calls JPGImage_setExternalUArray_ is responsible
 * for freeing "ba" and for making sure that this
 * JPGImage is not still using it after it is freed.
 */
IOIMAGE_API void JPGImage_setExternalUArray_(JPGImage *self, UArray *ba);

#endif
