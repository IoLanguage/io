/*
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IOGLUCYLINDER_DEFINED
#define IOGLUCYLINDER_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#include "IoGLU.h"
#include "IoGLUQuadric.h"

#define ISGLUCYLINDER(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoGLUCylinder_rawClone)

typedef IoObject IoGLUCylinder;

typedef struct
{
  GLUquadricObj *q;
  IoImage *texture;

  double base;
  double top;
  double height;
  double slices;
  double stacks;
} IoGLUCylinderData;

IoGLUCylinder *IoGLUCylinder_rawClone(IoGLUCylinder *self);
IoGLUCylinder *IoGLUCylinder_proto(void *state);
IoGLUCylinder *IoGLUCylinder_new(void *state);

void IoGLUCylinder_free(IoGLUCylinder *self);
void IoGLUCylinder_mark(IoGLUCylinder *self);

/* --- dimensions --- */
IoObject *IoGLUCylinder_setBaseWidth(IoGLUCylinder *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUCylinder_baseWidth(IoGLUCylinder *self, IoObject *locals, IoMessage *m);

IoObject *IoGLUCylinder_setTopWidth(IoGLUCylinder *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUCylinder_topWidth(IoGLUCylinder *self, IoObject *locals, IoMessage *m);

IoObject *IoGLUCylinder_setHeight(IoGLUCylinder *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUCylinder_height(IoGLUCylinder *self, IoObject *locals, IoMessage *m);

IoObject *IoGLUCylinder_setSlices(IoGLUCylinder *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUCylinder_slices(IoGLUCylinder *self, IoObject *locals, IoMessage *m);

IoObject *IoGLUCylinder_setStacks(IoGLUCylinder *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUCylinder_stacks(IoGLUCylinder *self, IoObject *locals, IoMessage *m);

/* --- draw --- */
IoObject *IoGLUCylinder_draw(IoGLUCylinder *self, IoObject *locals, IoMessage *m);

#endif
