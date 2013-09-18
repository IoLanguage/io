//metadoc DrawStuff copyright ∃teslos 2013
//metadoc DrawStuff license BSD revised
//metadoc DrawStuff category Graphics

#include "IoDrawStuff.h"

#include "List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoCFunction.h"
#include "IoSeq.h"
#include "IoList.h"
#include "List.h"

#define DATA(self) ((IoDrawStuffData *)IoObject_dataPointer(self))
#define DRAWMESSAGE(name) \
IoMessage_newWithName_label_(state, IOSYMBOL(name), IOSYMBOL("[DrawStuff]"))
static const char *protoId = "DrawStuff";
static IoDrawStuff *proto = NULL;

IoTag *IoDrawStuff_newTag(void *state)
{
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoDrawStuff_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoDrawStuff_free);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoDrawStuff_mark);
    return tag;
}

IoDrawStuff *IoDrawStuff_proto(void *state)
{
    IoObject *self = IoObject_new(state);
    proto = self;
    IoObject_tag_(self, IoDrawStuff_newTag(state));
    IoObject_setDataPointer_(self, calloc(1, sizeof(IoDrawStuffData)));

    DATA(self)->coroutine = IoCoroutine_new(state);
    //printf("DrawStuff coro = %p\n", DATA(self)->coroutine);

    DATA(self)->eventTarget = NULL;

    DATA(self)->keyboardMessage = DRAWMESSAGE("keyboard");
    DATA(self)->startMessage    = DRAWMESSAGE("start");
    DATA(self)->stepMessage     = DRAWMESSAGE("step");
    DATA(self)->stopMessage     = DRAWMESSAGE("stop");

    IoState_retain_(state, DATA(self)->coroutine); 
    IoState_retain_(state, DATA(self)->keyboardMessage);
    IoState_retain_(state, DATA(self)->startMessage);
    IoState_retain_(state, DATA(self)->stepMessage);
    IoState_retain_(state, DATA(self)->stopMessage);

    IoState_registerProtoWithId_(state, self, protoId);
    {
        IoMethodTable methodTable[] = {
        {"dsSimulationLoop", IoDrawStuff_dsSimulationLoop},
        {"dsSetViewpoint", IoDrawStuff_dsSetViewpoint},
        {"dsGetViewpoint", IoDrawStuff_dsGetViewpoint},
        {"dsElapsedTime", IoDrawStuff_dsElapsedTime},
        {"dsStop", IoDrawStuff_dsStop},
        {"dsSetTexture", IoDrawStuff_dsSetTexture},
        {"dsSetColor", IoDrawStuff_dsSetColor},
        {"dsSetColorAlpha", IoDrawStuff_dsSetColorAlpha},
        {"dsDrawBox", IoDrawStuff_dsDrawBox},
        {"dsDrawSphere", IoDrawStuff_dsDrawSphere}, 
        {"dsDrawTriangle", IoDrawStuff_dsDrawTriangle},
        {"dsDrawCylinder", IoDrawStuff_dsDrawCylinder},
        {"dsDrawCapsule", IoDrawStuff_dsDrawCapsule},
        {"dsDrawLine", IoDrawStuff_dsDrawLine},
        {"dsDrawConvex", IoDrawStuff_dsDrawConvex},
        {"dsSetSphereQuality", IoDrawStuff_dsSetSphereQuality},
        {"dsSetCapsuleQuality", IoDrawStuff_dsSetCapsuleQuality},
        {"dsSetDrawMode", IoDrawStuff_dsSetDrawMode},
        {"eventTarget", IoDrawStuff_eventTarget},

        {NULL, NULL},
        };
        IoObject_addMethodTable_(self, methodTable);
    }
    //IoDrawStuff_protoInit(self);
    return self;
}

IoDrawStuff *IoDrawStuff_new(void *state)
{
    return IoState_protoWithId_(state, protoId);
}

void IoDrawStuff_free(IoDrawStuff *self)
{
    free(IoObject_dataPointer(self));
}


