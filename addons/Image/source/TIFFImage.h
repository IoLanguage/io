/*  copyright: Steve Dekorte, 2002
 *  All rights reserved. See _BSDLicense.txt.
 *
 *  This is an object wrapper for libtiff.
 *
 *  It's possible to use an external UArray for loading by calling
 *  TIFFImage_setExternalUArray_(), but be carefull not to use the
 *  TIFFImage instance after freeing its external byte array.
 */

#ifndef TIFFIMAGE_DEFINED
#define TIFFIMAGE_DEFINED 1

#include "IoImageApi.h"
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
} TIFFImage;

IOIMAGE_API TIFFImage *TIFFImage_new(void);
IOIMAGE_API TIFFImage *TIFFImage_newWithPath_(char *fname);
IOIMAGE_API void TIFFImage_free(TIFFImage *self);

IOIMAGE_API void TIFFImage_path_(TIFFImage *self, const char *path);
IOIMAGE_API char *TIFFImage_path(TIFFImage *self);

IOIMAGE_API void TIFFImage_error_(TIFFImage *self, const char *path);
IOIMAGE_API char *TIFFImage_error(TIFFImage *self);

IOIMAGE_API void TIFFImage_load(TIFFImage *self);
IOIMAGE_API void TIFFImage_save(TIFFImage *self);

IOIMAGE_API int TIFFImage_width(TIFFImage *self);
IOIMAGE_API int TIFFImage_height(TIFFImage *self);

IOIMAGE_API void TIFFImage_width_(TIFFImage *self, int w);
IOIMAGE_API void TIFFImage_height_(TIFFImage *self, int h);
IOIMAGE_API void TIFFImage_components_(TIFFImage *self, int c);

IOIMAGE_API unsigned char TIFFImage_isRGB8(TIFFImage *self);
IOIMAGE_API unsigned char TIFFImage_isRGBA8(TIFFImage *self);
IOIMAGE_API unsigned char TIFFImage_isL8(TIFFImage *self);
IOIMAGE_API unsigned char TIFFImage_isLA8(TIFFImage *self);
IOIMAGE_API int TIFFImage_components(TIFFImage *self);

IOIMAGE_API UArray *TIFFImage_byteArray(TIFFImage *self);

/* Whomever calls TIFFImage_setExternalUArray_ is responsible
 * for freeing "ba" and for making sure that this
 * TIFFImage is not still using it after it is freed.
 */
IOIMAGE_API void TIFFImage_setExternalUArray_(TIFFImage *self, UArray *ba);

#endif
