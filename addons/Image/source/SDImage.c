/*
 * SDImage.c 
 * Simple SDImage manipulation library
 * Copyright Steve Dekorte, 2001
 */

#include "SDImage.h"
//#include "SDImage_loader.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

SDImage *SDImage_new(
					 int w, int h, 
					 int spp, int bps, 
					 unsigned char *data)
{
	SDImage *self = (SDImage *)IMAGE_MALLOC(sizeof(SDImage));  
	self->w = w;
	self->h = h;
	self->spp = spp;
	self->bps = bps;
	self->numBytes = (w*h*spp*bps)/8;
	
	if (data != NULL)
	{ 
		self->data = data;
		self->ownsData = 0;
	}
	else
	{ 
		self->data = IMAGE_MALLOC(self->numBytes); 
		self->ownsData = 1;
	}
	return self;
}

void SDImage_free(SDImage *self)
{
	if (self->ownsData) { IMAGE_FREE(self->data); }
	IMAGE_FREE(self);
}

void SDImage_print(SDImage *self)
{
	printf("  SDImage %ix%i spp:%i bps:%i bytes:%i ownsData:%i\n", 
		   self->w, self->h, self->spp, self->bps, (int)self->numBytes, self->ownsData);
}

SDImage *SDImage_clone(SDImage *self)
{
	SDImage *other = SDImage_new(self->w, self->h, self->spp, self->bps, NULL);
	SDImage_copy(self, other);
	return other;
}

void SDImage_copy(SDImage *self, SDImage *other)
{
	if (SDImage_sameFormatAs(self, other)!=1)
	{ printf("ERROR: SDImage_copy: SDImages need to have same format\n"); return; }
	memcpy(self->data, other->data, self->numBytes);
}

void SDImage_copyByPixel(SDImage *self, SDImage *other)
{
	/* this is mostly here as an example */
	int w = self->w;
	int h = self->h;
	int pixelSize = SDImage_pixelSize(self);
	int x, y;
	
	for (x=0; x<w; x++) 
	{    
		for (y=0; y<h; y++) 
		{    
			unsigned char *ip = SDImage_pixelAt(other, x, y);
			unsigned char *op = SDImage_pixelAt(self, x, y);
			memcpy(ip, op, pixelSize);
		}
	}
	
	if (SDImage_isEqual(self, other)==0)
	{ 
		printf("ERROR: SDImage_copyByPixel didn't work\n"); 
	}
}

/*=====================================================
Comparisions                                     
=====================================================*/

int SDImage_isEqual(SDImage *self, SDImage *other)
{
	return (SDImage_sameFormatAs(self, other) &&
			memcmp(&self->data, &other->data, (size_t)self->numBytes));
}

int SDImage_sameFormatAs(SDImage *self, SDImage *other)
{
	return (
			self->w == other->w &&
			self->h == other->h &&
			self->spp == other->spp  &&
			self->bps == other->bps  &&
			self->numBytes == other->numBytes);
}

int SDImage_sameSizeAs(SDImage *self, SDImage *other)
{
	return (
			self->w == other->w &&
			self->h == other->h);
}

/*=====================================================
Accessors                                     
=====================================================*/

size_t SDImage_dataLength(SDImage *self) { return self->numBytes; }
int SDImage_pixelSize(SDImage *self) { return (self->spp*self->bps)/8; }

unsigned char *SDImage_pixelAt(SDImage *self, int x, int y)
{
	int w = self->w;
	int h = self->h;
	if (x < 0) { x = 0; } else if (x > w-1) { x = w-1; }
	if (y < 0) { y = 0; } else if (y > h-1) { y = h-1; }
	
	return &self->data[((x + (y*w))*(self->spp*self->bps))/8];
}

#define IMAGE_PIXELAT(self, x, y, w, pixelSize) &self->data[(x+(y*w))*pixelSize]

/*=====================================================
Transformations                                     
=====================================================*/

