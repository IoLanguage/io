/*#io
GLUQuadric ioDoc(
			  docCopyright("Steve Dekorte", 2002)
			  docLicense("BSD revised")
			  docCategory("Graphics")
*/

#include "IoGLUQuadric.h"
#include "List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoFile.h"

#define DATA(self) ((IoGLUQuadricData *)IoObject_dataPointer(self))

IoObject *IoMessage_locals_gluQuadricArgAt_(IoMessage *self, IoObject *locals, int n)
{
    IoObject *v = IoMessage_locals_valueArgAt_(self, locals, n);
    if (!ISGLUQUADRIC(v)) IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "GLUQuadric");
    return v;
}

IoTag *IoGLUQuadric_newTag(void *state)
{
    IoTag *tag = IoTag_newWithName_("GLUQuadric");
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoGLUQuadric_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoGLUQuadric_free);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoGLUQuadric_mark);
    return tag;
}

IoGLUQuadric *IoGLUQuadric_proto(void *state)
{
    IoObject *self = IoObject_new(state);
    IoObject_tag_(self, IoGLUQuadric_newTag(state));
    
    IoObject_setDataPointer_(self, calloc(1, sizeof(IoGLUQuadricData)));
    
    IoState_registerProtoWithFunc_(state, self, IoGLUQuadric_proto);
    
    return self;
}

/* ----------------------------------------------------------- */

IoGLUQuadric *IoGLUQuadric_rawClone(IoGLUQuadric *proto)
{
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoGLUQuadricData)));
    return self;
}

IoGLUQuadric *IoGLUQuadric_new(void *state)
{
    IoGLUQuadric *proto = IoState_protoWithInitFunction_(state, IoGLUQuadric_proto);
    return IOCLONE(proto);
}

void IoGLUQuadric_free(IoGLUQuadric *self)
{
    if (DATA(self)->q) gluDeleteQuadric(DATA(self)->q);
    free(IoObject_dataPointer(self));
}

void IoGLUQuadric_mark(IoGLUQuadric *self) 
{ if (DATA(self)->texture) IoObject_shouldMark((IoObject *)(DATA(self)->texture)); }

GLUquadricObj *IoGLUQuadric_quadric(IoGLUQuadric *self) 
{ 
    if (!DATA(self)->q) 
    { DATA(self)->q = gluNewQuadric(); }
    return DATA(self)->q; 
}

