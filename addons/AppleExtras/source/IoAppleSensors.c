//metadoc AppleSensors copyright Steve Dekorte, 2004
//metadoc AppleSensors license BSD revised
//metadoc AppleSensors category Apple
/*metadoc AppleSensors description
A singleton which on Apple computers can:
<ul>
<li> get and set display and keyboard brightness
<li> read left and right ambient light sensors (laptops only)
<li> read accelerometer sensor (laptops only)
</ul>
Example use;
<pre>	
sensors = AppleSensors clone
value := sensors getRightLightSensor
</pre>	
*/

#include "IoAppleSensors.h"
#include "AppleSensors.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"

#define DATA(self) ((IoAppleSensorsData *)(IoObject_dataPointer(self)))

const char *protoId = "AppleSensors";

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

	IoState_registerProtoWithId_(state, self, protoId);

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
	IoObject *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoAppleSensors_free(IoAppleSensors *self)
{
	//free(DATA(self));
}

// -----------------------------------------------------------

IoObject *IoAppleSensors_getLeftLightSensor(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	/*doc AppleSensors getLeftLightSensor
		Returns a number for the left ambient light sensor.
	*/
	float left = -1, right = -1;
	getLightSensors(&left, &right);
	return IONUMBER(left);
}

IoObject *IoAppleSensors_getRightLightSensor(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	/*doc AppleSensors getRightLightSensor
		Returns a number for the right ambient light sensor.
	*/
	float left = -1, right = -1;
	getLightSensors(&left, &right);
	return IONUMBER(right);
}

IoObject *IoAppleSensors_getDisplayBrightness(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	/*doc AppleSensors getDisplayBrightness
		Returns a number for the display brightness.
	*/
	return IONUMBER(getDisplayBrightness());
}

IoObject *IoAppleSensors_setDisplayBrightness(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	/*doc AppleSensors setDisplayBrightness(aNumber)
		Sets the display brightness. Returns self.
	*/
	float v = IoMessage_locals_floatArgAt_(m, locals, 0);
	setDisplayBrightness(v);
	return self;
}

IoObject *IoAppleSensors_getKeyboardBrightness(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	/*doc AppleSensors getKeyboardBrightness
		Returns a number for the keyboard brightness.
	*/
	return IONUMBER(getKeyboardBrightness());
}

IoObject *IoAppleSensors_setKeyboardBrightness(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	/*doc AppleSensors setKeyboardBrightness(aNumber)
		Sets the keyboard brightness. Returns self.
	*/
	float v = IoMessage_locals_floatArgAt_(m, locals, 0);
	setKeyboardBrightness(v);
	return self;
}

#include "unimotion.h"

static int smsType = -1;

IoObject *IoAppleSensors_smsVector(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	/*doc AppleSensors smsVector(aVector)
		Sets aVector to the current x, y and z accelerometer values. 
		Returns true on success and false on failure.
	*/
	IoSeq *vector = IoMessage_locals_seqArgAt_(m, locals, 0);
	float *f = IoSeq_floatPointerOfLength_(vector, 3);
	int err;

	if (smsType == -1)
	{
		smsType = detect_sms();
	}

	int v[3] = {0, 0, 0};

	err = read_sms(smsType, &v[0], &v[1], &v[3]);
	
	f[0] = v[0];
	f[1] = v[1];
	f[2] = v[2];

	return err ? IOTRUE(self) : IOFALSE(self);
}
