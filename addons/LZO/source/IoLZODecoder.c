//metadoc LZODecoder copyright Steve Dekorte, 2004
//metadoc LZODecoder license BSD revised
//metadoc LZODecoder category Compression
/*metadoc LZODecoder description
The LZO object can be used to  uncompress data using the 
<a href=http://en.wikipedia.org/wiki/Lempel-Ziv-Oberhumer> Lempel-Ziv-Oberhumer (LZO)</a> 
lossless data compression algorithm.
<p>
Example use;
<pre>	
z = LZODecoder clone
z beginProcessing
z inputBuffer appendSeq("this is a message")
z process
z endProcess
result := z outputBuffer 
</pre>	
*/

#include "IoLZODecoder.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"

#define DATA(self) ((IoLZOData *)(IoObject_dataPointer(self)))

static const char *protoId = "LZODecoder";

IoTag *IoLZODecoder_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoLZODecoder_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoLZODecoder_rawClone);
	return tag;
}

IoLZODecoder *IoLZODecoder_proto(void *state)
{
	IoLZODecoder *self = IoObject_new(state);
	IoObject_tag_(self, IoLZODecoder_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoLZOData)));

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"beginProcessing", IoLZODecoder_beginProcessing},
		{"process", IoLZODecoder_process},
		{"endProcessing", IoLZODecoder_endProcessing},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoLZODecoder *IoLZODecoder_rawClone(IoLZODecoder *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoLZOData)));
	return self;
}

IoLZODecoder *IoLZODecoder_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoLZODecoder_free(IoLZODecoder *self)
{
	free(DATA(self));
}

// ----------------------------------------------------------- *

IoObject *IoLZODecoder_beginProcessing(IoLZODecoder *self, IoObject *locals, IoMessage *m)
{
	/*doc LZODecoder beginProcessing
	Initializes the algorithm.
	*/

	IOASSERT(lzo_init() == LZO_E_OK,  "Failed to init lzo");
	DATA(self)->isDone = 0;
	return self;
}

IoObject *IoLZODecoder_endProcessing(IoLZODecoder *self, IoObject *locals, IoMessage *m)
{
	/*doc LZODecoder endProcessing
	Finish processing remaining bytes of inputBuffer.
	*/

	IoLZODecoder_process(self, locals, m); // process the full blocks first
	DATA(self)->isDone = 1;
	return self;
}

// --------------------------------------------------------------------

IoObject *IoLZODecoder_process(IoLZODecoder *self, IoObject *locals, IoMessage *m)
{
	/*doc LZODecoder process
	Process the inputBuffer and appends the result to the outputBuffer.
	The processed inputBuffer is emptied except for the spare bytes at 
	the end which don't fit into a cipher block.
	*/
	
	lzo_align_t __LZO_MMODEL *wrkmem = DATA(self)->wrkmem;

	UArray *input  = IoObject_rawGetMutableUArraySlot(self, locals, m, IOSYMBOL("inputBuffer"));
	UArray *output = IoObject_rawGetMutableUArraySlot(self, locals, m, IOSYMBOL("outputBuffer"));

	unsigned char *inputBytes  = (uint8_t *)UArray_bytes(input);
	size_t inputSize           = UArray_sizeInBytes(input);

	if (inputSize)
	{
		int r;
		size_t oldOutputSize   = UArray_size(output);
		lzo_uint outputRoom    = (inputSize * 10);
		unsigned char *outputBytes;

		UArray_setSize_(output, oldOutputSize + outputRoom);
		outputBytes = (uint8_t *)UArray_bytes(output) + oldOutputSize;

		r = lzo1x_decompress(inputBytes, inputSize, outputBytes, &outputRoom, wrkmem);

		if (r != LZO_E_OK)
		{
			IoState_error_(IOSTATE,  m, "LZO compression failed: %d", r);
		}


		UArray_setSize_(output, oldOutputSize + outputRoom);
		UArray_setSize_(input, 0);
	}

	return self;
}
