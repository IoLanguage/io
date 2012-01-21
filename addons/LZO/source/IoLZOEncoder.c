//metadoc LZOEncoder copyright Steve Dekorte, 2004
//metadoc LZOEncoder license BSD revised
//metadoc LZOEncoder category Compression
/*metadoc LZOEncoder description
The LZOEncoder object can be used to compress  data using the 
<a href=http://en.wikipedia.org/wiki/Lempel-Ziv-Oberhumer> Lempel-Ziv-Oberhumer (LZO)</a> 
lossless data compression algorithm.
<p>
Example use;
<pre>	
z = LZOEncoder clone
z beginProcessing
z inputBuffer appendSeq("this is a message")
z process
z endProcess
result := z outputBuffer 
</pre>		
*/

#include "IoLZOEncoder.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"

#define DATA(self) ((IoLZOData *)(IoObject_dataPointer(self)))

static const char *protoId = "LZOEncoder";

IoTag *IoLZOEncoder_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoLZOEncoder_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoLZOEncoder_rawClone);
	return tag;
}

IoLZOEncoder *IoLZOEncoder_proto(void *state)
{
	IoLZOEncoder *self = IoObject_new(state);
	IoObject_tag_(self, IoLZOEncoder_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoLZOData)));

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"beginProcessing", IoLZOEncoder_beginProcessing},
		{"process", IoLZOEncoder_process},
		{"endProcessing", IoLZOEncoder_endProcessing},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoLZOEncoder *IoLZOEncoder_rawClone(IoLZOEncoder *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoLZOData)));
	return self;
}

IoLZOEncoder *IoLZOEncoder_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoLZOEncoder_free(IoLZOEncoder *self)
{
	free(DATA(self));
}

// ----------------------------------------------------------- *

IoObject *IoLZOEncoder_beginProcessing(IoLZOEncoder *self, IoObject *locals, IoMessage *m)
{
	/*doc LZOEncoder beginProcessing
	Initializes the algorithm.
	*/

	IOASSERT(lzo_init() == LZO_E_OK,  "Failed to init lzo");
	DATA(self)->isDone = 0;
	return self;
}

IoObject *IoLZOEncoder_endProcessing(IoLZOEncoder *self, IoObject *locals, IoMessage *m)
{
	/*doc LZOEncoder endProcessing
	Finish processing remaining bytes of inputBuffer.
	*/

	IoLZOEncoder_process(self, locals, m); // process the full blocks first
	DATA(self)->isDone = 1;
	return self;
}

// --------------------------------------------------------------------

IoObject *IoLZOEncoder_process(IoLZOEncoder *self, IoObject *locals, IoMessage *m)
{
	/*doc LZOEncoder process
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
		lzo_uint outputRoom    = (inputSize + inputSize / 64 + 16 + 3);
		unsigned char *outputBytes;

		UArray_setSize_(output, oldOutputSize + outputRoom);
		outputBytes = (uint8_t *)UArray_bytes(output) + oldOutputSize;

		r = lzo1x_1_compress(inputBytes, inputSize, outputBytes, &outputRoom, wrkmem);
		//	r = lzo1x_decompress(in, in_len, out, &out_len, wrkmem);

		if (r != LZO_E_OK)
		{
			IoState_error_(IOSTATE,  m, "LZO compression failed: %d", r);
		}


		UArray_setSize_(output, oldOutputSize + outputRoom);
		UArray_setSize_(input, 0);
	}

	return self;
}
