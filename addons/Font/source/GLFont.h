/* GLFont.h - by Mike Austin */

#ifndef GLFONT_DEFINED
#define GLFONT_DEFINED 1

#include <ft2build.h>
#include FT_FREETYPE_H
#include <setjmp.h>

#if defined(__APPLE__) || defined(MACOSX)
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>
#else
#ifdef WIN32
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#endif

//extern FT_Library gFreeTypeLib;

//#define GLFONT_TEX_SIZE 128
#define GLFONT_TEX_SIZE 256 /* must be power of 2 */

/* Each character will have one of these */
typedef struct {
	int xpos;  int ypos;   /* the x and y offset into the texture */
	int width; int height; /* the width and height of the bitmap */
	int left;  int top;    /* the left and top bitmap offset */
	int advance;           /* the x advance between characters */
} GLFont_Symbol;

/* Each character caches its texture coordinates for speed */
typedef struct {
	float left; float right;  /* the left and right glTexCoordf() parameters */
	float top;  float bottom; /* the top and bottom glTexCoordf() parameters */
} GLFont_TexCoords;

typedef struct GLFont_ {
	FT_Face face;
	int               maxWidth, maxHeight;  /* the max character width & height */
	int               pixelSize;
	char              isTextured;           /* boolean: is the font textured */
	void              (*drawString)(struct GLFont_*, const char*, int, int);
	GLFont_Symbol     symbol[256];          /* individual character information */
	GLFont_TexCoords  texCoords[256];       /* texture coordinate for each */
	GLuint            texId;
	unsigned char texture[GLFONT_TEX_SIZE][GLFONT_TEX_SIZE][4];
	unsigned char isLoaded;
	unsigned char didInit;
	FT_Error errorCode;
} GLFont;

/* initialize the FreeType library */
void GLFont_init(void);

/* release the freetype library */
void GLFont_done(void);

/* returns a new instance of GLFont */
GLFont *GLFont_new(void);

/* release the font face */
void GLFont_free(GLFont *self);

/* creates and loads a font */
void GLFont_loadFont(GLFont *self, const char *path);

/* sets the font's size, recreating the texture */
void GLFont_setPixelSize(GLFont *self, unsigned int size);
int GLFont_pixelSize(GLFont *self);
int GLFont_isTextured(GLFont *self);

/* get the lenght of string in pixels */
int GLFont_stringIndexAtWidth(GLFont *self, const char *string, int startIndex, int maxWidth);
int GLFont_lengthOfString(GLFont *self, const char *string, int startIndex, int endIndex);
int GLFont_lengthOfCharacter_(GLFont *self, unsigned char c);

/* get the font height */
int GLFont_fontHeight(GLFont *self);

/* draws a null-terminated string */
void GLFont_drawString(GLFont *self, const char *string, int startIndex, int endIndex);

const char *GLFont_error(GLFont *self);

#endif
