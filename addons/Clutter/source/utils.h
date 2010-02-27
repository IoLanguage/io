#include <stdlib.h>
#include "IoState.h"
#include "IoObject.h"
#include "IoNumber.h"
#include <clutter/clutter.h>

int origin_changed_flags  (int origin_changed);
int font_hinting_flags    (int use_hinting);
IoObject *clutter_geometry_to_ioObject(void *state, IoObject *self, ClutterGeometry *geom);