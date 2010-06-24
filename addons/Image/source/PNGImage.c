//metadoc PNGImage copyright Steve Dekorte 2002
//metadoc PNGImage license BSD revised

#include "PNGImage.h"
#include <png.h>

#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#include <string.h>

PNGImage *PNGImage_new(void)
{
	PNGImage *self = (PNGImage *)io_calloc(1, sizeof(PNGImage));
	PNGImage_path_(self, "");
	PNGImage_error_(self, "");
	self->byteArray = UArray_new();
	self->ownsBuffer = 1;
	return self;
}

PNGImage *PNGImage_newWithPath_(char *path)
{
	PNGImage *self = PNGImage_new();
	PNGImage_path_(self, path);
	PNGImage_load(self);
	return self;
}

void PNGImage_free(PNGImage *self)
{
	if (self->ownsBuffer) UArray_free(self->byteArray);
	if (self->error) io_free(self->error);
	io_free(self->path);
	io_free(self);
}

void PNGImage_path_(PNGImage *self, const char *path)
{ self->path = strcpy((char *)io_realloc(self->path, strlen(path)+1), path);  }

char *PNGImage_path(PNGImage *self) { return self->path; }

void PNGImage_error_(PNGImage *self, const char *error)
{
	self->error = strcpy((char *)io_realloc(self->error, strlen(error)+1), error);
	/*if (strlen(self->error)) printf("PNGImage error: %s\n", self->error);*/
}

char *PNGImage_error(PNGImage *self) { return self->error; }

void PNGImage_load(PNGImage *self)
{
	png_structp png_ptr;
	png_infop info_ptr;
	int bit_depth;
	int color_type;
	int interlace_type;
	png_uint_32 w;
	png_uint_32 h;
	int palleteComponents = 3;

	int number_passes, row;
	FILE *fp = fopen(self->path, "rb");
	PNGImage_error_(self, "");

	if (!fp)
	{
		PNGImage_error_(self, "file not found");
		return;
	}

	/* Create and initialize the png_struct with the desired error handler
		* functions.  If you want to use the default stderr and longjump method,
	* you can supply NULL for the last three parameters.  We also supply the
		* compiler header file version, so that we know if the application
		* was compiled with a compatible version of the library.  REQUIRED
		*/
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (png_ptr == NULL)
	{
		fclose(fp);
		PNGImage_error_(self, "unable to read png from file");
		return;
	}

	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);

	if (info_ptr == NULL)
	{
		fclose(fp);
		png_destroy_read_struct(&(png_ptr), png_infopp_NULL, png_infopp_NULL);
		PNGImage_error_(self, "error allocating png struct");
		return;
	}

	/* Set error handling if you are using the setjmp/longjmp method (this is
		* the normal method of doing things with libpng).  REQUIRED unless you
* set up your own error handlers in the png_create_read_struct() earlier.
*/

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		/* Free all of the memory associated with the png_ptr and info_ptr */
		png_destroy_read_struct(&(png_ptr), &(info_ptr), png_infopp_NULL);
		fclose(fp);
		PNGImage_error_(self, self->path);
		return;
	}

	/* Set up the input control if you are using standard C streams */
	png_init_io(png_ptr, fp);
	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr, &w, &h, &(bit_depth),
				 &(color_type), &(interlace_type), int_p_NULL, int_p_NULL);

	self->width  = w;
	self->height = h;

	/* tell libpng to strip 16 bit/color files down to 8 bits/color */
	png_set_strip_16(png_ptr);


	/* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
		* byte into separate bytes (useful for paletted and grayscale images).
		*/
	png_set_packing(png_ptr);

	/* Expand paletted colors into true RGB triplets */
	/*
	 if (color_type == PNG_COLOR_TYPE_PALETTE)
	 png_set_palette_rgb(png_ptr);
	 */

	/* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png_ptr);

	/* Expand paletted or RGB images with transparency to full alpha channels
		* so the data will be available as RGBA quartets.
		*/
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(png_ptr);
		palleteComponents = 4;
	}

	/* swap bytes of 16 bit files to least significant byte first */
	/*png_set_swap(png_ptr);*/

	/* Add filler (or alpha) byte (before/after each RGB triplet) */
	/*png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);*/

	/* Turn on interlace handling.  REQUIRED if you are not using
		* png_read_image().  To see how to handle interlacing passes,
		* see the png_read_row() method below:
		*/
	number_passes = png_set_interlace_handling(png_ptr);

	/* Allocate the memory to hold the image using the fields of info_ptr. */

	/* The easiest way to read the image: */
	{
		/*png_bytep row_pointers[height];*/
		png_bytep *row_pointers = (png_bytep *)io_malloc(self->height*sizeof(void *));

		for (row = 0; row < self->height; row++)
		{
			/*int bpr = png_get_rowbytes(png_ptr, info_ptr);*/
			int bpr = png_get_rowbytes(png_ptr, info_ptr) * 4;
			row_pointers[row] = png_malloc(png_ptr, bpr);
		}

		/* Now it's time to read the image.  One of these methods is REQUIRED */
		png_read_image(png_ptr, row_pointers);

		{
			int bytesPerRow;
			switch(color_type)
			{
				case PNG_COLOR_TYPE_GRAY:
					self->components = 1; break;
				case PNG_COLOR_TYPE_PALETTE:
					self->components = palleteComponents; break;
				case PNG_COLOR_TYPE_RGB:
					self->components = 3; break;
				case PNG_COLOR_TYPE_RGB_ALPHA:
					self->components = 4; break;
				case PNG_COLOR_TYPE_GRAY_ALPHA:
					self->components = 2; break;
			}

			bytesPerRow = self->width * self->components;
			UArray_setSize_(self->byteArray, self->width * self->height * self->components);

			for (row = 0; row < self->height; row++)
			{
				int i = row*(self->width*self->components);
				memcpy((uint8_t *)UArray_bytes(self->byteArray) + i, row_pointers[row], bytesPerRow);
				io_free(row_pointers[row]);
			}
		}

		io_free(row_pointers);
	}
	/* read rest of file, and get additional chunks in info_ptr - REQUIRED */
	png_read_end(png_ptr, (info_ptr));

	/* At this point you have read the entire image */

	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&(png_ptr), &(info_ptr), png_infopp_NULL);

	fclose(fp);
	return;
}

