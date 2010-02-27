#ifndef IoClutterShader_DEFINED
#define IoClutterShader_DEFINED 1

#define ISCLUTTERSHADER(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoClutterShader_rawClone)
#define IOCSHADER(self) ((ClutterShader*)IoObject_dataPointer(self))

#include "common.h"

typedef IoObject IoClutterShader;

// Initialisation functions
IoTag            *IoClutterShader_newTag        (void *state);
IoClutterShader  *IoClutterShader_proto         (void *state);
IoClutterShader  *IoClutterShader_rawClone      (IoClutterShader *proto);
//IoClutterShader  *IoClutterShader_new           (void *state);
IoClutterShader  *IoClutterShader_newWithShader (void *state, ClutterShader *shader);
void              IoClutterShader_free          (IoClutterShader *self);

IoObject *IoMessage_locals_clutterShaderArgAt_(IoMessage *self, IoObject *locals, int n);

IO_METHOD(IoClutterShader, getVertexSource);
IO_METHOD(IoClutterShader, setVertexSource);
IO_METHOD(IoClutterShader, getFragmentSource);
IO_METHOD(IoClutterShader, setFragmentSource);
IO_METHOD(IoClutterShader, compile);
IO_METHOD(IoClutterShader, release);
IO_METHOD(IoClutterShader, isCompiled);
IO_METHOD(IoClutterShader, getIsEnabled);
IO_METHOD(IoClutterShader, setIsEnabled);

#endif