SDImage *SDImage_smoothScaleTo(SDImage *self, int ow, int oh)
{
	int w = self->w;
	int h = self->h;
	
	if (ow==w && oh==h)
	{
		SDImage *outimg = SDImage_new(ow, oh, self->spp, self->bps, NULL);
		SDImage_copy(outimg, self);
		return outimg;
	}
	
	if ((ow*oh) < (w*h)) 
	{
		if ((ow*oh) > (w*h)/4)
		{ 
			SDImage *img1 = SDImage_scaleTo(self, ow*2, oh*2);
			SDImage *img2 = SDImage_blur(img1);
			SDImage *img3 = SDImage_scaleTo(img2, ow, oh);
			SDImage_free(img1);
			SDImage_free(img2);
			return img3;
		}
		else
		{ 
			SDImage *img = SDImage_smoothShrinkTo(self, ow, oh);
			//SDImage_flipxy(img); 
			return img;
		}
	}
	else
	{
		/* expand: scale then blur, repeat */
		SDImage *img = self;
		int sw = w;
		int sh = h;
		
		while (sw<ow || sh<oh)
		{
			sw = sw*2;
			sh = sh*2;
			if (sw>ow) { sw = ow; }
			if (sh>oh) { sh = oh; }
			
			// printf("expand s:%ix%i o:%ix%i\n", sw, sh, ow, oh);
			{
				SDImage *scaleimg = SDImage_scaleTo(img, sw, sh);
				if (img!=self) { SDImage_free(img); }
				{
					//SDImage *blurimg = SDImage_blurEvenPixels(img);
					SDImage *blurimg = SDImage_blur(scaleimg);
					SDImage_free(scaleimg);
					img = blurimg;
				}
			}
		}
		return img; 
	}
}

SDImage *SDImage_scaleTo(SDImage *self, int ow, int oh)
{
	SDImage *outimg = SDImage_new(ow, oh, self->spp, self->bps, NULL);
	int iw = self->w;
	int ih = self->h;
	int ox, oy;
	int pixelSize = SDImage_pixelSize(self);
	unsigned char *op = outimg->data;
	
	float rw = (float)iw/(float)ow;
	float rh = (float)ih/(float)oh;
	//printf("scaleTo\n");
	
	for (oy=0; oy<oh; oy++) 
	{  
		for (ox=0; ox<ow; ox++) 
		{
			int ix = ox*rw;
			int iy = oy*rh;    
			unsigned char *ip = IMAGE_PIXELAT(self, ix, iy, iw, pixelSize); //SDImage_pixelAt(self, ix, iy);
			memcpy(op, ip, pixelSize);
			op = op + pixelSize;
		}
	}
	
	return outimg;
}

SDImage *SDImage_blurTimes(SDImage *self, int times)
{
	SDImage *img = self;
	int n;
	
	for (n=0; n<times; n++)
	{
		SDImage *blurimg = SDImage_blur(img);
		if (img!=self) { SDImage_free(img); }
		img = blurimg;
	}
	
	return img;
}

SDImage *SDImage_blur(SDImage *self)
{
	SDImage *outimg = SDImage_new(self->w, self->h, self->spp, self->bps, NULL);
	int pixelSize = SDImage_pixelSize(self);
	int w = self->w;
	int h = self->h;
	int x, y;
	int r, g, b;
	unsigned char *op = outimg->data;
	//printf("blur\n");
	
	for (y=0; y<h; y++) 
	{    
		for (x=0; x<w; x++) 
		{
			int xoffset, yoffset;
			r = 0; g = 0; b = 0;
			for (yoffset=-1; yoffset<2; yoffset++) 
			{
				for (xoffset=-1; xoffset<2; xoffset++) 
				{
					unsigned char *p = SDImage_pixelAt(self, x+xoffset, y+yoffset);
					r = r + p[0];
					g = g + p[1];
					b = b + p[2];
				}
			}
			op[0] = (unsigned char)(r/9); 
			op[1] = (unsigned char)(g/9); 
			op[2] = (unsigned char)(b/9);
			op = op + pixelSize;
		}
	}
	
	return outimg;
}

