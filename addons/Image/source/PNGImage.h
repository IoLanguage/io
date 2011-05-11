/*  copyright: Steve Dekorte, 2002
 *  All rights reserved. See _BSDLicense.txt.
 *
 *  This is an object wrapper for libpng.
 *
 *  It's currently only useful for decoding a libpng file
 *  to a byte array and providing info about the
 *  format, width, height and # of components.
 *
 *  It's possible to use an external UArray for loading by calling
 *  PNGImage_setExternalUArray_(), but be careful not to use the
 *  PNGImage instance after freeing its external byte array.
 */

// these two defines have been depricated since libpng10 in favor of NULL
// makes Image addon compatible with libpng12 and libpng14
#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL

#ifndef PNGIMAGE_DEFINED
#define PNGIMAGE_DEFINED 1

#include "IoImageApi.h"

#include <png.h>
#include <UArray.h>

typedef struct
{
	char *path;
	int width;
	int height;
	int components;
	UArray *byteArray;
	unsigned char ownsBuffer;
	char *error;
} PNGImage;

IOIMAGE_API PNGImage *PNGImage_new(void);
IOIMAGE_API PNGImage *PNGImage_newWithPath_(char *fname);
IOIMAGE_API void PNGImage_free(PNGImage *self);

IOIMAGE_API void PNGImage_path_(PNGImage *self, const char *path);
IOIMAGE_API char *PNGImage_path(PNGImage *self);

IOIMAGE_API void PNGImage_error_(PNGImage *self, const char *path);
IOIMAGE_API char *PNGImage_error(PNGImage *self);

IOIMAGE_API void PNGImage_load(PNGImage *self);
IOIMAGE_API void PNGImage_save(PNGImage *self);

IOIMAGE_API int PNGImage_width(PNGImage *self);
IOIMAGE_API int PNGImage_height(PNGImage *self);

IOIMAGE_API void PNGImage_width_(PNGImage *self, int w);
IOIMAGE_API void PNGImage_height_(PNGImage *self, int h);
IOIMAGE_API void PNGImage_components_(PNGImage *self, int h);

IOIMAGE_API int PNGImage_pngColorType(PNGImage *self); /* private */

IOIMAGE_API unsigned char PNGImage_isRGB8(PNGImage *self);
IOIMAGE_API unsigned char PNGImage_isRGBA8(PNGImage *self);
IOIMAGE_API unsigned char PNGImage_isL8(PNGImage *self);
IOIMAGE_API unsigned char PNGImage_isLA8(PNGImage *self);
IOIMAGE_API int PNGImage_components(PNGImage *self);

IOIMAGE_API UArray *PNGImage_byteArray(PNGImage *self);

/* Whomever calls PNGImage_setExternalUArray_ is responsible
 * for freeing "ba" and for making sure that this
 * PNGImage is not still using it after it is freed.
 */
IOIMAGE_API void PNGImage_setExternalUArray_(PNGImage *self, UArray *ba);

#endif
