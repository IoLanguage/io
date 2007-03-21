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
 * image.h 
 * A simple image manipulation library
 * ANSI C, no external dependencies
 * supports RGB8 and RGBA8
 * Copyright Steve Dekorte, 2001
 */

#include <string.h>
#include <stdlib.h>

#define IMAGE_MALLOC malloc
#define IMAGE_FREE   free

typedef struct 
{
  unsigned char *data; /* image data in rgb or rgba format */
  size_t numBytes;     /* number of bytes in data */
  int ownsData;        /* if true, will free data when image_free() is called */
  int w;   /* pixels wide */
  int h;   /* pixels high */
  int spp; /* samples per pixel */
  int bps; /* bits per sample */
} image;

/* Management */

image *image_new(int w, int h, int spp, int bps, unsigned char *data);
void image_free(image *self);
void image_print(image *self);
image *image_clone(image *self);
void image_copy(image *self, image *other);
void image_copyByPixel(image *self, image *other);

/* Accessors */

size_t image_dataLength(image *self);
int image_pixelSize(image *self);
unsigned char *image_pixelAt(image *self, int x, int y);
unsigned char *image_pixelAtWrap(image *self, int x, int y);
//unsigned char *image_colorAt(image *self, int x, int y, float);

/* Tests */

int image_isEqual(image *self, image *other);
int image_sameFormatAs(image *self, image *other);
int image_sameSizeAs(image *self, image *other);

/* Transformation operations */

image *image_smoothScaleTo(image *self, int ow, int oh);
image *image_scaleTo(image *self, int ow, int oh);
image *image_blur(image *self);
image *image_blurTimes(image *self, int times);
image *image_highBoostFilter(image *self, float v);
image *image_flipx(image *self);
image *image_flipy(image *self);
void image_flipxy(image *self);

image *image_smoothShrinkTo(image *self, int ow, int oh);
void image_aveAtWithRangeTo(image *self, int x, int y, int range, unsigned char *op);

/* implemented but untested */

void image_clearWithColor(image *self, float r, float g, float b, float alpha);
int image_hasAlpha(image *self);
image *image_addAlpha(image *self);
image *image_removeAlpha(image *self);
image *image_clipRect(image *self, int x, int y, int w, int h);
void image_composite(image *self, image *other, int x, int y);
void image_invert(image *self);
void image_replaceRGBColorWith(image *self, int r, int g, int b, int nr, int ng, int nb);
image *image_grayVersionOf(image *self);

/* linear interpolation operations */

void image_interpolateWithImage(image *self, image *other, float v);
void image_interpolateWithRGBColor(image *self, int r, int g, int b, float v);
void image_darken(image *self, float v);
void image_lighten(image *self, float v);
void image_saturate(image *self, float v);
void image_contrast(image *self, float v);
void image_sharpen(image *self, float v);

/* rotation */

image *image_rotateWithSameSize(image *self, double degrees);
//double image_rangeFor(double ox, double oy, double radians);

/* image loading */

image *image_load(const char *path);
unsigned char *image_dataFromFile(char *path, int *length);
