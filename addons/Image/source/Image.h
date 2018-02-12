/*  copyright: Steve Dekorte, 2002
 *  All rights reserved. See _BSDLicense.txt.
 *
 *  An object for loading various image formats using
 *  image libraries. As well as manipulating and displaying
 *  the decoded image data using OpenGL.
 *
 *  It can use GL to scale, composite and convert the image format.
 */

#ifndef GLIMAGE_DEFINED
#define GLIMAGE_DEFINED 1

#include "IoImageApi.h"

#include <Common.h>
#include <UArray.h>

/*#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
//#include <OpenGL/OpenGL.h>
//#include <OpenGL/glext.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glu.h>
#include <GL/gl.h>
#endif
*/

typedef struct
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} ColorStruct;

typedef struct
{
	char *path;
	char *fileType;
	UArray *byteArray;
	unsigned char ownsUArray;
	int width;
	int height;
	int componentCount;
	char *error;

	float encodingQuality;
	int decodingWidthHint;
	int decodingHeightHint;
} Image;

typedef struct
{
	int xmin;
	int ymin;
	int xmax;
	int ymax;
} ImageBounds;

IOIMAGE_API Image *Image_new(void);
IOIMAGE_API Image *Image_newWithPath_(char *fname);
IOIMAGE_API Image *Image_copyWithUArray_(Image *self, UArray *ba);
IOIMAGE_API Image *Image_copyWithUArray(Image *self, UArray *ba);
IOIMAGE_API void Image_free(Image *self);

IOIMAGE_API void Image_setData_width_height_componentCount_(Image *self, UArray *ba, int width, int height, int componentCount);

IOIMAGE_API UArray *Image_byteArray(Image *self);
IOIMAGE_API void Image_setExternalUArray_(Image *self, UArray *ba);

IOIMAGE_API void Image_path_(Image *self, const char *path);
IOIMAGE_API char *Image_path(Image *self);

IOIMAGE_API void Image_fileType_(Image *self, const char *fileType);
IOIMAGE_API char *Image_fileType(Image *self);

IOIMAGE_API void Image_error_(Image *self, const char *path);
IOIMAGE_API char *Image_error(Image *self);

IOIMAGE_API void Image_load(Image *self);
IOIMAGE_API void Image_save(Image *self);

IOIMAGE_API int Image_width(Image *self);
IOIMAGE_API void Image_width_(Image *self, int w);
IOIMAGE_API int Image_height(Image *self);
IOIMAGE_API void Image_height_(Image *self, int h);

IOIMAGE_API int Image_componentCount(Image *self);
IOIMAGE_API int Image_isRGBA8(Image *self);
IOIMAGE_API int Image_isRGB8(Image *self);
IOIMAGE_API int Image_isGrayscale(Image *self);

IOIMAGE_API int Image_sizeInBytes(Image *self);
IOIMAGE_API uint8_t *Image_data(Image *self);
IOIMAGE_API void Image_data_length_(Image *self, unsigned char *data, size_t length);

// manipulation

IOIMAGE_API void Image_resizeTo(Image *self, int w, int h, Image *outImage);
IOIMAGE_API void Image_flipX(Image *self);
IOIMAGE_API void Image_flipY(Image *self);
IOIMAGE_API void Image_crop(Image *self, int x, int y, int w, int h);
IOIMAGE_API void Image_addAlpha(Image *self);
IOIMAGE_API void Image_removeAlpha(Image *self);

//  extras

IOIMAGE_API void Image_encodingQuality_(Image *self, float q);
IOIMAGE_API float Image_encodingQuality(Image *self);

IOIMAGE_API void Image_decodingWidthHint_(Image *self, int v);
IOIMAGE_API int Image_decodingWidthHint(Image *self);

IOIMAGE_API void Image_decodingHeightHint_(Image *self, int v);
IOIMAGE_API int Image_decodingHeightHint(Image *self);

IOIMAGE_API void Image_makeRGBA(Image *self);
IOIMAGE_API void Image_makeGrayscale(Image *self);

IOIMAGE_API ImageBounds Image_bounds(Image *self, int cutoff);

IOIMAGE_API int Image_baselineHeight(Image *self);
IOIMAGE_API ColorStruct Image_averageColor(Image *self);

IOIMAGE_API UArray* Image_histogram(Image *self);
IOIMAGE_API void Image_equalizeHistogram(Image *self, int mode);

IOIMAGE_API void Image_linearContrast(Image *self);
IOIMAGE_API void Image_bitPlain(Image *self, int component, int bit);
IOIMAGE_API void Image_setIntensityInRangeTo_(Image *self, int component, int left, int right, int newValue);

IOIMAGE_API void Image_thresholdByGradient(Image* self);
IOIMAGE_API void Image_thresholdByHistogram(Image* self);
IOIMAGE_API void Image_thresholdByOtsu(Image* self);

IOIMAGE_API Image* Image_applyLinearFilter(Image* self, int filterWidth, int filterHeight, UArray* filter);
IOIMAGE_API Image* Image_applyMaxFilter(Image* self, int filterWidth, int filterHeight);
IOIMAGE_API Image* Image_applyMinFilter(Image* self, int filterWidth, int filterHeight);
IOIMAGE_API Image* Image_applyWeightedMedianFilter(Image* self, int filterWidth, int filterHeight, UArray* filter);
IOIMAGE_API Image* Image_applyNonlinearGradientsFilter(Image* self);

IOIMAGE_API void Image_bitMask(Image *self, int component, int bitMask);

// internals

int Image_isL8(Image *self);

#endif

