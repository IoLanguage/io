//metadoc JPGImage copyright Steve Dekorte 2002
//metadoc JPGImage license BSD revised

#include "JPGImage.h"
#include <jpeglib.h>

#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#include <unistd.h>
#endif

#include <string.h>

JPGImage *JPGImage_new(void)
{
	JPGImage *self = (JPGImage *)io_calloc(1, sizeof(JPGImage));
	JPGImage_path_(self, "");
	JPGImage_error_(self, "");
	self->byteArray = UArray_new();
	self->ownsUArray = 1;
	self->quality = 0.5; /* default to meduim quality */
	self->decodingWidthHint = 0;
	self->decodingHeightHint = 0;
	return self;
}

JPGImage *JPGImage_newWithPath_(char *path)
{
	JPGImage *self = JPGImage_new();
	JPGImage_path_(self, path);
	JPGImage_load(self);
	return self;
}

void JPGImage_free(JPGImage *self)
{
	if (self->ownsUArray) UArray_free(self->byteArray);
	if (self->error) io_free(self->error);
	io_free(self->path);
	io_free(self);
}

void JPGImage_path_(JPGImage *self, const char *path)
{
	self->path = strcpy((char *)io_realloc(self->path, strlen(path)+1), path);
}

char *JPGImage_path(JPGImage *self)
{
	return self->path;
}

void JPGImage_quality_(JPGImage *self, float q)
{
	if (q<0) q = 0;
	if (q>1) q = 1;
	self->quality = q;
}

float JPGImage_quality(JPGImage *self)
{
	return self->quality;
}

void JPGImage_decodingWidthHint_(JPGImage *self, int w)
{
	self->decodingWidthHint = w;
}

int JPGImage_decodingWidthHint(JPGImage *self)
{
	return self->decodingWidthHint;
}

void JPGImage_decodingHeightHint_(JPGImage *self, int h)
{
	self->decodingHeightHint = h;
}

int JPGImage_decodingHeightHint(JPGImage *self)
{
	return self->decodingHeightHint;
}

void JPGImage_error_(JPGImage *self, const char *error)
{
	self->error = strcpy((char *)io_realloc(self->error, strlen(error)+1), error);
	/*if (strlen(self->error)) printf("JPGImage error: %s\n", self->error);*/
}

char *JPGImage_error(JPGImage *self)
{
	return self->error;
}

char JPGImage_isProgressive(JPGImage *self)
{
	FILE *infile;
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	if ((infile = fopen(self->path, "rb")) == NULL)
	{
		JPGImage_error_(self, "can't open file");
		return 0;
	}

	jpeg_stdio_src(&cinfo, infile);

	//jpeg_read_header(&cinfo, TRUE); // this causes an exit on a bad image
	jpeg_read_header(&cinfo, FALSE);
	return jpeg_has_multiple_scans(&cinfo);
}

void JPGImage_readScanLines(JPGImage *self, struct jpeg_decompress_struct *cinfo);

/*
struct MY_jpeg_error_mgr *jpeg_std_error(struct jpeg_error_mgr * err)
{
	err->error_exit = error_exit;
	err->emit_message = emit_message;
	err->output_message = output_message;
	err->format_message = format_message;
	err->reset_error_mgr = reset_error_mgr;

	err->trace_level = 0;		// default = no tracing
	err->num_warnings = 0;	// no warnings emitted yet
	err->msg_code = 0;		// may be useful as a flag for "no error"

	// Initialize message table pointers
	err->jpeg_message_table = jpeg_std_message_table;
	err->last_jpeg_message = (int) JMSG_LASTMSGCODE - 1;

	err->addon_message_table = NULL;
	err->first_addon_message = 0;	// for safety
	err->last_addon_message = 0;

	return err;
}
*/

#include <setjmp.h>

static jmp_buf env;

void MY_error_exit(j_common_ptr cinfo)
{
	(*cinfo->err->output_message) (cinfo);
	jpeg_destroy(cinfo);
	//exit(EXIT_FAILURE);
	printf("longjmp\n");
	longjmp(env, 1);
}

