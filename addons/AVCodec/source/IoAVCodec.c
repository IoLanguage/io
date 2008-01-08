/*#io
AVCodec ioDoc(
			docCopyright("Steve Dekorte", 2004)
			docLicense("BSD revised")
			docCategory("Media")
			docDescription("""An object for encoding and decoding audio and video streams.""")
*/

#include "IoAVCodec.h"
#include "List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "IoList.h"
#include <limits.h>
//#include <math.h>


#define DATA(self) ((IoAVCodecData *)IoObject_dataPointer(self))

void IoAVCodec_registerIfNeeded(IoAVCodec *self)
{
	avcodec_init();
	avcodec_register_all();
	av_register_all();
}

IoTag *IoAVCodec_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("AVCodec");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoAVCodec_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoAVCodec_mark);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoAVCodec_free);
	return tag;
}

IoAVCodec *IoAVCodec_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoAVCodec_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoAVCodecData)));

	DATA(self)->inputBuffer  = IoSeq_new(state);
	DATA(self)->outputBuffer = IoSeq_new(state);

	IoState_registerProtoWithFunc_(state, self, IoAVCodec_proto);

	IoAVCodec_registerIfNeeded(self);

	{
		IoMethodTable methodTable[] = {

		{"audioInputBuffer",  IoAVCodec_audioInputBuffer},
		{"audioOutputBuffer", IoAVCodec_audioOutputBuffer},

		{"encodeCodecNames",  IoAVCodec_encodeCodecNames},
		{"decodeCodecNames",  IoAVCodec_decodeCodecNames},

		{"open", IoAVCodec_open},
		{"close", IoAVCodec_close},

		{"decode", IoAVCodec_decode},
		{"isAtEnd", IoAVCodec_isAtEnd},
		//{"encode",     IoAVCodec_startEncoding},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoAVCodec *IoAVCodec_rawClone(IoAVCodec *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoAVCodecData)));

	DATA(self)->inputBuffer        = IOCLONE(DATA(proto)->inputBuffer);
	DATA(self)->outputBuffer       = IOCLONE(DATA(proto)->outputBuffer);

	return self;
}

IoAVCodec *IoAVCodec_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoAVCodec_proto);
	return IOCLONE(proto);
}

// -----------------------------------------------------------

void IoAVCodec_createContextIfNeeded(IoAVCodec *self)
{
	if(!DATA(self)->packet)
	{
		DATA(self)->packet = calloc(1, sizeof(AVPacket));
	}

	// video

	// frames
	if (!DATA(self)->frames)
	{
		DATA(self)->frames = IoList_new(IOSTATE);
		IoObject_setSlot_to_(self, IOSYMBOL("frames"), DATA(self)->frames);
	}

	// videoSize
	{
		UArray *sizeUArray = UArray_newWithData_type_encoding_size_copy_("", CTYPE_float32_t, CENCODING_NUMBER, 2, 1);
		IoSeq *sizeSeq = IoSeq_newWithUArray_copy_(IOSTATE, sizeUArray, 0);
		IoObject_setSlot_to_(self, IOSYMBOL("videoSize"), sizeSeq);
	}

	if (!DATA(self)->decodedFrame)
	{
		DATA(self)->decodedFrame = avcodec_alloc_frame();
	}

	// audio

	if(!DATA(self)->audioOutBuffer)
	{
		DATA(self)->audioOutBuffer = malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE);
	}
}

AVPicture *IoAVCode_allocDstPictureIfNeeded(IoAVCodec *self, int pix_fmt, int width, int height)
{
	if(!DATA(self)->rgbPicture)
	{
		DATA(self)->rgbPicture = calloc(1, sizeof(AVPicture));
		avpicture_alloc(DATA(self)->rgbPicture, PIX_FMT_RGB24, width, height);
	}

	return DATA(self)->rgbPicture;
}


