#include "IoClutterUnits.h"
#define DATA(self) ((IoClutterUnitsData*)IoObject_dataPointer(self))
//metadoc ClutterUnits category UserInterface
/*metadoc ClutterUnits description
<p>For more detailed docs see <a href="http://clutter-project.org/docs/clutter/stable/clutter-Unit-conversion.html">Clutter documentation</a>.</p>
Example:
<pre><code>ClutterActor clone setWidth((10 px + 1 em) value)</code></pre>
*/

/*** Initalisation functions ***/
IoTag *IoClutterUnits_newTag(void *state) {
  IoTag *tag = IoTag_newWithName_("ClutterUnits");

  IoTag_state_(tag, state);
  IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoClutterUnits_free);
  IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoClutterUnits_rawClone);

  return tag;
}

IoClutterUnits *IoClutterUnits_proto(void *state) {
  IoObject *self = IoObject_new(state);
  IoObject_tag_(self, IoClutterUnits_newTag(state));

  IoObject_setDataPointer_(self, calloc(1, sizeof(IoClutterUnitsData)));
  IoState_registerProtoWithFunc_(state, self, IoClutterUnits_proto);

  {
    IoMethodTable methodTable[] = {
      {"==",          IoClutterUnits_equals},
      {"compare",     IoClutterUnits_compare},
      {"+",           IoClutterUnits_add},
      {"-",           IoClutterUnits_subtract},

      {"withMm",      IoClutterUnits_withMm},
      {"withPt",      IoClutterUnits_withPt},
      {"withEm",      IoClutterUnits_withEm},
      {"withEmForFont",  IoClutterUnits_withEmForFont},

      {"withPx",      IoClutterUnits_withPx},
      {"with",        IoClutterUnits_withPx},
      {"asPixels",    IoClutterUnits_asPixels},

      {"withString",  IoClutterUnits_withString},
      {"asString",    IoClutterUnits_asString},

      {"unit",        IoClutterUnits_getUnit},
      {"value",       IoClutterUnits_getValue},

      {NULL, NULL}
    };
    IoObject_addMethodTable_(self, methodTable);
  }

  return self;
}

IoClutterUnits *IoClutterUnits_rawClone(IoClutterUnits *proto) {
  IoClutterUnits *self = IoObject_rawClonePrimitive(proto);

  IoObject_setDataPointer_(self, calloc(1, sizeof(IoClutterUnitsData)));

  return self;
}

IoClutterUnits *IoClutterUnits_new(void *state) {
  IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoClutterUnits_proto);

  return IOCLONE(proto);
}

IoClutterUnits *IoClutterUnits_newWithUnits(void *state, ClutterUnits units) {
  IoClutterUnits *io_units = IoClutterUnits_new(state);
  clutter_units_free(&IOCUNITS(io_units));
  IOCUNITS(io_units) = units;
  return io_units;
}

void IoClutterUnits_free(IoClutterUnits *self) {
  clutter_units_free(&IOCUNITS(self));
  io_free(IoObject_dataPointer(self));
}

IoObject *IoMessage_locals_clutterUnitsArgAt_(IoMessage *self, IoObject *locals, int n) {
  IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

  if(!ISCLUTTERUNITS(v)) {
    IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "ClutterUnits");
  }

  return v;
}

/*** Object functions ***/
//doc ClutterUnits ==(otherUnit)
IO_METHOD(IoClutterUnits, equals) {
  IoClutterUnits *other = IoMessage_locals_clutterUnitsArgAt_(m, locals, 0);
  int self_in_pixels = clutter_units_to_pixels(&IOCUNITS(self)),
      other_in_pixels = clutter_units_to_pixels(&IOCUNITS(other));

  return IOBOOL(self, self_in_pixels == other_in_pixels);
}

//doc ClutterUnits compare(otherUnit) Returns 0 if units are equal, -1 if <code>self</code> is smaller, 1 if <code>self</code> is bigger than <code>otherUnit</code>.
IO_METHOD(IoClutterUnits, compare) {
  IoClutterUnits *other = IoMessage_locals_clutterUnitsArgAt_(m, locals, 0);
  int self_in_pixels = clutter_units_to_pixels(&IOCUNITS(self)),
      other_in_pixels = clutter_units_to_pixels(&IOCUNITS(other));

  if(self_in_pixels < other_in_pixels)
    return IONUMBER(-1);
  else if(self_in_pixels > other_in_pixels)
    return IONUMBER(1);

  return IONUMBER(0);
}

