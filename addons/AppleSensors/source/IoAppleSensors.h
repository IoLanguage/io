//metadoc AppleSensors copyright Steve Dekorte 2002

#ifndef IoAppleSensors_DEFINED
#define IoAppleSensors_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoList.h"
#include "AppleLMU.h"

#define ISAppleSensors(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoAppleSensors_rawClone)

typedef IoObject IoAppleSensors;

IoTag *IoAppleSensors_newTag(void *state);
IoAppleSensors *IoAppleSensors_proto(void *state);
IoAppleSensors *IoAppleSensors_rawClone(IoAppleSensors *self);
void IoAppleSensors_free(IoAppleSensors *self);

IoObject *IoAppleSensors_getLeftLightSensor(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoAppleSensors_getRightLightSensor(IoObject *self, IoObject *locals, IoMessage *m);

IoObject *IoAppleSensors_getDisplayBrightness(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoAppleSensors_setDisplayBrightness(IoObject *self, IoObject *locals, IoMessage *m);

IoObject *IoAppleSensors_getKeyboardBrightness(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoAppleSensors_setKeyboardBrightness(IoObject *self, IoObject *locals, IoMessage *m);

IoObject *IoAppleSensors_getCPUTemperature(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoAppleSensors_getGPUTemperature(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoAppleSensors_getPalmTemperature(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoAppleSensors_getBatteryTemperature(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoAppleSensors_getRAMTemperature(IoAppleSensors *self, IoObject *locals, IoMessage *m);
IoObject *IoAppleSensors_getPCHTemperature(IoAppleSensors *self, IoObject *locals, IoMessage *m);
IoObject *IoAppleSensors_getPowerTemperature(IoAppleSensors *self, IoObject *locals, IoMessage *m);

//IoObject *IoAppleSensors_smsDetect(IoObject *self, IoObject *locals, IoMessage *m);
IoObject *IoAppleSensors_smsVector(IoObject *self, IoObject *locals, IoMessage *m);

#endif
