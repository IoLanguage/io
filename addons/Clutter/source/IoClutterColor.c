#include "IoClutterColor.h"
#define DATA(self) ((IoClutterColorData*)IoObject_dataPointer(self))

//metadoc ClutterColor category UserInterface
/*metadoc ClutterColor description
<p>
For more detailed docs see <a href="http://clutter-project.org/docs/clutter/stable/clutter-Colors.html">Clutter documentation</a>.</p>
<pre><code>red := ClutterColor fromString("red")
blue := "#00f" asClutterColor
(red + blue) asString println</code></pre>*/

/*** Initalisation functions ***/
IoTag *IoClutterColor_newTag(void *state) {
  IoTag *tag = IoTag_newWithName_("ClutterColor");

  IoTag_state_(tag, state);
  IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoClutterColor_free);
  IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoClutterColor_rawClone);

  return tag;
}

IoClutterColor *IoClutterColor_proto(void *state) {
  IoObject *self = IoObject_new(state);
  IoObject_tag_(self, IoClutterColor_newTag(state));

  IoObject_setDataPointer_(self, calloc(1, sizeof(IoClutterColorData)));
  IoState_registerProtoWithFunc_(state, self, IoClutterColor_proto);

  {
    IoMethodTable methodTable[] = {
      {"==",              IoClutterColor_equals},
      {"!=",              IoClutterColor_notEquals},

      {"with",            IoClutterColor_fromString},
      {"asString",        IoClutterColor_asString},
      {"withHLS",         IoClutterColor_fromHLS},
      {"toHLS",           IoClutterColor_toHLS},
      {"withPixel",       IoClutterColor_fromPixel},
      {"toPixel",         IoClutterColor_toPixel},

      {"+",               IoClutterColor_add},
      {"-",               IoClutterColor_subtract},
      {"addInPlace",      IoClutterColor_addInPlace},
      {"subtractInPlace", IoClutterColor_subtractInPlace},

      {"lighten",         IoClutterColor_lighten},
      {"darken",          IoClutterColor_darken},
      {"shade",           IoClutterColor_shade},

      {NULL, NULL}
    };
    IoObject_addMethodTable_(self, methodTable);
  }

  return self;
}

IoClutterColor *IoClutterColor_rawClone(IoClutterColor *proto) {
  IoClutterColor *self = IoObject_rawClonePrimitive(proto);

  IoObject_setDataPointer_(self, calloc(1, sizeof(IoClutterColorData)));

  return self;
}

IoClutterColor *IoClutterColor_new(void *state) {
  IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoClutterColor_proto);

  return IOCLONE(proto);
}

IoClutterColor *IoClutterColor_newWithColor(void *state, ClutterColor color) {
  IoClutterColor *io_color = IoClutterColor_new(state);
  DATA(io_color)->color = color;
  return io_color;
}

void IoClutterColor_free(IoClutterColor *self) {
  clutter_color_free(&(IOCCOLOR(self)));
  io_free(IoObject_dataPointer(self));
}

IoObject *IoMessage_locals_clutterColorArgAt_(IoMessage *self, IoObject *locals, int n) {
  IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

  if(!ISCLUTTERCOLOR(v)) {
    IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "ClutterColor");
  }

  return v;
}

//doc ClutterColor ==(otherColor)
IO_METHOD(IoClutterColor, equals) {
  ClutterColor other = IOCCOLOR(IoMessage_locals_clutterColorArgAt_(m, locals, 0));
  return IOBOOL(self, clutter_color_equal(&(IOCCOLOR(self)), &other));
}

//doc ClutterColor !=(otherColor)
IO_METHOD(IoClutterColor, notEquals) {
  ClutterColor other = IOCCOLOR(IoMessage_locals_clutterColorArgAt_(m, locals, 0));
  return IOBOOL(self, !clutter_color_equal(&(IOCCOLOR(self)), &other));
}

//doc ClutterColor fromString(str)
IO_METHOD(IoClutterColor, fromString) {
  ClutterColor color;
  char *seq = CSTRING(IoMessage_locals_seqArgAt_(m, locals, 0));
  clutter_color_from_string(&color, seq);

  return IoClutterColor_newWithColor(IOSTATE, color);
}

//doc ClutterColor asString
IO_METHOD(IoClutterColor, asString) {
  char *color = clutter_color_to_string(&(IOCCOLOR(self)));
  return IOSYMBOL(color);
}

