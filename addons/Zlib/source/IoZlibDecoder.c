//metadoc ZlibDecoder copyright Steve Dekorte, 2004
//metadoc ZlibDecoder license BSD revised
//metadoc ZlibDecoder category Compression
/*metadoc ZlibDecoder description
For <a href=http://en.wikipedia.org/wiki/Zlib>Zlib</a> uncompression.
Example use;
<pre>	
z = ZlibDecoder clone
z beginProcessing
z inputBuffer appendSeq(inputData)
z process
z endProcess
result := z outputBuffer 
</pre>	
*/

//doc ZlibDecoder inputBuffer The input buffer for decoding.
//doc ZlibDecoder outputBuffer The output buffer for decoding.


#include "IoZlibDecoder.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include <errno.h>

#define DATA(self) ((IoZlibDecoderData *)(IoObject_dataPointer(self)))

IoTag *IoZlibDecoder_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("ZlibDecoder");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoZlibDecoder_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoZlibDecoder_rawClone);
	return tag;
}

IoZlibDecoder *IoZlibDecoder_proto(void *state)
{
	IoZlibDecoder *self = IoObject_new(state);
	IoObject_tag_(self, IoZlibDecoder_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoZlibDecoderData)));
	DATA(self)->strm = calloc(1, sizeof(z_stream));

	IoState_registerProtoWithFunc_(state, self, IoZlibDecoder_proto);

	{
		IoMethodTable methodTable[] = {
		{"beginProcessing", IoZlibDecoder_beginProcessing},
		{"process", IoZlibDecoder_process},
		{"endProcessing", IoZlibDecoder_endProcessing},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoZlibDecoder *IoZlibDecoder_rawClone(IoZlibDecoder *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoZlibDecoderData)));
	DATA(self)->strm = calloc(1, sizeof(z_stream));
	return self;
}

IoZlibDecoder *IoZlibDecoder_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoZlibDecoder_proto);
	return IOCLONE(proto);
}

void IoZlibDecoder_free(IoZlibDecoder *self)
{
	free(DATA(self));
}

/* ----------------------------------------------------------- */

IoObject *IoZlibDecoder_beginProcessing(IoZlibDecoder *self, IoObject *locals, IoMessage *m)
{
	/*doc ZlibDecoder beginProcessing
	Initializes the algorithm.
	*/
	
	z_stream *strm = DATA(self)->strm;
	int ret;

	strm->zalloc = Z_NULL;
	strm->zfree = Z_NULL;
	strm->opaque = Z_NULL;
	strm->avail_in = 0;
	strm->next_in = Z_NULL;
	//ret = inflateInit(strm); // zlib format
	ret = inflateInit2(strm, 15 + 32); // gz format
	IOASSERT(ret == Z_OK, "unable to initialize zlib via inflateInit()");

	return self;
}

IoObject *IoZlibDecoder_endProcessing(IoZlibDecoder *self, IoObject *locals, IoMessage *m)
{
	/*doc ZlibDecoder endProcessing
	Finish processing remaining bytes of inputBuffer.
	*/
	
	z_stream *strm = DATA(self)->strm;

	IoZlibDecoder_process(self, locals, m);
	inflateEnd(strm);

	DATA(self)->isDone = 1;
	return self;
}


IoObject *IoZlibDecoder_process(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc ZlibDecoder process
	Process the inputBuffer and appends the result to the outputBuffer.
	The processed inputBuffer is empties except for the spare bytes at the end which don't fit into a cipher block.
	*/
	
	z_stream *strm = DATA(self)->strm;

	UArray *input  = IoObject_rawGetMutableUArraySlot(self, locals, m, IOSYMBOL("inputBuffer"));
	UArray *output = IoObject_rawGetMutableUArraySlot(self, locals, m, IOSYMBOL("outputBuffer"));

	uint8_t *inputBytes = (uint8_t *)UArray_bytes(input);
	size_t inputSize = UArray_sizeInBytes(input);

	//printf("inputBytes = [%s]\n", inputBytes);

	if (inputSize)
	{
		int ret;
		size_t oldOutputSize = UArray_size(output);
		size_t outputRoom    = (inputSize * 10);
		uint8_t *outputBytes;

		UArray_setSize_(output, oldOutputSize + outputRoom);
		outputBytes = (uint8_t *)UArray_bytes(output) + oldOutputSize;

		strm->next_in   = inputBytes;
		strm->avail_in  = inputSize;

		strm->next_out  = outputBytes;
		strm->avail_out = outputRoom;

		errno = 0;

		ret = inflate(strm, Z_NO_FLUSH);

		//printf("inputSize  = %i\n", (int)inputSize);
		//printf("outputRoom = %i\n", (int)outputRoom);

		if (errno)
		{
			char *s = strerror(errno);
			//printf("zlib error: %s\n", s);
			IoState_error_(IOSTATE, m, "ZlibDecoder error: %s\n", s);
			/*
			printf("Z_STREAM_END = %i\n", Z_STREAM_END);
			printf("Z_NEED_DICT = %i\n", Z_NEED_DICT);
			printf("Z_DATA_ERROR = %i\n", Z_DATA_ERROR);
			printf("Z_STREAM_ERROR = %i\n", Z_STREAM_ERROR);
			printf("Z_MEM_ERROR = %i\n", Z_MEM_ERROR);
			printf("Z_BUF_ERROR = %i\n", Z_BUF_ERROR);
			*/
		}

		//assert(ret != Z_STREAM_ERROR);
		{
		size_t outputSize = outputRoom - strm->avail_out;
		//size_t outputSize = strm->avail_out;
		//printf("outputSize = %i\n", (int)outputSize);
		UArray_setSize_(output, oldOutputSize + outputSize);
		}

		UArray_setSize_(input, 0);
	}

	return self;
}

