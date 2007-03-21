/*
 * image.c 
 * Simple image manipulation library
 * Copyright Steve Dekorte, 2001
 */

#include "image.h"
#include "image_loader.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>

image *image_new(
  int w, int h, 
  int spp, int bps, 
  unsigned char *data)
{
  image *self = (image *)IMAGE_MALLOC(sizeof(image));  
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

void image_free(image *self)
{
  if (self->ownsData) { IMAGE_FREE(self->data); }
  IMAGE_FREE(self);
}

void image_print(image *self)
{
  printf("  image %ix%i spp:%i bps:%i bytes:%i ownsData:%i\n", 
    self->w, self->h, self->spp, self->bps, (int)self->numBytes, self->ownsData);
}

image *image_clone(image *self)
{
  image *other = image_new(self->w, self->h, self->spp, self->bps, NULL);
  image_copy(self, other);
  return other;
}

void image_copy(image *self, image *other)
{
  if (image_sameFormatAs(self, other)!=1)
  { printf("ERROR: image_copy: images need to have same format\n"); return; }
  memcpy(self->data, other->data, self->numBytes);
}
  
void image_copyByPixel(image *self, image *other)
{
  /* this is mostly here as an example */
  int w = self->w;
  int h = self->h;
  int pixelSize = image_pixelSize(self);
  int x, y;
  for (x=0; x<w; x++) {    
  for (y=0; y<h; y++) {    
    unsigned char *ip = image_pixelAt(other, x, y);
    unsigned char *op = image_pixelAt(self, x, y);
    memcpy(ip, op, pixelSize);
  }}
 if (image_isEqual(self, other)==0)
 { printf("ERROR: image_copyByPixel didn't work\n"); }
}

/*=====================================================
   Comparisions                                     
  =====================================================*/

int image_isEqual(image *self, image *other)
{
  return (image_sameFormatAs(self, other) &&
    memcmp(&self->data, &other->data, (size_t)self->numBytes));
}

int image_sameFormatAs(image *self, image *other)
{
  return (
    self->w == other->w &&
    self->h == other->h &&
    self->spp == other->spp  &&
    self->bps == other->bps  &&
    self->numBytes == other->numBytes);
}

int image_sameSizeAs(image *self, image *other)
{
  return (
    self->w == other->w &&
    self->h == other->h);
}

/*=====================================================
   Accessors                                     
  =====================================================*/
  
size_t image_dataLength(image *self) { return self->numBytes; }
int image_pixelSize(image *self) { return (self->spp*self->bps)/8; }

unsigned char *image_pixelAt(image *self, int x, int y)
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

image *image_smoothScaleTo(image *self, int ow, int oh)
{
  int w = self->w;
  int h = self->h;
  if (ow==w && oh==h)
  {
    image *outimg = image_new(ow, oh, self->spp, self->bps, NULL);
    image_copy(outimg, self);
    return outimg;
  }
  if ((ow*oh) < (w*h)) 
  {
    if ((ow*oh) > (w*h)/4)
    { 
      image *img1 = image_scaleTo(self, ow*2, oh*2);
      image *img2 = image_blur(img1);
      image *img3 = image_scaleTo(img2, ow, oh);
      image_free(img1);
      image_free(img2);
      return img3;
    }
    else
    { 
      image *img = image_smoothShrinkTo(self, ow, oh);
      //image_flipxy(img); 
      return img;
    }
  }
  else
  {
    /* expand: scale then blur, repeat */
    image *img = self;
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
        image *scaleimg = image_scaleTo(img, sw, sh);
        if (img!=self) { image_free(img); }
        {
          //image *blurimg = image_blurEvenPixels(img);
          image *blurimg = image_blur(scaleimg);
          image_free(scaleimg);
          img = blurimg;
        }
      }
    }
    return img; 
  }
}

