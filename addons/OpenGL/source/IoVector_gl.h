
//metadoc Vector copyright Steve Dekorte 2002
//metadoc Vector license BSD revised


#ifndef IOPOINT_GL_DEFINED
#define IOPOINT_GL_DEFINED 1

#include "IoState.h"
#include "IoSeq.h"

void  IoVector_glInit(IoObject *context);

IoObject *IoSeq_glNormal(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_glTranslate(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_glTranslatei(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_glRotate(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_glScale(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_glUnproject(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_glProject(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_drawLineTo(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_drawLineLoopTo(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_drawQuadTo(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_drawLineLoop(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_drawLineLoopi(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_drawQuad(IoSeq *self, IoObject *locals, IoMessage *m);

IoObject *IoSeq_glVertex(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_glColor(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_glClearColor(IoSeq *self, IoObject *locals, IoMessage *m);

/* --- vector stuff --- */

IoObject *IoSeq_drawAsLine(IoSeq *self, IoObject *locals, IoMessage *m);
IoObject *IoSeq_drawFilled(IoSeq *self, IoObject *locals, IoMessage *m);

#endif
