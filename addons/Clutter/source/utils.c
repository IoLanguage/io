#include "utils.h"

int origin_changed_flags(int origin_changed) {
  return origin_changed ? CLUTTER_ABSOLUTE_ORIGIN_CHANGED : CLUTTER_ALLOCATION_NONE;
}

int font_hinting_flags(int use_hinting) {
  // NOTE: There seems to be some kind of bug, with CLUTTER_FONT_MIPMAPPING, which is set to (1<<0)
  return use_hinting ? CLUTTER_FONT_HINTING : (CLUTTER_FONT_MIPMAPPING & CLUTTER_FONT_HINTING);
}

IoObject *clutter_geometry_to_ioObject(void *state, IoObject *self, ClutterGeometry *geom) {
  IoObject *geom_obj = IoObject_new(state);
  IoObject_setSlot_to_(geom_obj, IOSYMBOL("x"),       IONUMBER(geom->x));
  IoObject_setSlot_to_(geom_obj, IOSYMBOL("y"),       IONUMBER(geom->y));
  IoObject_setSlot_to_(geom_obj, IOSYMBOL("width"),   IONUMBER(geom->width));
  IoObject_setSlot_to_(geom_obj, IOSYMBOL("height"),  IONUMBER(geom->height));

  return geom_obj;
}