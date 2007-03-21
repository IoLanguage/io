/*
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IOGLUSPHERE_DEFINED
#define IOGLUSPHERE_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#include "IoGLU.h"
#include "IoGLUQuadric.h"

#define ISGLUSPHERE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoGLUSphere_rawClone)

typedef IoObject IoGLUSphere;

typedef struct
{
  GLUquadricObj *q;
  IoImage *texture;

  double radius;
  double slices;
  double stacks;
} IoGLUSphereData;


IoGLUSphere *IoGLUSphere_rawClone(IoGLUSphere *self);
IoGLUSphere *IoGLUSphere_proto(void *state);
IoGLUSphere *IoGLUSphere_new(void *state);

void IoGLUSphere_free(IoGLUSphere *self);
void IoGLUSphere_mark(IoGLUSphere *self);

/* --- dimensions --- */
IoObject *IoGLUSphere_setRadius(IoGLUSphere *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUSphere_radius(IoGLUSphere *self, IoObject *locals, IoMessage *m);

/* --- model --- */
IoObject *IoGLUSphere_setSlices(IoGLUSphere *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUSphere_slices(IoGLUSphere *self, IoObject *locals, IoMessage *m);

IoObject *IoGLUSphere_setStacks(IoGLUSphere *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUSphere_stacks(IoGLUSphere *self, IoObject *locals, IoMessage *m);

/* --- draw --- */
IoObject *IoGLUSphere_draw(IoGLUSphere *self, IoObject *locals, IoMessage *m);

#endif