void PNGImage_save(PNGImage *self)
{
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;
	/*png_colorp palette;*/

	/* open the file */
	fp = fopen(self->path, "wb");

	if (fp == NULL)
	{
		PNGImage_error_(self, "unable to open file");
		return;
	}

	/* Create and initialize the png_struct with the desired error handler
		* functions.  If you want to use the default stderr and longjump method,
	* you can supply NULL for the last three parameters.  We also check that
		* the library version is compatible with the one used at compile time,
		* in case we are using dynamically linked libraries.  REQUIRED.
	*/
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (png_ptr == NULL)
	{
		fclose(fp);
		PNGImage_error_(self, "unable to create png struct");
		return;
	}

	/* Allocate/initialize the image information data.  REQUIRED */
	info_ptr = png_create_info_struct(png_ptr);

	if (info_ptr == NULL)
	{
		fclose(fp);
		png_destroy_write_struct(&png_ptr,  png_infopp_NULL);
		PNGImage_error_(self, "unable to create png struct");
		return;
	}

	/* Set error handling.  REQUIRED if you aren't supplying your own
		* error handling functions in the png_create_write_struct() call.
		*/
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		/* If we get here, we had a problem reading the file */
		fclose(fp);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		PNGImage_error_(self, "problem writing file");
		return;
	}

	/* One of the following I/O initialization functions is REQUIRED */
	/* set up the output control if you are using standard C streams */
	png_init_io(png_ptr, fp);



	/* Set the image information here.  Width and height are up to 2^31,
		* bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
		* the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
		* PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
		* or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
		* PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
		* currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
		*/
	png_set_IHDR(png_ptr, info_ptr, self->width, self->height, 8, PNGImage_pngColorType(self),
				 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	/* set the palette if there is one.  REQUIRED for indexed-color images */
	/*palette = (png_colorp)png_malloc(png_ptr, PNG_MAX_PALETTE_LENGTH * sizeof (png_color));*/
	/* ... set palette colors ... */
	/*png_set_PLTE(png_ptr, info_ptr, palette, PNG_MAX_PALETTE_LENGTH);*/
	/* You must not free palette here, because png_set_PLTE only makes a link to
		the palette that you malloced.  Wait until you are about to destroy
		the png structure. */

	/* optional significant bit chunk */
	/* if we are dealing with a grayscale image then */
	/*
	 if (self->components < 3)
	 { sig_bit.gray = true_bit_depth; }
	 else
	 */
	/* otherwise, if we are dealing with a color image then */
	/*
	 {
		 sig_bit.red   = true_red_bit_depth;
		 sig_bit.green = true_green_bit_depth;
		 sig_bit.blue   = true_blue_bit_depth;
	 }
	 */
	/* if the image has an alpha channel then */
	/*
	 if (self->components == 2 || self->components = 4)
	 {
		 sig_bit.alpha = true_alpha_bit_depth;
		 png_set_sBIT(png_ptr, info_ptr, sig_bit);
	 }
	 */

	/* Optional gamma chunk is strongly suggested if you have any guess
		* as to the correct gamma of the image.
		*/
	/*png_set_gAMA(png_ptr, info_ptr, gamma);*/

	/* Optionally write comments into the image */
	/*
	 text_ptr[0].key = "Title";
	 text_ptr[0].text = "Mona Lisa";
	 text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
	 text_ptr[1].key = "Author";
	 text_ptr[1].text = "Leonardo DaVinci";
	 text_ptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
	 text_ptr[2].key = "Description";
	 text_ptr[2].text = "<long text>";
	 text_ptr[2].compression = PNG_TEXT_COMPRESSION_zTXt;
#ifdef PNG_iTXt_SUPPORTED
	 text_ptr[0].lang = NULL;
	 text_ptr[1].lang = NULL;
	 text_ptr[2].lang = NULL;
#endif
	 png_set_text(png_ptr, info_ptr, text_ptr, 3);
	 */

	/* other optional chunks like cHRM, bKGD, tRNS, tIME, oFFs, pHYs, */
	/* note that if sRGB is present the gAMA and cHRM chunks must be ignored
		* on read and must be written in accordance with the sRGB profile */

	/* Write the file header information.  REQUIRED */
	png_write_info(png_ptr, info_ptr);

	/* If you want, you can write the info in two steps, in case you need to
		* write your private chunk ahead of PLTE:
		*
		*   png_write_info_before_PLTE(write_ptr, write_info_ptr);
	*   write_my_chunk();
	*   png_write_info(png_ptr, info_ptr);
	*
		* However, given the level of known- and unknown-chunk support in 1.1.0
		* and up, this should no longer be necessary.
		*/

	/* Once we write out the header, the compression type on the text
		* chunks gets changed to PNG_TEXT_COMPRESSION_NONE_WR or
		* PNG_TEXT_COMPRESSION_zTXt_WR, so it doesn't get written out again
		* at the end.
		*/

	/* set up the transformations you want.  Note that these are
		* all optional.  Only call them if you want them.
		*/

	/* invert monochrome pixels */
	/*png_set_invert_mono(png_ptr);*/

	/* Shift the pixels up to a legal bit depth and fill in
		* as appropriate to correctly scale the image.
		*/
	/*png_set_shift(png_ptr, &sig_bit);*/

	/* pack pixels into bytes */
	/*png_set_packing(png_ptr);*/

	/* swap location of alpha bytes from ARGB to RGBA */
	/*png_set_swap_alpha(png_ptr);*/

	/* Get rid of filler (OR ALPHA) bytes, pack XRGB/RGBX/ARGB/RGBA into
		* RGB (4 channels -> 3 channels). The second parameter is not used.
		*/
	/*png_set_filler(png_ptr, 0, PNG_FILLER_BEFORE);*/

	/* flip BGR pixels to RGB */
	/*png_set_bgr(png_ptr);*/

	/* swap bytes of 16-bit files to most significant byte first */
	/*png_set_swap(png_ptr);*/

	/* swap bits of 1, 2, 4 bit packed pixel formats */
	/*png_set_packswap(png_ptr);*/

	/* turn on interlace handling if you are not using png_write_image() */
	/*
	 if (interlacing)
	 number_passes = png_set_interlace_handling(png_ptr);
	 else
	 number_passes = 1;
	 */

	/* The easiest way to write the image (you may have a different memory
		* layout, however, so choose what fits your needs best).  You need to
* use the first method if you aren't handling interlacing yourself.
*/
	{
		png_uint_32 k;
		png_bytep *row_pointers = io_malloc(self->height*sizeof(png_bytep *));  // JEFF DRAKE MOD: YOU CAN'T HAVE A NON-CONSTANT
									   // ARRAY HERE!!!!
		for (k = 0; k < (png_uint_32)self->height; k++)
		{
			/*row_pointers[k] = UArray_bytes(self->byteArray) + k * self->width * 8 * self->components;*/
			row_pointers[k] = (uint8_t *)UArray_bytes(self->byteArray) + k * self->width * self->components;
		}

		/* write out the entire image data in one call */
		png_write_image(png_ptr, row_pointers);
		io_free(row_pointers);
	}

	/* You can write optional chunks like tEXt, zTXt, and tIME at the end
		* as well.  Shouldn't be necessary in 1.1.0 and up as all the public
		* chunks are supported and you can use png_set_unknown_chunks() to
		* register unknown chunks into the info structure to be written out.
		*/

	/* It is REQUIRED to call this to finish writing the rest of the file */
	png_write_end(png_ptr, info_ptr);
	/*png_write_IEND(png_ptr);*/

	/* If you png_malloced a palette, free it here (don't free info_ptr->palette,
		as recommended in versions 1.0.5m and earlier of this example; if
		libpng mallocs info_ptr->palette, libpng will free it).  If you
allocated it with malloc() instead of png_malloc(), use free() instead
of png_free(). */
	/*
	 png_free(png_ptr, palette);
	 palette=NULL;
	 */

	/* Similarly, if you png_malloced any data that you passed in with
		png_set_something(), such as a hist or trans array, free it here,
		when you can be sure that libpng is through with it. */
	/*
	 png_free(png_ptr, trans);
	 trans=NULL;
	 */

	/* clean up after the write, and free any memory allocated */
	png_destroy_write_struct(&png_ptr, &info_ptr);

	/* close the file */
	fclose(fp);
}

