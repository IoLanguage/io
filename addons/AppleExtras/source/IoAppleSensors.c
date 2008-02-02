//metadoc AppleSensors copyright Steve Dekorte, 2004
//metadoc AppleSensors license BSD revised
//metadoc AppleSensors category Encryption
/*metadoc AppleSensors description
The AppleSensors object can be used to do encryption and decryption using the AppleSensors algorithm.
Example use;
<pre>	
bf = AppleSensors clone
bf setKey("secret")
bf beginProcessing
bf inputBuffer appendSeq("this is a message")
bf process
bf endProcess
bf outputBuffer // this contains the encrypted data
</pre>	
*/

#include "IoAppleSensors.h"
#include "AppleSensors.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"

#define DATA(self) ((IoAppleSensorsData *)(IoObject_dataPointer(self)))

IoTag *IoAppleSensors_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("AppleSensors");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoAppleSensors_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoAppleSensors_rawClone);
	return tag;
}

IoAppleSensors *IoAppleSensors_proto(void *state)
{
	IoAppleSensors *self = IoObject_new(state);
	IoObject_tag_(self, IoAppleSensors_newTag(state));

	//IoObject_setDataPointer_(self, calloc(1, sizeof(IoAppleSensorsData)));

	IoState_registerProtoWithFunc_(state, self, IoAppleSensors_proto);

	{
		IoMethodTable methodTable[] = {
		{"getLeftLightSensor", IoAppleSensors_getLeftLightSensor},
		{"getRightLightSensor", IoAppleSensors_getRightLightSensor},

		{"getDisplayBrightness", IoAppleSensors_getDisplayBrightness},
		{"setDisplayBrightness", IoAppleSensors_setDisplayBrightness},

		{"getKeyboardBrightness", IoAppleSensors_getKeyboardBrightness},
		{"setKeyboardBrightness", IoAppleSensors_setKeyboardBrightness},

		//{"smsDetect", IoAppleSensors_smsDetect},
		{"smsVector", IoAppleSensors_smsVector},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoAppleSensors *IoAppleSensors_rawClone(IoAppleSensors *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	//IoObject_setDataPointer_(self, calloc(1, sizeof(IoAppleSensorsData)));
	return self;
}

IoAppleSensors *IoAppleSensors_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoAppleSensors_proto);
	return IOCLONE(proto);
}

void IoAppleSensors_free(IoAppleSensors *self)
{
	//free(DATA(self));
}

// -----------------------------------------------------------

IoObject *IoAppleSensors_getLeftLightSensor(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	float left = -1, right = -1;
	getLightSensors(&left, &right);
	return IONUMBER(left);
}

IoObject *IoAppleSensors_getRightLightSensor(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	float left = -1, right = -1;
	getLightSensors(&left, &right);
	return IONUMBER(right);
}

IoObject *IoAppleSensors_getDisplayBrightness(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(getDisplayBrightness());
}

IoObject *IoAppleSensors_setDisplayBrightness(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	float v = IoMessage_locals_floatArgAt_(m, locals, 0);
	setDisplayBrightness(v);
	return self;
}

IoObject *IoAppleSensors_getKeyboardBrightness(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(getKeyboardBrightness());
}

IoObject *IoAppleSensors_setKeyboardBrightness(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	float v = IoMessage_locals_floatArgAt_(m, locals, 0);
	setKeyboardBrightness(v);
	return self;
}

#include "unimotion.h"

static int smsType = -1;

IoObject *IoAppleSensors_smsVector(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	IoSeq *v = IoMessage_locals_seqArgAt_(m, locals, 0);
	float *f = IoSeq_makeFloatArrayOfSize_(v);
	int err;

	if (smsType == -1)
	{
		smsType = detect_sms();
	}

	err = read_sms(smsType, f+0, f+1, f+2);

	return err ? IOTRUE(self) : IOFALSE(self);
}
