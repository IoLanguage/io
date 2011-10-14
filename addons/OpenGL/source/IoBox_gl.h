
//metadoc Box copyright Steve Dekorte 2002
//metadoc Box license BSD revised

#ifndef IoBox_GL_DEFINED
#define IoBox_GL_DEFINED 1

#include "IoState.h"
#include "IoBox.h"

void IoBox_glInit(IoObject *context);

IoObject *IoBox_drawAsRect(IoBox *self, IoObject *locals, IoMessage *m);
/*
IoObject *IoBox_scissor(IoBox *self, IoObject *locals, IoMessage *m);
IoObject *IoBox_scissorToUnion(IoBox *self, IoObject *locals, IoMessage *m);
*/
IoObject *IoBox_getScissor(IoBox *self, IoObject *locals, IoMessage *m);
IoObject *IoBox_glProject(IoBox *self, IoObject *locals, IoMessage *m);
IoObject *IoBox_glUnproject(IoBox *self, IoObject *locals, IoMessage *m);

#endif