void IoDrawStuff_mark(IoDrawStuff *self)
{
    //printf("IoDrawStuff_mark\n");
    if(DATA(self)->eventTarget)
    {
        IoObject_shouldMark(DATA(self)->eventTarget);
    }
    
    IoObject_shouldMark(DATA(self)->keyboardMessage);
    IoObject_shouldMark(DATA(self)->startMessage);
    IoObject_shouldMark(DATA(self)->stepMessage);
    IoObject_shouldMark(DATA(self)->stopMessage);
}

IoObject *IoDrawStuff_rawClone(IoDrawStuff *self)
{
    return IoState_protoWithId_(IOSTATE, protoId);
}

IoObject *IoDrawStuff_tryCallback(IoDrawStuff *self, IoMessage *m)
{
    IoState *state = IoObject_state(proto);
    IoObject *tryCoro = DATA(self)->coroutine;
    IoObject *t = DATA(self)->eventTarget;
    IoObject *result = state->ioNil;
    //printf("IoDrawStuff_tryCallback(self, %p)\n", (void*)m);
    //printf("IoDrawStuff_tryCallback target: %p)\n", (void*)t);

    if(t)
    {
        IoMessage_locals_performOn_(m, t, t);
        if (IoCoroutine_rawException(tryCoro) != state->ioNil)
            IoState_exception_(state, tryCoro);
        
        IoCoroutine_clearStack(tryCoro);
        return IoCoroutine_rawResult(tryCoro);
    }
    return result;
}

// callback events
void IoDrawStuffStartFunc(void)
{
    //printf("IoDrawStuffStartFunc\n");
    IoState_pushRetainPool(IoObject_state(proto));
    
    IoDrawStuff_tryCallback(proto, DATA(proto)->startMessage);
    IoState_popRetainPool(IoObject_state(proto));
}

void IoDrawStuffStopFunc(void)
{
    //printf("IoDrawStuffStopFunc\n");
    IoState_pushRetainPool(IoObject_state(proto));
    
    IoDrawStuff_tryCallback(proto, DATA(proto)->stopMessage);
    IoState_popRetainPool(IoObject_state(proto));
}

void IoDrawStuffStepFunc(int pause)
{
    //printf("IoDrawStuffStepFunc\n");
    IoState_pushRetainPool(IoObject_state(proto));
    IoMessage_setCachedArg_toInt_(DATA(proto)->stepMessage, 0, pause);    
    IoDrawStuff_tryCallback(proto, DATA(proto)->stepMessage);
    IoState_popRetainPool(IoObject_state(proto));
}

void IoDrawStuffKeyboardFunc(int key)
{
    //printf("IoDrawStuffKeyboardFunc\n");
    IoState_pushRetainPool(IoObject_state(proto));
    IoMessage_setCachedArg_toInt_(DATA(proto)->keyboardMessage, 0, key);    
    IoDrawStuff_tryCallback(proto, DATA(proto)->keyboardMessage);
    IoState_popRetainPool(IoObject_state(proto));
}

IoObject *IoDrawStuff_dsSimulationLoop(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    struct dsFunctions fn;
    int argc = 0;
    char **argv = NULL;
    IoState *state = IOSTATE;
    char *path = "/usr/local/include/drawstuff";

    argc = state->mainArgs->argc;
    argv = (char **)(state->mainArgs->argv);

    fn.version = DS_VERSION;
    fn.start   = &IoDrawStuffStartFunc;
    fn.step    = &IoDrawStuffStepFunc;
    fn.command = &IoDrawStuffKeyboardFunc;
    fn.stop    = &IoDrawStuffStopFunc;
    fn.path_to_textures = path;
    
    int window_width = IoMessage_locals_intArgAt_(m, locals, 0);
    int window_height = IoMessage_locals_intArgAt_(m, locals, 1);
    dsSimulationLoop(argc, argv, window_width, window_height, &fn);
    return self;
}