//doc ClutterUnits +(otherUnit) <code>otherUnit</code> can be either [[ClutterUnits]] or [[Number]].
IO_METHOD(IoClutterUnits, add) {
  IoObject *other     = IoMessage_locals_valueArgAt_(m, locals, 0);
  int self_in_pixels  = clutter_units_to_pixels(&IOCUNITS(self)),
        other_in_pixels = 0;
  ClutterUnits units;

  if(ISNUMBER(other)) {
    other_in_pixels = CNUMBER(other);
  } else if(ISCLUTTERUNITS(other)) {
    other_in_pixels = clutter_units_to_pixels(&IOCUNITS(other));
  } else {
    IoState_error_(IOSTATE, m, "ClutterUnits arithmetic works only for Numbers and other ClutterUnits.");
    return IONIL(self);
  }

  clutter_units_from_pixels(&units, self_in_pixels + other_in_pixels);
  return IoClutterUnits_newWithUnits(IOSTATE, units);
}

//doc ClutterUnits -(otherUnit)
IO_METHOD(IoClutterUnits, subtract) {
  IoObject *other     = IoMessage_locals_valueArgAt_(m, locals, 0);
  int self_in_pixels  = clutter_units_to_pixels(&IOCUNITS(self)),
        other_in_pixels = 0;
  ClutterUnits units;

  if(ISNUMBER(other)) {
    other_in_pixels = CNUMBER(other);
  } else if(ISCLUTTERUNITS(other)) {
    other_in_pixels = clutter_units_to_pixels(&IOCUNITS(other));
  } else {
    IoState_error_(IOSTATE, m, "ClutterUnits arithmetic works only for Numbers and other ClutterUnits.");
    return IONIL(self);
  }

  clutter_units_from_pixels(&units, self_in_pixels - other_in_pixels);
  return IoClutterUnits_newWithUnits(IOSTATE, units);
}

//doc ClutterUnits withMm(valueInMm)
IO_METHOD(IoClutterUnits, withMm) {
  ClutterUnits units;
  clutter_units_from_mm(
    &units,
    IoMessage_locals_floatArgAt_(m, locals, 0)
  );

  return IoClutterUnits_newWithUnits(IOSTATE, units);
}

//doc ClutterUnits withPt(valueInPt)
IO_METHOD(IoClutterUnits, withPt) {
  ClutterUnits units;
  clutter_units_from_pt(
    &units,
    IoMessage_locals_floatArgAt_(m, locals, 0)
  );

  return IoClutterUnits_newWithUnits(IOSTATE, units);
}

//doc ClutterUnits withEm(valueInEm)
IO_METHOD(IoClutterUnits, withEm) {
  ClutterUnits units;
  clutter_units_from_em(
    &units,
    IoMessage_locals_floatArgAt_(m, locals, 0)
  );

  return IoClutterUnits_newWithUnits(IOSTATE, units);
}

//doc ClutterUnits withEmForFont(valueInEm, fontName)
IO_METHOD(IoClutterUnits, withEmForFont) {
  ClutterUnits units;
  clutter_units_from_em_for_font(
    &units,
    IoMessage_locals_cStringArgAt_(m, locals, 0),
    IoMessage_locals_floatArgAt_(m, locals, 1)
  );

  return IoClutterUnits_newWithUnits(IOSTATE, units);
}

//doc ClutterUnits withPx(valueInPx)
IO_METHOD(IoClutterUnits, withPx) {
  ClutterUnits units;
  clutter_units_from_pixels(
    &units,
    IoMessage_locals_intArgAt_(m, locals, 0)
  );

  return IoClutterUnits_newWithUnits(IOSTATE, units);
}

IO_METHOD(IoClutterUnits, asPixels) {
  return IONUMBER(clutter_units_to_pixels(&IOCUNITS(self)));
}

//doc ClutterUnits withString(string)
IO_METHOD(IoClutterUnits, withString) {
  ClutterUnits units;
  clutter_units_from_string(
    &units,
    IoMessage_locals_cStringArgAt_(m, locals, 0)
  );

  return IoClutterUnits_newWithUnits(IOSTATE, units);
}

//doc ClutterUnits asString
IO_METHOD(IoClutterUnits, asString) {
  return IOSYMBOL(clutter_units_to_string(&IOCUNITS(self)));
}

//doc ClutterUnits unit Returns an value from [[Clutter UNIT]].
IO_METHOD(IoClutterUnits, getUnit) {
  return IONUMBER(clutter_units_get_unit_type(&IOCUNITS(self)));
}

//doc ClutterUnits value
IO_METHOD(IoClutterUnits, getValue) {
  return IONUMBER(clutter_units_get_unit_value(&IOCUNITS(self)));
}