void IoAVCodec_freeContextIfNeeded(IoAVCodec *self)
{
	//printf("IoAVCodec_freeContextIfNeeded\n");

	DATA(self)->audioContext = NULL;
	DATA(self)->videoContext = NULL;

	if (DATA(self)->audioContext)
	{
		//avcodec_close(DATA(self)->audioContext);
		//av_free(DATA(self)->audioContext);
		DATA(self)->audioContext = NULL;
	}

	if (DATA(self)->videoContext)
	{
		//avcodec_close(DATA(self)->audioContext);
		//av_free(DATA(self)->audioContext);
		DATA(self)->audioContext = NULL;
	}

	if (DATA(self)->formatContext)
	{
		av_close_input_file(DATA(self)->formatContext);
		//av_free(DATA(self)->formatContext);
		DATA(self)->formatContext = NULL;
	}

	if(DATA(self)->packet)
	{
		//free(DATA(self)->packet);
		DATA(self)->packet = NULL;
	}

	if (DATA(self)->audioOutBuffer)
	{
		//free(DATA(self)->audioOutBuffer);
		DATA(self)->audioOutBuffer = NULL;
	}

	if(DATA(self)->decodedFrame)
	{
		//av_free(DATA(self)->decodedFrame);
		DATA(self)->decodedFrame = NULL;
	}

	if(DATA(self)->rgbPicture)
	{
		avpicture_free(DATA(self)->rgbPicture);
		//free(DATA(self)->rgbPicture);
		DATA(self)->rgbPicture = NULL;
	}

	//printf("IoAVCodec_freeContextIfNeeded done\n");
}

void IoAVCodec_free(IoAVCodec *self)
{
	IoAVCodec_freeContextIfNeeded(self);
	free(IoObject_dataPointer(self));
}

void IoAVCodec_mark(IoAVCodec *self)
{
	if(DATA(self)->frames) IoObject_shouldMark(DATA(self)->frames);

	IoObject_shouldMark(DATA(self)->inputBuffer);
	IoObject_shouldMark(DATA(self)->outputBuffer);
}

// -----------------------------------------------------------

void IoAVCodec_error_(IoAVCodec *self, IoMessage *m, char *s)
{
	fprintf(stderr, s);
	IoState_error_(IOSTATE, m, s);
}

IoObject *IoAVCodec_audioInputBuffer(IoAVCodec *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("inputBuffer", "Returns the input buffer.")
	*/
	return DATA(self)->inputBuffer;
}

IoObject *IoAVCodec_audioOutputBuffer(IoAVCodec *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("outputBuffer", "Returns the output buffer.")
	*/
	return DATA(self)->outputBuffer;
}

IoObject *IoAVCodec_decodeCodecNames(IoAVCodec *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("decodeCodecNames", "Returns a list of strings with the names of the decode codecs.")
	*/

	AVCodec *p = first_avcodec;
	IoList *names = IoList_new(IOSTATE);

	while (p)
	{
		if (p->decode)
		{
			IoList_rawAppend_(names, IOSYMBOL(p->name));
		}

		p = p->next;
	}

	return names;
}

IoObject *IoAVCodec_encodeCodecNames(IoAVCodec *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("encodeCodecNames", "Returns a list of strings with the names of the encode codecs.")
	*/

	AVCodec *p = first_avcodec;
	IoList *names = IoList_new(IOSTATE);

	while (p)
	{
		if (p->encode)
		{
			IoList_rawAppend_(names, IOSYMBOL(p->name));
		}

		p = p->next;
	}

	return names;
}

void IoAVCodec_ConvertShortToFloat(short *s, float *f, size_t sampleCount)
{
	size_t i;

	for (i = 0; i < sampleCount; i ++)
	{
		*f = (float)(*s) / (float)SHRT_MAX;
		f ++;
		*f = (float)(*s) / (float)SHRT_MAX;
		s ++;
	}
}

void IoAVCodec_ConvertFloatToShort(float *f, short *s, size_t sampleCount)
{
	size_t i;

	for (i = 0; i < sampleCount; i ++)
	{
		*s = (short)((*f) * SHRT_MAX);
		s ++;
		*s = (short)((*f) * SHRT_MAX);
		f ++;
	}
}

// -----------------------------------------------------------

#define INBUF_SIZE 4096

// ----------------------------------------------------------------------------------------------

IoObject *IoAVCodec_close(IoAVCodec *self, IoObject *locals, IoMessage *m)
{
	IoAVCodec_registerIfNeeded(self);
	IoAVCodec_freeContextIfNeeded(self);
}

int IoAVCodec_openFile(IoAVCodec *self)
{
	AVInputFormat *inputFormat;
	IoObject *fileName = IoObject_symbolGetSlot_(self, IOSYMBOL("path"));
	int err = av_open_input_file(&DATA(self)->formatContext, CSTRING(fileName), NULL, 0, NULL);
	return err;
}

