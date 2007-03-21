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

#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
/*
#include <OpenGL/OpenGL.h>
#include <OpenGL/glext.h>
*/
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/glu.h>
#include <GL/gl.h>
#endif


typedef struct
{
  char *path;
  char *fileType;
  UArray *byteArray;
  unsigned char ownsUArray;
  int width;
  int height;
  int format;
  char *error;
  int textureId;
  int textureWidth;
  int textureHeight;
  
  int originalWidth;
  int originalHeight;
  
  float encodingQuality;
  int decodingWidthHint;
  int decodingHeightHint;
} GLImage;

IOIMAGE_API GLImage *GLImage_new(void);
IOIMAGE_API GLImage *GLImage_newWithPath_(char *fname);
IOIMAGE_API void GLImage_free(GLImage *self);

IOIMAGE_API void GLImage_setData_width_height_componentCount_(GLImage *self, UArray *ba, int width, int height, int componentCount);

IOIMAGE_API UArray *GLImage_byteArray(GLImage *self);
IOIMAGE_API void GLImage_setExternalUArray_(GLImage *self, UArray *ba);

IOIMAGE_API void GLImage_path_(GLImage *self, const char *path);
IOIMAGE_API char *GLImage_path(GLImage *self);

IOIMAGE_API void GLImage_fileType_(GLImage *self, const char *fileType);
IOIMAGE_API char *GLImage_fileType(GLImage *self);

IOIMAGE_API void GLImage_error_(GLImage *self, const char *path);
IOIMAGE_API char *GLImage_error(GLImage *self);

IOIMAGE_API void GLImage_load(GLImage *self);
IOIMAGE_API void GLImage_save(GLImage *self);

IOIMAGE_API int GLImage_width(GLImage *self);
IOIMAGE_API void GLImage_width_(GLImage *self, int w);
IOIMAGE_API int GLImage_height(GLImage *self);
IOIMAGE_API void GLImage_height_(GLImage *self, int h);

IOIMAGE_API int GLImage_format(GLImage *self);
IOIMAGE_API void GLImage_format_(GLImage *self, int f);
IOIMAGE_API int GLImage_componentCountForFormat_(GLImage *self, int format);
IOIMAGE_API int GLImage_formatForComponentCount_(GLImage *self, int componentCount) ;
IOIMAGE_API int GLImage_componentCount(GLImage *self);

IOIMAGE_API int GLImage_sizeInBytes(GLImage *self);
IOIMAGE_API uint8_t *GLImage_data(GLImage *self);
IOIMAGE_API void GLImage_data_length_(GLImage *self, unsigned char *data, size_t length); 

// drawing 

IOIMAGE_API void GLImage_draw(GLImage *self);

// manipulation

IOIMAGE_API void GLImage_scaleTo(GLImage *self, int w, int h);
IOIMAGE_API void GLImage_resizeTo(GLImage *self, int w, int h);
IOIMAGE_API void GLImage_flipY(GLImage *self);
IOIMAGE_API void GLImage_crop(GLImage *self, int x, int y, int w, int h);
IOIMAGE_API void GLImage_composite_to_(GLImage *self, GLImage *other, int x, int y);
IOIMAGE_API void GLImage_grab(GLImage *self, int x, int y, int w, int h, int format);

// textures 

GLenum GLImage_textureFormat(GLImage *self);
IOIMAGE_API void GLImage_grabTexture(GLImage *self, int x, int y, int w, int h);
IOIMAGE_API int GLImage_textureId(GLImage *self);
IOIMAGE_API void GLImage_bindTexture(GLImage *self);
IOIMAGE_API void GLImage_updateTexture(GLImage *self);
IOIMAGE_API void GLImage_closeTexture(GLImage *self);
IOIMAGE_API void GLImage_drawTexture(GLImage *self);
IOIMAGE_API void GLImage_drawTextureArea(GLImage *self, int w, int h);
IOIMAGE_API void GLImage_drawScaledTextureArea(GLImage *self, int w, int h);

IOIMAGE_API void GLImage_setParameters(GLImage *self);

IOIMAGE_API void GLImage_resizeToPowerOf2(GLImage *self);

IOIMAGE_API int GLImage_textureWidth(GLImage *self);
IOIMAGE_API int GLImage_textureHeight(GLImage *self);

IOIMAGE_API int GLImage_originalWidth(GLImage *self);
IOIMAGE_API int GLImage_originalHeight(GLImage *self);

//  extras 

IOIMAGE_API void GLImage_encodingQuality_(GLImage *self, float q);
IOIMAGE_API float GLImage_encodingQuality(GLImage *self);

IOIMAGE_API void GLImage_decodingWidthHint_(GLImage *self, int v);
IOIMAGE_API int GLImage_decodingWidthHint(GLImage *self);

IOIMAGE_API void GLImage_decodingHeightHint_(GLImage *self, int v);
IOIMAGE_API int GLImage_decodingHeightHint(GLImage *self);

#endif

