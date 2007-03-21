/*
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef IOGLUQUADRIC_DEFINED
#define IOGLUQUADRIC_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"
#include "IoImage.h"

#include "IoGLU.h"

#define ISGLUQUADRIC(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoGLUQuadric_rawClone)

typedef IoObject IoGLUQuadric;

typedef struct
{
  GLUquadricObj *q;
  IoImage *texture;
  /* subclasses add extra members */
} IoGLUQuadricData;

IoObject *IoMessage_locals_gluQuadricArgAt_(IoMessage *self, IoObject *locals, int n);

IoGLUQuadric *IoGLUQuadric_rawClone(IoGLUQuadric *self);
IoGLUQuadric *IoGLUQuadric_proto(void *state);
IoGLUQuadric *IoGLUQuadric_new(void *state);

void IoGLUQuadric_free(IoGLUQuadric *self);
void IoGLUQuadric_mark(IoGLUQuadric *self);
GLUquadricObj *IoGLUQuadric_quadric(IoGLUQuadric *self);

/* ----------------------------------------------------------- */
IoObject *IoGLUQuadric_clone(IoGLUQuadric *self, IoObject *locals, IoMessage *m);

/* --- texture --- */
IoObject *IoGLUQuadric_setTexture(IoGLUQuadric *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUQuadric_texture(IoGLUQuadric *self, IoObject *locals, IoMessage *m);

/* --- draw style --- */
IoObject *IoGLUQuadric_useFillStyle(IoGLUQuadric *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUQuadric_useLineStyle(IoGLUQuadric *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUQuadric_useSilhouetteStyle(IoGLUQuadric *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUQuadric_usePointStyle(IoGLUQuadric *self, IoObject *locals, IoMessage *m);

/* --- normals --- */
IoObject *IoGLUQuadric_useNoNormals(IoGLUQuadric *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUQuadric_useFlatNormals(IoGLUQuadric *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUQuadric_useSmoothNormals(IoGLUQuadric *self, IoObject *locals, IoMessage *m);

/* --- orientation --- */
IoObject *IoGLUQuadric_useOutsideOrientation(IoGLUQuadric *self, IoObject *locals, IoMessage *m);
IoObject *IoGLUQuadric_useInsideOrientation(IoGLUQuadric *self, IoObject *locals, IoMessage *m);

/* --- draw --- */
IoObject *IoGLUQuadric_draw(IoGLUQuadric *self, IoObject *locals, IoMessage *m);

#endif
