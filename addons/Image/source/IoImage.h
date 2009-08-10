//metadoc Image copyright Steve Dekorte 2002
//metadoc Image license BSD revised

#ifndef IOIMAGE_DEFINED
#define IOIMAGE_DEFINED 1

#include "IoImageApi.h"

#include "IoObject.h"
#include "Image.h"

struct IoSeq;

#define ISIMAGE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoImage_rawClone)

typedef IoObject IoImage;

typedef struct
{
	int width;
	int height;
	IoSeq *buffer;
	Image *image;
	int textureWidth;
	int textureHeight;
} IoImageData;

IOIMAGE_API IoImage *IoImage_proto(void *state);
IOIMAGE_API IoImage *IoImage_new(void *state);
IOIMAGE_API IoImage *IoImage_newWithPath_(void *state, IoSymbol *path);
IOIMAGE_API IoImage *IoImage_rawClone(IoImage *self);

IOIMAGE_API void IoImage_free(IoImage *self);
IOIMAGE_API void IoImage_mark(IoImage *self);
IOIMAGE_API Image *IoImage_image(IoImage *self);
IOIMAGE_API Image *IoImage_rawImage(IoImage *self);

/* ----------------------------------------------------------- */
IOIMAGE_API IoObject *IoImage_setDataWidthHeightComponentCount(IoImage *self, IoObject *locals, IoMessage *m);

IOIMAGE_API IoObject *IoImage_path(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_setPath(IoImage *self, IoObject *locals, IoMessage *m);

IOIMAGE_API IoObject *IoImage_open(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_save(IoImage *self, IoObject *locals, IoMessage *m);

IOIMAGE_API IoObject *IoImage_width(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_height(IoImage *self, IoObject *locals, IoMessage *m);

IOIMAGE_API IoObject *IoImage_data(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_error(IoImage *self, IoObject *locals, IoMessage *m);

IOIMAGE_API IoObject *IoImage_buffer(IoImage *self, IoObject *locals, IoMessage *m);

IOIMAGE_API IoObject *IoImage_componentCount(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_isRGB8(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_isRGBA8(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_isL8(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_isLA8(IoImage *self, IoObject *locals, IoMessage *m);

IOIMAGE_API IoObject *IoImage_resizedTo(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_crop(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_addAlpha(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_removeAlpha(IoImage *self, IoObject *locals, IoMessage *m);

/* --- extras -------------------------------------------------------- */

IOIMAGE_API IoObject *IoImage_setEncodingQuality(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_encodingQuality(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_setDecodingWidthHint(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_decodingWidthHint(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_setDecodingHeightHint(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_decodingHeightHint(IoImage *self, IoObject *locals, IoMessage *m);

IOIMAGE_API IoObject *IoImage_flipX(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_flipY(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_baselineHeight(IoImage *self, IoObject *locals, IoMessage *m);
IOIMAGE_API IoObject *IoImage_bounds(IoImage *self, IoObject *locals, IoMessage *m);

IOIMAGE_API IoObject *IoImage_averageColor(IoImage *self, IoObject *locals, IoMessage *m);

#endif
