/*   copyright: Steve Dekorte, 2002
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoAVCodec_DEFINED
#define IoAVCodec_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoList.h"
#include <ffmpeg/avcodec.h>
#include <ffmpeg/avformat.h>

typedef IoObject IoAVCodec;

typedef struct
{
	int isRunning;
	
	IoMessage *willProcessMessage;
	IoMessage *didProcessMessage;
	
	// both
	
	AVFormatContext *formatContext;
	AVInputFormat *inputFormat;
    //AVFormatParameters static_formatParams, *formatParams = &static_formatParams;
	
	int audioStreamIndex;
	AVCodecContext *audioContext;
	
	int videoStreamIndex;
	AVCodecContext *videoContext;
		
	AVPacket *packet;
	
	// audio 
	
	IoSeq *inputBuffer;
	IoSeq *outputBuffer;	
	
	uint8_t *audioOutBuffer;
	
	// video
	
	AVFrame *decodedFrame;
	AVPicture *rgbPicture;
	IoList *frames;
	
} IoAVCodecData;

IoAVCodec *IoAVCodec_proto(void *state);
IoAVCodec *IoAVCodec_new(void *state);
IoAVCodec *IoAVCodec_rawClone(IoAVCodec *self);
void IoAVCodec_mark(IoAVCodec *self);
void IoAVCodec_free(IoAVCodec *self);

// ----------------------------------------------------------- 

IoObject *IoAVCodec_stop(IoAVCodec *self, IoObject *locals, IoMessage *m);

IoObject *IoAVCodec_audioInputBuffer(IoAVCodec *self, IoObject *locals, IoMessage *m);
IoObject *IoAVCodec_audioOutputBuffer(IoAVCodec *self, IoObject *locals, IoMessage *m);

IoObject *IoAVCodec_decodeCodecNames(IoAVCodec *self, IoObject *locals, IoMessage *m);
IoObject *IoAVCodec_encodeCodecNames(IoAVCodec *self, IoObject *locals, IoMessage *m);

IoObject *IoAVCodec_open(IoAVCodec *self, IoObject *locals, IoMessage *m);
IoObject *IoAVCodec_startDecoding(IoAVCodec *self, IoObject *locals, IoMessage *m);
//IoObject *IoAVCodec_startEncoding(IoAVCodec *self, IoObject *locals, IoMessage *m);

int IoAVCodec_openFile(IoAVCodec *self);
int IoAVCodec_findStreams(IoAVCodec *self);
int IoAVCodec_decodeStreams(IoAVCodec *self);

int IoAVCodec_decodeAudioPacket(IoAVCodec *self, AVCodecContext *c, uint8_t *inbuf, size_t size);
int IoAVCodec_decodeVideoPacket(IoAVCodec *self, AVCodecContext *c, uint8_t *inbuf, size_t size);

IoSeq *IoAVCode_frameSeqForAVFrame_(IoAVCodec *self, AVFrame *avframe, int srcPixelFormat, int width, int height);

#endif
