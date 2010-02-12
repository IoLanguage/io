#ifndef IoClutterColor_DEFINED
#define IoClutterColor_DEFINED 1

#define ISCLUTTERCOLOR(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoClutterColor_rawClone)

#define IOCCOLOR(self) (((IoClutterColorData*)IoObject_dataPointer(self))->color)

#include "common.h"

typedef IoObject IoClutterColor;

typedef struct {
  ClutterColor color;
} IoClutterColorData;

// Initialisation functions
IoTag           *IoClutterColor_newTag      (void *state);
IoClutterColor  *IoClutterColor_proto       (void *state);
IoClutterColor  *IoClutterColor_rawClone    (IoClutterColor *proto);
IoClutterColor  *IoClutterColor_new         (void *state);
IoClutterColor  *IoClutterColor_newWithColor(void *state, ClutterColor color);
void            IoClutterColor_free         (IoClutterColor *self);

IoObject *IoMessage_locals_clutterColorArgAt_(IoMessage *self, IoObject *locals, int n);

IO_METHOD(IoClutterColor, equals);
IO_METHOD(IoClutterColor, notEquals);
IO_METHOD(IoClutterColor, fromString);
IO_METHOD(IoClutterColor, asString);
IO_METHOD(IoClutterColor, fromHLS);
IO_METHOD(IoClutterColor, toHLS);
IO_METHOD(IoClutterColor, fromPixel);
IO_METHOD(IoClutterColor, toPixel);
IO_METHOD(IoClutterColor, add);
IO_METHOD(IoClutterColor, subtract);
IO_METHOD(IoClutterColor, addInPlace);
IO_METHOD(IoClutterColor, subtractInPlace);
IO_METHOD(IoClutterColor, lighten);
IO_METHOD(IoClutterColor, darken);
IO_METHOD(IoClutterColor, shade);

#endif