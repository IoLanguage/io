/*   copyright: Steve Dekorte, 2002
*   All rights reserved. See _BSDLicense.txt.
*
* user clones MP3Decoder and implements input and output methods 
* and then calls run.
* input should return a buffer, output will have a buffer argument.
*/

#include "IoMP3Encoder.h"
#include "IoObject_actor.h"
#include "base/List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoBuffer.h"
#include "IoNil.h"


#define DATA(self) ((IoMP3EncoderData *)self->data)

IoTag *IoMP3Encoder_newTag(void *state)
{
    IoTag *tag = IoTag_newWithName_("MP3Encoder");
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoMP3Encoder_rawClone);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoMP3Encoder_mark);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoMP3Encoder_free);
    return tag;
}

IoMP3Encoder *IoMP3Encoder_proto(void *state)
{
    IoObject *self = IoObject_new(state);
    IoObject_tag_(self, IoMP3Encoder_newTag(state));
    
    self->data = calloc(1, sizeof(IoMP3EncoderData));
    DATA(self)->outBuffer = IoBuffer_new(state);
    DATA(self)->encoder = MP3Encoder_new();
    MP3Encoder_setExternalOutputUArray_(DATA(self)->encoder, 
					   IoBuffer_rawUArray(DATA(self)->outBuffer));
    
    
    IoState_registerProtoWithFunc_(state, self, IoMP3Encoder_proto);
    
    {
	IoMethodTable methodTable[] = {
	{"encode", IoMP3Encoder_encode},
	{"end",  IoMP3Encoder_end},
	{"outBuffer",  IoMP3Encoder_outBuffer},
	{"setBitRate",  IoMP3Encoder_setBitRate},
	{"setSampleRate",  IoMP3Encoder_setSampleRate},
	{"setQuality",  IoMP3Encoder_setQuality},
	{"setCompressionRatio",  IoMP3Encoder_setCompressionRatio},
	{NULL, NULL},
	};
	IoObject_addMethodTable_(self, methodTable);
    }
    
    return self;
}

IoMP3Encoder *IoMP3Encoder_rawClone(IoMP3Encoder *proto) 
{ 
    IoObject *self = IoObject_rawClonePrimitive(proto);
    self->data = cpalloc(proto->data, sizeof(IoMP3EncoderData));
    DATA(self)->outBuffer = IoBuffer_new(IOSTATE);
    DATA(self)->encoder = MP3Encoder_new();
    MP3Encoder_setExternalOutputUArray_(DATA(self)->encoder, 
					   IoBuffer_rawUArray(DATA(self)->outBuffer));
    IoState_addValue_(IOSTATE, self);
    return self; 
}

IoMP3Encoder *IoMP3Encoder_new(void *state)
{
    IoObject *proto = IoState_protoWithInitFunction_(state, IoMP3Encoder_proto);
    return IOCLONE(proto);
}

/* ----------------------------------------------------------- */

void IoMP3Encoder_free(IoMP3Encoder *self) 
{ 
    MP3Encoder_free(DATA(self)->encoder);
    free(self->data); 
}

void IoMP3Encoder_mark(IoMP3Encoder *self) 
{
    IoObject_makeGrayIfWhite(DATA(self)->outBuffer);
}

/* --- Output Parameters -------------------------------------------------------- */

IoObject *IoMP3Encoder_setSampleRate(IoMP3Encoder *self, IoObject *locals, IoMessage *m)
{
    int n = IoMessage_locals_intArgAt_(m, locals, 0);
    MP3Encoder_setSampleRate_(DATA(self)->encoder, n);
    return IoMP3Encoder_checkError(self, locals, m);
}

IoObject *IoMP3Encoder_setBitRate(IoMP3Encoder *self, IoObject *locals, IoMessage *m)
{
    int n = IoMessage_locals_intArgAt_(m, locals, 0);
    MP3Encoder_setBitRate_(DATA(self)->encoder, n);
    return IoMP3Encoder_checkError(self, locals, m);
}

IoObject *IoMP3Encoder_setQuality(IoMP3Encoder *self, IoObject *locals, IoMessage *m)
{
    int n = IoMessage_locals_intArgAt_(m, locals, 0);
    MP3Encoder_setQuality_(DATA(self)->encoder, n);
    return IoMP3Encoder_checkError(self, locals, m);
}

IoObject *IoMP3Encoder_setCompressionRatio(IoMP3Encoder *self, IoObject *locals, IoMessage *m)
{
    float f = IoMessage_locals_doubleArgAt_(m, locals, 0);
    MP3Encoder_setCompressionRatio_(DATA(self)->encoder, f);
    return IoMP3Encoder_checkError(self, locals, m);
}

/* ----------------------------------------------------------- */


IoObject *IoMP3Encoder_checkError(IoMP3Encoder *self, IoObject *locals, IoMessage *m)
{
    char *e = (char *)MP3Encoder_error(DATA(self)->encoder);
    
    if (e)
    { 
	IoState_error_description_(IOSTATE, m, "MP3Encoder", "error: %s", e);
	return IONIL(self);
    }
    
    return self;  
}

/* ----------------------------------------------------------- */


IoObject *IoMP3Encoder_encode(IoMP3Encoder *self, IoObject *locals, IoMessage *m)
{
    UArray *inBa = IoBuffer_rawUArray(IoMessage_locals_bufferArgAt_(m, locals, 0));
    /*UArray *outBa = IoBuffer_rawUArray(DATA(self)->outBuffer);*/
    
    int start = 0;
    int end = UArray_length(inBa);
    
    if (IoMessage_argCount(m) > 1) start = IoMessage_locals_intArgAt_(m, locals, 1);
    if (IoMessage_argCount(m) > 2) end = IoMessage_locals_intArgAt_(m, locals, 2);
    if (start > end)
    { IoState_error_description_(IOSTATE, m, "MP3Encoder", "range error: start > end"); }
    if (end > UArray_length(inBa))
    { IoState_error_description_(IOSTATE, m, "MP3Encoder", "range error: end > length of input buffer"); }
    
    MP3Encoder_encode(DATA(self)->encoder, UArray_bytes(inBa) + start, end);
    
    return IoMP3Encoder_checkError(self, locals, m);
}

IoObject *IoMP3Encoder_end(IoMP3Encoder *self, IoObject *locals, IoMessage *m)
{
    MP3Encoder_end(DATA(self)->encoder);
    return IoMP3Encoder_checkError(self, locals, m);
}

IoObject *IoMP3Encoder_outBuffer(IoMP3Encoder *self, IoObject *locals, IoMessage *m)
{ return DATA(self)->outBuffer; }

