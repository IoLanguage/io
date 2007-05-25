/*
 docCopyright("Steve Dekorte", 2002)
 docLicense("BSD revised")
 */
#include "GLImage.h"
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

GLImage *GLImage_new(void)
{
	GLImage *self = (GLImage *)calloc(1, sizeof(GLImage));
	GLImage_path_(self, "");
	GLImage_fileType_(self, "");
	self->byteArray = UArray_new();
	self->ownsUArray = 1;
	self->format = GLImage_formatForComponentCount_(self, 4);
	self->encodingQuality = 1.0;
	self->textureId = 0;
	return self;
}

GLImage *GLImage_newWithPath_(char *path)
{
	GLImage *self = GLImage_new();
	GLImage_path_(self, path);
	GLImage_load(self);
	return self;
}

void GLImage_free(GLImage *self)
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
	
	if (self->textureId)
	{
		GLuint textureId = self->textureId;
		glDeleteTextures(1, &textureId);
	}
	
	free(self);
}

UArray *GLImage_byteArray(GLImage *self) 
{ 
	return self->byteArray; 
}

void GLImage_copyUArray_(GLImage *self, UArray *ba) /* private */
{ 
	UArray_copy_(self->byteArray, ba);
}

void GLImage_setExternalUArray_(GLImage *self, UArray *ba) 
{ 
	if (self->ownsUArray) UArray_free(self->byteArray);
	self->byteArray = ba;
	self->ownsUArray = 0;
}

void GLImage_getFileType(GLImage *self) /* private */
{
	char *ext = strrchr(self->path, '.');
	char *e;
	if (!ext) { GLImage_fileType_(self, ""); return; }
	
	ext++;
	GLImage_fileType_(self, ext);
	e = self->fileType;
	
	while (*e) { *e = tolower(*e); e++; }
	if (strcmp(self->fileType, "jpeg")==0) GLImage_fileType_(self, "jpg");
}

void GLImage_path_(GLImage *self, const char *path)
{ 
	self->path = strcpy((char *)realloc(self->path, strlen(path)+1), path); 
	GLImage_getFileType(self);
}

char *GLImage_path(GLImage *self) 
{ 
	return self->path; 
}

void GLImage_fileType_(GLImage *self, const char *fileType)
{ 
	self->fileType = strcpy((char *)realloc(self->fileType, strlen(fileType)+1), fileType);  
}

char *GLImage_fileType(GLImage *self) { return self->fileType; }

void GLImage_error_(GLImage *self, const char *error)
{ 
	if (error && strlen(error)) 
	{
		/*printf("GLImage_error_(%s)\n", error);*/
		self->error = strcpy((char *)realloc(self->error, strlen(error)+1), error); 
	}
	else
	{
		if (self->error) free(self->error);
		self->error = NULL;
	}
}

char *GLImage_error(GLImage *self) { return self->error; }

void GLImage_setData_width_height_componentCount_(GLImage *self, UArray *ba, int width, int height, int componentCount) 
{
	int size = width * height * componentCount;
	
	if (size != UArray_size(ba))
	{
		printf("GLImage_setData_width_height_componentCount_() error - %i x %i x %i = %i, but buffer is %i\n", 
			width, height, componentCount, size, (int)UArray_size(ba));
		return;
	}
	
	GLImage_copyUArray_(self, ba);
	self->width  = width;
	self->height = height;
	self->format = GLImage_formatForComponentCount_(self, componentCount);
	self->originalWidth = self->width;
	self->originalHeight = self->height;
	
	//if (self->textureId) 
	{
		GLImage_updateTexture(self);
	}
}