SDImage *SDImage_highBoostFilter(SDImage *self, float v)
{
	/* See: http://www.dai.ed.ac.uk/CVonline/LOCAL_COPIES/OWENS/LECT5/node3.html */
	SDImage *outimg = SDImage_new(self->w, self->h, self->spp, self->bps, NULL);
	int pixelSize = SDImage_pixelSize(self);
	int w = self->w;
	int h = self->h;
	int x, y;
	int r, g, b;
	float ww = (9*v)-1;
	unsigned char *op = outimg->data;
	//printf("blur\n");
	
	for (y=0; y<h; y++) 
	{    
		for (x=0; x<w; x++) 
		{
			int xoffset, yoffset;
			r = 0; g = 0; b = 0;
			for (yoffset=-1; yoffset<2; yoffset++) 
			{
				for (xoffset=-1; xoffset<2; xoffset++) 
				{
					unsigned char *p = SDImage_pixelAt(self, x+xoffset, y+yoffset);
					
					if (x == 0 && y == 0)
					{
						r = r + p[0]*ww;
						g = g + p[1]*ww;
						b = b + p[2]*ww;
					}
					else
					{
						r = r - p[0];
						g = g - p[1];
						b = b - p[2];        
					}
				}
			}
			
			op[0] = (unsigned char)(r/9); 
			op[1] = (unsigned char)(g/9); 
			op[2] = (unsigned char)(b/9);
			op = op + pixelSize;
		}
	}
	
	return outimg;
}

SDImage *SDImage_smoothShrinkTo(SDImage *self, int ow, int oh)
{
	SDImage *outimg = SDImage_new(ow, oh, self->spp, self->bps, NULL);
	int iw = self->w;
	int ih = self->h;
	int ox, oy;
	int pixelSize = SDImage_pixelSize(self);
	unsigned char *op = outimg->data;
	int range = (((float)iw/(float)ow)+((float)ih/(float)oh))/4;
	float rw = (float)iw/(float)ow;
	float rh = (float)ih/(float)oh;
	//printf("SDImage_smootherScaleTo range=%i\n", range);
	
	for (oy=0; oy<oh; oy++) 
	{  
		for (ox=0; ox<ow; ox++) 
		{
			//int ix = ox*rw;
			//int iy = oy*rh;    
			SDImage_aveAtWithRangeTo(self, ox*rw, oy*rh, range, op);
			op = op + pixelSize;
		}
	}
	
	return outimg;
}

void SDImage_aveAtWithRangeTo(SDImage *self, int x, int y, int range, unsigned char *op)
{
	long int r, g, b;
	int xoffset, yoffset;
	int area = 0;
	int minx = x-range;
	int miny = y-range;
	int maxx = x+range+1;
	int maxy = y+range+1;
	unsigned char *ip;
	r = g = b = 0;
	
	for (yoffset=miny; yoffset<maxy; yoffset++) 
	{
		for (xoffset=minx; xoffset<maxx; xoffset++) 
		{
			ip = SDImage_pixelAt(self, xoffset, yoffset);
			r = r + ip[0];
			g = g + ip[1];
			b = b + ip[2];
			area++;
		}
	}
	
	op[0] = (unsigned char)(r/area); 
	op[1] = (unsigned char)(g/area); 
	op[2] = (unsigned char)(b/area);
}

SDImage *SDImage_flipy(SDImage *self)
{
	SDImage *outimg = SDImage_new(self->w, self->h, self->spp, self->bps, NULL);
	int w = self->w;
	int h = self->h;
	int pixelSize = SDImage_pixelSize(self);
	int rowSize = pixelSize*w;
	int y;
	unsigned char *ip = self->data;
	
	for (y=0; y<h; y++) 
	{    
		unsigned char *op = SDImage_pixelAt(outimg, 0, h-y-1);
		memcpy(op, ip, rowSize);
		ip = ip + rowSize;
	}
	
	return outimg;
}

