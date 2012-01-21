//metadoc ODESimpleSpace copyright Jonathan Wright, 2006
//metadoc ODESimpleSpace license BSD revised
//metadoc ODESimpleSpace category Physics
/*metadoc ODESimpleSpace description
ODESimpleSpace binding
*/

#include "IoODESimpleSpace.h"
#include "IoODEPlane.h"
#include "IoODEBox.h"
#include "IoODEBody.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoList.h"
#include "IoMessage.h"
#include "geom.h"

#define DATA(self) ((IoODESimpleSpaceData *)IoObject_dataPointer(self))
#define SPACEID (DATA(self)->spaceId)

static const char *protoId = "ODESimpleSpace";


IoTag *IoODESimpleSpace_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoODESimpleSpace_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoODESimpleSpace_rawClone);
	return tag;
}

IoODESimpleSpace *IoODESimpleSpace_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoODESimpleSpace_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODESimpleSpaceData)));

	SPACEID = 0;

	IoState_registerProtoWithFunc_(state, self, IoODESimpleSpace_proto);

	{
		IoMethodTable methodTable[] = {
		{"spaceId", IoODESimpleSpace_spaceId},
		{"collide", IoODESimpleSpace_collide},

		{"plane", IoODESimpleSpace_plane},
		{"box", IoODESimpleSpace_box},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoODESimpleSpace *IoODESimpleSpace_rawClone(IoODESimpleSpace *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODESimpleSpaceData)));
	SPACEID = dSimpleSpaceCreate(0);
	dSpaceSetCleanup(SPACEID, 0);
	return self;
}

IoODESimpleSpace *IoODESimpleSpace_new(void *state)
{
	IoODESimpleSpace *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

void IoODESimpleSpace_free(IoODESimpleSpace *self)
{
	if(SPACEID)
	{
		dSpaceDestroy(SPACEID);
		SPACEID = 0;
	}

	free(IoObject_dataPointer(self));
}

void IoODESimpleSpace_mark(IoODESimpleSpace *self)
{
}

/* ----------------------------------------------------------- */

dSpaceID IoODESimpleSpace_rawSimpleSpaceId(IoODESimpleSpace *self)
{
	return SPACEID;
}

/* ----------------------------------------------------------- */

void IoODESimpleSpace_assertHasSimpleSpaceId(IoODESimpleSpace *self, IoObject *locals, IoMessage *m)
{
	IOASSERT(SPACEID, "ODE SimpleSpace cannot be used directly. Clone the space and use the clone.");
}


IoObject *IoODESimpleSpace_spaceId(IoODESimpleSpace *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER((long)SPACEID);
}

IoObject *IoODESimpleSpace_plane(IoODESimpleSpace *self, IoObject *locals, IoMessage *m)
{
	dReal a = IoMessage_locals_doubleArgAt_(m, locals, 0);
	dReal b = IoMessage_locals_doubleArgAt_(m, locals, 1);
	dReal c = IoMessage_locals_doubleArgAt_(m, locals, 2);
	dReal d = IoMessage_locals_doubleArgAt_(m, locals, 3);
	dGeomID geomId;

	// Don't require a valid space. Plane can be used on the proto to create a plane without a space.
	//IoODESimpleSpace_assertHasSimpleSpaceId(self, locals, m);

	geomId = dCreatePlane(SPACEID, a, b, c, d);

	return IoODEPlane_new(IOSTATE, geomId);
}

IoObject *IoODESimpleSpace_box(IoODESimpleSpace *self, IoObject *locals, IoMessage *m)
{
	dReal lx = IoMessage_locals_doubleArgAt_(m, locals, 0);
	dReal ly = IoMessage_locals_doubleArgAt_(m, locals, 1);
	dReal lz = IoMessage_locals_doubleArgAt_(m, locals, 2);

	dGeomID geomId;

	// Don't require a valid space. Box can be used on the proto to create a box without a space.
	//IoODESimpleSpace_assertHasSimpleSpaceId(self, locals, m);

	geomId = dCreateBox(SPACEID, lx, ly, lz);

	return IoODEBox_new(IOSTATE, geomId);
}

void nearCallback(void *data, dGeomID o1, dGeomID o2);

typedef struct
{
	IoMessage *message;
	IoObject *locals;
	IoObject *target;
} NearCallbackData;

IoObject *IoODESimpleSpace_collide(IoODESimpleSpace *self, IoObject *locals, IoMessage *m)
{
	NearCallbackData data;

	data.target = IoMessage_locals_valueArgAt_(m, locals, 0);
	data.message = IoMessage_locals_messageArgAt_(m, locals, 1);
	data.locals = locals;

	IoODESimpleSpace_assertHasSimpleSpaceId(self, locals, m);

	dSpaceCollide (SPACEID, &data, &nearCallback);
	return self;
}

void nearCallback(void *data, dGeomID o1, dGeomID o2)
{
	// This callback creates and disposes of a new mesage each time.
	// TODO: Reuse message objects to go faster and releave pressure on the GC.
	NearCallbackData *nearData = (NearCallbackData*)data;

	IoMessage *self = IoMessage_deepCopyOf_(nearData->message);

	IoMessage_addCachedArg_(self, IoODEGeom_geomFromId(IOSTATE, o1));
	IoMessage_addCachedArg_(self, IoODEGeom_geomFromId(IOSTATE, o2));

	IoMessage_locals_performOn_(self, nearData->locals, nearData->target);
}
