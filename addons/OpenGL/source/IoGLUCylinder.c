/*#io
GLUCylinder ioDoc(
			   docCopyright("Steve Dekorte", 2002)
			   docLicense("BSD revised")
			   docCategory("Graphics")
*/

#include "IoGLUCylinder.h"
#include "List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoList.h"
#include "IoFile.h"
#include "IoOpenGL.h"

#define DATA(self) ((IoGLUCylinderData *)IoObject_dataPointer(self))

IoTag *IoGLUCylinder_newTag(void *state)
{
    IoTag *tag = IoTag_newWithName_("GLUCylinder");
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoGLUCylinder_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoGLUCylinder_free);
    IoTag_markFunc_(tag, (IoTagMarkFunc *)IoGLUCylinder_mark);
    return tag;
}

IoGLUCylinder *IoGLUCylinder_proto(void *state)
{
    IoObject *self = IoObject_new(state);
    IoObject_tag_(self, IoGLUCylinder_newTag(state));
    
    IoObject_setDataPointer_(self, calloc(1, sizeof(IoGLUCylinderData)));
    
    IoState_registerProtoWithFunc_(state, self, IoGLUCylinder_proto);
    
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
	{"setBaseWidth", IoGLUCylinder_setBaseWidth},
	{"baseWidth", IoGLUCylinder_baseWidth},
	    
	{"setTopWidth", IoGLUCylinder_setTopWidth},
	{"topWidth", IoGLUCylinder_topWidth},
	    
	{"setHeight", IoGLUCylinder_setHeight},
	{"height", IoGLUCylinder_height},
	    
	{"setStacks", IoGLUCylinder_setStacks},
	{"stacks", IoGLUCylinder_stacks},
	    
	{"setSlices", IoGLUCylinder_setSlices},
	{"slices", IoGLUCylinder_slices},
	    
	{"draw", IoGLUCylinder_draw},
	{NULL, NULL},
	};
	IoObject_addMethodTable_(self, methodTable);
    }
    return self;
}

/* ----------------------------------------------------------- */

IoGLUCylinder *IoGLUCylinder_rawClone(IoGLUCylinder *proto) 
{ 
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(IoGLUCylinderData)));
    /* bug, need to clone texture */
    return self; 
}

IoGLUCylinder *IoGLUCylinder_new(void *state)
{
    IoGLUCylinder *proto = IoState_protoWithInitFunction_(state, IoGLUCylinder_proto);
    return IOCLONE(proto);
}

void IoGLUCylinder_free(IoGLUCylinder *self) 
{ 
    if (DATA(self)->q) gluDeleteQuadric(DATA(self)->q);
    free(IoObject_dataPointer(self)); 
}

void IoGLUCylinder_mark(IoGLUCylinder *self) 
{ if (DATA(self)->texture) IoObject_shouldMark((IoObject *)(DATA(self)->texture)); }

/* ----------------------------------------------------------- */


/* --- dimensions --- */
IoObject *IoGLUCylinder_setBaseWidth(IoGLUCylinder *self, IoObject *locals, IoMessage *m)
{ 
    DATA(self)->base = IoNumber_asDouble(IoMessage_locals_numberArgAt_(m, locals, 0)); 
    return self;
}
IoObject *IoGLUCylinder_baseWidth(IoGLUCylinder *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(DATA(self)->base); }


IoObject *IoGLUCylinder_setTopWidth(IoGLUCylinder *self, IoObject *locals, IoMessage *m)
{ 
    DATA(self)->top = IoNumber_asDouble(IoMessage_locals_numberArgAt_(m, locals, 0)); 
    return self;
}
IoObject *IoGLUCylinder_topWidth(IoGLUCylinder *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(DATA(self)->top); }


IoObject *IoGLUCylinder_setHeight(IoGLUCylinder *self, IoObject *locals, IoMessage *m)
{ 
    DATA(self)->top = IoNumber_asDouble(IoMessage_locals_numberArgAt_(m, locals, 0)); 
    return self;
}
IoObject *IoGLUCylinder_height(IoGLUCylinder *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(DATA(self)->height); }


IoObject *IoGLUCylinder_setSlices(IoGLUCylinder *self, IoObject *locals, IoMessage *m)
{ 
    DATA(self)->slices = IoNumber_asDouble(IoMessage_locals_numberArgAt_(m, locals, 0)); 
    return self;
}
IoObject *IoGLUCylinder_slices(IoGLUCylinder *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(DATA(self)->slices); }

IoObject *IoGLUCylinder_setStacks(IoGLUCylinder *self, IoObject *locals, IoMessage *m)
{ 
    DATA(self)->stacks = IoNumber_asDouble(IoMessage_locals_numberArgAt_(m, locals, 0)); 
    return self;
}
IoObject *IoGLUCylinder_stacks(IoGLUCylinder *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(DATA(self)->stacks); }

/* --- draw --- */

IoObject *IoGLUCylinder_draw(IoGLUCylinder *self, IoObject *locals, IoMessage *m)
{
    if (DATA(self)->texture) IoImage_bindTexture(DATA(self)->texture, locals, m);
    gluCylinder(IoGLUQuadric_quadric((IoGLUQuadric *)self), DATA(self)->base, DATA(self)->top, DATA(self)->height, DATA(self)->slices, DATA(self)->stacks);
    return self;
}