void GLImage_load(GLImage *self) 
{  
	if (strcmp(self->fileType, "png")==0)
	{
		PNGImage *image = PNGImage_new();
		PNGImage_setExternalUArray_(image, self->byteArray);
		PNGImage_path_(image, self->path);
		PNGImage_load(image);
		GLImage_error_(self, PNGImage_error(image));
		self->width  = PNGImage_width(image);
		self->height = PNGImage_height(image);
		self->format = GLImage_formatForComponentCount_(self, PNGImage_components(image));
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
		GLImage_error_(self, JPGImage_error(image));
		self->width  = JPGImage_width(image);
		self->height = JPGImage_height(image);
		self->format = GLImage_formatForComponentCount_(self, JPGImage_components(image));
		/*self->format = GL_RGB8;*/
		JPGImage_free(image);
	}
	else if (strcmp(self->fileType, "tif")==0 || strcmp(self->fileType, "tiff")==0)
	{
		TIFFImage *image = TIFFImage_new();
		TIFFImage_setExternalUArray_(image, self->byteArray);
		TIFFImage_path_(image, self->path);
		TIFFImage_load(image);
		GLImage_error_(self, TIFFImage_error(image));
		self->width  = TIFFImage_width(image);
		self->height = TIFFImage_height(image);
		self->format = GLImage_formatForComponentCount_(self, TIFFImage_components(image));
		TIFFImage_free(image);
	}
	else
	{ 
		GLImage_error_(self, "unknown file type"); 
	}
	
	if (UArray_size(self->byteArray) == 0)
	{ 
		GLImage_error_(self, "error reading file"); 
	}
	
	self->originalWidth = self->width;
	self->originalHeight = self->height;
	
	//GLImage_flipY(self);
}

void GLImage_save(GLImage *self) 
{
	//GLImage_flipY(self);
	
	if (strcmp(self->fileType, "png")==0)
	{
		PNGImage *image = PNGImage_new();
		PNGImage_setExternalUArray_(image, self->byteArray);
		PNGImage_path_(image, self->path);
		PNGImage_width_(image, self->width);
		PNGImage_height_(image, self->height);
		PNGImage_components_(image, GLImage_componentCount(self));
		PNGImage_save(image);
		GLImage_error_(self, PNGImage_error(image));
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
		/* TODO - convert image to RGB first */
		if (GLImage_componentCount(self) != 3)
		{
			GLImage_error_(self, "can only save RGB images to JPEG");
			return;
		}
		JPGImage_components_(image, GLImage_componentCount(self));
		JPGImage_save(image);
		GLImage_error_(self, JPGImage_error(image));
		JPGImage_free(image);
	}
	else if (strcmp(self->fileType, "tiff")==0 || strcmp(self->fileType, "tif")==0)
	{
		TIFFImage *image = TIFFImage_new();
		TIFFImage_setExternalUArray_(image, self->byteArray);
		TIFFImage_path_(image, self->path);
		TIFFImage_width_(image, self->width);
		TIFFImage_height_(image, self->height);
		TIFFImage_components_(image, GLImage_componentCount(self));
		TIFFImage_save(image);
		GLImage_error_(self, TIFFImage_error(image));
		TIFFImage_free(image);
	}
	else
	{ 
		GLImage_error_(self, "unknown file type"); 
	}
	
	//GLImage_flipY(self);
}

int GLImage_width(GLImage *self)  
{ 
	return self->width; 
}

void GLImage_width_(GLImage *self, int w)  
{ 
	if (w != self->width) 
	{
		GLImage_scaleTo(self, w, self->height); 
	}
}

int GLImage_height(GLImage *self) 
{ 
	return self->height; 
}

void GLImage_height_(GLImage *self, int h)  
{ 
	if (h != self->height) 
	{
		GLImage_scaleTo(self, self->width, h); 
	}
}

int GLImage_format(GLImage *self) 
{ 
	return self->format; 
}

int GLImage_componentCountForFormat_(GLImage *self, int format) 
{ 
	switch (format)
	{
		case GL_RGBA8: return 4;
		case GL_RGBA:  return 4;
		case GL_RGB8:  return 3;
		case GL_RGB:   return 3;
		case GL_LUMINANCE_ALPHA: return 2;
		case GL_LUMINANCE: return 1;  
	}
	return 0;
}

