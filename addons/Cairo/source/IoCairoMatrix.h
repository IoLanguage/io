/*
docCopyright("Daniel Rosengren", 2007)
docLicense("BSD revised")
*/

#ifndef IOCAIROMATRIX_DEFINED
#define IOCAIROMATRIX_DEFINED 1

#include "IoObject.h"
#include <cairo.h>

#define ISCAIROMATRIX(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCairoMatrix_rawClone)
void *IoMessage_locals_cairoMatrixArgAt_(IoMessage *self, void *locals, int n);

typedef IoObject IoCairoMatrix;

IoCairoMatrix *IoCairoMatrix_proto(void *state);
IoCairoMatrix *IoCairoMatrix_rawClone(IoCairoMatrix *proto);
IoCairoMatrix *IoCairoMatrix_new(void *state);
IoCairoMatrix *IoCairoMatrix_newWithRawMatrix_(void *state, cairo_matrix_t *matrix);
void IoCairoMatrix_free(IoCairoMatrix *self);

cairo_matrix_t *IoCairoMatrix_rawMatrix(IoCairoMatrix *self);

/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoMatrix_identity(IoCairoMatrix *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoMatrix_translate(IoCairoMatrix *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoMatrix_scale(IoCairoMatrix *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoMatrix_rotate(IoCairoMatrix *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoMatrix_invert(IoCairoMatrix *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoMatrix_multiply(IoCairoMatrix *self, IoObject *locals, IoMessage *m);

IoObject *IoCairoMatrix_transformDistance(IoCairoMatrix *self, IoObject *locals, IoMessage *m);
IoObject *IoCairoMatrix_transformPoint(IoCairoMatrix *self, IoObject *locals, IoMessage *m);

#endif
