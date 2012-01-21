/*
 *	Experimental rewrite
 */

#include "IoGLE.h"

#include "base/List.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoCFunction.h"
#include "IoNil.h"
#include "IoSeq.h"
#include "IoList.h"
#include "List.h"
#include <time.h>

#define DATA(self) ((IoGLEData *)self->data)

static const char *protoId = "GLE";

IoTag *IoGLE_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoGLE_rawClone;
	/*
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoGLE_free;
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoGLE_mark;
	*/
	return tag;
}

IoGLE *IoGLE_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoGLE_newTag(state));
	/*self->data = calloc(1, sizeof(IoGLEData));*/
	
	IoState_registerProtoWithId_(state, self, protoId);
	IoGLE_protoInit(self);
	return self;
}

IoGLE *IoGLE_new(void *state)
{ 
	return IoState_protoWithId_(state, protoId); 
}

/*
 void IoGLE_free(IoGLE *self) 
 { }
 
 void IoGLE_mark(IoGLE *self)
 {}
 */

/* ----------------------------------------------------------- */

IoObject *IoGLE_rawClone(IoGLE *self)
{ 
	return IoState_protoWithId_(IOSTATE, protoId); 
}

/* --- GLE -------------------------------------------------------- */

#define IOCFUNCTION_GLE(func) IOCFUNCTION(func, NULL)

#include "IoGLEconst.h"
#include "IoGLEfunc.h"

void IoGLE_protoInit(IoGLE *self) 
{ 
	IoObject_setSlot_to_(self, IOSYMBOL("clone"), IOCFUNCTION_GLE(IoObject_self));
	
	/* GLE Constants */
	{
		t_ioGLE_constTable *curpos=ioGLE_constTable;
		while (curpos->name) {
			IoObject_setSlot_to_(self, IOSYMBOL(curpos->name), IONUMBER(curpos->value));
			curpos++;
		}
	}
	
	/* GLE Functions */
	{
		t_ioGLE_funcTable *curpos=ioGLE_funcTable;
		while (curpos->name) {
			IoObject_setSlot_to_(self, IOSYMBOL(curpos->name), IOCFUNCTION_GLE(curpos->func));
			curpos++;
		}
	}
}