IoObject *IoAVCodec_open(IoAVCodec *self, IoObject *locals, IoMessage *m)
{
	int err;

	IoAVCodec_registerIfNeeded(self);
	IoAVCodec_freeContextIfNeeded(self);
	IoAVCodec_createContextIfNeeded(self);

	DATA(self)->isAtEnd = 0;

	err = IoAVCodec_openFile(self);

	if (err != 0)
	{
		IoObject *fileName = IoObject_symbolGetSlot_(self, IOSYMBOL("path"));
		IoState_error_(IOSTATE, m, "error %i opening file %s\n", err, CSTRING(fileName));
		return IONIL(self);
	}

	IoAVCodec_findStreams(self);
	av_read_play(DATA(self)->formatContext);

	return self;
}

int IoAVCodec_findStreams(IoAVCodec *self)
{
	AVFormatContext *formatContext = DATA(self)->formatContext;
	int i;

	av_find_stream_info(formatContext);

	//printf("formatContext = %p streams = %i\n", (void *)formatContext, formatContext->nb_streams);

	for(i = 0; i < formatContext->nb_streams; i++)
	{
		AVStream *stream = formatContext->streams[i];
		AVCodecContext *codecContext = stream->codec;

		switch(codecContext->codec_type)
		{
			case CODEC_TYPE_AUDIO:
				DATA(self)->audioStreamIndex = i;
				{
					AVCodec *codec = avcodec_find_decoder(codecContext->codec_id);

					if (codec)
					{
						int err = avcodec_open(codecContext, codec);

						if (err == 0)
						{
							DATA(self)->audioContext = codecContext;
						}
					}
				}

				//printf("audioStreamIndex = %i\n", DATA(self)->audioStreamIndex);
				IoObject_setSlot_to_(self, IOSYMBOL("audioChannels"),   IONUMBER(codecContext->channels));
				IoObject_setSlot_to_(self, IOSYMBOL("audioSampleRate"), IONUMBER(codecContext->sample_rate));
				IoObject_setSlot_to_(self, IOSYMBOL("audioBitRate"),	IONUMBER(codecContext->bit_rate));
				IoObject_setSlot_to_(self, IOSYMBOL("audioDuration"),	IONUMBER(stream->duration));
				IoObject_setSlot_to_(self, IOSYMBOL("audioFrameCount"),	IONUMBER(stream->nb_frames));
				break;

			case CODEC_TYPE_VIDEO:
				{
				DATA(self)->videoStreamIndex = i;

				{
					AVCodec *codec = avcodec_find_decoder(codecContext->codec_id);

					if (codec)
					{
						int err = avcodec_open(codecContext, codec);

						if (err == 0)
						{
							DATA(self)->videoContext = codecContext;
						}
					}
				}

				//printf("videoStreamIndex = %i\n", DATA(self)->videoStreamIndex);
				{
					float framePeriod = (((float)codecContext->time_base.num)/((float)codecContext->time_base.den));
					UArray *sizeUArray = UArray_newWithData_type_encoding_size_copy_("", CTYPE_float32_t, CENCODING_NUMBER, 2, 1);
					IoObject_setSlot_to_(self, IOSYMBOL("framePeriod"),     IONUMBER(framePeriod));
					IoObject_setSlot_to_(self, IOSYMBOL("videoDuration"),   IONUMBER(stream->duration));
					IoObject_setSlot_to_(self, IOSYMBOL("videoFrameCount"), IONUMBER(stream->nb_frames));
				}

				{
					UArray *sizeUArray = UArray_newWithData_type_encoding_size_copy_("", CTYPE_float32_t, CENCODING_NUMBER, 2, 1);
					IoSeq *sizeSeq = IoSeq_newWithUArray_copy_(IOSTATE, sizeUArray, 0);
					UArray_at_putDouble_(sizeUArray, 0, codecContext->width);
					UArray_at_putDouble_(sizeUArray, 1, codecContext->height);
					IoObject_setSlot_to_(self, IOSYMBOL("videoSize"), sizeSeq);
				}

				break;
				}
		}
	}

	return 0;
}

IoObject *IoAVCodec_isAtEnd(IoAVCodec *self, IoObject *locals, IoMessage *m)
{
	return IOBOOL(self, DATA(self)->isAtEnd);
}

