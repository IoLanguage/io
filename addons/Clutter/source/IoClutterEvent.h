#ifndef IoClutterEvent_DEFINED
#define IoClutterEvent_DEFINED 1

#define ISCLUTTEREVENT(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoClutterEvent_rawClone)
#define IOCEVENT(self) ((ClutterEvent*)IoObject_dataPointer(self))

#include "common.h"
#include "IoClutterActor.h"
#include "IoClutterInputDevice.h"

typedef IoObject IoClutterEvent;

// Initialisation functions
IoTag           *IoClutterEvent_newTag      (void *state);
IoClutterEvent  *IoClutterEvent_proto       (void *state);
IoClutterEvent  *IoClutterEvent_rawClone    (IoClutterEvent *proto);
IoClutterEvent  *IoClutterEvent_new         (void *state);
IoClutterEvent  *IoClutterEvent_newWithType (void *state, ClutterEventType type);
IoClutterEvent  *IoClutterEvent_newWithEvent(void *state, ClutterEvent *event);
void            IoClutterEvent_free         (IoClutterEvent *self);

IoObject *IoMessage_locals_clutterEventArgAt_(IoMessage *self, IoObject *locals, int n);

IO_METHOD(IoClutterEvent, eventType);
IO_METHOD(IoClutterEvent, getCoords);
IO_METHOD(IoClutterEvent, getState);
IO_METHOD(IoClutterEvent, getTime);
IO_METHOD(IoClutterEvent, getSource);
IO_METHOD(IoClutterEvent, getFlags);
IO_METHOD(IoClutterEvent, peek);
IO_METHOD(IoClutterEvent, put);
IO_METHOD(IoClutterEvent, putBack);
IO_METHOD(IoClutterEvent, hasPending);
IO_METHOD(IoClutterEvent, getButton);
IO_METHOD(IoClutterEvent, getClickCount);
IO_METHOD(IoClutterEvent, getKeySymbol);
IO_METHOD(IoClutterEvent, getKeyCode);
IO_METHOD(IoClutterEvent, getKeyUnicode);
IO_METHOD(IoClutterEvent, getRelatedActor);
IO_METHOD(IoClutterEvent, getScrollDirection);
IO_METHOD(IoClutterEvent, getDevice);

#endif
