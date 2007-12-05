/*   copyright: Steve Dekorte, 2002
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IOMP3DECODER_DEFINED
#define IOMP3DECODER_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "mad.h"

#define ISMP3DECODER(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoMP3Decoder_rawClone)

typedef IoObject IoMP3Decoder;

typedef struct
{
	struct mad_decoder decoder;
	int isRunning;
	IoMessage *willProcessMessage;
	IoMessage *didProcessMessage;
	IoSeq *inputBuffer;
	IoSeq *outputBuffer;
	size_t lastInputPos;
	UArray *tmpInputBa;
} IoMP3DecoderData;

IoMP3Decoder *IoMP3Decoder_proto(void *state);
IoMP3Decoder *IoMP3Decoder_new(void *state);
IoMP3Decoder *IoMP3Decoder_rawClone(IoMP3Decoder *self);
void IoMP3Decoder_mark(IoMP3Decoder *self);
void IoMP3Decoder_free(IoMP3Decoder *self);

/* ----------------------------------------------------------- */
IoObject *IoMP3Decoder_start(IoMP3Decoder *self, IoObject *locals, IoMessage *m);
IoObject *IoMP3Decoder_stop(IoMP3Decoder *self, IoObject *locals, IoMessage *m);

IoObject *IoMP3Decoder_inputBuffer(IoMP3Decoder *self, IoObject *locals, IoMessage *m);
IoObject *IoMP3Decoder_outputBuffer(IoMP3Decoder *self, IoObject *locals, IoMessage *m);

#endif