IoObject *IoDrawStuff_dsSetViewpoint(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    float *xyz;
    float *hpr;
    {
    IoSeq *other = IoMessage_locals_vectorArgAt_(m, locals, 0);
    xyz   = IoSeq_floatPointerOfLength_(other, 3);

    other = IoMessage_locals_vectorArgAt_(m, locals, 1);
    hpr = IoSeq_floatPointerOfLength_(other, 3);
    }

    dsSetViewpoint(xyz, hpr);
    return self;
}

IoObject *IoDrawStuff_dsGetViewpoint(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    float xyz[3];
    float hpr[3];
    vec3f v1, v2;
    IoSeq *list_xyz = IoMessage_locals_vectorArgAt_(m, locals, 0);
    IoSeq *list_hpr = IoMessage_locals_vectorArgAt_(m, locals, 1);
    dsGetViewpoint(xyz, hpr);
    v1.x = xyz[0]; v1.y = xyz[1]; v1.z = xyz[2];
    v2.x = hpr[0]; v2.y = hpr[1]; v2.z = hpr[2];
    memcpy(IoSeq_rawBytes(list_xyz), &v1, sizeof(vec3f));
    memcpy(IoSeq_rawBytes(list_hpr), &v2, sizeof(vec3f));

    return self;
} 

IoObject *IoDrawStuff_dsStop(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    dsStop();
    return self;
}

IoObject *IoDrawStuff_dsElapsedTime(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    double elapsed = dsElapsedTime();
    return self;
}

IoObject *IoDrawStuff_dsSetTexture(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    int texture_number = IoMessage_locals_intArgAt_(m, locals, 0);
    dsSetTexture(texture_number);
    return self;
}


IoObject *IoDrawStuff_dsSetColor(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    float red   = IoMessage_locals_intArgAt_(m, locals, 0);
    float green = IoMessage_locals_intArgAt_(m, locals, 1);
    float blue  = IoMessage_locals_intArgAt_(m, locals, 2);

    dsSetColor(red, green, blue);
    return self;
}

IoObject *IoDrawStuff_dsSetColorAlpha(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    float red   = IoMessage_locals_intArgAt_(m, locals, 0);
    float green = IoMessage_locals_intArgAt_(m, locals, 1);
    float blue  = IoMessage_locals_intArgAt_(m, locals, 2);
    float alpha = IoMessage_locals_intArgAt_(m, locals, 3);

    dsSetColorAlpha(red, green, blue, alpha);
    return self;
}

IoObject *IoDrawStuff_dsDrawBox(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    float *pos;
    float *R;
    float *sides;

    //IoSeq_assertIsVector(self, locals, m);
    {
    IoSeq *other = IoMessage_locals_vectorArgAt_(m, locals, 0);
    pos   = IoSeq_floatPointerOfLength_(other, 3);

    other = IoMessage_locals_vectorArgAt_(m, locals, 1);
    R   = IoSeq_floatPointerOfLength_(other, 12);
    
    other = IoMessage_locals_vectorArgAt_(m, locals, 2); 
    sides = IoSeq_floatPointerOfLength_(other, 3); 
    }
    dsDrawBox(pos, R, sides);
    return self;
}

IoObject *IoDrawStuff_dsDrawSphere(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    float *pos;
    float *R;
    float radius;

    //IoSeq_assertIsVector(self, locals, m);
    {
    IoSeq *other = IoMessage_locals_vectorArgAt_(m, locals, 0);
    pos = IoSeq_floatPointerOfLength_(other, 3);

    other = IoMessage_locals_vectorArgAt_(m, locals, 1);
    R = IoSeq_floatPointerOfLength_(other, 12);

    radius = IoMessage_locals_floatArgAt_(m, locals, 2);
    }

    dsDrawSphere(pos, R, radius);
    return self;
}