//doc ClutterColor fromHLS(h, l, s)
IO_METHOD(IoClutterColor, fromHLS) {
  ClutterColor color;
  float h = CNUMBER(IoMessage_locals_numberArgAt_(m, locals, 0)),
        l = CNUMBER(IoMessage_locals_numberArgAt_(m, locals, 1)),
        s = CNUMBER(IoMessage_locals_numberArgAt_(m, locals, 2));

  clutter_color_from_hls(&color, h, l, s);

  return IoClutterColor_newWithColor(IOSTATE, color);
}

//doc ClutterColor toHLS(h, l, s)
IO_METHOD(IoClutterColor, toHLS) {
  float h = CNUMBER(IoMessage_locals_numberArgAt_(m, locals, 0)),
        l = CNUMBER(IoMessage_locals_numberArgAt_(m, locals, 1)),
        s = CNUMBER(IoMessage_locals_numberArgAt_(m, locals, 2));

  clutter_color_to_hls(&IOCCOLOR(self), &h, &l, &s);
  return self;
}

//doc ClutterColor fromPixel(pixel)
IO_METHOD(IoClutterColor, fromPixel) {
  ClutterColor color;
  guint32 pixel = CNUMBER(IoMessage_locals_numberArgAt_(m, locals, 0));

  clutter_color_from_pixel(&color, pixel);
  return IoClutterColor_newWithColor(IOSTATE, color);
}

//doc ClutterColor toPixel
IO_METHOD(IoClutterColor, toPixel) {
  return IONUMBER(clutter_color_to_pixel(&IOCCOLOR(self)));
}

//doc ClutterColor +(otherColor)
IO_METHOD(IoClutterColor, add) {
  ClutterColor color_a = IOCCOLOR(self);
  ClutterColor color_b = IOCCOLOR(IoMessage_locals_clutterColorArgAt_(m, locals, 0));
  ClutterColor result;

  clutter_color_add(&color_a, &color_b, &result);
  return IoClutterColor_newWithColor(IOSTATE, result);
}

//doc ClutterColor addInPlace(otherColor)
IO_METHOD(IoClutterColor, addInPlace) {
  ClutterColor color_a = IOCCOLOR(self);
  ClutterColor color_b = IOCCOLOR(IoMessage_locals_clutterColorArgAt_(m, locals, 0));
  ClutterColor result;

  clutter_color_add(&color_a, &color_b, &result);
  clutter_color_free(&color_a);
  DATA(self)->color = result;

  return self;
}

//doc ClutterColor -(otherColor)
IO_METHOD(IoClutterColor, subtract) {
  ClutterColor color_a = IOCCOLOR(self);
  ClutterColor color_b = IOCCOLOR(IoMessage_locals_clutterColorArgAt_(m, locals, 0));
  ClutterColor result;

  clutter_color_subtract(&color_a, &color_b, &result);
  return IoClutterColor_newWithColor(IOSTATE, result);
}

//doc ClutterColor subtractInPlace(otherColor)
IO_METHOD(IoClutterColor, subtractInPlace) {
  ClutterColor color_a = IOCCOLOR(self);
  ClutterColor color_b = IOCCOLOR(IoMessage_locals_clutterColorArgAt_(m, locals, 0));
  ClutterColor result;

  clutter_color_subtract(&color_a, &color_b, &result);
  clutter_color_free(&color_a);
  DATA(self)->color = result;

  return self;
}

//doc ClutterColor lighten
IO_METHOD(IoClutterColor, lighten) {
  ClutterColor result;
  clutter_color_lighten(&IOCCOLOR(self), &result);

  return IoClutterColor_newWithColor(IOSTATE, result);
}

//doc ClutterColor darken
IO_METHOD(IoClutterColor, darken) {
  ClutterColor result;
  clutter_color_darken(&IOCCOLOR(self), &result);

  return IoClutterColor_newWithColor(IOSTATE, result);
}

//doc ClutterColor shade(amount)
IO_METHOD(IoClutterColor, shade) {
  ClutterColor result;
  double factor = CNUMBER(IoMessage_locals_numberArgAt_(m, locals, 0));
  clutter_color_shade(&IOCCOLOR(self), factor, &result);

  return IoClutterColor_newWithColor(IOSTATE, result);
}
