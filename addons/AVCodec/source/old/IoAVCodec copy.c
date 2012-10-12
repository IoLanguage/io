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
#define IVAR(name) (((IoAVCodecData *)IoObject_dataPointer(self))->name)

static const char *protoId = "AVCodec";

void IoAVCodec_registerIfNeeded(IoAVCodec *self) 
{ 
	avcodec_init();
	avcodec_register_all();
	av_register_all();
}

IoTag *IoAVCodec_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
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
	
	IVAR(willProcessMessage) = IoMessage_newWithName_label_(state, IOSYMBOL("willProcess"), IOSYMBOL("[AVCodec]"));
	IVAR(didProcessMessage)  = IoMessage_newWithName_label_(state, IOSYMBOL("didProcess"),  IOSYMBOL("[AVCodec]"));
	IVAR(inputBuffer)  = IoSeq_new(state);
	IVAR(outputBuffer) = IoSeq_new(state);

	IoState_registerProtoWithFunc_(state, self, IoAVCodec_proto);

	IoAVCodec_registerIfNeeded(self);
	
	{
		IoMethodTable methodTable[] = {
		
		{"audioInputBuffer",  IoAVCodec_audioInputBuffer},
		{"audioOutputBuffer", IoAVCodec_audioOutputBuffer},
		
		{"encodeCodecNames",  IoAVCodec_encodeCodecNames},
		{"decodeCodecNames",  IoAVCodec_decodeCodecNames},
		
		{"open", IoAVCodec_open},
		
		{"startDecoding",     IoAVCodec_startDecoding},
		//{"startEncoding",     IoAVCodec_startEncoding},

		{"stop",  IoAVCodec_stop},
		
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
	
	IVAR(willProcessMessage) = DATA(proto)->willProcessMessage;
	IVAR(didProcessMessage)  = DATA(proto)->didProcessMessage;
	
	IVAR(inputBuffer)        = IOCLONE(DATA(proto)->inputBuffer);
	IVAR(outputBuffer)       = IOCLONE(DATA(proto)->outputBuffer);

	return self; 
}

IoAVCodec *IoAVCodec_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

// ----------------------------------------------------------- 

void IoAVCodec_createContextIfNeeded(IoAVCodec *self) 
{
	if(!IVAR(packet))
	{
		IVAR(packet) = calloc(1, sizeof(AVPacket));
	}
	
	// video
	
	// frames
	if (!IVAR(frames))
	{
		IVAR(frames) = IoList_new(IOSTATE);
		IoObject_setSlot_to_(self, IOSYMBOL("frames"), IVAR(frames));
	}
	
	// videoSize
	{
		UArray *sizeUArray = UArray_newWithData_type_encoding_size_copy_("", CTYPE_float32_t, CENCODING_NUMBER, 2, 1);
		IoSeq *sizeSeq = IoSeq_newWithUArray_copy_(IOSTATE, sizeUArray, 0);
		IoObject_setSlot_to_(self, IOSYMBOL("videoSize"), sizeSeq);
	}
	
	if (!IVAR(decodedFrame))
	{
		IVAR(decodedFrame) = avcodec_alloc_frame();
	}
	
	// audio
	
	if(!IVAR(audioOutBuffer))
	{
		IVAR(audioOutBuffer) = malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE);
	}
}

AVPicture *IoAVCode_allocDstPictureIfNeeded(IoAVCodec *self, int pix_fmt, int width, int height)
{
	if(!IVAR(rgbPicture))
	{
		IVAR(rgbPicture) = calloc(1, sizeof(AVPicture));
		avpicture_alloc(IVAR(rgbPicture), PIX_FMT_RGB24, width, height);
	}
	
	return IVAR(rgbPicture);
}


