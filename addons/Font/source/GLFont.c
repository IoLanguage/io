/* GLFont.c - by Mike Austin */

#include "GLFont.h"
#include "FreeTypeErrorCodes.h"

static FT_Library gFreeTypeLib;

void GLFont_initFontTexture( GLFont *self );
void GLFont_setupTexture( GLFont *self );
void GLFont_drawTextureString_( GLFont *self, const char *string, int startIndex, int endIndex);
void GLFont_drawPixmapString_( GLFont *self, const char *string, int startIndex, int endIndex);

void GLFont_init( void )
{
	if (!gFreeTypeLib)
	{
		FT_Error error = FT_Init_FreeType( &gFreeTypeLib );
		if( error ) { puts( "Init FreeType failed" ); }
	}
}

void GLFont_done( void )
{
	FT_Done_FreeType(gFreeTypeLib);
	gFreeTypeLib = NULL;
}

GLFont *GLFont_new( void )
{
	GLFont *self = calloc( 1, sizeof( GLFont ) );
	self->pixelSize = 12;
	return self;
}

/*
GLFont *GLFont_clone(GLFont *self)
{
	GLFont *newObject = GLFont_new();
	newObject->pixelSize = 12;
	return newObject;
}
*/

int GLFont_textureId(GLFont *self)
{
	if (!self->texId) glGenTextures(1, &self->texId);
	return self->texId;
}

void GLFont_free( GLFont *self )
{
	/*printf("GLFont_free %p %p\n", (void *)self, (void *)self->face);*/
	if (self->face && gFreeTypeLib) FT_Done_Face( self->face );
	self->face = NULL;
	free(self);
}

void GLFont_initFontTexture( GLFont *self ) {
	int x, y;

	self->isTextured = 0;
	self->didInit = 1;
	if (self->pixelSize > GLFONT_TEX_SIZE/6) { return; } /* definitely can't texture it */

	for( y = 0; y < GLFONT_TEX_SIZE; ++y ) {
		for( x = 0; x < GLFONT_TEX_SIZE; ++x ) {
			self->texture[y][x][0] = 255;
			self->texture[y][x][1] = 255;
			self->texture[y][x][2] = 255;
			self->texture[y][x][3] = 0;
		}
	}

	GLFont_setupTexture( self );
}

void GLFont_setupTexture( GLFont *self )
{
	unsigned char c;
	int x, y;
	int left, top, width, height;
	int penX = 0, penY = 0;

	int minCharacter = 32;
	int maxCharacter = 128;

	/*
	 int minCharacter = 0;
	 int maxCharacter = 255;
	 */

	self->maxWidth = 0;
	self->maxHeight = 0;

	for( c = minCharacter; c <= maxCharacter; ++c )
	{
		FT_Load_Char( self->face, c, FT_LOAD_RENDER );

		width  = self->face->glyph->bitmap.width;
		height = self->face->glyph->bitmap.rows;

		if( width > self->maxWidth ) self->maxWidth = width;
		if( height > self->maxHeight ) self->maxHeight = height;

		self->symbol[c].advance = self->face->glyph->advance.x / 64.0;
		self->symbol[c].left = self->face->glyph->bitmap_left;
		self->symbol[c].top = self->face->glyph->bitmap_top;
	}

	for( c = minCharacter; c <= maxCharacter; ++c )
	{
		FT_Load_Char( self->face, c, FT_LOAD_RENDER );

		left   = self->face->glyph->bitmap_left;
		top    = self->face->glyph->bitmap_top;
		width  = self->face->glyph->bitmap.width;
		height = self->face->glyph->bitmap.rows;

		if( penX + width > GLFONT_TEX_SIZE ) {
			penY += self->maxHeight + 1;
			penX = 0;
		}
		if( penY + self->maxHeight + 1 > GLFONT_TEX_SIZE ) {
			self->isTextured = 0;
			self->drawString = GLFont_drawPixmapString_;
			/*puts( "Font not textured" );*/
			return;
		}

		for( y = 0; y < height; ++y ) {
			for( x = 0; x < width; ++x ) {
				self->texture[penY+y][penX+x][0] = 255;
				self->texture[penY+y][penX+x][1] = 255;
				self->texture[penY+y][penX+x][2] = 255;
				self->texture[penY+y][penX+x][3] = self->face->glyph->bitmap.buffer[y*width+x];
			}
		}

		self->symbol[c].xpos = penX;
		self->symbol[c].ypos = penY;
		self->symbol[c].width = width;
		self->symbol[c].height = height;

		self->texCoords[c].left   = (self->symbol[c].xpos / (float)GLFONT_TEX_SIZE);
		self->texCoords[c].right  = (self->symbol[c].xpos / (float)GLFONT_TEX_SIZE)
			+ (self->symbol[c].width / (float)GLFONT_TEX_SIZE);
		self->texCoords[c].top    = (self->symbol[c].ypos / (float)GLFONT_TEX_SIZE);
		self->texCoords[c].bottom = (self->symbol[c].ypos / (float)GLFONT_TEX_SIZE)
			+ (self->maxHeight / (float)GLFONT_TEX_SIZE);

		penX += width + 1;
	}

	/*puts( "Font is textured" );*/
	self->isTextured = 1;
	self->drawString = GLFont_drawTextureString_;

	glPixelStorei( GL_UNPACK_ROW_LENGTH, GLFONT_TEX_SIZE );
	glBindTexture( GL_TEXTURE_2D, GLFont_textureId(self) );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, GLFONT_TEX_SIZE, GLFONT_TEX_SIZE, 0,
				GL_RGBA, GL_UNSIGNED_BYTE, self->texture );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
}

