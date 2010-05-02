#include "IoClutterShader.h"
//metadoc ClutterShader category UserInterface
//metadoc ClutterShader description For more detailed docs see <a href="http://clutter-project.org/docs/clutter/stable/clutter-Shaders.html">Clutter documentation</a>.

/*** Initalisation functions ***/
IoTag *IoClutterShader_newTag(void *state) {
  IoTag *tag = IoTag_newWithName_("ClutterShader");

  IoTag_state_(tag, state);
  IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoClutterShader_free);
  IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoClutterShader_rawClone);

  return tag;
}

IoClutterShader *IoClutterShader_proto(void *state) {
  IoObject *self = IoObject_new(state);
  IoObject_tag_(self, IoClutterShader_newTag(state));

  IoState_registerProtoWithFunc_(state, self, IoClutterShader_proto);

  {
    IoMethodTable methodTable[] = {
      {"vertexSource",      IoClutterShader_getVertexSource},
      {"setVertexSource",   IoClutterShader_setVertexSource},
      {"fragmentSource",    IoClutterShader_getFragmentSource},
      {"setFragmentSource", IoClutterShader_setFragmentSource},

      {"compile",           IoClutterShader_compile},
      {"release",           IoClutterShader_release},
      {"isCompiled",        IoClutterShader_isCompiled},
      {"isEnabled",         IoClutterShader_getIsEnabled},
      {"setIsEnabled",      IoClutterShader_setIsEnabled},

      {NULL, NULL}
    };
    IoObject_addMethodTable_(self, methodTable);
  }

  return self;
}

IoClutterShader *IoClutterShader_rawClone(IoClutterShader *proto) {
  IoClutterShader *self = IoObject_rawClonePrimitive(proto);
  ClutterShader *shader = clutter_shader_new();
  IoObject_setDataPointer_(self, shader);

  return self;
}

IoClutterShader *IoClutterShader_new(void *state) {
  IoObject *proto = IoState_protoWithInitFunction_((IoState *)state, IoClutterShader_proto);
  return IOCLONE(proto);
}

IoClutterShader *IoClutterShader_newWithShader(void *state, ClutterShader *shader) {
  IoClutterShader *self = IoClutterShader_new(state);
  // Release old shader which was created by IoClutterShader_rawClone
  clutter_shader_release(IOCSHADER(self));
  IoObject_setDataPointer_(self, shader);
  return self;
}

void IoClutterShader_free(IoClutterShader *self) {
  if(IOCSHADER(self) != NULL)
    clutter_shader_release(IOCSHADER(self));
}

IoObject *IoMessage_locals_clutterShaderArgAt_(IoMessage *self, IoObject *locals, int n) {
  IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);

  if(!ISCLUTTERSHADER(v)) {
    IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "ClutterShader");
  }

  return v;
}

//doc ClutterShader vertexSource
IO_METHOD(IoClutterShader, getVertexSource) {
  if(IOCSHADER(self) == NULL) return IONIL(self);
  char *vertex = clutter_shader_get_vertex_source(IOCSHADER(self));
  return (strlen(vertex) == 0) ? IONIL(self) : IOSYMBOL(vertex);
}

//doc ClutterShader setVertexSource(source)
IO_METHOD(IoClutterShader, setVertexSource) {
  char *source = IoMessage_locals_cStringArgAt_(m, locals, 0);
  clutter_shader_set_vertex_source(IOCSHADER(self), source, -1);
  return self;
}

//doc ClutterShader fragmentSource
IO_METHOD(IoClutterShader, getFragmentSource) {
  if(IOCSHADER(self) == NULL) return IONIL(self);
  char *fragment = clutter_shader_get_fragment_source(IOCSHADER(self));
  return (strlen(fragment) == 0) ? IONIL(self) : IOSYMBOL(fragment);
}

//doc ClutterShader setFragmentSource(source)
IO_METHOD(IoClutterShader, setFragmentSource) {
  char *source = IoMessage_locals_cStringArgAt_(m, locals, 0);
  clutter_shader_set_fragment_source(IOCSHADER(self), source, -1);
  return self;
}

//doc ClutterShader compile
IO_METHOD(IoClutterShader, compile) {
  GError *error = NULL;
  int success = clutter_shader_compile(IOCSHADER(self), &error);
  if(success == 0) {
    IoState_error_(IOSTATE, m, error->message);
    g_error_free(error);
  }

  return IOBOOL(self, success);
}

//doc ClutterShader release
IO_METHOD(IoClutterShader, release) {
  clutter_shader_release(IOCSHADER(self));
  return self;
}

//doc ClutterShader isCompiled
IO_METHOD(IoClutterShader, isCompiled) {
  return IOBOOL(self, clutter_shader_is_compiled(IOCSHADER(self)));
}

//doc ClutterShader isEnabled
IO_METHOD(IoClutterShader, getIsEnabled) {
  return IOBOOL(self, clutter_shader_get_is_enabled(IOCSHADER(self)));
}

//doc ClutterShader setIsEnabled(state)
IO_METHOD(IoClutterShader, setIsEnabled) {
  int is_enabled = IoMessage_locals_boolArgAt_(m, locals, 0);
  clutter_shader_set_is_enabled(IOCSHADER(self), is_enabled);
  return self;
}

//doc Clutter setSharedInt(firstInt, ...)
/*IO_METHOD(IoClutterShader, setShaderInt) {
  GValue shared_value = {};
  int size = IoMessage_argCount(m),
      n = 0;
  int *values;

  if(size == 0)
    return IONIL(self);
  else
    values = malloc(size * sizeof(int));

  for( ; n < size; n++) {
    values[n] = (int)(CNUMBER(IoMessage_locals_numberArgAt_(m, locals, n)));
  }

  g_value_init(&shared_value, CLUTTER_TYPE_SHADER_INT);
  clutter_value_set_shader_int(&shared_value, size, values);

  return self;
}*/


