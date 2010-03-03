#include "IoClutter.h"
//metadoc Clutter category UserInterface
/*metadoc Clutter description 
Clutter is a GObject based library for creating fast, visually rich, graphical user interfaces.

Clutter works by manipulating a scene-graph of 2D surfaces, or 'actors', inside a 3D space.

ClutterActor is the base class for such surfaces. All ClutterActors can be positioned, scaled and rotated in 3D space. In addition, other properties can be set, such as 2D clipping, children and opacity. Tranforms applied to a parent actor also apply to any children. Actors are also able to receive events.

Subclasses of ClutterActor include ClutterStage, ClutterTexture, ClutterLabel, ClutterRectangle, ClutterEntry and ClutterGroup. ClutterActors are added to a parent, transformed and then made visible.

ClutterStage is the top level ClutterActor - it's the representation of a window, or framebuffer. It is created automatically when Clutter is initialised. ClutterStage is a ClutterGroup, a class implementing the ClutterCointainer interface.

ClutterTimelines provide the basis for Clutter's animation utilities. Multiple timelines can be synchronised using ClutterScore, and ClutterBehaviour and ClutterEffect allow for the creation of animation effects such as transitions.

Clutter further contains a number of utilities, including; ClutterScript - for loading 'UI definition' files formatted in JSON, ClutterShader - a class for applying GPU shaders to actors, ClutterModel - a utility class for MVC list type implementations, and fixed point math utilities.

For detailed docs, see <a href=http://clutter-project.org/docs/clutter/stable/>clutter-project</a>

*/ 

/*** Initalisation functions ***/
IoTag *IoClutter_newTag(void *state) {
  IoTag *tag = IoTag_newWithName_("Clutter");

  IoTag_state_(tag, state);
  IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoClutter_free);
  IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoClutter_rawClone);

  return tag;
}

IoClutter *IoClutter_proto(void *state) {
  IoObject *self = IoObject_new(state);
  IoObject_tag_(self, IoClutter_newTag(state));

  IoState_registerProtoWithFunc_(state, self, IoClutter_proto);

  {
    IoMethodTable methodTable[] = {
      {"init",                      IoClutter_init},
      {"initThreads",               IoClutter_initThreads},
      {"threadEnter",               IoClutter_threadEnter},
      {"threadLeave",               IoClutter_threadLeave},
      {"main",                      IoClutter_main},
      {"quitMain",                  IoClutter_quitMain},
      {"mainLevel",                 IoClutter_mainLevel},

      {"isFpsShown",                IoClutter_isFpsShown},
      {"defaultFrameRate",          IoClutter_getDefaultFrameRate},
      {"setDefaultFrameRate",       IoClutter_setDefaultFrameRate},
      {"enableMotionEvents",        IoClutter_enableMotionEvents},
      {"disableMotionEvents",       IoClutter_disableMotionEvents},
      {"motionEventsEnabled",       IoClutter_motionEventsEnabled},

      {"getActorByGid",             IoClutter_getActorByGid},
      {"clearGlyphCache",           IoClutter_clearGlyphCache},
      {"fontHinting",               IoClutter_getFontHinting},
      {"setFontHinting",            IoClutter_setFontHinting},

      {"getKeyboardGrab",           IoClutter_getKeyboardGrab},
      {"getPointerGrab",            IoClutter_getPointerGrab},
      {"grabKeyboard",              IoClutter_grabKeyboard},
      {"grabPointer",               IoClutter_grabPointer},
      {"ungrabKeyboard",            IoClutter_ungrabKeyboard},
      {"ungrabPointer",             IoClutter_ungrabPointer},

      {"keySymbolToUnicode",        IoClutter_keySymbolToUnicode},
      {"currentEventTime",          IoClutter_currentEventTime},

      {NULL, NULL}
    };
    IoObject_addMethodTable_(self, methodTable);
  }

  return self;
}

IoClutter *IoClutter_rawClone(IoClutter *proto) {
  IoClutter *self = IoObject_rawClonePrimitive(proto);
  return self;
}