void JPGImage_load(JPGImage *self)
{
	FILE *infile;

	/* 1. setup error structure */
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	if(setjmp(env) == 1)
	{
		printf("longjmped\n");
		JPGImage_error_(self, "jpeg decoding error");
		return;
	}


	cinfo.err = jpeg_std_error(&jerr);
	jerr.error_exit = MY_error_exit;

	jpeg_create_decompress(&cinfo);


	/* 2. input file */
	if ((infile = fopen(self->path, "r")) == NULL)
	{
		JPGImage_error_(self, "can't open file");
		return;
	}

	jpeg_stdio_src(&cinfo, infile);

	/* 3. Call jpeg_read_header() to obtain image info. */
	//jpeg_read_header(&cinfo, TRUE); // this causes an exit on a bad image
	jpeg_read_header(&cinfo, FALSE);

	if (jpeg_has_multiple_scans(&cinfo) &&
		(self->decodingWidthHint || self->decodingHeightHint) ) /* progressive thumbnail */
		{
		int wr = 0;
		int hr = 0;
		int s = 0;
		if (self->decodingWidthHint)  wr = cinfo.image_width/self->decodingWidthHint;
		if (self->decodingHeightHint) hr = cinfo.image_height/self->decodingHeightHint;
		if (wr && hr) { s = wr < hr ? wr : hr; } else  /* take the small so no dimension is smaller than the min */
			if (wr) { s = wr; } else if (hr) { s = hr; }
		/* valid scales are 1/1 1/2 1/4 and 1/8 */
		if (s <= 1) { s = 1; } else
			if (s <= 2) { s = 2; } else
				if (s <= 4) { s = 4; } else { s = 8; }

		cinfo.scale_num = 1;
		cinfo.scale_denom = s;

		/*printf("JPEG is Progressive\n");*/
		cinfo.buffered_image = TRUE; /* select buffered-image mode so we can handle progressive jpegs */
		jpeg_start_decompress(&cinfo);
		while (!jpeg_input_complete(&cinfo))
		{
			/*printf("reading progressive pass %i\n", cinfo.input_scan_number); */
			/* adjust output decompression parameters if required */
			cinfo.do_block_smoothing = 0;
			jpeg_start_output(&cinfo, cinfo.input_scan_number);
			JPGImage_readScanLines(self, &cinfo);
			/* display scanlines */
			jpeg_finish_output(&cinfo);	/* terminate output pass */
			if (cinfo.scale_denom != 1) break; /* hack - just break since we don't know the pass resolution */
		}
		}
		else /* non-progressive */
		{
			jpeg_start_decompress(&cinfo);
			/*printf("JPEG is NOT progressive\n");*/
			JPGImage_readScanLines(self, &cinfo);
		}

		/* Finish decompression and release memory.
		* I must do it in this order because output module has allocated memory
		* of lifespan JPOOL_IMAGE; it needs to finish before releasing memory.
		*/
		jpeg_finish_decompress(&cinfo);
		jpeg_destroy_decompress(&cinfo);

		/* Close files, if we opened them */
		if (infile != stdin) fclose(infile);
}

void JPGImage_readScanLines(JPGImage *self, struct jpeg_decompress_struct *cinfo)
{
	self->width      = cinfo->output_width;
	self->height     = cinfo->output_height;
	self->components = cinfo->out_color_components;
	/*printf("JPGImage_readScanLines %i x %i x %i\n", self->width, self->height, self->components);*/

	{
		int numbytes = cinfo->output_height * cinfo->output_width * cinfo->out_color_components;
		UArray_setSize_(self->byteArray, numbytes);
	}

	/* 6. while (scan lines remain to be read) */
	{
		unsigned char **rows = io_malloc(cinfo->output_height * sizeof(unsigned char *));
		int r;
		for (r=0; r < (int)cinfo->output_height; r++)
		{
			rows[r] = (uint8_t *)UArray_bytes(self->byteArray) + (r * cinfo->output_width * cinfo->out_color_components);
		}

		while (cinfo->output_scanline < cinfo->output_height)
		{
			jpeg_read_scanlines(cinfo, rows + cinfo->output_scanline, cinfo->output_height);
		}

		io_free(rows);
	}
}

