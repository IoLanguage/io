
//metadoc Blowfish copyright Steve Dekorte, 2004
//metadoc Blowfish license BSD revised
//metadoc Blowfish category Encryption
/*metadoc Blowfish description
The Blowfish object can be used to do encryption and decryption using 
the <a href=http://en.wikipedia.org/wiki/Blowfish_(cipher)>Blowfish</a> keyed, symmetric block cipher.
<p>
Example encryption and decription;

<pre>	
key := "secret"
data := "this is a message"

encryptedData := Blowfish clone setKey(key) encrypt(data)
decryptedData := Blowfish clone setKey(key) decrypt(encryptedData)
</pre>	

Or using the stream API:
<pre>	
key := "secret"
data := "this is a message"

cipher = Blowfish clone
cipher setIsEncrypting(true)
cipher setKey(key)
cipher beginProcessing
cipher inputBuffer appendSeq(data)
cipher process
cipher endProcess
encryptedData := cipher outputBuffer

cipher = Blowfish clone
cipher setIsEncrypting(false)
cipher setKey(key)
cipher beginProcessing
cipher inputBuffer appendSeq(encryptedData)
cipher process
cipher endProcess
decryptedData := cipher outputBuffer
</pre>	




*/

#include "IoBlowfish.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"

#define DATA(self) ((IoBlowfishData *)(IoObject_dataPointer(self)))

static const char *protoId = "Blowfish";

IoTag *IoBlowfish_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoBlowfish_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoBlowfish_rawClone);
	return tag;
}

IoBlowfish *IoBlowfish_proto(void *state)
{
	IoBlowfish *self = IoObject_new(state);
	IoObject_tag_(self, IoBlowfish_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoBlowfishData)));

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"setIsEncrypting", IoBlowfish_setIsEncrypting},
		{"beginProcessing", IoBlowfish_beginProcessing},
		{"process", IoBlowfish_process},
		{"endProcessing", IoBlowfish_endProcessing},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoBlowfish *IoBlowfish_rawClone(IoBlowfish *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoBlowfishData)));
	return self;
}

IoBlowfish *IoBlowfish_new(void *state)
{
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoBlowfish_free(IoBlowfish *self)
{
	free(DATA(self));
}

// -----------------------------------------------------------

IoObject *IoBlowfish_setIsEncrypting(IoBlowfish *self, IoObject *locals, IoMessage *m)
{
	/*doc Blowfish setIsEncrypting(aBool)
	If aBool is true, encrypting mode is on, otherwise, decrypting mode is on.
	*/

	IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);
	IOASSERT(ISTRUE(v) || ISFALSE(v), "requires boolean argument");
	DATA(self)->isEncrypting = ISTRUE(v);
	return self;
}

IoObject *IoBlowfish_beginProcessing(IoObject *self, IoObject *locals, IoMessage *m)
{
	/*doc Blowfish beginProcessing
	Sets the key from the key slot and initializes the cipher.
	*/

	UArray *key = IoObject_rawGetUArraySlot(self, locals, m, IOSYMBOL("key"));
	blowfish_ctx *context = &(DATA(self)->context);

	blowfish_init(context, (uint8_t *)UArray_bytes(key), (int)UArray_sizeInBytes(key));

	return self;
}

IoObject *IoBlowfish_process(IoBlowfish *self, IoObject *locals, IoMessage *m)
{
	/*doc Blowfish process
	Process the inputBuffer and appends the result to the outputBuffer.
	The processed inputBuffer is emptied except for the spare 
	bytes at the end which don't fit into a cipher block.
	*/
	blowfish_ctx *context = &(DATA(self)->context);
	int isEncrypting = DATA(self)->isEncrypting;

	UArray *input = IoObject_rawGetMutableUArraySlot(self, locals, m, IOSYMBOL("inputBuffer"));
	UArray *output = IoObject_rawGetMutableUArraySlot(self, locals, m, IOSYMBOL("outputBuffer"));

	const unsigned char *inputBytes  = (uint8_t *)UArray_bytes(input);
	size_t inputSize = UArray_sizeInBytes(input);

	unsigned long lr[2];
	size_t i, runs = inputSize / sizeof(lr);

	for (i = 0; i < runs; i ++)
	{
		memcpy(lr, inputBytes, sizeof(lr));

		inputBytes += sizeof(lr);

		if (isEncrypting)
		{
			blowfish_encrypt(context, &lr[0], &lr[1]);
		}
		else
		{
			blowfish_decrypt(context, &lr[0], &lr[1]);
		}

		UArray_appendBytes_size_(output, (unsigned char *)&lr, sizeof(lr));
	}

	UArray_removeRange(input, 0, runs * sizeof(lr));
	return self;
}

IoObject *IoBlowfish_endProcessing(IoBlowfish *self, IoObject *locals, IoMessage *m)
{
	/*doc Blowfish endProcessing
	Finish processing remaining bytes of inputBuffer.
	*/

	blowfish_ctx *context = &(DATA(self)->context);
	unsigned long lr[2];

	IoBlowfish_process(self, locals, m); // process the full blocks first


	{
		int isEncrypting = DATA(self)->isEncrypting;

		UArray *input  = IoObject_rawGetMutableUArraySlot(self, locals, m, IOSYMBOL("inputBuffer"));
		UArray *output = IoObject_rawGetMutableUArraySlot(self, locals, m, IOSYMBOL("outputBuffer"));

		IOASSERT(UArray_sizeInBytes(input) < sizeof(lr), "internal error - too many bytes left in inputBuffer");

		memset(lr, 0, sizeof(lr));
		memcpy(lr, (uint8_t *)UArray_bytes(input), UArray_sizeInBytes(input));

		if (isEncrypting)
		{
			blowfish_encrypt(context, &lr[0], &lr[1]);
		}
		else
		{
			blowfish_decrypt(context, &lr[0], &lr[1]);
		}

		UArray_appendBytes_size_(output, (unsigned char *)&lr, sizeof(lr));

		UArray_setSize_(input, 0);
	}
	return self;
}
