
//metadoc SHA1 copyright Steve Dekorte 2002
//metadoc SHA1 license BSD revised
/*metadoc SHA1 description
An object for calculating SHA1 hashes. Each has calculation 
should instiate it's own SHA1 instance.
*/
//metadoc SHA1 category Digests

#include "IoSHA1.h"
#include "IoState.h"
#include "IoSeq.h"

#define DATA(self) ((IoSHA1Data *)IoObject_dataPointer(self))

IoTag *IoSHA1_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("SHA1");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoSHA1_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoSHA1_rawClone);
	return tag;
}

IoSHA1 *IoSHA1_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoSHA1_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoSHA1Data)));
	SHA1Init(&(DATA(self)->context));

	IoState_registerProtoWithFunc_(state, self, IoSHA1_proto);

	{
		IoMethodTable methodTable[] = {
		{"appendSeq", IoSHA1_appendSeq},
		{"sha1", IoSHA1_sha1},
		{"sha1String", IoSHA1_sha1String},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoSHA1 *IoSHA1_rawClone(IoSHA1 *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoSHA1Data)));
	SHA1Init(&(DATA(self)->context));
	return self;
}

IoSHA1 *IoSHA1_new(void *state)
{
	IoSHA1 *proto = IoState_protoWithInitFunction_(state, IoSHA1_proto);
	return IOCLONE(proto);
}

void IoSHA1_free(IoSHA1 *self)
{
	free(IoObject_dataPointer(self));
}

/* ----------------------------------------------------------- */

IoObject *IoSHA1_appendSeq(IoSHA1 *self, IoObject *locals, IoMessage *m)
{
	/*doc SHA1 appendSeq(aSequence)
	Appends aSequence to the hash calculation. Returns self.
	 */

	IoSeq *buffer = IoMessage_locals_seqArgAt_(m, locals, 0);
	IOASSERT(DATA(self)->isDone == 0, "cannot append to an already calculated sha1");
	SHA1Update(&(DATA(self)->context),
			 (unsigned char const *)IoSeq_rawBytes(buffer),
			 IoSeq_rawSize(buffer));
	return self;
}

UArray *IoSHA1_sha1UArray(IoSHA1 *self)
{
	if (DATA(self)->isDone == 0)
	{
		SHA1Final(DATA(self)->digest, &(DATA(self)->context));
		DATA(self)->isDone = 1;
	}

	return UArray_newWithData_type_size_copy_(DATA(self)->digest, CTYPE_uint8_t, SHA1_DIGEST_LENGTH, 1);
}

IoObject *IoSHA1_sha1(IoSHA1 *self, IoObject *locals, IoMessage *m)
{
	/*doc SHA1 sha1
	Completes the SHA1 calculation and returns the hash as a Buffer.
	Once this method is called, append() should not be called again on the receiver or it will raise an exception.")
	*/
	
	return IoSeq_newWithUArray_copy_(IOSTATE, IoSHA1_sha1UArray(self), 0);
}

IoObject *IoSHA1_sha1String(IoSHA1 *self, IoObject *locals, IoMessage *m)
{
	/*doc SHA1 sha1String
	Returns a string containing a hexadecimal representation of the sha1 hash.")
	*/
	
	UArray *ba = IoSHA1_sha1UArray(self);
	UArray *baString = UArray_asNewHexStringUArray(ba);
	UArray_free(ba);
	return IoState_symbolWithUArray_copy_(IOSTATE, baString, 0);
}


