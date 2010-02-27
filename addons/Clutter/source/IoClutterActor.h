#ifndef IoClutterActor_DEFINED
#define IoClutterActor_DEFINED 1

#define ISCLUTTERACTOR(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoClutterActor_rawClone)
#define IOCACTOR(self) ((ClutterActor*)IoObject_dataPointer(self))
#include "common.h"
#include "IoClutterActorBox.h"
#include "IoClutterEvent.h"
#include "IoClutterVertex.h"
#include "IoClutterStage.h"

typedef IoObject IoClutterActor;

// Initialisation functions
IoTag           *IoClutterActor_newTag      (void *state);
IoClutterActor  *IoClutterActor_proto       (void *state);
IoClutterActor  *IoClutterActor_rawClone    (IoClutterActor *proto);
IoClutterActor  *IoClutterActor_new         (void *state);
IoClutterActor  *IoClutterActor_newWithActor(void *state, ClutterActor *actor);
void            IoClutterActor_free         (IoClutterActor *self);

IoObject *IoMessage_locals_clutterActorArgAt_(IoMessage *self, IoObject *locals, int n);

// Methods
IO_METHOD(IoClutterActor, isMapped);
IO_METHOD(IoClutterActor, isRealized);
IO_METHOD(IoClutterActor, isVisible);
IO_METHOD(IoClutterActor, isReactive);
IO_METHOD(IoClutterActor, show);
IO_METHOD(IoClutterActor, showAll);
IO_METHOD(IoClutterActor, hide);
IO_METHOD(IoClutterActor, hideAll);
IO_METHOD(IoClutterActor, realize);
IO_METHOD(IoClutterActor, unrealize);
IO_METHOD(IoClutterActor, paint);
IO_METHOD(IoClutterActor, queueRedraw);
IO_METHOD(IoClutterActor, queueRelayout);
IO_METHOD(IoClutterActor, destroy);
IO_METHOD(IoClutterActor, event);
IO_METHOD(IoClutterActor, shouldPickPaint);
IO_METHOD(IoClutterActor, map);
IO_METHOD(IoClutterActor, unmap);
IO_METHOD(IoClutterActor, allocate);
IO_METHOD(IoClutterActor, allocatePrefferedSize);
IO_METHOD(IoClutterActor, allocateAvailableSize);
IO_METHOD(IoClutterActor, getAllocationBox);
IO_METHOD(IoClutterActor, getAllocationGeometry);
IO_METHOD(IoClutterActor, getAllocationVertices);
IO_METHOD(IoClutterActor, getPreferredSize);
IO_METHOD(IoClutterActor, getPreferredWidth);
IO_METHOD(IoClutterActor, getPreferredHeight);
IO_METHOD(IoClutterActor, getFixedPosition);
IO_METHOD(IoClutterActor, setFixedPosition);

IO_METHOD(IoClutterActor, getGeometry);
IO_METHOD(IoClutterActor, setGeometry);
IO_METHOD(IoClutterActor, getSize);
IO_METHOD(IoClutterActor, setSize);
IO_METHOD(IoClutterActor, getPosition);
IO_METHOD(IoClutterActor, setPosition);
IO_METHOD(IoClutterActor, getWidth);
IO_METHOD(IoClutterActor, setWidth);
IO_METHOD(IoClutterActor, getHeight);
IO_METHOD(IoClutterActor, setHeight);
IO_METHOD(IoClutterActor, getX);
IO_METHOD(IoClutterActor, setX);
IO_METHOD(IoClutterActor, getY);
IO_METHOD(IoClutterActor, setY);
IO_METHOD(IoClutterActor, moveBy);

IO_METHOD(IoClutterActor, getRotation);
IO_METHOD(IoClutterActor, setRotation);
IO_METHOD(IoClutterActor, getZRotationGravity);
IO_METHOD(IoClutterActor, setZRotationFromGravity);
IO_METHOD(IoClutterActor, isRotated);

IO_METHOD(IoClutterActor, getOpacity);
IO_METHOD(IoClutterActor, setOpacity);

IO_METHOD(IoClutterActor, getName);
IO_METHOD(IoClutterActor, setName);
IO_METHOD(IoClutterActor, getGid);

IO_METHOD(IoClutterActor, getClip);
IO_METHOD(IoClutterActor, setClip);
IO_METHOD(IoClutterActor, hasClip);
IO_METHOD(IoClutterActor, removeClip);

IO_METHOD(IoClutterActor, getParent);
IO_METHOD(IoClutterActor, setParent);
IO_METHOD(IoClutterActor, reparent);

IO_METHOD(IoClutterActor, raise);
IO_METHOD(IoClutterActor, lower);
IO_METHOD(IoClutterActor, raiseToTop);
IO_METHOD(IoClutterActor, lowerToBottom);

IO_METHOD(IoClutterActor, getStage);

IO_METHOD(IoClutterActor, getDepth);
IO_METHOD(IoClutterActor, setDepth);
#endif