void GLFont_loadFont( GLFont *self, const char *path )
{
	self->didInit = 0;
	self->errorCode = FT_New_Face( gFreeTypeLib, path, 0, &self->face );
	if( self->errorCode ) { return; }
	self->isLoaded = 1;
	GLFont_setPixelSize(self, self->pixelSize);
}

void GLFont_setPixelSize( GLFont *self, unsigned int size )
{
	self->pixelSize = size;
	if (self->isLoaded)
	{ self->errorCode = FT_Set_Pixel_Sizes(self->face, 0, size ); }
	self->didInit = 0;
}

/*
void GLFont_setPointSize( GLFont *self, unsigned int size )
{
	self->pixelSize = size;
	if (self->isLoaded)
	{ self->errorCode = FT_Set_Char_Size( self->face, 0, size * 64, 72, 72 ); }
	self->didInit = 0;
}
*/

int GLFont_pixelSize(GLFont *self) { return self->pixelSize; }
int GLFont_isTextured(GLFont *self) { return self->isTextured; }

int GLFont_stringIndexAtWidth(GLFont *self, const char *string, int startIndex, int maxWidth)
{
	const unsigned char *c = (const unsigned char *)string + startIndex;
	int width = 0;
	int i = 0;

	while (*c)
	{
		int w = GLFont_lengthOfCharacter_(self, *c);
		if (width + (w*.5) > maxWidth) return i;
		width += w;
		i++;
		if (width > maxWidth) return i;
		if (self->errorCode) return -1;
		c++;
	}
	return i;
}

int GLFont_lengthOfString(GLFont *self, const char *string, int startIndex, int endIndex)
{
	const unsigned char *c = (const unsigned char *)string + startIndex;
	int width = 0;
	int i = 0;

	while( *c && i != endIndex)
	{
		width += GLFont_lengthOfCharacter_(self, *c);
		if (self->errorCode) return -1;
		c++; i++;
	}
	return width;
}

int GLFont_lengthOfCharacter_(GLFont *self, unsigned char c)
{
	if( self->isTextured ) return self->symbol[c].advance;
	self->errorCode = FT_Load_Char(self->face, c, FT_LOAD_RENDER);
	if (self->errorCode) return -1;
	return self->face->glyph->advance.x / 64.0;
}

int GLFont_fontHeight( GLFont *self ) {
	return self->maxHeight;
}

void GLFont_drawString( GLFont *self, const char *string, int startIndex, int endIndex) {
	if (!self->isLoaded) return;
	if (!self->didInit) GLFont_initFontTexture( self );
	if( self->isTextured ) {
		GLFont_drawTextureString_( self, string, startIndex, endIndex );
	} else {
		GLFont_drawPixmapString_( self, string, startIndex, endIndex );
	}
}