SDImage *SDImage_flipx(SDImage *self)
{
	SDImage *outimg = SDImage_new(self->w, self->h, self->spp, self->bps, NULL);
	int w = self->w;
	int h = self->h;
	int pixelSize = SDImage_pixelSize(self);
	int x, y;
	
	for (x=0; x<w; x++) 
	{    
		for (y=0; y<h; y++) 
		{    
			unsigned char *ip = SDImage_pixelAt(self, x, y);
			unsigned char *op = SDImage_pixelAt(outimg, w-x-1, y);
			memcpy(op, ip, pixelSize);
		}
	}
	
	return outimg;
}

void SDImage_flipxy(SDImage *self)
{
	int pixelSize = SDImage_pixelSize(self);
	unsigned char *b[4];
	unsigned char *ip = self->data;
	unsigned char *op = ip + self->numBytes;
	
	while (ip!=op)
	{
		memcpy(b, op, pixelSize);
		memcpy(op, ip, pixelSize);
		memcpy(ip, b, pixelSize); 
		ip +=pixelSize;
		op -=pixelSize;
	}
}

/* the following functions are untested */

void SDImage_clearWithColor(SDImage *self, float r, float g, float b, float alpha)
{
    if (!SDImage_hasAlpha(self) && (r == g && r == b))
    { 
		memset(self->data, (unsigned char)255*r, self->numBytes); 
	}
    else
    {
        int pixelSize = SDImage_pixelSize(self);
        unsigned char color[4] =
        { (unsigned char)255*r,
			(unsigned char)255*g,
			(unsigned char)255*b,
			(unsigned char)255*alpha };
        unsigned char *p = self->data;
        unsigned char *pend = self->data + self->numBytes;
        int spp = self->spp;
		
        while (p<pend)  
        {
			memcpy(p, color, spp);
            p += pixelSize;
        }
    }
}

int SDImage_hasAlpha(SDImage *self)
{
	int spp = self->spp;
	return (spp == 4 || spp == 2);
}

SDImage *SDImage_addAlpha(SDImage *self)
{
	SDImage *outimg = SDImage_new(self->w, self->h, self->spp+1, self->bps, NULL);
	int w = self->w;
	int h = self->h;
	int pixelSize = SDImage_pixelSize(self);
	int x, y;
	
	for (x=0; x<w; x++) 
	{    
		for (y=0; y<h; y++) 
		{    
			unsigned char *ip = SDImage_pixelAt(self, x, y);
			unsigned char *op = SDImage_pixelAt(outimg, x, y);
			memcpy(op, ip, pixelSize);
		}
	}
	return outimg;
}

SDImage *SDImage_removeAlpha(SDImage *self)
{
	SDImage *outimg = SDImage_new(self->w, self->h, self->spp-1, self->bps, NULL);
	int w = self->w;
	int h = self->h;
	int pixelSize = SDImage_pixelSize(outimg);
	int x, y;
	
	for (x=0; x<w; x++) 
	{    
		for (y=0; y<h; y++) 
		{    
			unsigned char *ip = SDImage_pixelAt(self, x, y);
			unsigned char *op = SDImage_pixelAt(outimg, x, y);
			memcpy(op, ip, pixelSize);
		}
	}
	
	return outimg;
}

SDImage *SDImage_grayVersionOf(SDImage *self)
{
	SDImage *outimg = SDImage_new(self->w, self->h, self->spp, self->bps, NULL);
	int pixelSize = SDImage_pixelSize(self);
	unsigned char *p = self->data;
	unsigned char *op = outimg->data;
	unsigned char *end = self->data + self->numBytes;
	
	while (p<end) 
	{
		int ave = (p[0] + p[1] + p[2])/3;
		memset(op, (char)ave, 3);
		p += pixelSize;
		op += pixelSize;
	}
	return outimg;
}

SDImage *SDImage_L8toRGBA8(SDImage *self)
{
	SDImage *outimg = SDImage_new(self->w, self->h, 4, 8, NULL);
	unsigned char *p = self->data;
	unsigned char *op = outimg->data;
	unsigned char *end = self->data + self->numBytes;
	
	while (p<end) 
	{
		memset(op, (char)*p, 3);
		p++;
		op += 4;
	}
	return outimg;
}

