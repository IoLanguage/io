/*   copyright: Steve Dekorte, 2002
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IOMP3ENCODER_DEFINED
#define IOMP3ENCODER_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoSound.h"
#include "MP3Encoder.h"

#define ISMP3ENCODER(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoMP3Encoder_rawClone)

typedef IoObject IoMP3Encoder;

typedef struct
{
  MP3Encoder *encoder;
  IoSeq *outBuffer;
} IoMP3EncoderData;

IoMP3Encoder *IoMP3Encoder_proto(void *state);
IoMP3Encoder *IoMP3Encoder_new(void *state);
IoMP3Encoder *IoMP3Encoder_rawClone(IoMP3Encoder *self);
void IoMP3Encoder_mark(IoMP3Encoder *self);
void IoMP3Encoder_free(IoMP3Encoder *self);

/* --- Output Parameters -------------------------------------------------------- */

IoObject *IoMP3Encoder_setSampleRate(IoMP3Encoder *self, IoObject *locals, IoMessage *m);
IoObject *IoMP3Encoder_setBitRate(IoMP3Encoder *self, IoObject *locals, IoMessage *m);
IoObject *IoMP3Encoder_setQuality(IoMP3Encoder *self, IoObject *locals, IoMessage *m);

/* ----------------------------------------------------------- */
IoObject *IoMP3Encoder_begin(IoMP3Encoder *self, IoObject *locals, IoMessage *m);
IoObject *IoMP3Encoder_encode(IoMP3Encoder *self, IoObject *locals, IoMessage *m);
IoObject *IoMP3Encoder_end(IoMP3Encoder *self, IoObject *locals, IoMessage *m);
IoObject *IoMP3Encoder_outBuffer(IoMP3Encoder *self, IoObject *locals, IoMessage *m);
IoObject *IoMP3Encoder_setCompressionRatio(IoMP3Encoder *self, IoObject *locals, IoMessage *m);
IoObject *IoMP3Encoder_checkError(IoMP3Encoder *self, IoObject *locals, IoMessage *m);


#endif
