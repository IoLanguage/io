#ifndef IoClutterActorBox_DEFINED
#define IoClutterActorBox_DEFINED 1

#define ISCLUTTERACTORBOX(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoClutterActorBox_rawClone)
#define IOCABOX(self) ((ClutterActorBox*)IoObject_dataPointer(self))

#include "common.h"

typedef IoObject IoClutterActorBox;

// Initialisation functions
IoTag             *IoClutterActorBox_newTag           (void *state);
IoClutterActorBox *IoClutterActorBox_proto            (void *state);
IoClutterActorBox *IoClutterActorBox_rawClone         (IoClutterActorBox *proto);
IoClutterActorBox *IoClutterActorBox_new              (void *state);
IoClutterActorBox *IoClutterActorBox_newWithActorBox  (void *state, ClutterActorBox *actorBox);
void               IoClutterActorBox_free             (IoClutterActorBox *self);

IoObject *IoMessage_locals_clutterActorBoxArgAt_(IoMessage *self, IoObject *locals, int n);

IO_METHOD(IoClutterActorBox, equals);
IO_METHOD(IoClutterActorBox, with);
IO_METHOD(IoClutterActorBox, getOrigin);
IO_METHOD(IoClutterActorBox, getSize);
IO_METHOD(IoClutterActorBox, contains);

#endif
