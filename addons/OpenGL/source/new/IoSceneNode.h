/*
 *  IoSceneNode.h 
 *  All rights reserved. See _BSDLicense.txt.
 */

#ifndef IoSceneNode_DEFINED
#define IoSceneNode_DEFINED

#include "IoObject.h"
#include "IoList.h"

#define AXIS_Y    1
#define LESS_THAN 0

typedef struct {
  int  var;
  int  test;
  int  val;
  IoMessage *message;
} IoSceneEvent;

typedef IoObject IoSceneNode;

typedef struct {
  IoSeq *position;
  IoSeq *velocity;
  IoList *subnodes;
  IoSceneEvent *event;
} IoSceneNodeData;

/*
void  IoSceneGraph_glInit(IoState *state, IoObject *context);
void  IoSceneActor_glInit(IoState *state, IoObject *context);
*/

IoObject *IoSceneNode_rawClone(IoSceneNode *self);
IoSceneNode *IoSceneNode_proto(void *state);
IoSceneNode *IoSceneNode_new(void *state);
void IoSceneNode_free(IoSceneNode *self);
void IoSceneNode_mark(IoSceneNode *self);
void IoSceneNode_protoInit(IoSceneNode *self);

IoObject *IoSceneNode_advance(IoSceneNode *self, IoObject *locals, IoMessage *m);
IoObject *IoSceneNode_display(IoSceneNode *self, IoObject *locals, IoMessage *m);
IoObject *IoSceneNode_setPosition(IoSceneNode *self, IoObject *locals, IoMessage *m);
IoObject *IoSceneNode_setVelocity(IoSceneNode *self, IoObject *locals, IoMessage *m);
IoObject *IoSceneNode_position(IoSceneNode *self, IoObject *locals, IoMessage *m);
IoObject *IoSceneNode_velocity(IoSceneNode *self, IoObject *locals, IoMessage *m);
IoObject *IoSceneNode_addChildNode(IoSceneNode *self, IoObject *locals, IoMessage *m);
IoObject *IoSceneNode_addCallback(IoSceneNode *self, IoObject *locals, IoMessage *m);

#endif