image *image_scaleTo(image *self, int ow, int oh)
{
  image *outimg = image_new(ow, oh, self->spp, self->bps, NULL);
  int iw = self->w;
  int ih = self->h;
  int ox, oy;
  int pixelSize = image_pixelSize(self);
  unsigned char *op = outimg->data;
  
  float rw = (float)iw/(float)ow;
  float rh = (float)ih/(float)oh;
  //printf("scaleTo\n");
  for (oy=0; oy<oh; oy++) {  
  for (ox=0; ox<ow; ox++) {
    int ix = ox*rw;
    int iy = oy*rh;    
    unsigned char *ip = IMAGE_PIXELAT(self, ix, iy, iw, pixelSize); //image_pixelAt(self, ix, iy);
    memcpy(op, ip, pixelSize);
    op = op + pixelSize;
  }}
  
  return outimg;
}

image *image_blurTimes(image *self, int times)
{
  image *img = self;
  int n;
  for (n=0; n<times; n++)
  {
    image *blurimg = image_blur(img);
    if (img!=self) { image_free(img); }
    img = blurimg;
  }
  return img;
}

image *image_blur(image *self)
{
  image *outimg = image_new(self->w, self->h, self->spp, self->bps, NULL);
  int pixelSize = image_pixelSize(self);
  int w = self->w;
  int h = self->h;
  int x, y;
  int r, g, b;
  unsigned char *op = outimg->data;
  //printf("blur\n");
  for (y=0; y<h; y++) {    
  for (x=0; x<w; x++) {
      int xoffset, yoffset;
      r = 0; g = 0; b = 0;
      for (yoffset=-1; yoffset<2; yoffset++) {
      for (xoffset=-1; xoffset<2; xoffset++) {
        unsigned char *p = image_pixelAt(self, x+xoffset, y+yoffset);
        r = r + p[0];
        g = g + p[1];
        b = b + p[2];
      }}
      op[0] = (unsigned char)(r/9); 
      op[1] = (unsigned char)(g/9); 
      op[2] = (unsigned char)(b/9);
      op = op + pixelSize;
  }}
  return outimg;
}

image *image_highBoostFilter(image *self, float v)
{
  /* See: http://www.dai.ed.ac.uk/CVonline/LOCAL_COPIES/OWENS/LECT5/node3.html */
  image *outimg = image_new(self->w, self->h, self->spp, self->bps, NULL);
  int pixelSize = image_pixelSize(self);
  int w = self->w;
  int h = self->h;
  int x, y;
  int r, g, b;
  float ww = (9*v)-1;
  unsigned char *op = outimg->data;
  //printf("blur\n");
  for (y=0; y<h; y++) {    
  for (x=0; x<w; x++) {
      int xoffset, yoffset;
      r = 0; g = 0; b = 0;
      for (yoffset=-1; yoffset<2; yoffset++) {
      for (xoffset=-1; xoffset<2; xoffset++) {
        unsigned char *p = image_pixelAt(self, x+xoffset, y+yoffset);
        if (x==y==0)
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
      }}
      op[0] = (unsigned char)(r/9); 
      op[1] = (unsigned char)(g/9); 
      op[2] = (unsigned char)(b/9);
      op = op + pixelSize;
  }}
  return outimg;
}

image *image_smoothShrinkTo(image *self, int ow, int oh)
{
  image *outimg = image_new(ow, oh, self->spp, self->bps, NULL);
  int iw = self->w;
  int ih = self->h;
  int ox, oy;
  int pixelSize = image_pixelSize(self);
  unsigned char *op = outimg->data;
  int range = (((float)iw/(float)ow)+((float)ih/(float)oh))/4;
  float rw = (float)iw/(float)ow;
  float rh = (float)ih/(float)oh;
  //printf("image_smootherScaleTo range=%i\n", range);
  for (oy=0; oy<oh; oy++) {  
  for (ox=0; ox<ow; ox++) {
    //int ix = ox*rw;
    //int iy = oy*rh;    
    image_aveAtWithRangeTo(self, ox*rw, oy*rh, range, op);
    op = op + pixelSize;
  }}
  
  return outimg;
}

