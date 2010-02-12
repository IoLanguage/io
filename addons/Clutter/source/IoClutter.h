#ifndef IoClutter_DEFINED
#define IoClutter_DEFINED 1
#define ISCLUTTER(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoClutter_rawClone)

#include "common.h"
#include "IoClutterActor.h"

typedef IoObject IoClutter;

// Initialisation functions
IoTag     *IoClutter_newTag   (void *state);
IoClutter *IoClutter_proto    (void *state);
IoClutter *IoClutter_rawClone (IoClutter *proto);
IoClutter *IoClutter_new      (void *state);
void       IoClutter_free     (IoClutter *self);

IoObject *IoMessage_locals_clutterActorArgAt_(IoMessage *self, IoObject *locals, int n);

IO_METHOD(IoClutter, init);
IO_METHOD(IoClutter, initThreads);
IO_METHOD(IoClutter, threadEnter);
IO_METHOD(IoClutter, threadLeave);
IO_METHOD(IoClutter, main);
IO_METHOD(IoClutter, quitMain);
IO_METHOD(IoClutter, mainLevel);
IO_METHOD(IoClutter, isFpsShown);
IO_METHOD(IoClutter, getDefaultFrameRate);
IO_METHOD(IoClutter, setDefaultFrameRate);
IO_METHOD(IoClutter, enableMotionEvents);
IO_METHOD(IoClutter, disableMotionEvents);
IO_METHOD(IoClutter, motionEventsEnabled);
IO_METHOD(IoClutter, clearGlyphCache);
IO_METHOD(IoClutter, getFontHinting);
IO_METHOD(IoClutter, setFontHinting);
IO_METHOD(IoClutter, getActorByGid);
IO_METHOD(IoClutter, getKeyboardGrab);
IO_METHOD(IoClutter, getPointerGrab);
IO_METHOD(IoClutter, grabKeyboard);
IO_METHOD(IoClutter, grabPointer);
IO_METHOD(IoClutter, ungrabKeyboard);
IO_METHOD(IoClutter, ungrabPointer);
IO_METHOD(IoClutter, keySymbolToUnicode);
IO_METHOD(IoClutter, currentEventTime);

#endif