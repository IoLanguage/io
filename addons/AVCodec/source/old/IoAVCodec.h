/*   copyright: Steve Dekorte, 2002
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoAVCodec_DEFINED
#define IoAVCodec_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include <ffmpeg/avcodec.h>

typedef IoObject IoAVCodec;

typedef struct
{
	int isRunning;
	IoMessage *willProcessMessage;
	IoMessage *didProcessMessage;
	IoSeq *inputBuffer;
	IoSeq *outputBuffer;
	AVCodecContext *codecContext;
	uint8_t *inbuf;
	uint8_t *outbuf;
	uint8_t *outbufResampled;
	
	AVPicture *dstPicture;
	
	AVStream *audioStream;
	AVStream *videoStream;
	
} IoAVCodecData;

IoAVCodec *IoAVCodec_proto(void *state);
IoAVCodec *IoAVCodec_new(void *state);
IoAVCodec *IoAVCodec_rawClone(IoAVCodec *self);
void IoAVCodec_mark(IoAVCodec *self);
void IoAVCodec_free(IoAVCodec *self);

// ----------------------------------------------------------- 

IoObject *IoAVCodec_stop(IoAVCodec *self, IoObject *locals, IoMessage *m);

IoObject *IoAVCodec_inputBuffer(IoAVCodec *self, IoObject *locals, IoMessage *m);
IoObject *IoAVCodec_outputBuffer(IoAVCodec *self, IoObject *locals, IoMessage *m);

IoObject *IoAVCodec_decodeCodecNames(IoAVCodec *self, IoObject *locals, IoMessage *m);
IoObject *IoAVCodec_encodeCodecNames(IoAVCodec *self, IoObject *locals, IoMessage *m);

IoObject *IoAVCodec_startAudioDecoding(IoAVCodec *self, IoObject *locals, IoMessage *m);
IoObject *IoAVCodec_startAudioEncoding(IoAVCodec *self, IoObject *locals, IoMessage *m);

IoObject *IoAVCodec_startVideoDecoding(IoAVCodec *self, IoObject *locals, IoMessage *m);
//IoObject *IoAVCodec_startVideoEncoding(IoAVCodec *self, IoObject *locals, IoMessage *m);


#endif