int GLImage_formatForComponentCount_(GLImage *self, int componentCount) 
{ 
	switch (componentCount)
	{
		case 4: return GL_RGBA8;
		case 3: return GL_RGB8;
		case 2: return GL_LUMINANCE_ALPHA;
		case 1: return GL_LUMINANCE;   
	}
	
	return 0;
}

int GLImage_componentCount(GLImage *self)
{ 
	return GLImage_componentCountForFormat_(self, self->format); 
}

int GLImage_sizeInBytes(GLImage *self) 
{ 
	return self->height * self->width * 
	GLImage_componentCountForFormat_(self, self->format); 
}

uint8_t *GLImage_data(GLImage *self) 
{ 
	return (uint8_t *)UArray_bytes(self->byteArray); 
}

void GLImage_data_length_(GLImage *self, unsigned char *data, size_t length) 
{ 
	UArray_setData_type_size_copy_(self->byteArray, data, CTYPE_uint8_t, length, 1); 
}


/* --- drawing --------------------------------------------------------- */

void GLImage_draw(GLImage *self)
{
	int components = GLImage_componentCount(self);
	int width = self->width;
	int height = self->height;
	const GLvoid *data = UArray_data(self->byteArray);
	int len = UArray_size(self->byteArray);
	glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
	//glPixelZoom(1, -1);
	//glBitmap(0, 0, 0.0, 0.0, 0, height, (const GLubyte *)0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
	
	if (len != components * width * height) 
	{
		printf("%s image array is of length %i but is expected to be %i (%ix%ix%i)\n",
			  GLImage_path(self), len,  components * width * height, width, height, components);
		exit(1);
	}
	
	glPushMatrix();
	// flip the y axis since y=0 starts at first byte of image data
	//glTranslated(0, height, 0);
	glRasterPos2d(0, height);
	glPixelZoom(1,-1);
	//glScaled(1, -1, 1);
	
	switch (self->format)
	{
		case GL_RGB8:
		case GL_RGB:
			glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
			break;
		case GL_RGBA8:
		case GL_RGBA:
			glDrawPixels(width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
			break;
		case GL_LUMINANCE:
			glDrawPixels(width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
			break;
		case GL_LUMINANCE_ALPHA:
			glDrawPixels(width, height, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data);
			break;
		default:
			GLImage_error_(self, "unrecognized image data format");
	}
	glPixelZoom(1,1);
	glPopMatrix();
	
	//glBitmap(0, 0, 0.0, 0.0, 0, -height, (const GLubyte *)0); /* only to move raster pos */
	//glPixelZoom(1, 1);
	glPopClientAttrib();
}

void GLImage_flipY(GLImage *self)
{  
	int y;
	int w = self->width;
	int h = self->height;
	int componentCount = GLImage_componentCountForFormat_(self, self->format);
	uint8_t *bytes = UArray_mutableBytes(self->byteArray);
	int bytesPerLine = componentCount * w;
	unsigned char *buf = malloc(bytesPerLine);
	
	for (y = 0; y < self->height/2; y ++)
	{
		uint8_t *a = bytes + componentCount * (y * w);
		uint8_t *b = bytes + componentCount * ((h-1-y) * w);
		
		memcpy(buf, a, bytesPerLine);
		memcpy(a,   b, bytesPerLine);
		memcpy(b,   buf, bytesPerLine);
	}
}


void GLImage_resizeTo(GLImage *self, int w, int h)
{  
	//self->originalWidth = self->width;
	//self->originalHeight = self->height;
	
	if ( (self->width != w) || (self->height != h))
	{
		int x, y;
		int componentCount = GLImage_componentCountForFormat_(self, self->format);
		UArray *outUArray = UArray_new();
		UArray_setSize_(outUArray, w*h*componentCount);
		
		//memset(UArray_bytes(outUArray), 0, UArray_size(outUArray));
		
		{
		
		const uint8_t *selfData = UArray_bytes(self->byteArray);
		uint8_t *otherData = UArray_mutableBytes(outUArray);
		
		for (y = 0; y < self->height; y ++)
		{
			if (y > h) break;
			
			for (x = 0; x < self->width; x ++)
			{
				if (x > w) break;
				
				{
					const uint8_t *pi = selfData  + componentCount * (x + y * self->width);
					uint8_t *po = otherData + componentCount * (x + y * w);
					memcpy(po, pi, componentCount);
				}
			}
		}
		}
		
		self->width = w;
		self->height = h;
		GLImage_copyUArray_(self, outUArray);
		UArray_free(outUArray);
	}
}

void GLImage_scaleTo(GLImage *self, int w, int h)
{
	int error = 0;
	
	if ( (self->width != w) || (self->height != h))
	{
		UArray *outUArray = UArray_new();
		UArray_setSize_(outUArray, w*h*GLImage_componentCountForFormat_(self, self->format));
		
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, self->width);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ROW_LENGTH, w);
		
		error = gluScaleImage(GLImage_textureFormat(self), 
						  self->width, self->height, 
						  GL_UNSIGNED_BYTE, 
						  (void *)UArray_bytes(self->byteArray), 
						  w, h, 
						  GL_UNSIGNED_BYTE, 
						  (void *)UArray_bytes(outUArray));
		
		if (error) 
		{ 
			UArray_free(outUArray); 
			GLImage_error_(self, (char *)gluErrorString(error)); 
			return; 
		}
		
		self->width = w;
		self->height = h;
		GLImage_copyUArray_(self, outUArray);
		UArray_free(outUArray);
	}
}