void image_aveAtWithRangeTo(image *self, int x, int y, int range, unsigned char *op)
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
  
  for (yoffset=miny; yoffset<maxy; yoffset++) {
  for (xoffset=minx; xoffset<maxx; xoffset++) {
    ip = image_pixelAt(self, xoffset, yoffset);
    r = r + ip[0];
    g = g + ip[1];
    b = b + ip[2];
    area++;
  }}
  op[0] = (unsigned char)(r/area); 
  op[1] = (unsigned char)(g/area); 
  op[2] = (unsigned char)(b/area);
}

image *image_flipy(image *self)
{
  image *outimg = image_new(self->w, self->h, self->spp, self->bps, NULL);
  int w = self->w;
  int h = self->h;
  int pixelSize = image_pixelSize(self);
  int rowSize = pixelSize*w;
  int y;
  unsigned char *ip = self->data;
  for (y=0; y<h; y++) {    
    unsigned char *op = image_pixelAt(outimg, 0, h-y-1);
    memcpy(op, ip, rowSize);
    ip = ip + rowSize;
  }
  return outimg;
}

image *image_flipx(image *self)
{
  image *outimg = image_new(self->w, self->h, self->spp, self->bps, NULL);
  int w = self->w;
  int h = self->h;
  int pixelSize = image_pixelSize(self);
  int x, y;
  for (x=0; x<w; x++) {    
  for (y=0; y<h; y++) {    
    unsigned char *ip = image_pixelAt(self, x, y);
    unsigned char *op = image_pixelAt(outimg, w-x-1, y);
    memcpy(op, ip, pixelSize);
  }}
  return outimg;
}

