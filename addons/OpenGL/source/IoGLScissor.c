/*#io
GLScissor ioDoc(
			 docCopyright("Steve Dekorte", 2002)
			 docLicense("BSD revised")
			 docCategory("Graphics")
*/

#include "IoGLScissor.h"
#include "IoState.h"
#include "IoBox_gl.h"

#define DATA(self) ((IoGLScissorData *)IoObject_dataPointer(self))

IoTag *IoGLScissor_newTag(void *state)
{
    IoTag *tag = IoTag_newWithName_("GLScissor");
    IoTag_state_(tag, state);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoGLScissor_mark);
    return tag;
}

IoGLScissor *IoGLScissor_proto(void *state)
{
    IoObject *self = IoObject_new(state);
    IoObject_tag_(self, IoGLScissor_newTag(state));
    
    IoObject_setDataPointer_(self, calloc(1, sizeof(IoGLScissorData)));
    
    DATA(self)->rect    = IoBox_new(state);
    DATA(self)->tmpRect = IoBox_new(state);
    
    IoState_registerProtoWithFunc_(state, self, IoGLScissor_proto);
    
    {
	IoMethodTable methodTable[] = {
	{"sync", IoGLScissor_sync},
	{"set", IoGLScissor_set},
	{"on", IoGLScissor_on},
	{"off", IoGLScissor_off},
	{"isOn", IoGLScissor_isOn},
	{"push", IoGLScissor_push},
	{"pop", IoGLScissor_pop},
	{"isVisible", IoGLScissor_isVisible},
	    
	{"rect", IoGLScissor_rect},
	{"setRect", IoGLScissor_setScreenRect},
	{"setViewRect", IoGLScissor_setViewRect},
	{"unionWithViewRect", IoGLScissor_unionWithViewRect},
	{"unionWithScreenRect", IoGLScissor_unionWithScreenRect},
	{NULL, NULL},
	};
	IoObject_addMethodTable_(self, methodTable);
    }
    return self;
}

/* ----------------------------------------------------------- */

IoGLScissor *IoGLScissor_rawClone(IoGLScissor *proto) 
{ 
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoGLScissorData)));
    
    DATA(self)->rect    = IOCLONE(DATA(proto)->rect); 
    DATA(self)->tmpRect = IOCLONE(DATA(proto)->tmpRect); 
    return self; 
}

IoGLScissor *IoGLScissor_new(void *state)
{
    IoGLScissor *proto = IoState_protoWithInitFunction_(state, IoGLScissor_proto);
    return IOCLONE(proto);
}

void IoGLScissor_mark(IoGLScissor *self) 
{ 
    IoObject_shouldMark(DATA(self)->rect); 
    IoObject_shouldMark(DATA(self)->tmpRect); 
}

/* ----------------------------------------------------------- */

IoObject *IoGLScissor_rect(IoGLScissor *self, IoObject *locals, IoMessage *m)
{ return DATA(self)->rect; }

IoObject *IoGLScissor_setScreenRect(IoGLScissor *self, IoObject *locals, IoMessage *m)
{ 
    IoBox *box = IoMessage_locals_boxArgAt_(m, locals, 0);
    IoBox_rawCopy(DATA(self)->rect, box);
    IoGLScissor_set(self, locals, m);
    return self;
}

IoObject *IoGLScissor_setViewRect(IoGLScissor *self, IoObject *locals, IoMessage *m)
{ 
    IoBox *box = IoMessage_locals_boxArgAt_(m, locals, 0);
    IoBox_rawCopy(DATA(self)->rect, box);
    IoBox_glProject(DATA(self)->rect, locals, m);
    IoGLScissor_set(self, locals, m);
    return self;
}

void IoGLScissor_rawSync(IoGLScissor *self)
{ 
    double v[4];
    glGetDoublev(GL_SCISSOR_BOX, v);
    IoBox_rawSet(DATA(self)->rect, v[0], v[1], 0, v[2], v[3], 0);
}

IoObject *IoGLScissor_sync(IoGLScissor *self, IoObject *locals, IoMessage *m)
{
    IoGLScissor_rawSync(self);
    return self;
}

IoObject *IoGLScissor_set(IoGLScissor *self, IoObject *locals, IoMessage *m)
{
	vec2f o = IoSeq_vec2f(IoBox_rawOrigin(DATA(self)->rect));
	vec2f s = IoSeq_vec2f(IoBox_rawSize(DATA(self)->rect));
	glScissor((GLint)o.x, (GLint)o.y, (GLsizei)s.x, (GLsizei)s.y);
	return self;
}

IoObject *IoGLScissor_on(IoGLScissor *self, IoObject *locals, IoMessage *m)
{
    glEnable(GL_SCISSOR_TEST);
    return self;
}

IoObject *IoGLScissor_off(IoGLScissor *self, IoObject *locals, IoMessage *m)
{
    glDisable(GL_SCISSOR_TEST);
    return self;
}

IoObject *IoGLScissor_isOn(IoGLScissor *self, IoObject *locals, IoMessage *m)
{
    GLboolean b;
    glGetBooleanv(GL_SCISSOR_TEST, &b);
    return IOBOOL(self, b == GL_TRUE);
}

IoObject *IoGLScissor_push(IoGLScissor *self, IoObject *locals, IoMessage *m)
{
    glPushAttrib(GL_SCISSOR_BIT);
    return self;
}

IoObject *IoGLScissor_pop(IoGLScissor *self, IoObject *locals, IoMessage *m)
{
    glPopAttrib();
    IoGLScissor_sync(self, locals, m);
    return self;
}

IoObject *IoGLScissor_unionWithViewRect(IoGLScissor *self, IoObject *locals, IoMessage *m)
{
    IoBox *box = IoMessage_locals_boxArgAt_(m, locals, 0);
    IoBox_rawCopy(DATA(self)->tmpRect, box);
    IoBox_glProject(DATA(self)->tmpRect, locals, m);
    IoBox_rawUnion(DATA(self)->rect, DATA(self)->tmpRect);
    IoGLScissor_set(self, locals, m);
    return self;
}

IoObject *IoGLScissor_unionWithScreenRect(IoGLScissor *self, IoObject *locals, IoMessage *m)
{
    IoBox *box = IoMessage_locals_boxArgAt_(m, locals, 0);
    IoBox_rawUnion(DATA(self)->rect, box);
    IoGLScissor_set(self, locals, m);
    return self;
}

IoObject *IoGLScissor_isVisible(IoGLScissor *self, IoObject *locals, IoMessage *m)
{ 
	return IOBOOL(self, UArray_isZero(IoSeq_rawUArray(IoBox_rawSize(DATA(self)->rect)))); 
}



