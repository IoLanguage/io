/*#io
GLUSphere ioDoc(
			 docCopyright("Steve Dekorte", 2002)
			 docLicense("BSD revised")
			 docCategory("Graphics")
*/

#include "IoGLUSphere.h"
#include "List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoFile.h"
#include "IoOpenGL.h"

#define DATA(self) ((IoGLUSphereData *)IoObject_dataPointer(self))

IoTag *IoGLUSphere_newTag(void *state)
{
    IoTag *tag = IoTag_newWithName_("GLUSphere");
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoGLUSphere_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoGLUSphere_free);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoGLUSphere_mark);
    return tag;
}

IoGLUSphere *IoGLUSphere_proto(void *state)
{
    IoObject *self = IoObject_new(state);
    IoObject_tag_(self, IoGLUSphere_newTag(state));
    
    IoObject_setDataPointer_(self, calloc(1, sizeof(IoGLUSphereData)));
    
    IoState_registerProtoWithFunc_(state, self, IoGLUSphere_proto);
    
    {
	IoMethodTable methodTable[] = {
	    /* -- from parent --- */
	{"setTexture", IoGLUQuadric_setTexture},
	{"texture", IoGLUQuadric_texture},
	    
	{"useFillStyle", IoGLUQuadric_useFillStyle},
	{"useLineStyle", IoGLUQuadric_useLineStyle},
	{"useSilhouetteStyle", IoGLUQuadric_useSilhouetteStyle},
	    
	{"useNoNormals", IoGLUQuadric_useNoNormals},
	{"useFlatNormals", IoGLUQuadric_useFlatNormals},
	{"useSmoothNormals", IoGLUQuadric_useSmoothNormals},
	    
	{"useOutsideOrientation", IoGLUQuadric_useOutsideOrientation},
	{"useInsideOrientation", IoGLUQuadric_useInsideOrientation},
	    
	    /* --- */
	{"setRadius", IoGLUSphere_setRadius},
	{"radius", IoGLUSphere_radius},
	    
	{"setStacks", IoGLUSphere_setStacks},
	{"stacks", IoGLUSphere_stacks},
	    
	{"setSlices", IoGLUSphere_setSlices},
	{"slices", IoGLUSphere_slices},
	    
	{"draw", IoGLUSphere_draw},
	{NULL, NULL},
	};
	IoObject_addMethodTable_(self, methodTable);
    }
    return self;
}

/* ----------------------------------------------------------- */

IoGLUSphere *IoGLUSphere_rawClone(IoGLUSphere *proto) 
{ 
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoGLUSphereData)));
    return self; 
}

IoGLUSphere *IoGLUSphere_new(void *state)
{
    IoGLUSphere *proto = IoState_protoWithInitFunction_(state, IoGLUSphere_proto);
    return IOCLONE(proto);
}

void IoGLUSphere_free(IoGLUSphere *self) 
{ 
    if (DATA(self)->q) gluDeleteQuadric(DATA(self)->q);
    free(IoObject_dataPointer(self)); 
}

void IoGLUSphere_mark(IoGLUSphere *self) 
{ if (DATA(self)->texture) IoObject_shouldMark((IoObject *)(DATA(self)->texture)); }

/* ----------------------------------------------------------- */

/* --- dimensions --- */
IoObject *IoGLUSphere_setRadius(IoGLUSphere *self, IoObject *locals, IoMessage *m)
{ 
    DATA(self)->radius = IoNumber_asDouble(IoMessage_locals_numberArgAt_(m, locals, 0)); 
    return self;
}
IoObject *IoGLUSphere_radius(IoGLUSphere *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(DATA(self)->radius); }

/* --- model --- */
IoObject *IoGLUSphere_setSlices(IoGLUSphere *self, IoObject *locals, IoMessage *m)
{ 
    DATA(self)->slices = IoNumber_asDouble(IoMessage_locals_numberArgAt_(m, locals, 0)); 
    return self;
}
IoObject *IoGLUSphere_slices(IoGLUSphere *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(DATA(self)->slices); }

IoObject *IoGLUSphere_setStacks(IoGLUSphere *self, IoObject *locals, IoMessage *m)
{ 
    DATA(self)->stacks = IoNumber_asDouble(IoMessage_locals_numberArgAt_(m, locals, 0)); 
    return self;
}
IoObject *IoGLUSphere_stacks(IoGLUSphere *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(DATA(self)->stacks); }

/* --- draw --- */

IoObject *IoGLUSphere_draw(IoGLUSphere *self, IoObject *locals, IoMessage *m)
{
    if (DATA(self)->texture) IoImage_bindTexture(DATA(self)->texture, locals, m);
    gluSphere(IoGLUQuadric_quadric((IoGLUQuadric *)self), DATA(self)->radius, DATA(self)->slices, DATA(self)->stacks);
    return self;
}