/*-----------------------------------------------------------
- linear interpolation
- out = (1 - v)*in0 + v*in1
- See: http://www.sgi.com/grafica/interp/index.html
- for info on SDImage processing
-----------------------------------------------------------*/

void SDImage_interpolateWithImage(SDImage *self, SDImage *other, float v)
{
	int pixelSize = SDImage_pixelSize(self);
	int pixelSizeOther = SDImage_pixelSize(other);
	unsigned char *p = self->data;
	unsigned char *end = self->data + self->numBytes;
	unsigned char *p2 = other->data;
	float vv = (1.0 - v);
	
	while (p<end) 
	{
		p[0] = (unsigned char)(vv*p[0] + v*p2[0]);
		p[1] = (unsigned char)(vv*p[1] + v*p2[1]);
		p[2] = (unsigned char)(vv*p[2] + v*p2[2]);
		p  += pixelSize;
		p2 += pixelSizeOther;
	}
}

void SDImage_interpolateWithRGBColor(SDImage *self, int r, int g, int b, float v)
{
	int pixelSize = SDImage_pixelSize(self);
	unsigned char *p = self->data;
	unsigned char *end = self->data + self->numBytes;
	float vv = (1.0 - v);
	r *= v; g *= v; b *= v;
	
	while (p<end) 
	{
		int or = (vv*p[0] + r);
		int og = (vv*p[1] + r);
		int ob = (vv*p[2] + r);
		
		if (or<0) { or = 0; } else if (or>255) { or = 0; } 
		if (og<0) { og = 0; } else if (og>255) { og = 0; } 
		if (ob<0) { ob = 0; } else if (ob>255) { ob = 0; } 
		p[0] = (unsigned char)or;
		p[1] = (unsigned char)og;
		p[2] = (unsigned char)ob;
		p += pixelSize;
	}
}

void SDImage_darken(SDImage *self, float v)
{ 
	SDImage_interpolateWithRGBColor(self, 0, 0, 0, v); 
}

void SDImage_lighten(SDImage *self, float v)
{ 
	SDImage_interpolateWithRGBColor(self, 255, 255, 255, v); 
}

void SDImage_saturate(SDImage *self, float v)
{
	SDImage *grayImage = SDImage_grayVersionOf(self);
	SDImage_interpolateWithImage(self, grayImage, v);
	SDImage_free(grayImage);
}

void SDImage_contrast(SDImage *self, float v)
{
	SDImage_interpolateWithRGBColor(self, 255/2, 255/2, 255/2, v);
}

void SDImage_sharpen(SDImage *self, float v)
{
	SDImage *blurredImage = SDImage_blur(self);
	SDImage_interpolateWithImage(self, blurredImage, v);
	SDImage_free(blurredImage);
}

/* ------------------------------------------------------------------*/

SDImage *SDImage_clipRect(SDImage *self, int cx, int cy, int w, int h)
{
	SDImage *outimg = SDImage_new(w, h, self->spp, self->bps, NULL);
	int pixelSize = SDImage_pixelSize(outimg);
	int x, y;
	
	for (x=0; x<w; x++) 
	{    
		for (y=0; y<h; y++) 
		{    
			unsigned char *ip = SDImage_pixelAt(self, cx+x, cy+y);
			unsigned char *op = SDImage_pixelAt(outimg, x, y);
			memcpy(op, ip, pixelSize);
		}
	}
	
	return outimg;
}

void SDImage_composite(SDImage *self, SDImage *other, int cx, int cy)
{
	// assume self->spp >= other->spp
	int pixelSize = SDImage_pixelSize(self);
	int w = other->w;
	int h = other->h;
	int x, y;
	
	for (x=0; x<w; x++) 
	{    
		for (y=0; y<h; y++) 
		{ 
			unsigned char *ip = SDImage_pixelAt(other, x, y);
			unsigned char *p = SDImage_pixelAt(self, cx+x, cy+y);
			memcpy(p, ip, pixelSize);
		}
	}
}

