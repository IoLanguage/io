#ifndef IoClutterUnits_DEFINED
#define IoClutterUnits_DEFINED 1

#define ISCLUTTERUNITS(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoClutterUnits_rawClone)
#define IOCUNITS(self) (((IoClutterUnitsData*)IoObject_dataPointer(self))->units)

#include "common.h"

typedef IoObject IoClutterUnits;

typedef struct {
  ClutterUnits units;
} IoClutterUnitsData;

// Initialisation functions
IoTag           *IoClutterUnits_newTag        (void *state);
IoClutterUnits  *IoClutterUnits_proto         (void *state);
IoClutterUnits  *IoClutterUnits_rawClone      (IoClutterUnits *proto);
IoClutterUnits  *IoClutterUnits_new           (void *state);
IoClutterUnits  *IoClutterUnits_newWithUnits  (void *state, ClutterUnits units);
void            IoClutterUnits_free           (IoClutterUnits *self);

IoObject *IoMessage_locals_clutterUnitsArgAt_(IoMessage *self, IoObject *locals, int n);

IO_METHOD(IoClutterUnits, equals);
IO_METHOD(IoClutterUnits, compare);
IO_METHOD(IoClutterUnits, add);
IO_METHOD(IoClutterUnits, subtract);
IO_METHOD(IoClutterUnits, withMm);
IO_METHOD(IoClutterUnits, withPt);
IO_METHOD(IoClutterUnits, withEm);
IO_METHOD(IoClutterUnits, withEmForFont);
IO_METHOD(IoClutterUnits, withPx);
IO_METHOD(IoClutterUnits, asPixels);
IO_METHOD(IoClutterUnits, withString);
IO_METHOD(IoClutterUnits, asString);
IO_METHOD(IoClutterUnits, getUnit);
IO_METHOD(IoClutterUnits, getValue);
#endif