/*
 void PNGImage_setBuffer_length_(PNGImage *self, void *data, int size)
 { self->data = png_create( self->width, self->height, self->format); }
 */

int PNGImage_width(PNGImage *self)  { return self->width; }
int PNGImage_height(PNGImage *self) { return self->height; }

void PNGImage_width_(PNGImage *self, int w)  { self->width = w; }
void PNGImage_height_(PNGImage *self, int h) { self->height = h; }
void PNGImage_components_(PNGImage *self, int c) { self->components = c; }

int PNGImage_pngColorType(PNGImage *self)
{
	switch (self->components)
	{
		case 1: return PNG_COLOR_TYPE_GRAY;
		case 2: return PNG_COLOR_TYPE_GRAY_ALPHA;
		case 3: return PNG_COLOR_TYPE_RGB;
		case 4: return PNG_COLOR_TYPE_RGBA;
	}
	return -1;
}

unsigned char PNGImage_isL8(PNGImage *self)    { return (self->components == 1); }
unsigned char PNGImage_isLA8(PNGImage *self)   { return (self->components == 2); }
unsigned char PNGImage_isRGB8(PNGImage *self)  { return (self->components == 3); }
unsigned char PNGImage_isRGBA8(PNGImage *self) { return (self->components == 4); }

int PNGImage_components(PNGImage *self)
{
	return self->components;
}

int PNGImage_sizeInBytes(PNGImage *self)
{
	return self->height * self->width * self->components;
}

void *PNGImage_data(PNGImage *self)
{
	return (uint8_t *)UArray_bytes(self->byteArray);
}

UArray *PNGImage_byteArray(PNGImage *self)
{
	return self->byteArray;
}

void PNGImage_setExternalUArray_(PNGImage *self, UArray *ba)
{
	if (self->ownsBuffer) UArray_free(self->byteArray);
	self->byteArray = ba;
	self->ownsBuffer = 0;
}


