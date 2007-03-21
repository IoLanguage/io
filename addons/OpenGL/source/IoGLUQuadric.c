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
    
    {
	IoMethodTable methodTable[] = {
	    
	{"setTexture", IoGLUQuadric_setTexture},
	{"texture", IoGLUQuadric_texture},
	    
	{"useFillStyle", IoGLUQuadric_useFillStyle},
	{"useLineStyle", IoGLUQuadric_useLineStyle},
	{"usePointStyle", IoGLUQuadric_usePointStyle},
	{"useSilhouetteStyle", IoGLUQuadric_useSilhouetteStyle},
	    
	{"useNoNormals", IoGLUQuadric_useNoNormals},
	{"useFlatNormals", IoGLUQuadric_useFlatNormals},
	{"useSmoothNormals", IoGLUQuadric_useSmoothNormals},
	    
	{"useOutsideOrientation", IoGLUQuadric_useOutsideOrientation},
	{"useInsideOrientation", IoGLUQuadric_useInsideOrientation},
	{NULL, NULL},
	};
	IoObject_addMethodTable_(self, methodTable);
    }
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

/* ----------------------------------------------------------- */

/* --- texture --- */

IoObject *IoGLUQuadric_setTexture(IoGLUQuadric *self, IoObject *locals, IoMessage *m)
{ 
    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0); 
    if (ISIMAGE(v)) 
    { 
	DATA(self)->texture = (IoImage *)v;
	gluQuadricTexture(IoGLUQuadric_quadric(self), GL_TRUE);
    } 
    else 
    { 
	DATA(self)->texture = NULL;
	gluQuadricTexture(IoGLUQuadric_quadric(self), GL_FALSE);
    }
    return self;
}

IoObject *IoGLUQuadric_texture(IoGLUQuadric *self, IoObject *locals, IoMessage *m)
{ 
	return DATA(self)->texture ? (IoObject *)DATA(self)->texture : IONIL(self); 
}

/* --- draw style --- */

IoObject *IoGLUQuadric_useFillStyle(IoGLUQuadric *self, IoObject *locals, IoMessage *m)
{ gluQuadricDrawStyle(IoGLUQuadric_quadric(self), GLU_FILL); return self; }

IoObject *IoGLUQuadric_useLineStyle(IoGLUQuadric *self, IoObject *locals, IoMessage *m)
{ gluQuadricDrawStyle(IoGLUQuadric_quadric(self), GLU_LINE); return self; }

IoObject *IoGLUQuadric_useSilhouetteStyle(IoGLUQuadric *self, IoObject *locals, IoMessage *m)
{ gluQuadricDrawStyle(IoGLUQuadric_quadric(self), GLU_SILHOUETTE); return self; }

IoObject *IoGLUQuadric_usePointStyle(IoGLUQuadric *self, IoObject *locals, IoMessage *m)
{ gluQuadricDrawStyle(IoGLUQuadric_quadric(self), GLU_POINT); return self; }

/* --- normals --- */

IoObject *IoGLUQuadric_useNoNormals(IoGLUQuadric *self, IoObject *locals, IoMessage *m)
{ gluQuadricNormals(IoGLUQuadric_quadric(self), GLU_NONE); return self; }

IoObject *IoGLUQuadric_useFlatNormals(IoGLUQuadric *self, IoObject *locals, IoMessage *m)
{ gluQuadricNormals(IoGLUQuadric_quadric(self), GLU_FLAT); return self; }

IoObject *IoGLUQuadric_useSmoothNormals(IoGLUQuadric *self, IoObject *locals, IoMessage *m)
{ gluQuadricNormals(IoGLUQuadric_quadric(self), GLU_SMOOTH); return self; }

/* --- orientation --- */

IoObject *IoGLUQuadric_useOutsideOrientation(IoGLUQuadric *self, IoObject *locals, IoMessage *m)
{ gluQuadricOrientation(IoGLUQuadric_quadric(self), GLU_OUTSIDE); return self; }

IoObject *IoGLUQuadric_useInsideOrientation(IoGLUQuadric *self, IoObject *locals, IoMessage *m)
{ gluQuadricOrientation(IoGLUQuadric_quadric(self), GLU_INSIDE); return self; }

/* --- draw --- */

IoObject *IoGLUQuadric_draw(IoGLUQuadric *self, IoObject *locals, IoMessage *m)
{
    if (DATA(self)->texture) IoImage_bindTexture(DATA(self)->texture, locals, m);
    /* subclasses implement their own drawing */
    return self;
}

