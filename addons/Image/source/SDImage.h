/*
(This is a BSD License)

Copyright (c) 2001, Steve Dekorte
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

¥  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
¥  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
¥  Neither the name of the author nor the names of other contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


/*
 * SDImage.h 
 * A simple SDImage manipulation library
 * ANSI C, no external dependencies
 * supports RGB8 and RGBA8
 * Copyright Steve Dekorte, 2001
 */

#include "IoImageApi.h"

#include <string.h>
#include <stdlib.h>

#define IMAGE_MALLOC malloc
#define IMAGE_FREE   free

typedef struct 
{
  unsigned char *data; /* SDImage data in rgb or rgba format */
  size_t numBytes;     /* number of bytes in data */
  int ownsData;        /* if true, will free data when SDImage_free() is called */
  int w;   /* pixels wide */
  int h;   /* pixels high */
  int spp; /* samples per pixel */
  int bps; /* bits per sample */
} SDImage;

/* Management */

IOIMAGE_API SDImage *SDImage_new(int w, int h, int spp, int bps, unsigned char *data);
IOIMAGE_API void SDImage_free(SDImage *self);
IOIMAGE_API void SDImage_print(SDImage *self);
IOIMAGE_API SDImage *SDImage_clone(SDImage *self);
IOIMAGE_API void SDImage_copy(SDImage *self, SDImage *other);
IOIMAGE_API void SDImage_copyByPixel(SDImage *self, SDImage *other);

/* Accessors */

IOIMAGE_API size_t SDImage_dataLength(SDImage *self);
IOIMAGE_API int SDImage_pixelSize(SDImage *self);
IOIMAGE_API unsigned char *SDImage_pixelAt(SDImage *self, int x, int y);
IOIMAGE_API unsigned char *SDImage_pixelAtWrap(SDImage *self, int x, int y);
//unsigned char *SDImage_colorAt(SDImage *self, int x, int y, float);

/* Tests */

IOIMAGE_API int SDImage_isEqual(SDImage *self, SDImage *other);
IOIMAGE_API int SDImage_sameFormatAs(SDImage *self, SDImage *other);
IOIMAGE_API int SDImage_sameSizeAs(SDImage *self, SDImage *other);

/* Transformation operations */

IOIMAGE_API SDImage *SDImage_smoothScaleTo(SDImage *self, int ow, int oh);
IOIMAGE_API SDImage *SDImage_scaleTo(SDImage *self, int ow, int oh);
IOIMAGE_API SDImage *SDImage_blur(SDImage *self);
IOIMAGE_API SDImage *SDImage_blurTimes(SDImage *self, int times);
IOIMAGE_API SDImage *SDImage_highBoostFilter(SDImage *self, float v);
IOIMAGE_API SDImage *SDImage_flipx(SDImage *self);
IOIMAGE_API SDImage *SDImage_flipy(SDImage *self);
IOIMAGE_API void SDImage_flipxy(SDImage *self);

IOIMAGE_API SDImage *SDImage_smoothShrinkTo(SDImage *self, int ow, int oh);
IOIMAGE_API void SDImage_aveAtWithRangeTo(SDImage *self, int x, int y, int range, unsigned char *op);

/* implemented but untested */

IOIMAGE_API void SDImage_clearWithColor(SDImage *self, float r, float g, float b, float alpha);
IOIMAGE_API int SDImage_hasAlpha(SDImage *self);
IOIMAGE_API SDImage *SDImage_addAlpha(SDImage *self);
IOIMAGE_API SDImage *SDImage_removeAlpha(SDImage *self);
IOIMAGE_API SDImage *SDImage_clipRect(SDImage *self, int x, int y, int w, int h);
IOIMAGE_API void SDImage_composite(SDImage *self, SDImage *other, int x, int y);
IOIMAGE_API void SDImage_invert(SDImage *self);
IOIMAGE_API void SDImage_replaceRGBColorWith(SDImage *self, int r, int g, int b, int nr, int ng, int nb);
IOIMAGE_API SDImage *SDImage_grayVersionOf(SDImage *self);

/* linear interpolation operations */

IOIMAGE_API void SDImage_interpolateWithImage(SDImage *self, SDImage *other, float v);
IOIMAGE_API void SDImage_interpolateWithRGBColor(SDImage *self, int r, int g, int b, float v);
IOIMAGE_API void SDImage_darken(SDImage *self, float v);
IOIMAGE_API void SDImage_lighten(SDImage *self, float v);
IOIMAGE_API void SDImage_saturate(SDImage *self, float v);
IOIMAGE_API void SDImage_contrast(SDImage *self, float v);
IOIMAGE_API void SDImage_sharpen(SDImage *self, float v);

/* rotation */

IOIMAGE_API SDImage *SDImage_rotateWithSameSize(SDImage *self, double degrees);
IOIMAGE_API SDImage *SDImage_rotate(SDImage *self, double degrees);
//double SDImage_rangeFor(double ox, double oy, double radians);

/* SDImage loading */

IOIMAGE_API SDImage *SDImage_load(const char *path);
IOIMAGE_API unsigned char *SDImage_dataFromFile(char *path, int *length);
