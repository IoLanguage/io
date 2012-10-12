#include "IoClutterVertex.h"
//metadoc ClutterVertex category UserInterface
//metadoc ClutterVertex description For more detailed docs see <a href="http://clutter-project.org/docs/clutter/stable/ClutterActor.html#ClutterVertex">Clutter documentation</a>.

static const char *protoId = "ClutterVertex";

/*** Initalisation functions ***/
IoTag *IoClutterVertex_newTag(void *state) {
  IoTag *tag = IoTag_newWithName_(protoId);

  IoTag_state_(tag, state);
  IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoClutterVertex_free);
  IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoClutterVertex_rawClone);

  return tag;
}

IoClutterVertex *IoClutterVertex_proto(void *state) {
  IoObject *self = IoObject_new(state);
  IoObject_tag_(self, IoClutterVertex_newTag(state));

  IoState_registerProtoWithFunc_(state, self, IoClutterVertex_proto);

  {
    IoMethodTable methodTable[] = {
      {"==",    IoClutterVertex_equals},
      {"!=",    IoClutterVertex_notEqual},
      {"with",  IoClutterVertex_with},

      {"x",     IoClutterVertex_getX},
      {"y",     IoClutterVertex_getY},
      {"z",     IoClutterVertex_getZ},
      {"setX",  IoClutterVertex_setX},
      {"setY",  IoClutterVertex_setY},
      {"setZ",  IoClutterVertex_setZ},

      {NULL, NULL}
    };
    IoObject_addMethodTable_(self, methodTable);
  }

  return self;
}

IoClutterVertex *IoClutterVertex_rawClone(IoClutterVertex *proto) {
  IoClutterVertex *self = IoObject_rawClonePrimitive(proto);
  return self;
}

IoClutterVertex *IoClutterVertex_new(void *state) {
  IoObject *proto = IoState_protoWithId_((IoState *)state, protoId);
  return IOCLONE(proto);
}

IoClutterVertex *IoClutterVertex_newWithVertex(void *state, ClutterVertex *clutterVertex) {
  IoClutterVertex *self = IoClutterVertex_new(state);
  IoObject_setDataPointer_(self, clutterVertex);

  return self;
}

void IoClutterVertex_free(IoClutterVertex *self) {
  if(IOCVERTEX(self) != NULL)
    clutter_vertex_free(IOCVERTEX(self));
}

IoObject *IoMessage_locals_clutterVertexArgAt_(IoMessage *self, IoObject *locals, int n) {
  IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

  if(!ISCLUTTERVERTEX(v)) {
    IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "ClutterVertex");
  }

  return v;
}

/*** Object functions ***/
//doc ClutterVertex ==(otherVertex)
IO_METHOD(IoClutterVertex, equals) {
  ClutterVertex *other = IOCVERTEX(IoMessage_locals_clutterVertexArgAt_(m, locals, 0));
  return IOBOOL(self, clutter_vertex_equal(IOCVERTEX(self), other));
}

//doc ClutterVertex !=(otherVertex)
IO_METHOD(IoClutterVertex, notEqual) {
  ClutterVertex *other = IOCVERTEX(IoMessage_locals_clutterVertexArgAt_(m, locals, 0));
  return IOBOOL(self, !clutter_vertex_equal(IOCVERTEX(self), other));
}

//doc ClutterVertex with(x, y, z)
IO_METHOD(IoClutterVertex, with) {
  float x = IoMessage_locals_floatArgAt_(m, locals, 0),
        y = IoMessage_locals_floatArgAt_(m, locals, 1),
        z = IoMessage_locals_floatArgAt_(m, locals, 2);
  ClutterVertex *vertex = clutter_vertex_new(x, y, z);

  return IoClutterVertex_newWithVertex(IOSTATE, vertex);
}

//doc ClutterVertex x
IO_METHOD(IoClutterVertex, getX) {
  if(IOCVERTEX(self) == NULL) return IONIL(self);
  return IONUMBER(IOCVERTEX(self)->x);
}

//doc ClutterVertex y
IO_METHOD(IoClutterVertex, getY) {
  if(IOCVERTEX(self) == NULL) return IONIL(self);
  return IONUMBER(IOCVERTEX(self)->y);
}

//doc ClutterVertex z
IO_METHOD(IoClutterVertex, getZ) {
  if(IOCVERTEX(self) == NULL) return IONIL(self);
  return IONUMBER(IOCVERTEX(self)->z);
}

//doc ClutterVertex setX(x)
IO_METHOD(IoClutterVertex, setX) {
  IOCVERTEX(self)->x = IoMessage_locals_floatArgAt_(m, locals, 0);
  return self;
}

//doc ClutterVertex setY(y)
IO_METHOD(IoClutterVertex, setY) {
  IOCVERTEX(self)->y = IoMessage_locals_floatArgAt_(m, locals, 0);
  return self;
}

//doc ClutterVertex setZ(z)
IO_METHOD(IoClutterVertex, setZ) {
  IOCVERTEX(self)->z = IoMessage_locals_floatArgAt_(m, locals, 0);
  return self;
}
