#include "IoClutterInputDevice.h"
//metadoc ClutterInputDevice category UserInterface
//metadoc ClutterInputDevice description For more detailed docs see <a href="http://clutter-project.org/docs/clutter/stable/ClutterInputDevice.html">Clutter documentation</a>.

/*** Initalisation functions ***/
IoTag *IoClutterInputDevice_newTag(void *state) {
  IoTag *tag = IoTag_newWithName_("ClutterInputDevice");

  IoTag_state_(tag, state);
  IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoClutterInputDevice_free);
  IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoClutterInputDevice_rawClone);

  return tag;
}

IoClutterInputDevice *IoClutterInputDevice_proto(void *state) {
  IoObject *self = IoObject_new(state);
  IoObject_tag_(self, IoClutterInputDevice_newTag(state));

  IoState_registerProtoWithFunc_(state, self, IoClutterInputDevice_proto);

  {
    IoMethodTable methodTable[] = {
      {"deviceId",    IoClutterInputDevice_getId},
      {"deviceType",  IoClutterInputDevice_getType},
      {NULL, NULL}
    };
    IoObject_addMethodTable_(self, methodTable);
  }

  return self;
}

IoClutterInputDevice *IoClutterInputDevice_rawClone(IoClutterInputDevice *proto) {
  IoClutterInputDevice *self = IoObject_rawClonePrimitive(proto);
  return self;
}

IoClutterInputDevice *IoClutterInputDevice_new(void *state) {
  IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoClutterInputDevice_proto);
  return IOCLONE(proto);
}

IoClutterInputDevice *IoClutterInputDevice_newWithDevice(void *state, ClutterInputDevice *device) {
  IoClutterInputDevice *self = IoClutterInputDevice_new(state);
  IoObject_setDataPointer_(self, device);
  return self;
}

void IoClutterInputDevice_free(IoClutterInputDevice *self) {
  //io_free(IoObject_dataPointer(self));
}

IoObject *IoMessage_locals_clutterDeviceArgAt_(IoMessage *self, IoObject *locals, int n) {
  IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

  if(!ISCLUTTERINPUTDEVICE(v)) {
    IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "ClutterInputDevice");
  }

  return v;
}

/*** Object functions ***/
//doc ClutterInputDevice deviceId
IO_METHOD(IoClutterInputDevice, getId) {
  return IONUMBER(clutter_input_device_get_device_id(IOCIDEVICE(self)));
}

//doc ClutterInputDevice deviceType
IO_METHOD(IoClutterInputDevice, getType) {
  return IONUMBER(clutter_input_device_get_device_type(IOCIDEVICE(self)));
}