IoObject *IoDrawStuff_dsDrawTriangle(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    float *pos;
    float *R;
    float *v0;
    float *v1;
    float *v2;
    int solid;
    //IoSeq_assertIsVector(self, locals, m);
    {
    IoSeq *other = IoMessage_locals_vectorArgAt_(m, locals, 0);
    pos = IoSeq_floatPointerOfLength_(other, 3);
    
    other = IoMessage_locals_vectorArgAt_(m, locals, 1);
    R = IoSeq_floatPointerOfLength_(other, 12);
    
    other = IoMessage_locals_vectorArgAt_(m, locals, 1);
    v0 = IoSeq_floatPointerOfLength_(other, 3);
    other = IoMessage_locals_vectorArgAt_(m, locals, 2);
    v1 = IoSeq_floatPointerOfLength_(other, 3);
    other = IoMessage_locals_vectorArgAt_(m, locals, 3);
    v2 = IoSeq_floatPointerOfLength_(other, 3);

    solid = IoMessage_locals_intArgAt_(m, locals, 4);
    }
    dsDrawTriangle(pos, R, v0, v1, v2, solid);
    return self;
}

IoObject *IoDrawStuff_dsDrawCapsule(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    float *pos;
    float *R;
    float length;
    float radius;
    //IoSeq_assertIsVector(self, locals, m);
    {
    IoSeq *other = IoMessage_locals_vectorArgAt_(m, locals, 0);
    pos = IoSeq_floatPointerOfLength_(other, 3);
    
    other = IoMessage_locals_vectorArgAt_(m, locals, 1);
    R = IoSeq_floatPointerOfLength_(other, 12);
    
    length = IoMessage_locals_floatArgAt_(m, locals, 2);
    radius = IoMessage_locals_floatArgAt_(m, locals, 3);
    }
    dsDrawCapsule(pos, R, length, radius);
    return self;
} 

IoObject *IoDrawStuff_dsDrawCylinder(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    float *pos;
    float *R;
    float length;
    float radius;
    //IoSeq_assertIsVector(self, locals, m);
    {
    IoSeq *other = IoMessage_locals_vectorArgAt_(m, locals, 0);
    pos = IoSeq_floatPointerOfLength_(other, 3);
    
    other = IoMessage_locals_vectorArgAt_(m, locals, 1);
    R = IoSeq_floatPointerOfLength_(other, 12);
    
    length = IoMessage_locals_floatArgAt_(m, locals, 2);
    radius = IoMessage_locals_floatArgAt_(m, locals, 3);
    }
    dsDrawCylinder(pos, R, length, radius);
    return self;
}    

IoObject *IoDrawStuff_dsDrawLine(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    float *pos1;
    float *pos2;

    //IoSeq_assertIsVector(self, locals, m);
    {
    IoSeq *other = IoMessage_locals_vectorArgAt_(m, locals, 0);
    pos1 = IoSeq_floatPointerOfLength_(other, 3);
    
    other = IoMessage_locals_vectorArgAt_(m, locals, 1);
    pos2 = IoSeq_floatPointerOfLength_(other, 3);
    }

    dsDrawLine(pos1, pos2);
    return self;
}

IoObject *IoDrawStuff_dsDrawConvex(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    float *pos;
    float *R;
    float *_planes;
    int _planecount;
    float *points;
    int _pointcount;
    int *_polygons;
    return self;
}

IoObject *IoDrawStuff_dsSetSphereQuality(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    int n;
    n = IoMessage_locals_intArgAt_(m, locals, 0);
    dsSetSphereQuality(n);
    return self;
}

IoObject *IoDrawStuff_dsSetCapsuleQuality(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    int n;
    n = IoMessage_locals_intArgAt_(m, locals, 0);
    dsSetCapsuleQuality(n);
    return self;
}

IoObject *IoDrawStuff_dsSetDrawMode(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    int mode;
    mode = IoMessage_locals_intArgAt_(m, locals, 0);
    dsSetDrawMode(mode);
    return self;
}

IoObject *IoDrawStuff_eventTarget(IoDrawStuff *self, IoObject *locals, IoMessage *m)
{
    DATA(proto)->eventTarget = IOREF(IoMessage_locals_valueArgAt_(m, locals, 0));
    return self;
}        

    