inline unsigned char *GLImage_pixelAt(GLImage *self, int x, int y)
{
	int bps = 8;
	int spp = GLImage_componentCount(self);
	int w = self->width;
	int h = self->height;
	uint8_t *p = (uint8_t *)UArray_bytes(self->byteArray);
	
	if (x < 0) { x = 0; } else if (x > w - 1) { x = w - 1; }
	if (y < 0) { y = 0; } else if (y > h - 1) { y = h - 1; }
	return p + (((x + (y * w)) * (spp * bps)) / 8);
}

void GLImage_crop(GLImage *self, int cx, int cy, int w, int h)
{
	int pixelSize = GLImage_componentCount(self);
	int x, y;
	
	if (cx > self->width)  { GLImage_error_(self, "crop x > width"); return; }
	if (cy > self->height) { GLImage_error_(self, "crop y > height"); return; }
	if (cx+w > self->width)  { w = self->width - cx; }
	if (cy+h > self->height) { h = self->height - cy; }
	
	for (x = 0; x < w; x ++) 
	{    
		for (y = 0; y < h; y ++) 
		{    
			unsigned char *ip = GLImage_pixelAt(self, cx+x, cy+y);
			unsigned char *op = GLImage_pixelAt(self, x, y);
			memcpy(op, ip, pixelSize);
		}
	}
	UArray_setSize_(self->byteArray, w*h*pixelSize);
	self->width  = w;
	self->height = h;
}

void GLImage_format_(GLImage *self, int f)
{
	if (f != self->format)
	{
		int error;
		UArray *outUArray = UArray_new();
		
		UArray_setSize_(outUArray, self->width*self->height*GLImage_componentCountForFormat_(self, f));
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, self->width);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_PACK_ROW_LENGTH, self->width);
		
		error = gluScaleImage(self->format, self->width, self->height, 
						  self->format, 
						  (uint8_t *)UArray_bytes(self->byteArray), 
						  self->width, self->height, 
						  f, (uint8_t *)UArray_bytes(outUArray));
		
		if (error) 
		{ 
			UArray_free(outUArray); 
			return; 
		}
		
		self->format = f;
		GLImage_copyUArray_(self, outUArray);
		UArray_free(outUArray);
	}
}

void GLImage_composite_to_(GLImage *self, GLImage *other, int x, int y)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, self->width);
	glTexSubImage2D(GL_TEXTURE_2D, x, y, 0, GLImage_width(other), GLImage_height(other), 
				 self->format, GL_UNSIGNED_BYTE, UArray_bytes(self->byteArray));
}