void GLFont_drawTextureString_( GLFont *self, const char *string, int startIndex, int endIndex) {
	int i = startIndex;
	GLFont_TexCoords texCoords;

	glRasterPos2d(0,0);
	glEnable( GL_TEXTURE_2D );
	glBindTexture(GL_TEXTURE_2D, GLFont_textureId(self));
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glPushMatrix();
	while( i < endIndex ) {
		unsigned char c = string[i];
		texCoords = self->texCoords[c];
		glBegin( GL_QUADS );
		glTexCoord2f( texCoords.left,  texCoords.bottom );
		  glVertex2i( self->symbol[c].left,
					  self->symbol[c].top - self->maxHeight );
		glTexCoord2f( texCoords.right, texCoords.bottom );
		  glVertex2i( self->symbol[c].left + self->symbol[c].width,
					  self->symbol[c].top - self->maxHeight );
		glTexCoord2f( texCoords.right, texCoords.top );
		  glVertex2i( self->symbol[c].left + self->symbol[c].width,
					  self->symbol[c].top );
		glTexCoord2f( texCoords.left,  texCoords.top );
		  glVertex2i( self->symbol[c].left,
					  self->symbol[c].top );
		glEnd();
		/*glBegin( GL_QUADS );*/
		glTexCoord2f( texCoords.left,  texCoords.top ); glVertex2i( self->symbol[c].left,
														self->symbol[c].top + self->maxHeight );
		glTexCoord2f( texCoords.right, texCoords.top ); glVertex2i( self->symbol[c].left + self->symbol[c].width,
														self->symbol[c].top + self->maxHeight );
		glTexCoord2f( texCoords.right, texCoords.bottom ); glVertex2i( self->symbol[c].left + self->symbol[c].width,
														   self->symbol[c].top );
		glTexCoord2f( texCoords.left,  texCoords.bottom ); glVertex2i( self->symbol[c].left,
														   self->symbol[c].top );
		glEnd();
		glTranslatef( self->symbol[c].advance, 0, 0 );
		i++;
	}
	glPopMatrix();
	glDisable( GL_TEXTURE_2D );
}

void GLFont_drawPixmapString_( GLFont *self, const char *string, int startIndex, int endIndex) {
	const unsigned char *c = (const unsigned char *)string;
	int penX = 0, x, y;
	int width, height;
	float color[4];

	/*printf("GLFont_drawPixmapString_\n");*/

	glDisable( GL_TEXTURE_2D );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

	while( *c != '\0' ) {
		self->errorCode = FT_Load_Char( self->face, *c, FT_LOAD_RENDER );
		if (self->errorCode) return;

		width = self->face->glyph->bitmap.width;
		height = self->face->glyph->bitmap.rows;

		if( width > GLFONT_TEX_SIZE || height > GLFONT_TEX_SIZE ) {
			/*fprintf( stderr, "Font too large\n" );*/
			return;
		}

		glGetFloatv( GL_CURRENT_COLOR, color );
		memset( self->texture, 0, GLFONT_TEX_SIZE * GLFONT_TEX_SIZE * 4 );
		for( y = 0; y < height; y++ ) {
			for( x= 0; x < width; x++ ) {
				self->texture[y][x][0] = 255 * color[0];
				self->texture[y][x][1] = 255 * color[1];
				self->texture[y][x][2] = 255 * color[2];
				self->texture[y][x][3] = self->face->glyph->bitmap.buffer[y*width+x];
			}
		}

		glPixelStorei( GL_UNPACK_ROW_LENGTH, GLFONT_TEX_SIZE );
		glPixelZoom( 1, -1 );
		glRasterPos2d( penX + self->face->glyph->bitmap_left, self->face->glyph->bitmap_top );
		glDrawPixels( GLFONT_TEX_SIZE, GLFONT_TEX_SIZE, GL_RGBA, GL_UNSIGNED_BYTE, self->texture );
		penX += self->face->glyph->advance.x / 64.0;
		++c;
	}
}

const char *GLFont_error(GLFont *self)
{
	if (!self->errorCode) return NULL;
	return FreeTypeErrorStringForCode(self->errorCode);
}


