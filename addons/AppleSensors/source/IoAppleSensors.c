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
#include "AppleLMU.h"
#include "AppleSMC.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"
#include "IoState.h"
#include "IoObject.h"
#include "unimotion.h"
#include <mach/mach.h>
//#include <Carbon/Carbon.h>
#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>

#define DATA(self) ((IoAppleSensorsData *)(IoObject_dataPointer(self)))

const char *protoId = "AppleSensors";

IoTag *IoAppleSensors_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
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
			{"getCPUTemperature", IoAppleSensors_getCPUTemperature},
			{"getGPUTemperature", IoAppleSensors_getGPUTemperature},
			{"getPalmTemperature", IoAppleSensors_getPalmTemperature},
			{"getBatteryTemperature", IoAppleSensors_getBatteryTemperature},
			{"getPowerTemperature", IoAppleSensors_getPowerTemperature},
			{"getPCHTemperature", IoAppleSensors_getPCHTemperature},
			{"getRAMTemperature", IoAppleSensors_getRAMTemperature},
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
	SMCClose();
	LMUClose();
	//free(IoObject_dataPointer(self));
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

IoObject *IoAppleSensors_getCPUTemperature(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	/*doc AppleSensors getCPUTemperature
		Returns a number for the CPU temperature sensor.
	 */
	SMCVal_t val = createEmptyValue();
	char cStr[16];
	kern_return_t result = SMCReadKey("TC0P", &val);
	if (result == kIOReturnSuccess) {
		result = representValue(val, &cStr[0]);
		if (result == TRUE) {
			float ftemp = atof(&cStr[0]);
			return IONUMBER(ftemp);
		}
	}
	return IONUMBER(-127);
}

IoObject *IoAppleSensors_getGPUTemperature(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	/*doc AppleSensors getGPUTemperature
		Returns a number for the GPU temperature sensor.
	 */
	SMCVal_t val = createEmptyValue();
	char cStr[16];
	kern_return_t result = SMCReadKey("TG0P", &val);
	if (result == kIOReturnSuccess) {
		result = representValue(val, &cStr[0]);
		if (result == TRUE) {
			float ftemp = atof(&cStr[0]);
			return IONUMBER(ftemp);
		}
	}
	return IONUMBER(-127);
}

IoObject *IoAppleSensors_getPalmTemperature(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	/*doc AppleSensors getPalmTemperature
		Returns a number for the Palm Rest Area temperature sensor.
	 */
	SMCVal_t val = createEmptyValue();
	char cStr[16];
	kern_return_t result = SMCReadKey("Ts0P", &val);
	if (result == kIOReturnSuccess) {
		result = representValue(val, &cStr[0]);
		if (result == TRUE) {
			float ftemp = atof(&cStr[0]);
			return IONUMBER(ftemp);
		}
	}
	return IONUMBER(-127);
}

IoObject *IoAppleSensors_getBatteryTemperature(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	/*doc AppleSensors getBatteryTemperature
		Returns a number for the Battery temperature sensor.
	 */
	SMCVal_t val = createEmptyValue();
	char cStr[16];
	kern_return_t result = SMCReadKey("TB0T", &val);
	if (result == kIOReturnSuccess) {
		result = representValue(val, &cStr[0]);
		if (result == TRUE) {
			float ftemp = atof(&cStr[0]);
			return IONUMBER(ftemp);
		}
	}
	return IONUMBER(-127);
}

IoObject *IoAppleSensors_getPowerTemperature(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	/*doc AppleSensors getPowerTemperature
		Returns a number for the Power board temperature sensor.
	 */
	SMCVal_t val = createEmptyValue();
	char cStr[16];
	kern_return_t result = SMCReadKey("Ts0S", &val);
	if (result == kIOReturnSuccess) {
		result = representValue(val, &cStr[0]);
		if (result == TRUE) {
			float ftemp = atof(&cStr[0]);
			return IONUMBER(ftemp);
		}
	}
	return IONUMBER(-127);
}

IoObject *IoAppleSensors_getPCHTemperature(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	/*doc AppleSensors getPCHTemperature
		Returns a number for Intel's Platform Controller Hub temperature sensor.
	 */
	SMCVal_t val = createEmptyValue();
	char cStr[16];
	kern_return_t result = SMCReadKey("TP0P", &val);
	if (result == kIOReturnSuccess) {
		result = representValue(val, &cStr[0]);
		if (result == TRUE) {
			float ftemp = atof(&cStr[0]);
			return IONUMBER(ftemp);
		}
	}
	return IONUMBER(-127);
}

IoObject *IoAppleSensors_getRAMTemperature(IoAppleSensors *self, IoObject *locals, IoMessage *m)
{
	/*doc AppleSensors getRAMTemperature
		Returns a number for the RAM temperature sensor.
	 */
	SMCVal_t val = createEmptyValue();
	char cStr[16];
	kern_return_t result = SMCReadKey("TM0P", &val);
	if (result == kIOReturnSuccess) {
		result = representValue(val, &cStr[0]);
		if (result == TRUE) {
			float ftemp = atof(&cStr[0]);
			return IONUMBER(ftemp);
		}
	}
	return IONUMBER(-127);
}

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
