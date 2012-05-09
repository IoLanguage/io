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

#define DATA(self) ((IoAVCodecData *)IoObject_dataPointer(self))

static const char *protoId = "AVCodec";

void IoAVCodec_registerIfNeeded(IoAVCodec *self) 
{ 
	avcodec_init();
	avcodec_register_all();
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
	
	//DATA(self)->outSound = NULL;
	DATA(self)->willProcessMessage = IoMessage_newWithName_label_(state, IOSYMBOL("willProcess"), IOSYMBOL("[AVCodec]"));
	DATA(self)->didProcessMessage = IoMessage_newWithName_label_(state, IOSYMBOL("didProcess"), IOSYMBOL("[AVCodec]"));
	DATA(self)->inputBuffer  = IoSeq_new(state);
	DATA(self)->outputBuffer = IoSeq_new(state);

	IoState_registerProtoWithFunc_(state, self, IoAVCodec_proto);

	IoAVCodec_registerIfNeeded(self);
	
	{
		IoMethodTable methodTable[] = {
		
		{"inputBuffer",  IoAVCodec_inputBuffer},
		{"outputBuffer",  IoAVCodec_outputBuffer},
		
		{"encodeCodecNames", IoAVCodec_encodeCodecNames},
		{"decodeCodecNames", IoAVCodec_decodeCodecNames},
		
		{"startAudioDecoding", IoAVCodec_startAudioDecoding},
		{"startVideoDecoding", IoAVCodec_startVideoDecoding},

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
	
	DATA(self)->willProcessMessage = DATA(proto)->willProcessMessage;
	DATA(self)->didProcessMessage = DATA(proto)->didProcessMessage;
	DATA(self)->inputBuffer  = IOCLONE(DATA(proto)->inputBuffer);
	DATA(self)->outputBuffer = IOCLONE(DATA(proto)->outputBuffer);

	return self; 
}

IoAVCodec *IoAVCodec_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

// ----------------------------------------------------------- 

void IoAVCodec_freeContextIfNeeded(IoAVCodec *self) 
{ 
	if (DATA(self)->codecContext)
	{
		avcodec_close(DATA(self)->codecContext);
		av_free(DATA(self)->codecContext);
		DATA(self)->codecContext = NULL;
	}
	
	if (DATA(self)->outbuf)
	{
		free(DATA(self)->outbuf);
		DATA(self)->outbuf = NULL;
	}
	
	if (DATA(self)->inbuf)
	{
		free(DATA(self)->inbuf);
		DATA(self)->inbuf = NULL;
	}
}


void IoAVCodec_freeDstPictureIfNeeded(IoAVCodec *self) 
{ 
	if(DATA(self)->dstPicture) 
	{ 
		avpicture_free(DATA(self)->dstPicture);
		free(DATA(self)->dstPicture);
		DATA(self)->dstPicture = NULL; 
	}
}

void IoAVCodec_free(IoAVCodec *self) 
{ 
	IoAVCodec_freeDstPictureIfNeeded(self);
	IoAVCodec_freeContextIfNeeded(self);
	free(IoObject_dataPointer(self)); 
}

void IoAVCodec_mark(IoAVCodec *self) 
{
	IoObject_shouldMark(DATA(self)->inputBuffer);
	IoObject_shouldMark(DATA(self)->outputBuffer);
	IoObject_shouldMark(DATA(self)->willProcessMessage);
	IoObject_shouldMark(DATA(self)->didProcessMessage);
}

// ----------------------------------------------------------- 

void IoAVCodec_error_(IoAVCodec *self, IoMessage *m, char *s)
{
	fprintf(stderr, s);
	IoState_error_(IOSTATE, m, s);	
}

IoObject *IoAVCodec_inputBuffer(IoAVCodec *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("inputBuffer", "Returns the input buffer.")
	*/
	return DATA(self)->inputBuffer; 
}

IoObject *IoAVCodec_outputBuffer(IoAVCodec *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("outputBuffer", "Returns the output buffer.")
	*/
	return DATA(self)->outputBuffer; 
}

IoObject *IoAVCodec_stop(IoAVCodec *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("stop", "Stops processing data.")
	*/
	DATA(self)->isRunning = 0;
	return self; 
}

IoObject *IoAVCodec_isRunning(IoAVCodec *self, IoObject *locals, IoMessage *m)
{ 
	/*#io
	docSlot("isRunning", "Returns true if it's running, false otherwise.")
	*/
	return IOBOOL(self, DATA(self)->isRunning); 
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

IoObject *IoAVCodec_startAudioDecoding(IoAVCodec *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("startAudioDecoding", "Starts the decode processing. 
	The input codec should be set in the codecName slot. 
	The bitRate, sampleRate and channels slots will be set with the values of the input stream.")
	*/
	
	UArray *inba = IoSeq_rawUArray(DATA(self)->inputBuffer);
	UArray *outba = IoSeq_rawUArray(DATA(self)->outputBuffer);

	AVCodec *codec;
	AVCodecContext *c = NULL;
	int out_size, size, len;
	uint8_t *outbuf;
	//uint8_t *outbufResampled;
	uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE];
	uint8_t *inbuf_ptr;
	int needsToSetParams = 1;
	IoObject *codecName = IoObject_symbolGetSlot_(self, IOSYMBOL("codecName"));
	
	// set end of buffer to 0 (this ensures that no overreading happens for damaged mpeg streams) 
	memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);
	
	IoAVCodec_freeContextIfNeeded(self);
	IoAVCodec_registerIfNeeded(self);
	    

	codec = avcodec_find_decoder_by_name(CSTRING(codecName));
		
	if (!codec) 
	{
		printf("codec %s not found\n", CSTRING(codecName));
		IoState_error_(IOSTATE, m, "codec %s not found", CSTRING(codecName));
	}
	
	c = avcodec_alloc_context();
	DATA(self)->codecContext = c;
	
	if (avcodec_open(c, codec) < 0) 
	{
		IoState_error_(IOSTATE, m, "could not open codec");
	}
	
	for(i = 0; i < ic->nb_streams; i++) 
	{
        AVCodecContext *enc = c->streams[i]->codec;
		
        switch(enc->codec_type) 
		{
			case CODEC_TYPE_AUDIO:
					audio_index = i;
				break;
			case CODEC_TYPE_VIDEO:
					video_index = i;
				break;
			default:
				break;
		}
	}
	
	outbuf = malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE);
	DATA(self)->outbuf = outbuf;
			
	// decode until eof 
	inbuf_ptr = inbuf;
	
	DATA(self)->isRunning = 1;
	
	while (DATA(self)->isRunning) 
	{
		IoMessage_locals_performOn_(DATA(self)->willProcessMessage, self, self);

		size = UArray_size(inba) < INBUF_SIZE ? UArray_size(inba) : INBUF_SIZE;
		memcpy(inbuf, UArray_bytes(inba), size);
		UArray_removeRange(inba, 0, size);
		
		if (size == 0)
		{
		
				break;
		}
		
		inbuf_ptr = inbuf;
		

		while (size > 0) 
		{
			len = avcodec_decode_audio(c, (int16_t *)outbuf, &out_size, inbuf_ptr, size);
			
			if (len < 0) 
			{
				IoState_error_(IOSTATE, m, "Error while decoding");
			}
			
			if (out_size > 0) 
			{
				// if a frame has been decoded, output it 
				// convert short ints to floats

				size_t sampleCount = out_size / c->channels; 
				float *outf;
				size_t oldSize = UArray_size(outba);
				UArray_setSize_(outba, oldSize + sampleCount * sizeof(float));
				
				outf = (float *)(UArray_bytes(outba) + oldSize);
				
				if (needsToSetParams)
				{
					needsToSetParams = 0;
					IoObject_setSlot_to_(self, IOSYMBOL("bitRate"),    IONUMBER(c->bit_rate));
					IoObject_setSlot_to_(self, IOSYMBOL("sampleRate"), IONUMBER(c->sample_rate));
					IoObject_setSlot_to_(self, IOSYMBOL("channels"),   IONUMBER(c->channels));
				}
								
				IoAVCodec_ConvertShortToFloat((short *)outbuf, outf, sampleCount);
			}
			
			size -= len;
			inbuf_ptr += len;
		}
		
		
		IoMessage_locals_performOn_(DATA(self)->didProcessMessage, self, self);
	}
	
	IoAVCodec_freeContextIfNeeded(self);
	
	return self;
}

IoObject *IoAVCodec_startAudioEncoding(IoAVCodec *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("startAudioEncoding", "Starts the encode processing.")
	*/
	UArray *inba  = IoSeq_rawUArray(DATA(self)->inputBuffer);
	UArray *outba = IoSeq_rawUArray(DATA(self)->outputBuffer);
	
	AVCodec *codec;
	AVCodecContext *c = NULL;
	
	IoObject *codecName = IoObject_symbolGetSlot_(self, IOSYMBOL("codecName"));
	codec = avcodec_find_encoder_by_name(CSTRING(codecName));
	
	if (!codec) 
	{
		IoState_error_(IOSTATE, m, "codec %s not found", CSTRING(codecName));
	}
	
	c = avcodec_alloc_context();
	DATA(self)->codecContext = c;
	
	c->bit_rate    = (int)IoObject_doubleGetSlot_(self, IOSYMBOL("bitRate"));
	c->sample_rate = (int)IoObject_doubleGetSlot_(self, IOSYMBOL("sampleRate"));
	c->channels    = (int)IoObject_doubleGetSlot_(self, IOSYMBOL("channels"));
		
	if (avcodec_open(c, codec) < 0) 
	{
		IoState_error_(IOSTATE, m, "could not open codec");
	}
	
	{
	// the codec gives us the frame size, in samples 
	int samplesPerFrame = c->frame_size;
	short *inbuf = malloc(samplesPerFrame * 2 * c->channels);
	int outbuf_size = 10000;
	uint8_t *outbuf = malloc(outbuf_size);

	DATA(self)->inbuf = (uint8_t *)inbuf;
	DATA(self)->outbuf = (uint8_t *)outbuf;
	
	DATA(self)->isRunning = 1;

	while (DATA(self)->isRunning) 
	{
		float *f = (float *)UArray_bytes(inba);
		size_t inSize = UArray_size(inba) < outbuf_size ? UArray_size(inba) : outbuf_size;
		int outSize;
		
		// 1. copy part of inputBuffer to inbuf
		memcpy(inbuf, UArray_bytes(inba), inSize);
		UArray_removeRange(inba, 0, inSize);
		
		IoMessage_locals_performOn_(DATA(self)->willProcessMessage, self, self);
		
		// 2. convert input floats to shorts
		IoAVCodec_ConvertFloatToShort(f, inbuf, samplesPerFrame); // could be smaller  - inSize / sizeof(float)?

		// 3. encode shorts to bytes
		outSize = avcodec_encode_audio(c, outbuf, outbuf_size, inbuf); // what about inbuf size? is it assumed to be a frame worth?
		// what about frameless encodings?

		// 4. copy bytes to outba
		UArray_appendBytes_size_(outba, outbuf, outSize);
		
		IoMessage_locals_performOn_(DATA(self)->didProcessMessage, self, self);
	}
	
	}
	
	IoAVCodec_freeContextIfNeeded(self);
	
	return self;
}

AVPicture *IoAVCode_allocDstPictureIfNeeded(IoAVCodec *self, int pix_fmt, int width, int height)
{
	if(!DATA(self)->dstPicture)
	{
		DATA(self)->dstPicture = calloc(1, sizeof(AVPicture));
		avpicture_alloc(DATA(self)->dstPicture, PIX_FMT_RGB24, width, height);
	}
	
	return DATA(self)->dstPicture;
}

IoSeq *IoAVCode_frameSeqForAVFrame_(IoAVCodec *self, AVFrame *avframe, int srcPixelFormat, int width, int height)
{
	AVPicture *dstPicture = IoAVCode_allocDstPictureIfNeeded(self, srcPixelFormat, width, height);
	AVPicture srcPicture;
	int result;
	
	/*
	srcPicture.data[0] = avframe->data[0];
	srcPicture.data[1] = avframe->data[1];
	srcPicture.data[2] = avframe->data[2];
	srcPicture.data[3] = avframe->data[3];
	*/
	memcpy(srcPicture.data, avframe->data, sizeof(uint8_t *)*4);
	memcpy(srcPicture.linesize, avframe->linesize, sizeof(int)*4);
		
	result = img_convert(dstPicture, PIX_FMT_RGB24, &srcPicture, srcPixelFormat, width, height);
	
	if(result)
	{
		printf("AVCodec: img_convert error?\n");
	}
	
	UArray *data = UArray_newWithData_type_encoding_size_copy_(dstPicture->data[0], 
		CTYPE_uint8_t, 
		CENCODING_NUMBER, 
		width * height * 3, 1);
		
	return IoSeq_newWithUArray_copy_(IOSTATE, data, 0);
}


IoObject *IoAVCodec_startVideoDecoding(IoAVCodec *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("startVideoDecoding", "Starts the video decoding processing.")
	*/

    AVCodec *codec;
    AVCodecContext *c = NULL;
    int frame = 0, size, got_picture, len;
    FILE *f;
    AVFrame *picture;
    uint8_t inbuf[INBUF_SIZE + FF_INPUT_BUFFER_PADDING_SIZE], *inbuf_ptr;
	
	IoObject *fileName = IoObject_symbolGetSlot_(self, IOSYMBOL("fileName"));
	IoObject *videoCodecName = IoObject_symbolGetSlot_(self, IOSYMBOL("videoCodecName"));
	IoList *frames = IoList_new(IOSTATE);
	UArray *sizeUArray = UArray_newWithData_type_encoding_size_copy_("", CTYPE_float32_t, CENCODING_NUMBER, 2, 1);
	IoSeq *sizeSeq = IoSeq_newWithUArray_copy_(IOSTATE, sizeUArray, 0);
	
	IoObject_setSlot_to_(self, IOSYMBOL("frames"), frames);
	IoObject_setSlot_to_(self, IOSYMBOL("videoSize"), sizeSeq);
	IoAVCodec_freeDstPictureIfNeeded(self);

	
    /* set end of buffer to 0 (this ensures that no overreading happens for damaged mpeg streams) */
    memset(inbuf + INBUF_SIZE, 0, FF_INPUT_BUFFER_PADDING_SIZE);

	codec = avcodec_find_encoder_by_name(CSTRING(videoCodecName));
	codec = avcodec_find_decoder(CODEC_ID_MPEG1VIDEO);
	
    if (!codec) { IoState_error_(IOSTATE, m, "video codec '%s' not found", CSTRING(videoCodecName)); }

    c = avcodec_alloc_context();
    picture = avcodec_alloc_frame();

    if (codec->capabilities & CODEC_CAP_TRUNCATED)
	{
        c->flags|= CODEC_FLAG_TRUNCATED; // we dont send complete frames 
	}

    if (avcodec_open(c, codec) < 0) { IoState_error_(IOSTATE, m, "could not open video codec"); }
    
    // the codec gives us the frame size, in samples

    f = fopen(CSTRING(fileName), "rb");
	
    if (!f) { IoState_error_(IOSTATE, m, "unable to open file %s", CSTRING(fileName)); }
	
	c->debug = 0;
	
    for(;;) 
	{
		IoState_pushRetainPool(IOSTATE);	
		IoMessage_locals_performOn_(DATA(self)->willProcessMessage, self, self);

        size = fread(inbuf, 1, INBUF_SIZE, f);
        if (size == 0) { IoState_popRetainPool(IOSTATE); break; }
		   
        inbuf_ptr = inbuf;
        
		while (size > 0) 
		{
            len = avcodec_decode_video(c, picture, &got_picture, inbuf_ptr, size);
            if (len < 0) { IoState_error_(IOSTATE, m, "Error while decoding frame %d\n", frame); }
			
            if (got_picture) 
			{
				IoObject_setSlot_to_(self, IOSYMBOL("framePeriod"), IONUMBER(((float)c->time_base.num)/((float)c->time_base.den)));
				IoList_rawAppend_(frames, IoAVCode_frameSeqForAVFrame_(self, picture, c->pix_fmt, c->width, c->height));
				UArray_at_putDouble_(sizeUArray, 0, c->width);
				UArray_at_putDouble_(sizeUArray, 1, c->height);
                frame ++;
            }
			
            size -= len;
            inbuf_ptr += len;
        }
		
		IoMessage_locals_performOn_(DATA(self)->didProcessMessage, self, self);
		IoState_popRetainPool(IOSTATE);
    }
	   
    len = avcodec_decode_video(c, picture, &got_picture, NULL, 0);
	
    if (got_picture) 
	{
		IoList_rawAppend_(frames, IoAVCode_frameSeqForAVFrame_(self, picture, c->pix_fmt, c->width, c->height));
        frame ++;
    }
        
    fclose(f);

    avcodec_close(c);
    av_free(c);
    av_free(picture);
    printf("\n");
	return self;
}