IoClutter *IoClutter_new(void *state) {
  IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoClutter_proto);
  return IOCLONE(proto);
}

void IoClutter_free(IoClutter *self) {
  // Should we do something here? quitMainLoop?
}

IO_METHOD(IoClutter, init) {
  // Clutter mask consts
  IoObject *cmod = IoObject_new(IOSTATE);
  IoObject_setSlot_to_(cmod, IOSYMBOL("SHIFT"),     IONUMBER(CLUTTER_SHIFT_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("LOCK"),      IONUMBER(CLUTTER_LOCK_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("CONTROL"),   IONUMBER(CLUTTER_CONTROL_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("MOD1"),      IONUMBER(CLUTTER_MOD1_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("MOD2"),      IONUMBER(CLUTTER_MOD2_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("MOD3"),      IONUMBER(CLUTTER_MOD3_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("MOD4"),      IONUMBER(CLUTTER_MOD4_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("MOD5"),      IONUMBER(CLUTTER_MOD5_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("BUTTON1"),   IONUMBER(CLUTTER_BUTTON1_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("BUTTON2"),   IONUMBER(CLUTTER_BUTTON2_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("BUTTON3"),   IONUMBER(CLUTTER_BUTTON3_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("BUTTON4"),   IONUMBER(CLUTTER_BUTTON4_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("BUTTON5"),   IONUMBER(CLUTTER_BUTTON5_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("SUPER"),     IONUMBER(CLUTTER_SUPER_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("HYPER"),     IONUMBER(CLUTTER_HYPER_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("META"),      IONUMBER(CLUTTER_META_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("RELEASE"),   IONUMBER(CLUTTER_RELEASE_MASK));
  IoObject_setSlot_to_(cmod, IOSYMBOL("MODIFIER"),  IONUMBER(CLUTTER_MODIFIER_MASK));

  // Clutter scroll direction consts
  IoObject *sdir = IoObject_new(IOSTATE);
  IoObject_setSlot_to_(sdir, IOSYMBOL("UP"),    IONUMBER(CLUTTER_SCROLL_UP));
  IoObject_setSlot_to_(sdir, IOSYMBOL("DOWN"),  IONUMBER(CLUTTER_SCROLL_DOWN));
  IoObject_setSlot_to_(sdir, IOSYMBOL("LEFT"),  IONUMBER(CLUTTER_SCROLL_LEFT));
  IoObject_setSlot_to_(sdir, IOSYMBOL("RIGHT"), IONUMBER(CLUTTER_SCROLL_RIGHT));


  // Clutter stage states
  IoObject *stag = IoObject_new(IOSTATE);
  IoObject_setSlot_to_(stag, IOSYMBOL("FULLSCREEN"),  IONUMBER(CLUTTER_STAGE_STATE_FULLSCREEN));
  IoObject_setSlot_to_(stag, IOSYMBOL("OFFSCREEN"),   IONUMBER(CLUTTER_STAGE_STATE_OFFSCREEN));
  IoObject_setSlot_to_(stag, IOSYMBOL("ACTIVATED"),   IONUMBER(CLUTTER_STAGE_STATE_ACTIVATED));

  // Clutter event types
  IoObject *evnt = IoObject_new(IOSTATE);
  IoObject_setSlot_to_(evnt, IOSYMBOL("NOTHING"),         IONUMBER(CLUTTER_NOTHING));
  IoObject_setSlot_to_(evnt, IOSYMBOL("KEY_PRESS"),       IONUMBER(CLUTTER_KEY_PRESS));
  IoObject_setSlot_to_(evnt, IOSYMBOL("KEY_RELEASE"),     IONUMBER(CLUTTER_KEY_RELEASE));
  IoObject_setSlot_to_(evnt, IOSYMBOL("MOTION"),          IONUMBER(CLUTTER_MOTION));
  IoObject_setSlot_to_(evnt, IOSYMBOL("ENTER"),           IONUMBER(CLUTTER_ENTER));
  IoObject_setSlot_to_(evnt, IOSYMBOL("LEAVE"),           IONUMBER(CLUTTER_LEAVE));
  IoObject_setSlot_to_(evnt, IOSYMBOL("BUTTON_PRESS"),    IONUMBER(CLUTTER_BUTTON_PRESS));
  IoObject_setSlot_to_(evnt, IOSYMBOL("BUTTON_RELEASE"),  IONUMBER(CLUTTER_BUTTON_RELEASE));
  IoObject_setSlot_to_(evnt, IOSYMBOL("SCROLL"),          IONUMBER(CLUTTER_SCROLL));
  IoObject_setSlot_to_(evnt, IOSYMBOL("STAGE_STATE"),     IONUMBER(CLUTTER_STAGE_STATE));
  IoObject_setSlot_to_(evnt, IOSYMBOL("DESTROY_NOTIFY"),  IONUMBER(CLUTTER_DESTROY_NOTIFY));
  IoObject_setSlot_to_(evnt, IOSYMBOL("CLIENT_MESSAGE"),  IONUMBER(CLUTTER_CLIENT_MESSAGE));
  IoObject_setSlot_to_(evnt, IOSYMBOL("DELETE"),          IONUMBER(CLUTTER_DELETE));

  // Clutter event flags
  IoObject *eflg = IoObject_new(IOSTATE);
  IoObject_setSlot_to_(eflg, IOSYMBOL("NONE"),      IONUMBER(CLUTTER_EVENT_NONE));
  IoObject_setSlot_to_(eflg, IOSYMBOL("SYNTHETIC"), IONUMBER(CLUTTER_EVENT_FLAG_SYNTHETIC));

  IoObject *devt = IoObject_new(IOSTATE);
  IoObject_setSlot_to_(devt, IOSYMBOL("POINTER"),   IONUMBER(CLUTTER_POINTER_DEVICE));
  IoObject_setSlot_to_(devt, IOSYMBOL("KEYBOARD"),  IONUMBER(CLUTTER_KEYBOARD_DEVICE));
  IoObject_setSlot_to_(devt, IOSYMBOL("EXTENSION"), IONUMBER(CLUTTER_EXTENSION_DEVICE));
  IoObject_setSlot_to_(devt, IOSYMBOL("COUNT"),     IONUMBER(CLUTTER_N_DEVICE_TYPES));

  IoObject *shde = IoObject_new(IOSTATE);
  IoObject_setSlot_to_(shde, IOSYMBOL("NO_ASM"),    IONUMBER(CLUTTER_SHADER_ERROR_NO_ASM));
  IoObject_setSlot_to_(shde, IOSYMBOL("NO_GLSL"),   IONUMBER(CLUTTER_SHADER_ERROR_NO_GLSL));
  IoObject_setSlot_to_(shde, IOSYMBOL("NO_COMPILE"),IONUMBER(CLUTTER_SHADER_ERROR_COMPILE));

  IoObject *unit = IoObject_new(IOSTATE);
  IoObject_setSlot_to_(unit, IOSYMBOL("PIXEL"), IONUMBER(CLUTTER_UNIT_PIXEL));
  IoObject_setSlot_to_(unit, IOSYMBOL("MM"),    IONUMBER(CLUTTER_UNIT_MM));
  IoObject_setSlot_to_(unit, IOSYMBOL("EM"),    IONUMBER(CLUTTER_UNIT_EM));
  IoObject_setSlot_to_(unit, IOSYMBOL("POINT"), IONUMBER(CLUTTER_UNIT_POINT));

  IoObject *axis = IoObject_new(IOSTATE);
  IoObject_setSlot_to_(axis, IOSYMBOL("X"),   IONUMBER(CLUTTER_X_AXIS));
  IoObject_setSlot_to_(axis, IOSYMBOL("Y"),   IONUMBER(CLUTTER_Y_AXIS));
  IoObject_setSlot_to_(axis, IOSYMBOL("Z"),   IONUMBER(CLUTTER_Z_AXIS));

  IoObject *grav = IoObject_new(IOSTATE);
  IoObject_setSlot_to_(grav, IOSYMBOL("NONE"),        IONUMBER(CLUTTER_GRAVITY_NONE));
  IoObject_setSlot_to_(grav, IOSYMBOL("NORTH"),       IONUMBER(CLUTTER_GRAVITY_NORTH));
  IoObject_setSlot_to_(grav, IOSYMBOL("NORTH_EAST"),  IONUMBER(CLUTTER_GRAVITY_NORTH_EAST));
  IoObject_setSlot_to_(grav, IOSYMBOL("EAST"),        IONUMBER(CLUTTER_GRAVITY_EAST));
  IoObject_setSlot_to_(grav, IOSYMBOL("SOUTH_EAST"),  IONUMBER(CLUTTER_GRAVITY_SOUTH_EAST));
  IoObject_setSlot_to_(grav, IOSYMBOL("SOUTH"),       IONUMBER(CLUTTER_GRAVITY_SOUTH));
  IoObject_setSlot_to_(grav, IOSYMBOL("SOUTH_WEST"),  IONUMBER(CLUTTER_GRAVITY_SOUTH_WEST));
  IoObject_setSlot_to_(grav, IOSYMBOL("WEST"),        IONUMBER(CLUTTER_GRAVITY_WEST));
  IoObject_setSlot_to_(grav, IOSYMBOL("NORTH_WEST"),  IONUMBER(CLUTTER_GRAVITY_NORTH_WEST));
  IoObject_setSlot_to_(grav, IOSYMBOL("CENTER"),      IONUMBER(CLUTTER_GRAVITY_CENTER));

  IoObject_setSlot_to_(self, IOSYMBOL("MASK"),        cmod);
  IoObject_setSlot_to_(self, IOSYMBOL("SCROLL"),      sdir);
  IoObject_setSlot_to_(self, IOSYMBOL("STAGE_STATE"), stag);
  IoObject_setSlot_to_(self, IOSYMBOL("EVENT"),       evnt);
  IoObject_setSlot_to_(self, IOSYMBOL("EVENT_FLAG"),  eflg);
  IoObject_setSlot_to_(self, IOSYMBOL("DEVICE_TYPE"), devt);
  IoObject_setSlot_to_(self, IOSYMBOL("SHADER_ERROR"),shde);
  IoObject_setSlot_to_(self, IOSYMBOL("UNIT"),        unit);
  IoObject_setSlot_to_(self, IOSYMBOL("AXIS"),        axis);
  IoObject_setSlot_to_(self, IOSYMBOL("GRAVITY"),     grav);

  IoObject_setSlot_to_(self, IOSYMBOL("version"), IOSYMBOL(CLUTTER_VERSION_S));
  IoObject_setSlot_to_(self, IOSYMBOL("flavour"), IOSYMBOL(CLUTTER_FLAVOUR));
  IoObject_setSlot_to_(self, IOSYMBOL("cogl"),    IOSYMBOL(CLUTTER_COGL));

  return IOBOOL(self, clutter_init(NULL, NULL));
}

IO_METHOD(IoClutter, initThreads) {
  g_thread_init(NULL);
  clutter_threads_init();
  return self;
}

IO_METHOD(IoClutter, threadEnter) {
  clutter_threads_enter();
  return self;
}

IO_METHOD(IoClutter, threadLeave) {
  clutter_threads_leave();
  return self;
}

// TODO: clutter_threads_add_idle(), _add_idle_full(), _add_timeout, _add_timeout_full

IO_METHOD(IoClutter, main) {
  clutter_main();
  return IONIL(self);
}

IO_METHOD(IoClutter, quitMain) {
  clutter_main_quit();
  return IONIL(self);
}

IO_METHOD(IoClutter, mainLevel) {
  return IONUMBER(clutter_main_level());
}

IO_METHOD(IoClutter, isFpsShown) {
  return IOBOOL(self, clutter_get_show_fps());
}

IO_METHOD(IoClutter, getDefaultFrameRate) {
  return IONUMBER(clutter_get_default_frame_rate());
}

IO_METHOD(IoClutter, setDefaultFrameRate) {
  clutter_set_default_frame_rate((guint)(IoMessage_locals_floatArgAt_(m, locals, 0)));
  return self;
}

IO_METHOD(IoClutter, enableMotionEvents) {
  clutter_set_motion_events_enabled(1);
  return self;
}

IO_METHOD(IoClutter, disableMotionEvents) {
  clutter_set_motion_events_enabled(0);
  return self;
}

IO_METHOD(IoClutter, motionEventsEnabled) {
  return IOBOOL(self, clutter_get_motion_events_enabled());
}

IO_METHOD(IoClutter, clearGlyphCache) {
  clutter_clear_glyph_cache();
  return self;
}

//doc Clutter fontHinting Returns <code>true</code> if <code>CLUTTER_FONT_HINTING</code>
IO_METHOD(IoClutter, getFontHinting) {
  return IOBOOL(self, clutter_get_font_flags());
}

/*doc Clutter setFontHinting(useFontHinting)
If <code>useFontHinting</code> is <code>false</code>, <code>CLUTTER_FONT_MIPMAPPING</code> flag will be used.
*/
IO_METHOD(IoClutter, setFontHinting) {
  clutter_set_font_flags(font_hinting_flags(IoMessage_locals_boolArgAt_(m, locals, 0)));
  return self;
}

IO_METHOD(IoClutter, getActorByGid) {
  guint32 gid = (guint32)(IoMessage_locals_doubleArgAt_(m, locals, 0));
  ClutterActor *actor = clutter_get_actor_by_gid(gid);
  return (actor == NULL)? IONIL(self) : IoClutterActor_newWithActor(IOSTATE, actor);
}

IO_METHOD(IoClutter, getKeyboardGrab) {
  return IoClutterActor_newWithActor(IOSTATE, clutter_get_keyboard_grab());
}

IO_METHOD(IoClutter, getPointerGrab) {
  return IoClutterActor_newWithActor(IOSTATE, clutter_get_pointer_grab());
}

//doc Clutter grabKeyboard(actor)
IO_METHOD(IoClutter, grabKeyboard) {
  ClutterActor *actor = IOCACTOR(IoMessage_locals_clutterActorArgAt_(m, locals, 0));
  clutter_grab_keyboard(actor);
  return self;
}

//doc Clutter grabPointer(actor[, deviceId])
IO_METHOD(IoClutter, grabPointer) {
  ClutterActor *actor = IOCACTOR(IoMessage_locals_clutterActorArgAt_(m, locals, 0));
  IoObject *device_n = IoMessage_locals_valueArgAt_(m, locals, 1);

  if(ISNUMBER(device_n))
    clutter_grab_pointer_for_device(actor, CNUMBER(device_n));
  else
    clutter_grab_pointer(actor);

  return self;
}

//IoObject *IoClutter_ungrabKeyboard(IoClutter *self, IoObject *l, IoMessage *m) {
IO_METHOD(IoClutter, ungrabKeyboard) {
  clutter_ungrab_keyboard();
  return self;
}

IO_METHOD(IoClutter, ungrabPointer) {
  IoObject *device_n = IoMessage_locals_valueArgAt_(m, locals, 0);

  if(ISNUMBER(device_n))
    clutter_ungrab_pointer_for_device(CNUMBER(device_n));
  else
    clutter_ungrab_pointer();

  return self;
}

IO_METHOD(IoClutter, keySymbolToUnicode) {
  return IONUMBER(clutter_keysym_to_unicode(IoMessage_locals_doubleArgAt_(m, locals, 0)));
}

IO_METHOD(IoClutter, currentEventTime) {
  return IoDate_newWithTime_(IOSTATE, clutter_get_current_event_time());
}