void image_flipxy(image *self)
{
  int pixelSize = image_pixelSize(self);
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

void image_clearWithColor(image *self, float r, float g, float b, float alpha)
{
    if (!image_hasAlpha(self) && (r==g==b))
    { memset(self->data, (unsigned char)255*r, self->numBytes); }
    else
    {
        int pixelSize = image_pixelSize(self);
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

int image_hasAlpha(image *self)
{
  int spp = self->spp;
  return (spp == 4 || spp == 2);
}

image *image_addAlpha(image *self)
{
  image *outimg = image_new(self->w, self->h, self->spp+1, self->bps, NULL);
  int w = self->w;
  int h = self->h;
  int pixelSize = image_pixelSize(self);
  int x, y;
  for (x=0; x<w; x++) {    
  for (y=0; y<h; y++) {    
    unsigned char *ip = image_pixelAt(self, x, y);
    unsigned char *op = image_pixelAt(outimg, x, y);
    memcpy(op, ip, pixelSize);
  }}
  return outimg;
}

image *image_removeAlpha(image *self)
{
  image *outimg = image_new(self->w, self->h, self->spp-1, self->bps, NULL);
  int w = self->w;
  int h = self->h;
  int pixelSize = image_pixelSize(outimg);
  int x, y;
  for (x=0; x<w; x++) {    
  for (y=0; y<h; y++) {    
    unsigned char *ip = image_pixelAt(self, x, y);
    unsigned char *op = image_pixelAt(outimg, x, y);
    memcpy(op, ip, pixelSize);
  }}
  return outimg;
}

image *image_grayVersionOf(image *self)
{
  image *outimg = image_new(self->w, self->h, self->spp, self->bps, NULL);
  int pixelSize = image_pixelSize(self);
  unsigned char *p = self->data;
  unsigned char *op = outimg->data;
  unsigned char *end = self->data + self->numBytes;
  while (p<end) {
    int ave = (p[0] + p[1] + p[2])/3;
    memset(op, (char)ave, 3);
    p += pixelSize;
    op += pixelSize;
  }
  return outimg;
}

image *image_L8toRGBA8(image *self)
{
  image *outimg = image_new(self->w, self->h, 4, 8, NULL);
  unsigned char *p = self->data;
  unsigned char *op = outimg->data;
  unsigned char *end = self->data + self->numBytes;
  while (p<end) {
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
  - for info on image processing
  -----------------------------------------------------------*/

void image_interpolateWithImage(image *self, image *other, float v)
{
  int pixelSize = image_pixelSize(self);
  int pixelSizeOther = image_pixelSize(other);
  unsigned char *p = self->data;
  unsigned char *end = self->data + self->numBytes;
  unsigned char *p2 = other->data;
  float vv = (1.0 - v);
  while (p<end) {
    p[0] = (unsigned char)(vv*p[0] + v*p2[0]);
    p[1] = (unsigned char)(vv*p[1] + v*p2[1]);
    p[2] = (unsigned char)(vv*p[2] + v*p2[2]);
    p  += pixelSize;
    p2 += pixelSizeOther;
  }
}

void image_interpolateWithRGBColor(image *self, int r, int g, int b, float v)
{
  int pixelSize = image_pixelSize(self);
  unsigned char *p = self->data;
  unsigned char *end = self->data + self->numBytes;
  float vv = (1.0 - v);
  r *= v; g *= v; b *= v;
  while (p<end) {
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

void image_darken(image *self, float v)
{ image_interpolateWithRGBColor(self, 0, 0, 0, v); }

void image_lighten(image *self, float v)
{ image_interpolateWithRGBColor(self, 255, 255, 255, v); }

void image_saturate(image *self, float v)
{
  image *grayImage = image_grayVersionOf(self);
  image_interpolateWithImage(self, grayImage, v);
  image_free(grayImage);
}

void image_contrast(image *self, float v)
{
  image_interpolateWithRGBColor(self, 255/2, 255/2, 255/2, v);
}

void image_sharpen(image *self, float v)
{
  image *blurredImage = image_blur(self);
  image_interpolateWithImage(self, blurredImage, v);
  image_free(blurredImage);
}

/* ------------------------------------------------------------------*/

image *image_clipRect(image *self, int cx, int cy, int w, int h)
{
  image *outimg = image_new(w, h, self->spp, self->bps, NULL);
  int pixelSize = image_pixelSize(outimg);
  int x, y;
  for (x=0; x<w; x++) {    
  for (y=0; y<h; y++) {    
    unsigned char *ip = image_pixelAt(self, cx+x, cy+y);
    unsigned char *op = image_pixelAt(outimg, x, y);
    memcpy(op, ip, pixelSize);
  }}
  return outimg;
}

void image_composite(image *self, image *other, int cx, int cy)
{
  // assume self->spp >= other->spp
  int pixelSize = image_pixelSize(self);
  int w = other->w;
  int h = other->h;
  int x, y;
  for (x=0; x<w; x++) {    
  for (y=0; y<h; y++) { 
    unsigned char *ip = image_pixelAt(other, x, y);
    unsigned char *p = image_pixelAt(self, cx+x, cy+y);
    memcpy(p, ip, pixelSize);
  }}
}

void image_invert(image *self)
{
  int pixelSize = image_pixelSize(self);
  unsigned char *p = self->data;
  unsigned char *end = p + self->numBytes;
  while (p<end) {    
    p[0] = (unsigned char)(255-p[0]);
    p[1] = (unsigned char)(255-p[0]);
    p[2] = (unsigned char)(255-p[0]);
    p += pixelSize;
  }
}

void image_replaceRGBColorWith(image *self, int r, int g, int b, int nr, int ng, int nb)
{
  int pixelSize = image_pixelSize(self);
  unsigned char *p = self->data;
  unsigned char *end = p + self->numBytes;
  char oldColor[3] = { r, g, b };
  char newColor[3] = { nr, ng, nb };
  while (p<end) {
    if (memcmp(p, oldColor, 3)==0)
    { memcpy(p, newColor, 3); }
    p += pixelSize;
  }
}

/*-------------------------------------------------------
    Rotate
  -------------------------------------------------------*/
  
  
unsigned char *image_pixelAtWrap(image *self, int x, int y)
{
  int w = self->w;
  int h = self->h;
  //if (x < 0) { x = w+x; } else if (x > w-1) { x = w-x; }
  //if (y < 0) { y = h+y; } else if (y > h-1) { y = h-y; }
  if (x < 0) { return NULL; } else if (x > w-1) { return NULL; }
  if (y < 0) { return NULL; } else if (y > h-1) { return NULL; }
  return &self->data[((x + (y*w))*(self->spp*self->bps))/8];
}


#define ROTATE_PI 3.1415926535897932384626433832795

image *image_rotateWithSameSize(image *self, double degrees)
{
    double radians = degrees * ROTATE_PI / 180.0; // convert to radians
    int w = self->w;
    int h = self->h;
    image *outimg = image_new(w, h, self->spp, self->bps, NULL);
    int pixelSize = image_pixelSize(self);
    int ox, oy;
    //unsigned char *op = outimg->data;
    image_clearWithColor(outimg, 0, 0, 0, 1);
    for (oy=0; oy<h; oy++) {
    for (ox=0; ox<w; ox++) {
        double r = sqrt(ox*ox + oy*oy);
        //double ra = acos(ox/r);
        double ra = asin(oy/r);
        int ix = cos(ra-radians)*r;
        int iy = sin(ra-radians)*r;
        unsigned char *ip = image_pixelAtWrap(self, ix, iy);
        unsigned char *op = image_pixelAt(outimg, ox, oy);
        if (ip!=NULL) memcpy(op, ip, pixelSize);
        //op = op + pixelSize;
    }}
    return outimg;
}

/*
double image_rangeFor(double ox, double oy, double radians)
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

image *image_smoothRotateWithSameSize(image *self, double degrees)
{
    double radians = degrees * ROTATE_PI / 180.0; // convert to radians
    int w = self->w;
    int h = self->h;
    image *outimg = image_new(w, h, self->spp, self->bps, NULL);
    int pixelSize = image_pixelSize(self);
    int ox, oy;
    
    for (oy=0; oy<h; oy++) {
    for (ox=0; ox<w; ox++) {
        double r = sqrt(ox*ox + oy*oy);
        double ra;
        if (r==0) { ra = 0; } else { ra = asin(oy/r); }
        {
            int ix = cos(ra-radians)*r;
            int iy = sin(ra-radians)*r;
            unsigned char *ip = image_pixelAtWrap(self, ix, iy);
            unsigned char *op = image_pixelAt(outimg, ox, oy);
            memcpy(op, ip, pixelSize);
        }
    }}
    return outimg;
}

/*----------------------------------------------------------
   image loading 
  ----------------------------------------------------------*/

image *image_load(const char *path)
{
  char *extension = strrchr(path, '.');
  int length;
  unsigned char *data = image_dataFromFile((char *)path, &length);
  image *img;
  char *ext = extension;
  while (*ext != '\0') { *ext = tolower(*ext); ext++; }
  img = image_newFromFormat(data, length, extension);
  IMAGE_FREE(data);
  return img;
}


unsigned char *image_dataFromFile(char *path, int *length)
{
  FILE *fp = fopen(path, "r");
  size_t fileSize;
  unsigned char *buf;
  
  /* get file size */
  fseek(fp, 0, SEEK_END);
  fileSize = ftell(fp);
  rewind(fp);
  
  /* read whole file into buffer */
  buf = IMAGE_MALLOC(fileSize);
  {
  size_t n = fread(buf, 1, fileSize, fp);
  if (n!=fileSize) { printf("ERROR: %i!=fileSize\n", (int)n); }
  }
  fclose(fp);
  return buf;
}
