/*#io
DisplayList ioDoc(
			   docCopyright("Steve Dekorte", 2002)
			   docCategory("Graphics")
			   docLicense("BSD revised")
*/

#include "IoDisplayList.h"
#include "IoNumber.h"

#define DLIST(self) ((GLuint)(IoObject_dataPointer(self)))

IoTag *IoDisplayList_newTag(void *state)
{
    IoTag *tag = IoTag_newWithName_("DisplayList");
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoDisplayList_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoDisplayList_free);
    //IoTag_markFunc_(tag, (IoTagMarkFunc *)IoDisplayList_mark);
    return tag;
}

IoDisplayList *IoDisplayList_proto(void *state)
{
    IoObject *self = IoObject_new(state);
    IoObject_tag_(self, IoDisplayList_newTag(state));
    
    IoDisplayList_number_(self, -1);
    
    IoState_registerProtoWithFunc_(state, self, IoDisplayList_proto);
    
    {
	IoMethodTable methodTable[] = {
	{"begin", IoDisplayList_begin},
	{"end", IoDisplayList_end},
	{"call", IoDisplayList_call},
	{"id", IoDisplayList_id},
	{NULL, NULL},
	};
	IoObject_addMethodTable_(self, methodTable);
    }
    return self;
}

IoDisplayList *IoDisplayList_rawClone(IoDisplayList *proto) 
{ 
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoDisplayList_number_(self, -1);
    return self; 
}

IoDisplayList *IoDisplayList_new(void *state)
{
    IoObject *proto = IoState_protoWithInitFunction_(state, IoDisplayList_proto);
    return IOCLONE(proto);
}

/* ----------------------------------------------------------- */

void IoDisplayList_free(IoDisplayList *self) 
{ 
    /*printf("freeing display list %i\n", (int)DLIST(self));*/
    if (DLIST(self) != -1) glDeleteLists(DLIST(self), 1);
}

void IoDisplayList_mark(IoDisplayList *self) 
{ }

/* ----------------------------------------------------------- */

void IoDisplayList_number_(IoDisplayList *self, GLuint n)
{ 
	IoObject_setDataUint32_(self, (void *)n); 
}

GLuint IoDisplayList_number(IoDisplayList *self)
{
    if (DLIST(self) == -1) 
    {
	IoDisplayList_number_(self,  glGenLists(1));
	/*printf("create display list %i\n", (int)DLIST(self));*/
    }
    return DLIST(self);
}

IoObject *IoDisplayList_begin(IoDisplayList *self, IoObject *locals, IoMessage *m)
{ glNewList(IoDisplayList_number(self), GL_COMPILE); return self; }

IoObject *IoDisplayList_end(IoDisplayList *self, IoObject *locals, IoMessage *m)
{ glEndList(); return self; }

IoObject *IoDisplayList_call(IoDisplayList *self, IoObject *locals, IoMessage *m)
{ glCallList(IoDisplayList_number(self)); return self; }

IoObject *IoDisplayList_id(IoDisplayList *self, IoObject *locals, IoMessage *m)
{ return IONUMBER(IoDisplayList_number(self)); }

