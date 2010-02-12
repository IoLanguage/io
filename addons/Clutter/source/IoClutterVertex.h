#ifndef IoClutterVertex_DEFINED
#define IoClutterVertex_DEFINED 1

#define ISCLUTTERVERTEX(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoClutterVertex_rawClone)
#define IOCVERTEX(self) ((ClutterVertex*)IoObject_dataPointer(self))

#include "common.h"

typedef IoObject IoClutterVertex;

// Initialisation functions
IoTag            *IoClutterVertex_newTag        (void *state);
IoClutterVertex  *IoClutterVertex_proto         (void *state);
IoClutterVertex  *IoClutterVertex_rawClone      (IoClutterVertex *proto);
IoClutterVertex  *IoClutterVertex_new           (void *state);
IoClutterVertex  *IoClutterVertex_newWithVertex (void *state, ClutterVertex *clutterVertex);
void            IoClutterVertex_free            (IoClutterVertex *self);

IoObject *IoMessage_locals_clutterVertexArgAt_(IoMessage *self, IoObject *locals, int n);

IO_METHOD(IoClutterVertex, equals);
IO_METHOD(IoClutterVertex, notEqual);
IO_METHOD(IoClutterVertex, with);
IO_METHOD(IoClutterVertex, getX);
IO_METHOD(IoClutterVertex, getY);
IO_METHOD(IoClutterVertex, getZ);
IO_METHOD(IoClutterVertex, setX);
IO_METHOD(IoClutterVertex, setY);
IO_METHOD(IoClutterVertex, setZ);

#endif
