
//metadoc GLUQuadric copyright Steve Dekorte 2002
//metadoc GLUQuadric license BSD revised

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
} IoGLUQuadricData;

IoObject *IoMessage_locals_gluQuadricArgAt_(IoMessage *self, IoObject *locals, int n);

IoGLUQuadric *IoGLUQuadric_rawClone(IoGLUQuadric *self);
IoGLUQuadric *IoGLUQuadric_proto(void *state);
IoGLUQuadric *IoGLUQuadric_new(void *state);

void IoGLUQuadric_free(IoGLUQuadric *self);
void IoGLUQuadric_mark(IoGLUQuadric *self);
GLUquadricObj *IoGLUQuadric_quadric(IoGLUQuadric *self);

IoObject *IoGLUQuadric_clone(IoGLUQuadric *self, IoObject *locals, IoMessage *m);

#endif
