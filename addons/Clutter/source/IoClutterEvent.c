#include "IoClutterEvent.h"

/*** Initalisation functions ***/
IoTag *IoClutterEvent_newTag(void *state) {
  IoTag *tag = IoTag_newWithName_("ClutterEvent");

  IoTag_state_(tag, state);
  IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoClutterEvent_free);
  IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoClutterEvent_rawClone);

  return tag;
}

IoClutterEvent *IoClutterEvent_proto(void *state) {
  IoObject *self = IoObject_new(state);
  IoObject_tag_(self, IoClutterEvent_newTag(state));

  IoState_registerProtoWithFunc_(state, self, IoClutterEvent_proto);

  {
    IoMethodTable methodTable[] = {
      {"eventType",       IoClutterEvent_eventType},
      {"coords",          IoClutterEvent_getCoords},
      {"state",           IoClutterEvent_getState},
      {"time",            IoClutterEvent_getTime},
      {"source",          IoClutterEvent_getSource},
      //{"stage",     IoClutterEvent_getStage},
      {"flags",           IoClutterEvent_getFlags},
      {"peek",            IoClutterEvent_peek},
      {"put",             IoClutterEvent_put},
      {"putBack",         IoClutterEvent_putBack},
      {"hasPending",      IoClutterEvent_hasPending},
      {"button",          IoClutterEvent_getButton},
      {"clickCount",      IoClutterEvent_getClickCount},
      {"keySymbol",       IoClutterEvent_getKeySymbol},
      {"keyCode",         IoClutterEvent_getKeyCode},
      {"keyUnicode",      IoClutterEvent_getKeyUnicode},
      {"relatedActor",    IoClutterEvent_getRelatedActor},
      {"scrollDirecton",  IoClutterEvent_getScrollDirection},
      {"device",          IoClutterEvent_getDevice},

      {NULL, NULL}
    };
    IoObject_addMethodTable_(self, methodTable);
  }

  return self;
}

IoClutterEvent *IoClutterEvent_rawClone(IoClutterEvent *proto) {
  IoClutterEvent *self = IoObject_rawClonePrimitive(proto);
  return self;
}

IoClutterEvent *IoClutterEvent_new(void *state) {
  IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoClutterEvent_proto);
  return IOCLONE(proto);
}

IoClutterEvent *IoClutterEvent_newWithType(void *state, ClutterEventType type) {
  IoClutterEvent *self = IoClutterEvent_new(state);
  ClutterEvent *event  = clutter_event_new(type);
  IoObject_setDataPointer_(self, event);

  return self;
}

IoClutterEvent *IoClutterEvent_newWithEvent(void *state, ClutterEvent *event) {
  IoClutterEvent *self = IoClutterEvent_new(state);
  IoObject_setDataPointer_(self, event);
  return self;
}

void IoClutterEvent_free(IoClutterEvent *self) {
  clutter_event_free(IOCEVENT(self));
}

IoObject *IoMessage_locals_clutterEventArgAt_(IoMessage *self, IoObject *locals, int n) {
  IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

  if(!ISCLUTTEREVENT(v)) {
    IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "ClutterColor");
  }

  return v;
}

IO_METHOD(IoClutterEvent, eventType) {
  return IONUMBER(clutter_event_type(IOCEVENT(self)));
}

IO_METHOD(IoClutterEvent, getCoords) {
  float x = 0,
        y = 0;
  IoObject *coords = IoObject_new(IOSTATE);

  clutter_event_get_coords(IOCEVENT(self), &x, &y);
  IoObject_setSlot_to_(coords, IOSYMBOL("x"), IONUMBER(x));
  IoObject_setSlot_to_(coords, IOSYMBOL("y"), IONUMBER(y));

  return coords;
}

IO_METHOD(IoClutterEvent, getState) {
  return IONUMBER(clutter_event_get_state(IOCEVENT(self)));
}

IO_METHOD(IoClutterEvent, getTime) {
  return IoDate_newWithTime_(IOSTATE, clutter_event_get_time(IOCEVENT(self)));
}

IO_METHOD(IoClutterEvent, getSource) {
  return IoClutterActor_newWithActor(IOSTATE, clutter_event_get_source(IOCEVENT(self)));
}

//IO_METHOD(IoClutterEvent, getStage) {
//  return IoDate_newWithTime_(IOSTATE, clutter_event_get_time(IOCEVENT(self)));
//}

IO_METHOD(IoClutterEvent, getFlags) {
  return IONUMBER(clutter_event_get_flags(IOCEVENT(self)));
}

IO_METHOD(IoClutterEvent, peek) {
  ClutterEvent *event = clutter_event_peek();
  return (event == NULL) ? IONIL(self) : IoClutterEvent_newWithEvent(IOSTATE, event);
}

IO_METHOD(IoClutterEvent, put) {
  clutter_event_put(IOCEVENT(IoMessage_locals_clutterEventArgAt_(m, locals, 0)));
  return self;
}

// ClutterEvent putBack() Puts <code>self</code> back to event queue.
IO_METHOD(IoClutterEvent, putBack) {
  clutter_event_put(IOCEVENT(self));
  return self;
}

IO_METHOD(IoClutterEvent, hasPending) {
  return IOBOOL(self, clutter_events_pending());
}

IO_METHOD(IoClutterEvent, getButton) {
  return IONUMBER(clutter_event_get_button(IOCEVENT(self)));
}

IO_METHOD(IoClutterEvent, getClickCount) {
  return IONUMBER(clutter_event_get_click_count(IOCEVENT(self)));
}

IO_METHOD(IoClutterEvent, getKeySymbol) {
  return IONUMBER(clutter_event_get_key_symbol(IOCEVENT(self)));
}

IO_METHOD(IoClutterEvent, getKeyCode) {
  return IONUMBER(clutter_event_get_key_code(IOCEVENT(self)));
}

IO_METHOD(IoClutterEvent, getKeyUnicode) {
  return IONUMBER(clutter_event_get_key_unicode(IOCEVENT(self)));
}

IO_METHOD(IoClutterEvent, getRelatedActor) {
  ClutterActor *actor = clutter_event_get_related(IOCEVENT(self));
  return (actor == NULL) ? IONIL(self) : IoClutterActor_newWithActor(IOSTATE, actor);
}

IO_METHOD(IoClutterEvent, getScrollDirection) {
  return IONUMBER(clutter_event_get_scroll_direction(IOCEVENT(self)));
}

IO_METHOD(IoClutterEvent, getDevice) {
  return IoClutterInputDevice_newWithDevice(
    IOSTATE,
    clutter_event_get_device(IOCEVENT(self))
  );
}