IoObject *IoAVCodec_decode(IoAVCodec *self, IoObject *locals, IoMessage *m)
{
	AVFormatContext *formatContext = DATA(self)->formatContext;
	int audioStreamIndex = DATA(self)->audioStreamIndex;
	int videoStreamIndex = DATA(self)->videoStreamIndex;
	AVPacket *packet = DATA(self)->packet;
	int ret;


	if(DATA(self)->audioContext == NULL && DATA(self)->videoContext == NULL)
	{
		//printf("not open\n");
		return IONIL(self);
	}

	ret = av_read_frame(formatContext, packet);

	if (ret < 0)
	{
		//printf("av_read_frame ret = %i\n", ret);

		if(ret == AVERROR_IO)
		{
			DATA(self)->isAtEnd = 1;
		}

		return IONIL(self);
	}

	if (packet->stream_index == audioStreamIndex && DATA(self)->audioContext)
	{
		IoAVCodec_decodeAudioPacket(self,
			formatContext->streams[audioStreamIndex]->codec,
			packet->data, packet->size);
	}
	else if (packet->stream_index == videoStreamIndex && DATA(self)->videoContext)
	{
		IoAVCodec_decodeVideoPacket(self,
			formatContext->streams[videoStreamIndex]->codec,
			packet->data, packet->size);
	}
	else
	{
		av_free_packet(packet);
	}

	return self;
}

int IoAVCodec_decodeAudioPacket(IoAVCodec *self, AVCodecContext *c, uint8_t *inbuf, size_t size)
{
	UArray  *outba  = IoSeq_rawUArray(DATA(self)->outputBuffer);
	uint8_t *outbuf = DATA(self)->audioOutBuffer;

	//UArray_setItemType_(outba, CTYPE_float32_t);

	while (size > 0)
	{
		int outSize;
		int len = avcodec_decode_audio(c, (int16_t *)outbuf, &outSize, inbuf, size);

		if (len < 0)
		{
			printf("Error while decoding audio packet\n");
			return -1;
		}

		if (outSize > 0)
		{
			// if a frame has been decoded, output it
			// convert short ints to floats

			size_t sampleCount = outSize / c->channels;
			size_t oldSize = UArray_size(outba);
			//UArray_setSize_(outba, oldSize + sampleCount); // knows it's a float32 array
			UArray_setSize_(outba, oldSize + sampleCount * sizeof(float)); // knows it's a float32 array

			IoAVCodec_ConvertShortToFloat((short *)outbuf, (float *)(UArray_bytes(outba) + oldSize), sampleCount);
		}

		size -= len;
		inbuf += len;
	}

	return 0;
}

int IoAVCodec_decodeVideoPacket(IoAVCodec *self, AVCodecContext *c, uint8_t *inbuf, size_t size)
{
	AVFrame *decodeFrame = DATA(self)->decodedFrame;

	while (size > 0)
	{
		int got_picture;
		size_t len = avcodec_decode_video(c, DATA(self)->decodedFrame, &got_picture, inbuf, size);

		if (len < 0)
		{
			printf("Error while decoding video packet\n");
			return -1;
		}

		if (got_picture)
		{
			IoList_rawAppend_(DATA(self)->frames, IoAVCode_frameSeqForAVFrame_(self, decodeFrame, c->pix_fmt, c->width, c->height));
		}

		size -= len;
		inbuf += len;
	}

	return 0;
}

IoSeq *IoAVCode_frameSeqForAVFrame_(IoAVCodec *self, AVFrame *avframe, int srcPixelFormat, int width, int height)
{
	AVPicture *rgbPicture = IoAVCode_allocDstPictureIfNeeded(self, srcPixelFormat, width, height);
	AVPicture srcPicture;
	int result;

	memcpy(srcPicture.data,     avframe->data,     sizeof(uint8_t *) * 4);
	memcpy(srcPicture.linesize, avframe->linesize, sizeof(int)       * 4);

	result = img_convert(rgbPicture, PIX_FMT_RGB24, &srcPicture, srcPixelFormat, width, height);

	if (result)
	{
		printf("AVCodec: img_convert error?\n");
	}

	UArray *data = UArray_newWithData_type_encoding_size_copy_(rgbPicture->data[0], CTYPE_uint8_t, CENCODING_NUMBER, width * height * 3, 1);

	return IoSeq_newWithUArray_copy_(IOSTATE, data, 0);
}

