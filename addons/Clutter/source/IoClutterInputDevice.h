#ifndef IoClutterInputDevice_DEFINED
#define IoClutterInputDevice_DEFINED 1

#define ISCLUTTERINPUTDEVICE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoClutterInputDevice_rawClone)
#define IOCIDEVICE(self) ((ClutterInputDevice*)IoObject_dataPointer(self))

#include "common.h"

typedef IoObject IoClutterInputDevice;

// Initialisation functions
IoTag                 *IoClutterInputDevice_newTag        (void *state);
IoClutterInputDevice  *IoClutterInputDevice_proto         (void *state);
IoClutterInputDevice  *IoClutterInputDevice_rawClone      (IoClutterInputDevice *proto);
IoClutterInputDevice  *IoClutterInputDevice_new           (void *state);
IoClutterInputDevice  *IoClutterInputDevice_newWithDevice (void *state, ClutterInputDevice *device);
void                  IoClutterInputDevice_free           (IoClutterInputDevice *self);

IoObject *IoMessage_locals_clutterDeviceArgAt_(IoMessage *self, IoObject *locals, int n);

IO_METHOD(IoClutterInputDevice, getId);
IO_METHOD(IoClutterInputDevice, getType);

#endif