void GLImage_grab(GLImage *self, int x, int y, int w, int h, int format)
{
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ROW_LENGTH, w);
	self->format = format;
	self->width = w;
	self->height = h;
	UArray_setSize_(self->byteArray, GLImage_componentCount(self)*w*h);
	glReadPixels(x, y, w, h, format, GL_UNSIGNED_BYTE, (uint8_t *)UArray_bytes(self->byteArray));
}

int GLImage_justTextureId(GLImage *self) 
{
	if (!self->textureId) 
	{ 
		GLuint tid;
		glGenTextures(1, &tid); 
		self->textureId = tid; 

		glBindTexture(GL_TEXTURE_2D, self->textureId);
		/*printf("self->textureId = %i w:%i h:%i\n", self->textureId, self->width, self->height);*/
		
		GLImage_setParameters(self);
	}
	
	return self->textureId;
}

void GLImage_setParameters(GLImage *self) 
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

int GLImage_textureId(GLImage *self) 
{
	if (!self->textureId) 
	{ 
		GLImage_justTextureId(self);
		GLImage_updateTexture(self);
	}
	return self->textureId;
}

void GLImage_bindTexture(GLImage *self)
{ 
	glBindTexture(GL_TEXTURE_2D, GLImage_textureId(self)); 
	//GLImage_setParameters(self);
}

GLenum GLImage_textureFormat(GLImage *self)
{ 
	switch (self->format)
	{
		case GL_RGBA8: return GL_RGBA; 
		case GL_RGB8:  return GL_RGB; 
	}
	return self->format;
}

void GLImage_updateTexture(GLImage *self)
{
	//if (!self->textureId) 
	{ 
		GLImage_bindTexture(self); 
		//return; // return since GLImage_bindTexture will call this method 
	} 
	
	GLImage_resizeToPowerOf2(self);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, self->width);
	/*
	 {
		 GLenum errorCode = glGetError();
		 if (errorCode != GL_NO_ERROR)
		 { printf("GL Error1: '%s'\n", gluErrorString(errorCode)); }
	 }
	 */
	/*printf("texture %i, %i\n", self->width, self->height);*/
	glTexImage2D(GL_TEXTURE_2D, 0, GLImage_textureFormat(self), self->width, self->height, 0,
			   GLImage_textureFormat(self), GL_UNSIGNED_BYTE, UArray_bytes(self->byteArray));
	self->textureWidth  = self->width;
	self->textureHeight = self->height;
	/*
	 {
		 GLenum errorCode = glGetError();
		 if (errorCode != GL_NO_ERROR)
		 { printf("GL Error2: '%s'\n", gluErrorString(errorCode)); }
	 }
	 */
}

/*
 void   glCopyTexSubImage2d(GLenum target, GLint level, GLint offsetx, GLint offsety,
					   GLint x, GLint y, GLsizei width, GLsizei height);
 */

void GLImage_grabTexture(GLImage *self, int x, int y, int w, int h)
{
	self->width = w;
	self->height = h;
	GLImage_resizeToPowerOf2(self);
	glBindTexture(GL_TEXTURE_2D, GLImage_justTextureId(self));
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT,1);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, self->width);
	glTexImage2D(GL_TEXTURE_2D, 0, GLImage_textureFormat(self), self->width, self->height, 0,
			   GLImage_textureFormat(self), GL_UNSIGNED_BYTE, UArray_bytes(self->byteArray));
	/*
	 glTexImage2D(GL_TEXTURE_2D, 0, self->format, self->width, self->height, 0,
			    GLImage_textureFormat(self), GL_UNSIGNED_BYTE, NULL);
	 */
	self->textureWidth = self->width;
	self->textureHeight = self->height;
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, x, y, w, h);
}

void GLImage_closeTexture(GLImage *self)
{
	if (self->textureId)
	{
		GLuint textureId = self->textureId;
		glDeleteTextures(1, &textureId);
	}
}

