/*#io
ODEBox ioDoc(
		 docCopyright("Jonathan Wright", 2006)
		 docLicense("BSD revised")
		 docDescription("ODEBox binding")
		 */

#include "IoODEBox.h"
#include "IoODEBody.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoVector_ode.h"
#include "IoList.h"
#include "geom.h"

#define DATA(self) ((IoODEBoxData *)IoObject_dataPointer(self))
#define GEOMID (DATA(self)->geomId)

IoTag *IoODEBox_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("ODEBox");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoODEBox_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoODEBox_rawClone);
	return tag;
}

IoODEBox *IoODEBox_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoODEBox_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODEBoxData)));

	GEOMID = 0;
	
	IoState_registerProtoWithFunc_(state, self, IoODEBox_proto);
	
	{
		IoMethodTable methodTable[] = {
		{"geomId", IoODEBox_geomId},
		{"lengths", IoODEBox_lengths},
		{"setLengths", IoODEBox_setLengths},
		{"pointDepth", IoODEBox_pointDepth},
		{"body", IoODEBox_body},
		{"setBody", IoODEBox_setBody},
		{"collide", IoODEGeom_collide},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoODEBox *IoODEBox_rawClone(IoODEBox *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODEBoxData)));
	return self; 
}

IoODEBox *IoODEBox_new(void *state, dGeomID geomId)
{
	IoODEBox *proto = IoState_protoWithInitFunction_(state, IoODEBox_proto);
	IoODEBox *self = IOCLONE(proto);
	GEOMID = geomId;
	dGeomSetData(GEOMID, self);
	return self;
}

void IoODEBox_free(IoODEBox *self) 
{ 
	if(GEOMID)
	{
		dGeomDestroy(GEOMID);
		GEOMID = 0;
	}

	free(IoObject_dataPointer(self)); 
}

void IoODEBox_mark(IoODEBox *self) 
{ 
	if (GEOMID)
	{
		dBodyID body = dGeomGetBody(GEOMID);
		if (body)
		{
			IoObject_shouldMark(IoODEBody_bodyFromId(IOSTATE, body)); 
		}
	}
}

/* ----------------------------------------------------------- */

dGeomID IoODEBox_rawGeomId(IoODEBox *self)
{
	return GEOMID;
}

/* ----------------------------------------------------------- */

void IoODEBox_assertHasBoxId(IoODEBox *self, IoObject *locals, IoMessage *m)
{
	IOASSERT(GEOMID, "ODE Box cannot be used directly. Clone the space and use the clone.");
}


IoObject *IoODEBox_geomId(IoODEBox *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER((long)GEOMID);
}

IoObject *IoODEBox_lengths(IoODEBox *self, IoObject *locals, IoMessage *m)
{
	dVector3 lengths;
	IoODEBox_assertHasBoxId(self, locals, m);
	dGeomBoxGetLengths(GEOMID, lengths);
	return IoVector_newWithODEPoint(IOSTATE, lengths);
}

IoObject *IoODEBox_setLengths(IoODEBox *self, IoObject *locals, IoMessage *m)
{
	dReal lx = IoMessage_locals_doubleArgAt_(m, locals, 0);
	dReal ly = IoMessage_locals_doubleArgAt_(m, locals, 1);
	dReal lz = IoMessage_locals_doubleArgAt_(m, locals, 2);

	IoODEBox_assertHasBoxId(self, locals, m);
	dGeomBoxSetLengths(GEOMID, lx, ly, lz);
	return self;
}

IoObject *IoODEBox_pointDepth(IoODEBox *self, IoObject *locals, IoMessage *m)
{
	dReal lx = IoMessage_locals_doubleArgAt_(m, locals, 0);
	dReal ly = IoMessage_locals_doubleArgAt_(m, locals, 1);
	dReal lz = IoMessage_locals_doubleArgAt_(m, locals, 2);

	IoODEBox_assertHasBoxId(self, locals, m);
	return IONUMBER(dGeomBoxPointDepth(GEOMID, lx, ly, lz));
}

IoObject *IoODEBox_body(IoODEBox *self, IoObject *locals, IoMessage *m)
{
	IoODEBox_assertHasBoxId(self, locals, m);
	return IoODEBody_bodyFromId(IOSTATE, dGeomGetBody(GEOMID));
}

IoObject *IoODEBox_setBody(IoODEBox *self, IoObject *locals, IoMessage *m)
{
	dBodyID body = IoMessage_locals_odeBodyIdArgAt_(m, locals, 0);

	IoODEBox_assertHasBoxId(self, locals, m);
	dGeomSetBody(GEOMID, body);
	return self;
}
