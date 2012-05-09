#include "IoClutterStage.h"
//metadoc ClutterStage category UserInterface
//metadoc ClutterStage description For more detailed docs see <a href="http://clutter-project.org/docs/clutter/stable/ClutterStage.html">Clutter documentation</a>.

static const char *protoId = "ClutterStage";

/*** Initalisation functions ***/
IoTag *IoClutterStage_newTag(void *state) {
  IoTag *tag = IoTag_newWithName_(protoId);

  IoTag_state_(tag, state);
  IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoClutterActor_free);
  IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoClutterStage_rawClone);

  return tag;
}

IoClutterStage *IoClutterStage_proto(void *state) {
  IoObject *self = IoObject_new(state);
  IoObject_tag_(self, IoClutterStage_newTag(state));

  IoState_registerProtoWithFunc_(state, self, IoClutterStage_proto);

  {
    IoMethodTable methodTable[] = {
      {"default",   IoClutterStage_getDefault},

      {"color",     IoClutterStage_getColor},
      {"setColor",  IoClutterStage_setColor},
      {"title",     IoClutterStage_getTitle},
      {"setTitle",  IoClutterStage_setTitle},

      {NULL, NULL}
    };
    IoObject_addMethodTable_(self, methodTable);
  }

  return self;
}

IoClutterStage *IoClutterStage_rawClone(IoClutterStage *proto) {
  IoClutterStage *self = IoObject_rawClonePrimitive(proto);
  return self;
}

IoClutterStage *IoClutterStage_new(void *state) {
  IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
  return IOCLONE(proto);
}

IoClutterStage *IoClutterStage_newWithActor(void *state, ClutterActor *actor) {
  IoClutterStage *self = IoClutterStage_new(state);
  IoObject_setDataPointer_(self, actor);
  return self;
}

void IoClutterStage_free(IoClutterStage *self) {
  // Should we destroy the stage or?
}

IoObject *IoMessage_locals_clutterStageArgAt_(IoMessage *self, IoObject *locals, int n) {
  IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

  if(!ISCLUTTERSTAGE(v)) {
    IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "ClutterStage");
  }

  return v;
}

/*** Object functions ***/
//doc ClutterStage default
IO_METHOD(IoClutterStage, getDefault) {
  ClutterActor *default_stage = clutter_stage_get_default();
  IoClutterStage *stage = IoClutterStage_newWithActor(IOSTATE, default_stage);
  IoClutterActor *actor = IoClutterActor_newWithActor(IOSTATE, default_stage);

  IoObject_setSlot_to_(stage, IOSYMBOL("actor"), actor);

  return actor;
}

//doc ClutterStage color
IO_METHOD(IoClutterStage, getColor) {
  ClutterColor color = { 0, };
  clutter_stage_get_color(IOCSTAGE(self), &color);
  return IoClutterColor_newWithColor(IOSTATE, color);
}

//doc ClutterStage setColor(color)
IO_METHOD(IoClutterStage, setColor) {
  ClutterColor color = IOCCOLOR(IoMessage_locals_clutterColorArgAt_(m, locals, 0));
  clutter_stage_set_color(IOCSTAGE(self), &color);
  return self;
}

//doc ClutterStage title
IO_METHOD(IoClutterStage, getTitle) {
  return IOSYMBOL(clutter_stage_get_title(IOCSTAGE(self)));
}

//doc ClutterStage setTitle(title)
IO_METHOD(IoClutterStage, setTitle) {
  char *title = IoMessage_locals_cStringArgAt_(m, locals, 0);
  clutter_stage_set_title(IOCSTAGE(self), title);
  return self;
}
