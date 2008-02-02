//metadoc GLU copyright Steve Dekorte 2002
//metadoc GLU license BSD revised

#ifndef IOGLU_DEFINED
#define IOGLU_DEFINED 1

#include "IoObject.h"
#include "IoSeq.h"

#define ISGLU(v) (IoObject_tag(v)->cloneFunc == (IoTagCloneFunc *)IoGLU_rawClone)

#include "GLIncludes.h"

typedef IoObject IoGLU;

// WTF? An empty structure is a syntax error
/*
typedef struct
{
} IoGLUData;
*/
IoObject *IoGLU_rawClone(IoGLU *self);
IoGLU *IoGLU_proto(void *state);
IoGLU *IoGLU_new(void *state);
void IoGLU_free(IoGLU *self);
void IoGLU_mark(IoGLU *self);
void IoGLU_protoInit(IoGLU *self);

/* --- Quadrics ----------------------------------------------------------------*/

IoObject *IoGLU_gluNewQuadric(IoGLU *self, IoObject *locals, IoMessage *m);
IoObject *IoGLU_gluDeleteQuadric(IoGLU *self, IoObject *locals, IoMessage *m);
IoObject *IoGLU_gluDisk(IoGLU *self, IoObject *locals, IoMessage *m);
IoObject *IoGLU_gluPartialDisk(IoGLU *self, IoObject *locals, IoMessage *m);
IoObject *IoGLU_gluCylinder(IoGLU *self, IoObject *locals, IoMessage *m);
IoObject *IoGLU_gluQuadricDrawStyle(IoGLU *self, IoObject *locals, IoMessage *m);
IoObject *IoGLU_gluQuadricNormals(IoGLU *self, IoObject *locals, IoMessage *m);
IoObject *IoGLU_gluQuadricOrientation(IoGLU *self, IoObject *locals, IoMessage *m);
IoObject *IoGLU_gluQuadricTexture(IoGLU *self, IoObject *locals, IoMessage *m);
IoObject *IoGLU_gluSphere(IoGLU *self, IoObject *locals, IoMessage *m);
IoObject *IoGLU_gluRoundedBox(IoGLU *self, IoObject *locals, IoMessage *m);
IoObject *IoGLU_gluRoundedBoxOutline(IoGLU *self, IoObject *locals, IoMessage *m);

#endif