/*
void GLImage_drawTextureArea2(GLImage *self, int w, int h)
{
	float wr = (float)self->originalWidth  / (float)self->width;
	float hr = (float)self->originalHeight / (float)self->height;

	glPushAttrib(GL_TEXTURE_BIT);
	glEnable(GL_TEXTURE_2D);
	GLImage_bindTexture(self);
	glBegin(GL_QUADS);

	glTexCoord2f(0,  0);
	glVertex2i(0, 0);
	
	glTexCoord2f(0,  hr); 
	glVertex2i(0, h);

	glTexCoord2f(wr, hr);
	glVertex2i(w, h);

	glTexCoord2f(wr,  0);
	glVertex2i(w, 0);
	
	glEnd();
	glPopAttrib();
}
*/

void GLImage_drawScaledTextureArea(GLImage *self, int w, int h)
{	
	glPushAttrib(GL_TEXTURE_BIT);
	glEnable(GL_TEXTURE_2D);
	GLImage_bindTexture(self);
	
	glBegin(GL_QUADS);
	
	glTexCoord2f(0,  1);
	glVertex2i(0, h);
	
	glTexCoord2f(0,  0);
	glVertex2i(0, 0);
	
	glTexCoord2f(1,  0);
	glVertex2i(w, 0);
	
	glTexCoord2f(1, 1);
	glVertex2i(w, h);
	
	glEnd();
	glPopAttrib();
}

void GLImage_drawTextureArea(GLImage *self, int w, int h)
{
	float wr, hr;
	
	glPushAttrib(GL_TEXTURE_BIT);
	glEnable(GL_TEXTURE_2D);
	GLImage_bindTexture(self);
	
	// need to bind texture so width/height is correct
	wr = (float)w / (float)self->width;
	hr = (float)h / (float)self->height;

	// the y texture coords are flipped since the image data starts with y=0
	
	glBegin(GL_QUADS);

	glTexCoord2f(0,  0);
	glVertex2i(0, h);
		
	glTexCoord2f(0,  hr);
	glVertex2i(0, 0);
	
	glTexCoord2f(wr,  hr);
	glVertex2i(w, 0);
	
	glTexCoord2f(wr, 0);
	glVertex2i(w, h);
	
	glEnd();
	
	glPopAttrib();
}

void GLImage_drawTexture(GLImage *self)
{
	GLImage_drawTextureArea(self, self->originalWidth, self->originalHeight);
}

void GLImage_resizeToPowerOf2(GLImage *self)
{
	int w, h;
	int exp;
	float fraction = frexp(self->width, &exp);
	
	w = pow(2, exp - 1 + ceil(fraction - .5));
	
	fraction = frexp(self->height, &exp);
	
	h = pow(2, exp - 1 + ceil(fraction - .5));
	
	GLImage_resizeTo(self, w, h);
	//GLImage_scaleTo(self, w, h);
}

int GLImage_textureWidth(GLImage *self)  
{ 
	return self->textureWidth; 
}

int GLImage_textureHeight(GLImage *self) 
{ 
	return self->textureHeight; 
}

int GLImage_originalWidth(GLImage *self)  
{ 
	return self->originalWidth; 
}

int GLImage_originalHeight(GLImage *self) 
{ 
	return self->originalHeight; 
}

/* --- extras --------------------------------------------------------- */

void GLImage_encodingQuality_(GLImage *self, float q) 
{ 
	self->encodingQuality = q; 
}

float GLImage_encodingQuality(GLImage *self) 
{ 
	return self->encodingQuality; 
}

void GLImage_decodingWidthHint_(GLImage *self, int v)
{ 
	self->decodingWidthHint = v; 
}

int GLImage_decodingWidthHint(GLImage *self)
{ 
	return self->decodingWidthHint; 
}

void GLImage_decodingHeightHint_(GLImage *self, int v)
{ 
	self->decodingHeightHint = v; 
}

int GLImage_decodingHeightHint(GLImage *self)
{ 
	return self->decodingHeightHint; 
}