void IoAVCodec_freeContextIfNeeded(IoAVCodec *self) 
{ 
	//printf("IoAVCodec_freeContextIfNeeded\n");
	
	IVAR(audioContext) = NULL;
	IVAR(videoContext) = NULL;
	
	if (IVAR(audioContext))
	{
		//avcodec_close(IVAR(audioContext));
		//av_free(IVAR(audioContext));
		IVAR(audioContext) = NULL;
	}
	
	if (IVAR(videoContext))
	{
		//avcodec_close(IVAR(audioContext));
		//av_free(IVAR(audioContext));
		IVAR(audioContext) = NULL;
	}

	if (IVAR(formatContext))
	{
		av_close_input_file(IVAR(formatContext));
		//av_free(IVAR(formatContext));
		IVAR(formatContext) = NULL;
	}	
	
	if(IVAR(packet))
	{
		//free(IVAR(packet));
		IVAR(packet) = NULL;
	}
	
	if (IVAR(audioOutBuffer))
	{
		//free(IVAR(audioOutBuffer));
		IVAR(audioOutBuffer) = NULL;
	}
	
	if(IVAR(decodedFrame))
	{
		//av_free(IVAR(decodedFrame));
		IVAR(decodedFrame) = NULL;
	}
		
	if(IVAR(rgbPicture)) 
	{ 
		avpicture_free(IVAR(rgbPicture));
		//free(IVAR(rgbPicture));
		IVAR(rgbPicture) = NULL; 
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
	if(IVAR(frames)) IoObject_shouldMark(IVAR(frames));
	
	IoObject_shouldMark(IVAR(inputBuffer));
	IoObject_shouldMark(IVAR(outputBuffer));
	
	IoObject_shouldMark(IVAR(willProcessMessage));
	IoObject_shouldMark(IVAR(didProcessMessage));
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
	return IVAR(inputBuffer); 
}

IoObject *IoAVCodec_audioOutputBuffer(IoAVCodec *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("outputBuffer", "Returns the output buffer.")
	*/
	return IVAR(outputBuffer); 
}

IoObject *IoAVCodec_stop(IoAVCodec *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("stop", "Stops processing data.")
	*/
	IVAR(isRunning) = 0;
	return self; 
}

IoObject *IoAVCodec_isRunning(IoAVCodec *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("isRunning", "Returns true if it's running, false otherwise.")
	*/
	return IOBOOL(self, IVAR(isRunning)); 
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


int IoAVCodec_openFile(IoAVCodec *self)
{
	AVInputFormat *inputFormat;
	IoObject *fileName = IoObject_symbolGetSlot_(self, IOSYMBOL("fileName"));
    int err = av_open_input_file(&IVAR(formatContext), CSTRING(fileName), NULL, 0, NULL);
	return err;
}

IoObject *IoAVCodec_open(IoAVCodec *self, IoObject *locals, IoMessage *m)
{
	int err;
	
	IoAVCodec_registerIfNeeded(self);
	IoAVCodec_freeContextIfNeeded(self);
	IoAVCodec_createContextIfNeeded(self);
	
	err = IoAVCodec_openFile(self);
		
    if (err != 0)
	{
		IoObject *fileName = IoObject_symbolGetSlot_(self, IOSYMBOL("fileName"));
		IoState_error_(IOSTATE, m, "error %i opening file %s\n", err, CSTRING(fileName));
		return IONIL(self);
	}
	
	IoAVCodec_findStreams(self);
	IoAVCodec_freeContextIfNeeded(self);

	return self;
}

IoObject *IoAVCodec_startDecoding(IoAVCodec *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("startDecoding", 
	"Opens the file, starts decoding A/V streams and calling willProcess and didProcess methods.")
	*/

	int err;
	
	IoAVCodec_registerIfNeeded(self);
	IoAVCodec_freeContextIfNeeded(self);
	IoAVCodec_createContextIfNeeded(self);
			
	err = IoAVCodec_openFile(self);
		
    if (err != 0)
	{
		IoObject *fileName = IoObject_symbolGetSlot_(self, IOSYMBOL("fileName"));
		IoState_error_(IOSTATE, m, "error %i opening file %s\n", err, CSTRING(fileName));
		return IONIL(self);
	}

	IoAVCodec_findStreams(self);
	IoAVCodec_decodeStreams(self);	
	IoAVCodec_freeContextIfNeeded(self);
	
	return self;
}

int IoAVCodec_findStreams(IoAVCodec *self)
{
    AVFormatContext *formatContext = IVAR(formatContext);
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
				IVAR(audioStreamIndex) = i;
				{
					AVCodec *codec = avcodec_find_decoder(codecContext->codec_id);
					
					if (codec) 
					{
						int err = avcodec_open(codecContext, codec);
				
						if (err == 0)
						{
							IVAR(audioContext) = codecContext;
						}
					}
				}
				
				//printf("audioStreamIndex = %i\n", IVAR(audioStreamIndex));
				IoObject_setSlot_to_(self, IOSYMBOL("audioChannels"),   IONUMBER(codecContext->channels));
				IoObject_setSlot_to_(self, IOSYMBOL("audioSampleRate"), IONUMBER(codecContext->sample_rate));
				IoObject_setSlot_to_(self, IOSYMBOL("audioBitRate"),	IONUMBER(codecContext->bit_rate));
				IoObject_setSlot_to_(self, IOSYMBOL("audioDuration"),	IONUMBER(stream->duration));
				IoObject_setSlot_to_(self, IOSYMBOL("audioFrameCount"),	IONUMBER(stream->nb_frames));
				break;
				
			case CODEC_TYPE_VIDEO:
				{
				IVAR(videoStreamIndex) = i;
				
				{
					AVCodec *codec = avcodec_find_decoder(codecContext->codec_id);
					
					if (codec) 
					{
						int err = avcodec_open(codecContext, codec);
				
						if (err == 0)
						{
							IVAR(videoContext) = codecContext;
						}
					}
				}
				
				//printf("videoStreamIndex = %i\n", IVAR(videoStreamIndex));
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

int IoAVCodec_decodeStreams(IoAVCodec *self)
{
	AVFormatContext *formatContext = IVAR(formatContext);
	int audioStreamIndex = IVAR(audioStreamIndex);
	int videoStreamIndex = IVAR(videoStreamIndex);
    AVPacket *packet = IVAR(packet);

	IVAR(isRunning) = 1;
	
	if(IVAR(audioContext) == NULL && IVAR(videoContext) == NULL) return -1;
	
	av_read_play(formatContext);
	
	while (IVAR(isRunning)) 
	{
		int ret;
		
		IoState_pushRetainPool(IOSTATE);	
		IoMessage_locals_performOn_(IVAR(willProcessMessage), self, self);

		ret = av_read_frame(formatContext, packet);
		
		if (ret < 0) { break; }

		if (packet->stream_index == audioStreamIndex && IVAR(audioContext)) 
		{
			IoAVCodec_decodeAudioPacket(self, 
				formatContext->streams[audioStreamIndex]->codec, 
				packet->data, packet->size);
		} 
		else if (packet->stream_index == videoStreamIndex && IVAR(videoContext)) 
		{
			IoAVCodec_decodeVideoPacket(self, 
				formatContext->streams[videoStreamIndex]->codec, 
				packet->data, packet->size);
		}
		else 
		{
			av_free_packet(packet);
		}
		
		IoMessage_locals_performOn_(IVAR(didProcessMessage), self, self);
		IoState_popRetainPool(IOSTATE);	
	}
	
	return 0;
}

int IoAVCodec_decodeAudioPacket(IoAVCodec *self, AVCodecContext *c, uint8_t *inbuf, size_t size)
{
	UArray  *outba  = IoSeq_rawUArray(IVAR(outputBuffer));
	uint8_t *outbuf = IVAR(audioOutBuffer);
	
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
    AVFrame *decodeFrame = IVAR(decodedFrame);

	while (size > 0)
	{
		int got_picture;
		size_t len = avcodec_decode_video(c, IVAR(decodedFrame), &got_picture, inbuf, size);
		
		if (len < 0)
		{ 
			printf("Error while decoding video packet\n");
			return -1;
		}

		if (got_picture) 
		{
			IoList_rawAppend_(IVAR(frames), IoAVCode_frameSeqForAVFrame_(self, decodeFrame, c->pix_fmt, c->width, c->height));
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

