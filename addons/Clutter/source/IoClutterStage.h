#ifndef IoClutterStage_DEFINED
#define IoClutterStage_DEFINED 1

#define ISCLUTTERSTAGE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoClutterStage_rawClone)
#define IOCSTAGE(self) (CLUTTER_STAGE((ClutterActor*)IoObject_dataPointer(self)))

#include "common.h"
#include "IoClutterActor.h"
#include "IoClutterColor.h"

typedef IoObject IoClutterStage;

// Initialisation functions
IoTag           *IoClutterStage_newTag        (void *state);
IoClutterStage  *IoClutterStage_proto         (void *state);
IoClutterStage  *IoClutterStage_rawClone      (IoClutterStage *proto);
IoClutterStage  *IoClutterStage_new           (void *state);
IoClutterStage  *IoClutterStage_newWithActor  (void *state, ClutterActor *actor);
void            IoClutterStage_free           (IoClutterStage *self);

IoObject *IoMessage_locals_clutterStageArgAt_(IoMessage *self, IoObject *locals, int n);

IO_METHOD(IoClutterStage, getDefault);

IO_METHOD(IoClutterStage, getColor);
IO_METHOD(IoClutterStage, setColor);
IO_METHOD(IoClutterStage, getTitle);
IO_METHOD(IoClutterStage, setTitle);


#endif
