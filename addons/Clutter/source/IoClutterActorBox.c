#include "IoClutterActorBox.h"

/*** Initalisation functions ***/
IoTag *IoClutterActorBox_newTag(void *state) {
  IoTag *tag = IoTag_newWithName_("ClutterActorBox");

  IoTag_state_(tag, state);
  IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoClutterActorBox_free);
  IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoClutterActorBox_rawClone);

  return tag;
}

IoClutterActorBox *IoClutterActorBox_proto(void *state) {
  IoObject *self = IoObject_new(state);
  IoObject_tag_(self, IoClutterActorBox_newTag(state));

  IoState_registerProtoWithFunc_(state, self, IoClutterActorBox_proto);

  {
    IoMethodTable methodTable[] = {
      {"==",      IoClutterActorBox_equals},
      {"with",    IoClutterActorBox_with},

      {"origin",  IoClutterActorBox_getOrigin},
      {"size",    IoClutterActorBox_getSize},
      {"contains", IoClutterActorBox_contains},

      {NULL, NULL}
    };
    IoObject_addMethodTable_(self, methodTable);
  }

  return self;
}

IoClutterActorBox *IoClutterActorBox_rawClone(IoClutterActorBox *proto) {
  IoClutterActorBox *self = IoObject_rawClonePrimitive(proto);

  return self;
}

IoClutterActorBox *IoClutterActorBox_new(void *state) {
  IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoClutterActorBox_proto);

  return IOCLONE(proto);
}

IoClutterActorBox *IoClutterActorBox_newWithActorBox(void *state, ClutterActorBox *actorBox) {
  IoClutterActorBox *self = IoClutterActorBox_new(state);
  IoObject_setDataPointer_(self, actorBox);
  return self;
}

void IoClutterActorBox_free(IoClutterActorBox *self) {
  if(IOCABOX(self) != NULL)
    clutter_actor_box_free(IOCABOX(self));
}

IoObject *IoMessage_locals_clutterActorBoxArgAt_(IoMessage *self, IoObject *locals, int n) {
  IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

  if(!ISCLUTTERACTORBOX(v)) {
    IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "ClutterActorBox");
  }

  return v;
}

/*** Object functions ***/
//doc ClutterActorBox with(x1, y1, x2, y2)
IO_METHOD(IoClutterActorBox, equals) {
  ClutterActorBox *other = IOCABOX(IoMessage_locals_clutterActorBoxArgAt_(m, locals, 0));
  return IOBOOL(self, clutter_actor_box_equal(IOCABOX(self), other));
}

IO_METHOD(IoClutterActorBox, with) {
  float x1 = IoMessage_locals_floatArgAt_(m, locals, 0),
        y1 = IoMessage_locals_floatArgAt_(m, locals, 1),
        x2 = IoMessage_locals_floatArgAt_(m, locals, 2),
        y2 = IoMessage_locals_floatArgAt_(m, locals, 3);

  ClutterActorBox *actorBox = clutter_actor_box_new(x1, y1, x2, y2);
  IoClutterActorBox *clone = IoClutterActorBox_newWithActorBox(IOSTATE, actorBox);

  IoObject_setSlot_to_(clone,
    IOSYMBOL("x1"), IoMessage_locals_numberArgAt_(m, locals, 0)
  );

  IoObject_setSlot_to_(clone,
    IOSYMBOL("y1"), IoMessage_locals_numberArgAt_(m, locals, 1)
  );

  IoObject_setSlot_to_(clone,
    IOSYMBOL("x2"), IoMessage_locals_numberArgAt_(m, locals, 2)
  );

  IoObject_setSlot_to_(clone,
    IOSYMBOL("y2"), IoMessage_locals_numberArgAt_(m, locals, 3)
  );

  IoObject_setSlot_to_(clone,
    IOSYMBOL("width"), IONUMBER(clutter_actor_box_get_width(actorBox))
  );

  IoObject_setSlot_to_(clone,
    IOSYMBOL("height"), IONUMBER(clutter_actor_box_get_height(actorBox))
  );

  IoObject_setSlot_to_(clone,
    IOSYMBOL("area"), IONUMBER(clutter_actor_box_get_area(actorBox))
  );

  return clone;
}

IO_METHOD(IoClutterActorBox, getOrigin) {
  float x = 0,
        y = 0;
  IoObject *coords = IoObject_new(IOSTATE);

  clutter_actor_box_get_origin(IOCABOX(self), &x, &y);
  IoObject_setSlot_to_(coords, IOSYMBOL("x"), IONUMBER(x));
  IoObject_setSlot_to_(coords, IOSYMBOL("y"), IONUMBER(y));

  return coords;
}

IO_METHOD(IoClutterActorBox, getSize) {
  float width  = 0,
        height = 0;
  IoObject *size = IoObject_new(IOSTATE);

  clutter_actor_box_get_origin(IOCABOX(self), &width, &height);
  IoObject_setSlot_to_(size, IOSYMBOL("width"),  IONUMBER(width));
  IoObject_setSlot_to_(size, IOSYMBOL("height"), IONUMBER(height));

  return size;
}

//doc ClutterActorBox contains(x1, y1)
IO_METHOD(IoClutterActorBox, contains) {
  float x = IoMessage_locals_floatArgAt_(m, locals, 0),
        y = IoMessage_locals_floatArgAt_(m, locals, 1);
  int contains = clutter_actor_box_contains(IOCABOX(self), x, y);

  return IOBOOL(self, contains);
}