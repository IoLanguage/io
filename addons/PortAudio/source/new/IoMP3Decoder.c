/*   copyright: Steve Dekorte, 2002
*   All rights reserved. See _BSDLicense.txt.
*
* user clones MP3Decoder and implements input and output methods 
* and then calls run.
* input should return a buffer, output will have a buffer argument.
*/

#include "IoMP3Decoder.h"
#include "List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include <limits.h>

#define DATA(self) ((IoMP3DecoderData *)IoObject_dataPointer(self))

static const char *protoId = "MP3Decoder";

IoTag *IoMP3Decoder_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoMP3Decoder_rawClone);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoMP3Decoder_mark);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoMP3Decoder_free);
	return tag;
}

IoMP3Decoder *IoMP3Decoder_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoMP3Decoder_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoMP3DecoderData)));
	
	//DATA(self)->outSound = 0x0;
	DATA(self)->willProcessMessage = IoMessage_newWithName_label_(state, IOSYMBOL("willProcess"), IOSYMBOL("[MP3Decoder]"));
	DATA(self)->didProcessMessage = IoMessage_newWithName_label_(state, IOSYMBOL("didProcess"), IOSYMBOL("[MP3Decoder]"));
	DATA(self)->inputBuffer  = IoSeq_new(state);
	DATA(self)->outputBuffer = IoSeq_new(state);
	DATA(self)->tmpInputBa = UArray_new();
	
	IoState_registerProtoWithId_(state, self, protoId);
	
	{
		IoMethodTable methodTable[] = {
		{"start", IoMP3Decoder_start},
		{"stop",  IoMP3Decoder_stop},
		{"inputBuffer",  IoMP3Decoder_inputBuffer},
		{"outputBuffer",  IoMP3Decoder_outputBuffer},
		
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoMP3Decoder *IoMP3Decoder_rawClone(IoMP3Decoder *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoMP3DecoderData)));
	
	DATA(self)->willProcessMessage = DATA(proto)->willProcessMessage;
	DATA(self)->didProcessMessage = DATA(proto)->didProcessMessage;
	DATA(self)->inputBuffer  = IOCLONE(DATA(proto)->inputBuffer);
	DATA(self)->outputBuffer = IOCLONE(DATA(proto)->outputBuffer);
	DATA(self)->tmpInputBa = UArray_new();
	
	IoState_addValue_(IOSTATE, self);
	return self; 
}

IoMP3Decoder *IoMP3Decoder_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

/* ----------------------------------------------------------- */

void IoMP3Decoder_free(IoMP3Decoder *self) 
{ 
	UArray_free(DATA(self)->tmpInputBa);
	free(IoObject_dataPointer(self)); 
}

void IoMP3Decoder_mark(IoMP3Decoder *self) 
{
	IoObject_shouldMark(DATA(self)->inputBuffer);
	IoObject_shouldMark(DATA(self)->outputBuffer);
	IoObject_shouldMark(DATA(self)->willProcessMessage);
	IoObject_shouldMark(DATA(self)->didProcessMessage);
}

/* ----------------------------------------------------------- */

/* (re)fill the stream buffer which is to be decoded. */

static enum mad_flow IoMP3Decoder_inputCallback(void *data, struct mad_stream *stream)
{
	IoMP3Decoder *self = data;
	struct mad_decoder *decoder = &(DATA(self)->decoder);
	
	IoMessage_locals_performOn_(DATA(self)->willProcessMessage, self, self);

	if (DATA(self)->isRunning)
	{
		UArray *ba = IoSeq_rawUArray(DATA(self)->inputBuffer);
		
		UArray_removeRange(ba, 0, DATA(self)->lastInputPos);
		
		{
			size_t size = UArray_size(ba);
			UArray_setSize_(ba, size + MAD_BUFFER_GUARD);
			memset(UArray_bytes(ba) + size, 0x0, MAD_BUFFER_GUARD);
			UArray_setSize_(ba, size);
		}
		
		if (UArray_size(ba) == 0) 
		{
			return MAD_FLOW_CONTINUE;
		}
		
		DATA(self)->lastInputPos = UArray_size(ba);
		
		mad_stream_buffer(stream, UArray_bytes(ba), UArray_size(ba));
	}
	
	return DATA(self)->isRunning ? MAD_FLOW_CONTINUE : MAD_FLOW_STOP;
}

