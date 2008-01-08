/*#io
ZlibEncoder ioDoc(
			docCopyright("Steve Dekorte", 2004)
			docLicense("BSD revised")
			docCategory("Compression")
			docDescription("""For Zlib compression.
Example use;
<pre>
bf = ZlibEncoder clone
bf beginProcessing
bf inputBuffer appendSeq("this is a message")
bf process
bf endProcess
bf outputBuffer // this contains the result data
</pre>
""")
*/

#include "IoZlibEncoder.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"

#define DATA(self) ((IoZlibEncoderData *)(IoObject_dataPointer(self)))

IoTag *IoZlibEncoder_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("ZlibEncoder");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoZlibEncoder_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoZlibEncoder_rawClone);
	return tag;
}

IoZlibEncoder *IoZlibEncoder_proto(void *state)
{
	IoZlibEncoder *self = IoObject_new(state);
	IoObject_tag_(self, IoZlibEncoder_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoZlibEncoderData)));
	DATA(self)->strm = calloc(1, sizeof(z_stream));
	DATA(self)->level = 9;

	IoState_registerProtoWithFunc_(state, self, IoZlibEncoder_proto);

	{
		IoMethodTable methodTable[] = {
		{"beginProcessing", IoZlibEncoder_beginProcessing},
		{"process", IoZlibEncoder_process},
		{"endProcessing", IoZlibEncoder_endProcessing},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoZlibEncoder *IoZlibEncoder_rawClone(IoZlibEncoder *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoZlibEncoderData)));
	DATA(self)->strm = calloc(1, sizeof(z_stream));
	return self;
}

IoZlibEncoder *IoZlibEncoder_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoZlibEncoder_proto);
	return IOCLONE(proto);
}

void IoZlibEncoder_free(IoZlibEncoder *self)
{
	free(DATA(self));
}

// -----------------------------------------------------------

IoObject *IoZlibEncoder_beginProcessing(IoZlibEncoder *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("beginProcessing", "Initializes the algorithm.")
	*/
	z_stream *strm = DATA(self)->strm;
	int ret;

	strm->zalloc = Z_NULL;
	strm->zfree = Z_NULL;
	strm->opaque = Z_NULL;
	strm->avail_in = 0;
	strm->next_in = Z_NULL;
	ret = deflateInit(strm, DATA(self)->level);
	IOASSERT(ret == Z_OK, "unable to initialize zlib via inflateInit()");

	return self;
}

IoObject *IoZlibEncoder_endProcessing(IoZlibEncoder *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("endProcessing", "Finish processing remaining bytes of inputBuffer.")
	*/
	z_stream *strm = DATA(self)->strm;

	IoZlibEncoder_process(self, locals, m);
	deflateEnd(strm);

	DATA(self)->isDone = 1;
	return self;
}

IoObject *IoZlibEncoder_process(IoZlibEncoder *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("process", "Process the inputBuffer and appends the result to the outputBuffer.
The processed inputBuffer is empties except for the spare bytes at the end which don't fit into a cipher block.")
	*/
	z_stream *strm = DATA(self)->strm;

	UArray *input  = IoObject_rawGetMutableUArraySlot(self, locals, m, IOSYMBOL("inputBuffer"));
	UArray *output = IoObject_rawGetMutableUArraySlot(self, locals, m, IOSYMBOL("outputBuffer"));

	uint8_t *inputBytes = (uint8_t *)UArray_bytes(input);
	size_t inputSize = UArray_sizeInBytes(input);

	if (inputSize)
	{
		int ret;
		size_t oldOutputSize = UArray_size(output);
		size_t outputRoom = (inputSize * 2);
		uint8_t *outputBytes;

		UArray_setSize_(output, oldOutputSize + outputRoom);
		outputBytes = (uint8_t *)UArray_bytes(output) + oldOutputSize;

		strm->next_in   = inputBytes;
		strm->avail_in  = inputSize;

		strm->next_out  = outputBytes;
		strm->avail_out = outputRoom;

		ret = deflate(strm, Z_NO_FLUSH);
		//assert(ret != Z_STREAM_ERROR);
		{
		size_t outputSize = outputRoom - strm->avail_out;
		UArray_setSize_(output, oldOutputSize + outputSize);
		}

		UArray_setSize_(input, 0);
	}

	return self;
}

