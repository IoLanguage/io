//metadoc DrawStuff copyright ∃teslos 2013
//metadoc DrawStuff license BSD revised

#ifndef IODRAWSTUFF_DEFINED
#define IODRAWSTUFF_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoCoroutine.h"

#include <drawstuff/drawstuff.h>

typedef IoObject IoDrawStuff;

typedef struct
{
    IoCoroutine *coroutine;
    IoObject  *eventTarget;
    IoMessage *keyboardMessage;
    IoMessage *startMessage;
    IoMessage *stepMessage;
    IoMessage *stopMessage;
    
} IoDrawStuffData;

IoObject *IoDrawStuff_rawClone(IoDrawStuff *self);
IoDrawStuff *IoDrawStuff_proto(void *state);
void IoDrawStuff_free(IoDrawStuff *self);
void IoDrawStuff_mark(IoDrawStuff *self);

void IoDrawStuff_protoInit(IoDrawStuff *self);

// ---- events -----------------------------------
IoObject *IoDrawStuff_dsSimulationLoop(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsSetViewpoint(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsGetViewpoint(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsElapsedTime(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsStop(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsSetTexture(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsSetColor(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsSetColorAlpha(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsDrawBox(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsDrawSphere(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsDrawTriangle(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsDrawCylinder(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsDrawCapsule(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsDrawLine(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsDrawConvex(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsSetSphereQuality(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsSetCapsuleQuality(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_dsSetDrawMode(IoDrawStuff *self, IoObject *locals, IoMessage *m);
IoObject *IoDrawStuff_eventTarget(IoDrawStuff *self, IoObject *locals, IoMessage *m);
void IoDrawStuffKeyboardFunc();
void IoDrawStuffStartFunc();
void IoDrawStuffStepFunc(int pause);
void IoDrawStuffStopFunc();

#endif