// This is the output callback function. It is called after each frame of
// MPEG audio data has been completely decoded. The purpose of this callback
// is to output (or play) the decoded PCM audio.

static enum mad_flow IoMP3Decoder_outputCallback(void *data,
									    struct mad_header const *header,
									    struct mad_pcm *pcm)
{
	IoMP3Decoder *self = data;
	UArray *ba = IoSeq_rawUArray(DATA(self)->outputBuffer);
	unsigned int oldSize = UArray_size(ba);
	unsigned int newSize = oldSize + (pcm->length * 2 * sizeof(float));

	UArray_setSize_(ba, newSize);

	if (!DATA(self)->isRunning) 
	{
		return MAD_FLOW_STOP;
	}
	
	// MAD data is in 4 byte signed ints 
	// and on separated (not interleaved channels) 
	// so we interleave them here 
	
	{
		float *out = (float *)(UArray_bytes(ba) + oldSize);
		unsigned int nsamples  = pcm->length;
		
		mad_fixed_t const *left  = pcm->samples[0];
		mad_fixed_t const *right = pcm->samples[1];
		
		if (pcm->channels == 2)
		{
			// this would be much faster as a vector op
			while (nsamples --) 
			{
				*out = ((float)(*left)) / INT_MAX; 
				out ++; 
				*out = ((float)(*right)) / INT_MAX;
				out ++; 
				left ++;
				right ++;      
			}
		}
		else
		{
			while (nsamples --) 
			{
				float f = ((float)(*left)) / INT_MAX; 
				*out = f; 
				out ++; 
				*out = f;
				out ++; 
				left ++;
			}
		}		
	}
	
	IoMessage_locals_performOn_(DATA(self)->didProcessMessage, self, self);
	
	return DATA(self)->isRunning ? MAD_FLOW_CONTINUE : MAD_FLOW_STOP;
}

/*
 * This is the error callback function. It is called whenever a decoding
 * error occurs. The error is indicated by stream->error; the list of
 * possible MAD_ERROR_* errors can be found in the mad.h (or
 * libmad/stream.h) header file.
 */

static enum mad_flow IoMP3Decoder_errorCallback(void *data,
									   struct mad_stream *stream,
									   struct mad_frame *frame)
{
	//IoMP3Decoder *self = data;

	 fprintf(stderr, "lbmad error %i (%s)\n", stream->error, mad_stream_errorstr(stream));

	 /*
	IoState_error_(IOSTATE, 0x0, "MP3Decoder %s", mad_stream_errorstr(stream));
	
	DATA(self)->isRunning = 0;
	return MAD_FLOW_BREAK;
	*/
	return MAD_FLOW_CONTINUE;
}

IoObject *IoMP3Decoder_start(IoMP3Decoder *self, IoObject *locals, IoMessage *m)
{ 
	int result;
	DATA(self)->isRunning = 1;
	DATA(self)->lastInputPos = 0;
	
	mad_decoder_init(&(DATA(self)->decoder), (void *)self,
				  IoMP3Decoder_inputCallback, 
				  0 /* header */, 
				  0 /* filter */, 
				  IoMP3Decoder_outputCallback,
				  IoMP3Decoder_errorCallback, 
				  0 /* message */);
	
	/* start decoding */
	
	result = mad_decoder_run(&(DATA(self)->decoder), MAD_DECODER_MODE_SYNC);
	
	mad_decoder_finish(&(DATA(self)->decoder));
	
	/* release the decoder */
		
	return IONUMBER(result);
}

IoObject *IoMP3Decoder_inputBuffer(IoMP3Decoder *self, IoObject *locals, IoMessage *m)
{ 
	return DATA(self)->inputBuffer; 
}

IoObject *IoMP3Decoder_outputBuffer(IoMP3Decoder *self, IoObject *locals, IoMessage *m)
{ 
	return DATA(self)->outputBuffer; 
}

IoObject *IoMP3Decoder_stop(IoMP3Decoder *self, IoObject *locals, IoMessage *m)
{ 
	DATA(self)->isRunning = 0;
	DATA(self)->lastInputPos = 0;
	return self; 
}

IoObject *IoMP3Decoder_isRunning(IoMP3Decoder *self, IoObject *locals, IoMessage *m)
{ 
	return IOBOOL(self, DATA(self)->isRunning); 
}

