/*
//metadoc AppleSensors copyright Steve Dekorte 2002
*/
*/

#ifndef IoAppleSensors_DEFINED
#define IoAppleSensors_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoList.h"
#include "AppleSensors.h"

#define ISAppleSensors(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoAppleSensors_rawClone)

typedef IoObject IoAppleSensors;

IoTag *IoAppleSensors_newTag(void *state);
IoAppleSensors *IoAppleSensors_proto(void *state);
IoAppleSensors *IoAppleSensors_rawClone(IoAppleSensors *self);
void IoAppleSensors_free(IoAppleSensors *self);

IoObject *IoAppleSensors_getLeftLightSensor(IoAppleSensors *self, IoObject *locals, IoMessage *m);
IoObject *IoAppleSensors_getRightLightSensor(IoAppleSensors *self, IoObject *locals, IoMessage *m);

IoObject *IoAppleSensors_getDisplayBrightness(IoAppleSensors *self, IoObject *locals, IoMessage *m);
IoObject *IoAppleSensors_setDisplayBrightness(IoAppleSensors *self, IoObject *locals, IoMessage *m);

IoObject *IoAppleSensors_getKeyboardBrightness(IoAppleSensors *self, IoObject *locals, IoMessage *m);
IoObject *IoAppleSensors_setKeyboardBrightness(IoAppleSensors *self, IoObject *locals, IoMessage *m);

//IoObject *IoAppleSensors_smsDetect(IoAppleSensors *self, IoObject *locals, IoMessage *m);
IoObject *IoAppleSensors_smsVector(IoAppleSensors *self, IoObject *locals, IoMessage *m);

#endif