void SDImage_invert(SDImage *self)
{
	int pixelSize = SDImage_pixelSize(self);
	unsigned char *p = self->data;
	unsigned char *end = p + self->numBytes;
	
	while (p<end) 
	{    
		p[0] = (unsigned char)(255-p[0]);
		p[1] = (unsigned char)(255-p[0]);
		p[2] = (unsigned char)(255-p[0]);
		p += pixelSize;
	}
}

void SDImage_replaceRGBColorWith(SDImage *self, int r, int g, int b, int nr, int ng, int nb)
{
	int pixelSize = SDImage_pixelSize(self);
	unsigned char *p = self->data;
	unsigned char *end = p + self->numBytes;
	char oldColor[3] = { r, g, b };
	char newColor[3] = { nr, ng, nb };
	
	while (p<end) 
	{
		if (memcmp(p, oldColor, 3)==0)
		{ 
			memcpy(p, newColor, 3); 
		}
		p += pixelSize;
	}
}

/*-------------------------------------------------------
Rotate
-------------------------------------------------------*/


unsigned char *SDImage_pixelAtWrap(SDImage *self, int x, int y)
{
	int w = self->w;
	int h = self->h;
	//if (x < 0) { x = w+x; } else if (x > w-1) { x = w-x; }
	//if (y < 0) { y = h+y; } else if (y > h-1) { y = h-y; }
	if (x < 0) { return NULL; } else if (x > w-1) { return NULL; }
	if (y < 0) { return NULL; } else if (y > h-1) { return NULL; }
	
	return &self->data[((x + (y*w))*(self->spp*self->bps))/8];
}


#define ROTATE_PI 3.1415926
/* 3.1415926535897932384626433832795 */

SDImage *SDImage_rotateWithSameSize(SDImage *self, double degrees)
{
    double radians = degrees * ROTATE_PI / 180.0; // convert to radians
    int w = self->w;
    int h = self->h;
    SDImage *outimg = SDImage_new(w, h, self->spp, self->bps, NULL);
    int pixelSize = SDImage_pixelSize(self);
    int ox, oy;
    //unsigned char *op = outimg->data;
	
    SDImage_clearWithColor(outimg, 0, 0, 0, 1);
    for (oy=0; oy<h; oy++) 
	{
		for (ox=0; ox<w; ox++) 
		{
			double r = sqrt(ox*ox + oy*oy);
			//double ra = acos(ox/r);
			double ra = asin(oy/r);
			int ix = cos(ra-radians)*r;
			int iy = sin(ra-radians)*r;
			unsigned char *ip = SDImage_pixelAtWrap(self, ix, iy);
			unsigned char *op = SDImage_pixelAt(outimg, ox, oy);
			
			if (ip!=NULL) memcpy(op, ip, pixelSize);
			//op = op + pixelSize;
		}
	}
	
    return outimg;
}

/*
 double SDImage_rangeFor(double ox, double oy, double radians)
 {
	 double r, ra;
	 int ix1, iy1, ix2, iy2;
	 r = sqrt(ox*ox + oy*oy);
	 if (r==0) { ra = 0; } else { ra = asin(oy/r); }
	 ix1 = cos(ra-radians)*r;
	 iy1 = sin(ra-radians)*r;
	 
	 ox++; oy++;
	 r = sqrt(ox*ox + oy*oy);
	 if (r==0) { ra = 0; } else { ra = asin(oy/r); }
	 ix2 = cos(ra-radians)*r;
	 iy2 = sin(ra-radians)*r;
	 return sqrt(pow(ix1-ix2, 2) + pow(iy1-iy2, 2));
 }
 */

SDImage *SDImage_smoothRotateWithSameSize(SDImage *self, double degrees)
{
    double radians = degrees * ROTATE_PI / 180.0; // convert to radians
    int w = self->w;
    int h = self->h;
    SDImage *outimg = SDImage_new(w, h, self->spp, self->bps, NULL);
    int pixelSize = SDImage_pixelSize(self);
    int ox, oy;
    
    for (oy=0; oy<h; oy++) 
	{
		for (ox=0; ox<w; ox++) 
		{
			double r = sqrt(ox*ox + oy*oy);
			double ra;
			
			if (r==0) { ra = 0; } else { ra = asin(oy/r); }
			{
				int ix = cos(ra-radians)*r;
				int iy = sin(ra-radians)*r;
				unsigned char *ip = SDImage_pixelAtWrap(self, ix, iy);
				unsigned char *op = SDImage_pixelAt(outimg, ox, oy);
				memcpy(op, ip, pixelSize);
			}
		}
	}
	
    return outimg;
}