void JPGImage_save(JPGImage *self)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	FILE *outfile;		/* target file */
	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	int row_stride;		/* physical row width in image buffer */

	/* Step 1: allocate and initialize JPEG compression object */

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	/* Step 2: specify data destination (eg, a file) */
	/* Note: steps 2 and 3 can be done in either order. */

	/* Here we use the library-supplied code to send compressed data to a
		* stdio stream.  You can also write your own code to do something else.
		* VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
		* requires it in order to write binary files.
		*/
	if ((outfile = fopen(self->path, "wb")) == NULL)
	{
		JPGImage_error_(self, "can't open output file");
		return;
	}
	jpeg_stdio_dest(&cinfo, outfile);

	/* Step 3: set parameters for compression */

	/* First we supply a description of the input image.
		* Four fields of the cinfo struct must be filled in:
		*/
	cinfo.image_width = self->width;	/* image width and height, in pixels */
	cinfo.image_height = self->height;
	cinfo.input_components = self->components;  /* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB;		/* colorspace of input image */
	/* Now use the library's routine to set default compression parameters.
	 * (You must set at least cinfo.in_color_space before calling this,
	 * since the defaults depend on the source color space.)
	 */


	jpeg_set_defaults(&cinfo);
	/* Now you can set any non-default parameters you wish to.
	 * Here we just illustrate the use of quality (quantization table) scaling:
	 */

	jpeg_simple_progression(&cinfo); /* is this the right spot for this? */


	jpeg_set_quality(&cinfo, JPGImage_quality(self)*100, TRUE); /* limit to baseline-JPEG values */

	/* Step 4: Start compressor */

	/* TRUE ensures that we will write a complete interchange-JPEG file.
	 * Pass TRUE unless you are very sure of what you're doing.
	 */
	jpeg_start_compress(&cinfo, TRUE);

	/* Step 5: while (scan lines remain to be written) */
	/*           jpeg_write_scanlines(...); */

	/* Here we use the library's state variable cinfo.next_scanline as the
	 * loop counter, so that we don't have to keep track ourselves.
	 * To keep things simple, we pass one scanline per call; you can pass
	 * more if you wish, though.
	 */
	row_stride = self->width * 3;	/* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height) {
		/* jpeg_write_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could pass
		 * more than one scanline at a time if that's more convenient.
		 */
		row_pointer[0] = ((uint8_t *)UArray_bytes(self->byteArray) + (cinfo.next_scanline * row_stride));
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	/* Step 6: Finish compression */

	jpeg_finish_compress(&cinfo);
	/* After finish_compress, we can close the output file. */
	fclose(outfile);

	/* Step 7: release JPEG compression object */

	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_compress(&cinfo);
}

/*
 void JPGImage_setBuffer_length_(JPGImage *self, void *data, int size)
 { self->data = png_create( self->width, self->height, self->format); }
 */

int JPGImage_width(JPGImage *self)
{
	return self->width;
}

int JPGImage_height(JPGImage *self)
{
	return self->height;
}

void JPGImage_width_(JPGImage *self, int w)
{
	self->width = w;
}

void JPGImage_height_(JPGImage *self, int h)
{
	self->height = h;
}

void JPGImage_components_(JPGImage *self, int c)
{
	self->components = c;
}

unsigned char JPGImage_isL8(JPGImage *self)
{
	return (self->components == 1); /* LUMINANCE */
}

unsigned char JPGImage_isLA8(JPGImage *self)
{
	return (self->components == 2); /* LUMINANCE, ALPHA */
}

unsigned char JPGImage_isRGB8(JPGImage *self)
{
	return (self->components == 3); /* RGB */
}

unsigned char JPGImage_isRGBA8(JPGImage *self)
{
	return (self->components == 4); /* RGBA */
}

int JPGImage_components(JPGImage *self)
{
	return self->components;
}

int JPGImage_sizeInBytes(JPGImage *self)
{
	return self->height * self->width * self->components;
}

void *JPGImage_data(JPGImage *self)
{
	return (uint8_t *)UArray_bytes(self->byteArray);
}

UArray *JPGImage_byteArray(JPGImage *self)
{
	return self->byteArray;
}

void JPGImage_setExternalUArray_(JPGImage *self, UArray *ba)
{
	if (self->ownsUArray)
	{
		UArray_free(self->byteArray);
	}
	self->byteArray = ba;
	self->ownsUArray = 0;
}


