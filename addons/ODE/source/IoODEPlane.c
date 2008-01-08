/*#io
ODEPlane ioDoc(
		 docCopyright("Jonathan Wright", 2006)
		 docLicense("BSD revised")
		 docDescription("ODEPlane binding")
		 */

#include "IoODEPlane.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoVector_ode.h"
#include "IoList.h"
#include "geom.h"

#define DATA(self) ((IoODEPlaneData *)IoObject_dataPointer(self))
#define GEOMID (DATA(self)->geomId)

IoTag *IoODEPlane_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("ODEPlane");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoODEPlane_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoODEPlane_rawClone);
	return tag;
}

IoODEPlane *IoODEPlane_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoODEPlane_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODEPlaneData)));

	GEOMID = 0;

	IoState_registerProtoWithFunc_(state, self, IoODEPlane_proto);

	{
		IoMethodTable methodTable[] = {
		{"geomId", IoODEPlane_geomId},
		{"params", IoODEPlane_params},
		{"setParams", IoODEPlane_setParams},
		{"collide", IoODEGeom_collide},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoODEPlane *IoODEPlane_rawClone(IoODEPlane *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODEPlaneData)));
	return self;
}

IoODEPlane *IoODEPlane_new(void *state, dGeomID geomId)
{
	IoODEPlane *proto = IoState_protoWithInitFunction_(state, IoODEPlane_proto);
	IoODEPlane *self = IOCLONE(proto);
	GEOMID = geomId;
	dGeomSetData(GEOMID, self);
	return self;
}

void IoODEPlane_free(IoODEPlane *self)
{
	if(GEOMID)
	{
		dGeomDestroy(GEOMID);
		GEOMID = 0;
	}

	free(IoObject_dataPointer(self));
}

void IoODEPlane_mark(IoODEPlane *self)
{
}

/* ----------------------------------------------------------- */

dGeomID IoODEPlane_rawGeomId(IoODEPlane *self)
{
	return GEOMID;
}

/* ----------------------------------------------------------- */

void IoODEPlane_assertHasPlaneId(IoODEPlane *self, IoObject *locals, IoMessage *m)
{
	IOASSERT(GEOMID, "ODE Plane cannot be used directly. Clone the space and use the clone.");
}


IoObject *IoODEPlane_geomId(IoODEPlane *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER((long)GEOMID);
}

IoObject *IoODEPlane_params(IoODEPlane *self, IoObject *locals, IoMessage *m)
{
	dVector4 params;
	IoODEPlane_assertHasPlaneId(self, locals, m);
	dGeomPlaneGetParams(GEOMID, params);
	return IoVector_newWithODEVector4(IOSTATE, params);
}

IoObject *IoODEPlane_setParams(IoODEPlane *self, IoObject *locals, IoMessage *m)
{
	dReal a = IoMessage_locals_doubleArgAt_(m, locals, 0);
	dReal b = IoMessage_locals_doubleArgAt_(m, locals, 1);
	dReal c = IoMessage_locals_doubleArgAt_(m, locals, 2);
	dReal d = IoMessage_locals_doubleArgAt_(m, locals, 3);

	IoODEPlane_assertHasPlaneId(self, locals, m);
	dGeomPlaneSetParams(GEOMID, a, b, c, d);
	return self;
}