double distanceFromOrigin(double x, double y) { return sqrt(x*x + y*y); }
double mind(double a, double b) { if (a > b) { return b; } return a; }
double maxd(double a, double b) { if (a < b) { return b; } return a; }
double radiansFromDegrees(double degrees) { return degrees * ROTATE_PI / 180.0; }
double degreesFromRadians(double radians) { return radians * 180.0 / ROTATE_PI; }
double dabs(double v) { return v<0?-v:v; }

SDImage *SDImage_rotate(SDImage *self, double degrees)
{
    double w = self->w;
    double h = self->h;
    double radians = -radiansFromDegrees(degrees); // Angle in radians
    
    double dx = h*dabs(sin(radians));
    double dy = w*dabs(sin(radians));
    
    double nw = w + dx;
    double nh = h + dy;
    
    //double yoffset = 0;
    //if (miny == ny3) { yoffset = sin(radians)*r3; }
    printf("cos(radians) = %f\n", cos(radians));
    printf("abs(cos(radians)) = %f\n", dabs(cos(radians)));
    printf("%f %f\n", dx, dy);
    
    if (degrees == 0.0)
    {
        SDImage *outimg = SDImage_new(self->w, self->h, self->spp, self->bps, NULL);
        SDImage_copy(outimg, self);
        return outimg;
    }
    
    //printf("degrees = %f \n", degrees);
    //printf("radians = %f \n", radiansFromDegrees(degrees));
    //printf("degrees = %f \n", degreesFromRadians(radiansFromDegrees(degrees)));
    
    {
		SDImage *outimg = SDImage_new(nw, nh, self->spp, self->bps, NULL);
		int pixelSize = SDImage_pixelSize(self);
		int ox, oy;
		
		for (oy=-dy; oy<nh; oy++) 
		{
			for (ox=0; ox<nw; ox++) 
			{
				double r = sqrt(ox*ox + oy*oy);
				double ra;
				if (r==0) { ra = 0; } else { ra = asin(oy/r); }
				{
					int ix = cos(ra-radians)*r;
					int iy = sin(ra-radians)*r;
					if (ix > 0 && iy > 0 && ix < w && iy < h && oy + dy < nh)
					{
						unsigned char *ip = SDImage_pixelAtWrap(self, ix, iy);
						unsigned char *op = SDImage_pixelAt(outimg, ox, oy + dy);
						memcpy(op, ip, pixelSize);
					}
				}
			}
		}
		
		return outimg;
    }
}


/*----------------------------------------------------------
SDImage loading 
----------------------------------------------------------*/

/*
 SDImage *SDImage_load(const char *path)
 {
	 char *extension = strrchr(path, '.');
	 int length;
	 unsigned char *data = SDImage_dataFromFile((char *)path, &length);
	 SDImage *img;
	 char *ext = extension;
	 while (*ext != '\0') { *ext = tolower(*ext); ext++; }
	 img = SDImage_newFromFormat(data, length, extension);
	 IMAGE_FREE(data);
	 return img;
 }
 
 unsigned char *SDImage_dataFromFile(char *path, int *length)
 {
	 FILE *fp = fopen(path, "r");
	 size_t fileSize;
	 unsigned char *buf;
	 
	 // get file size 
	 fseek(fp, 0, SEEK_END);
	 fileSize = ftell(fp);
	 rewind(fp);
	 
	 // read whole file into buffer 
	 buf = IMAGE_MALLOC(fileSize);
	 {
		 size_t n = fread(buf, 1, fileSize, fp);
		 if (n!=fileSize) { printf("ERROR: %i!=fileSize\n", (int)n); }
	 }
	 fclose(fp);
	 return buf;
 }
 */

