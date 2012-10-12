
//metadoc MD5 copyright Steve Dekorte 2002
//metadoc MD5 license BSD revised
//metadoc MD5 category Digests
/*metadoc MD5 description
An object for calculating MD5 hashes. Each hash calculation should instantiate its own MD5 instance.
<p>
Example:
<pre>
digest := MD5 clone
digest appendSeq("this is a message")
out := digest md5String
</pre>
*/

#include "IoMD5.h"
#include "IoState.h"
#include "IoSeq.h"

#define DATA(self) ((IoMD5Data *)IoObject_dataPointer(self))

static const char *protoId = "MD5";

IoTag *IoMD5_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoMD5_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoMD5_rawClone);
	return tag;
}

IoMD5 *IoMD5_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoMD5_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoMD5Data)));
	io_md5_init(&(DATA(self)->mstate));

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		{"appendSeq", IoMD5_appendSeq},
		{"md5", IoMD5_md5},
		{"md5String", IoMD5_md5String},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoMD5 *IoMD5_rawClone(IoMD5 *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoMD5Data)));
	io_md5_init(&(DATA(self)->mstate));
	return self;
}

IoMD5 *IoMD5_new(void *state)
{
	IoMD5 *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoMD5_free(IoMD5 *self)
{
	free(IoObject_dataPointer(self));
}

/* ----------------------------------------------------------- */

IoObject *IoMD5_appendSeq(IoMD5 *self, IoObject *locals, IoMessage *m)
{
	/*doc MD5 appendSeq(aSequence)
	Appends aSequence to the hash calculation. Returns self.
	*/

	IoSeq *buffer = IoMessage_locals_seqArgAt_(m, locals, 0);
	IOASSERT(DATA(self)->isDone == 0, "cannot append to an already calculated md5");
	io_md5_append(&(DATA(self)->mstate),
				(unsigned char const *)IoSeq_rawBytes(buffer),
				(int)IoSeq_rawSize(buffer));
	return self;
}

UArray *IoMD5_md5UArray(IoMD5 *self)
{
	if (DATA(self)->isDone == 0)
	{
		io_md5_finish(&(DATA(self)->mstate), DATA(self)->digest);
		DATA(self)->isDone = 1;
	}

	return UArray_newWithData_type_size_copy_(DATA(self)->digest, CTYPE_uint8_t, 16, 1);
}

IoObject *IoMD5_md5(IoMD5 *self, IoObject *locals, IoMessage *m)
{
	/*doc MD5 md5
	Completes the MD5 calculation and returns the hash as a Buffer. Once this method is called, append() should not be called again on the receiver or it will raise an exception.
	*/
	
	return IoSeq_newWithUArray_copy_(IOSTATE, IoMD5_md5UArray(self), 0);
}

IoObject *IoMD5_md5String(IoMD5 *self, IoObject *locals, IoMessage *m)
{
	/*doc MD5 md5String
	Returns a string containing a hexadecimal representation of the md5 hash.
	*/
	
	UArray *ba = IoMD5_md5UArray(self);
	UArray *baString = UArray_asNewHexStringUArray(ba);
	UArray_free(ba);
	return IoState_symbolWithUArray_copy_(IOSTATE, baString, 0);
}
