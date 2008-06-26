//metadoc ODEWorld copyright Jonathan Wright, 2006
//metadoc ODEWorld license BSD revised
/*metadoc ODEWorld description
ODEWorld binding
*/

#include "IoODEWorld.h"
#include "IoODEBody.h"
#include "IoODEJointGroup.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoList.h"

#define DATA(self) ((IoODEWorldData *)IoObject_dataPointer(self))
#define WORLDID (DATA(self)->worldId)

IoTag *IoODEWorld_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("ODEWorld");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoODEWorld_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoODEWorld_rawClone);
	return tag;
}

IoODEWorld *IoODEWorld_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoODEWorld_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODEWorldData)));

	WORLDID = 0;
	DATA(self)->bodies = 0L;
	DATA(self)->jointGroups = 0L;

	IoState_registerProtoWithFunc_(state, self, IoODEWorld_proto);

	{
		IoMethodTable methodTable[] = {
		{"worldId", IoODEWorld_worldId},
		{"bodies", IoODEWorld_bodies},
		{"jointGroups", IoODEWorld_jointGroups},

		{"gravity", IoODEWorld_gravity},
		{"setGravity", IoODEWorld_setGravity},
		{"erp", IoODEWorld_erp},
		{"setErp", IoODEWorld_setErp},
		{"cfm", IoODEWorld_cfm},
		{"setCfm", IoODEWorld_setCfm},

		{"step", IoODEWorld_step},
		{"quickStep", IoODEWorld_quickStep},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoODEWorld *IoODEWorld_rawClone(IoODEWorld *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODEWorldData)));
	WORLDID = dWorldCreate();
	DATA(self)->bodies = List_new();
	DATA(self)->jointGroups = List_new();
	IoObject_inlineSetSlot_to_(self, IOSYMBOL("Body"), IoODEBody_newBodyProtoWithWorld(IOSTATE, self));
	IoObject_inlineSetSlot_to_(self, IOSYMBOL("JointGroup"), IoODEJointGroup_newJointGroupProtoWithWorld(IOSTATE, self));
	return self;
}

IoODEWorld *IoODEWorld_new(void *state)
{
	IoODEWorld *proto = IoState_protoWithInitFunction_(state, IoODEWorld_proto);
	return IOCLONE(proto);
}

void IoODEWorld_free(IoODEWorld *self)
{
	IoODEWorld_emptyJointGroups(self);

	if(WORLDID)
	{
		dWorldDestroy(WORLDID);
		WORLDID = 0;
	}
	if(DATA(self)->bodies)
	{
		LIST_FOREACH(DATA(self)->bodies, i, body,
			IoODEBody_worldDestroyed((IoODEBody*)body);
			)
		List_free(DATA(self)->bodies);
		DATA(self)->bodies = 0L;
	}
	if(DATA(self)->jointGroups)
	{
		LIST_FOREACH(DATA(self)->jointGroups, i, jointGroup,
			IoODEJointGroup_worldDestoryed((IoODEJointGroup*)jointGroup);
			)
		List_free(DATA(self)->jointGroups);
		DATA(self)->jointGroups = 0L;
	}
	free(IoObject_dataPointer(self));
}

void IoODEWorld_mark(IoODEWorld *self)
{
}

/* ----------------------------------------------------------- */

dWorldID IoODEWorld_rawWorldId(IoODEWorld *self)
{
	return WORLDID;
}

void IoODEWorld_addBody(IoODEWorld *self, IoODEBody *body)
{
	List_append_(DATA(self)->bodies, body);
}

void IoODEWorld_removeBody(IoODEWorld *self, IoODEBody *body)
{
	List_remove_(DATA(self)->bodies, body);
}

void IoODEWorld_addJointGroup(IoODEWorld *self, IoODEJointGroup *jointGroup)
{
	List_append_(DATA(self)->jointGroups, jointGroup);
}

void IoODEWorld_removeJointGroup(IoODEWorld *self, IoODEJointGroup *jointGroup)
{
	List_remove_(DATA(self)->jointGroups, jointGroup);
}

void IoODEWorld_emptyJointGroups(IoODEWorld *self)
{
	if(DATA(self)->jointGroups)
	{
		LIST_FOREACH(DATA(self)->jointGroups, i, jointGroup,
			IoODEJointGroup_rawEmpty((IoODEJointGroup*)jointGroup);
			)
	}
}

/* ----------------------------------------------------------- */

void IoODEWorld_assertHasWorldId(IoODEWorld *self, IoObject *locals, IoMessage *m)
{
	IOASSERT(WORLDID, "ODE World cannot be used directly. Clone the world and use the clone.");
}


IoObject *IoODEWorld_worldId(IoODEWorld *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER((long)WORLDID);
}

IoObject *IoODEWorld_setGravity(IoODEWorld *self, IoObject *locals, IoMessage *m)
{
	const double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	const double z = IoMessage_locals_doubleArgAt_(m, locals, 2);

	IoODEWorld_assertHasWorldId(self, locals, m);
	dWorldSetGravity(WORLDID, x, y, z);
	return self;
}

IoObject *IoODEWorld_gravity(IoODEWorld *self, IoObject *locals, IoMessage *m)
{
	dVector3 gravity;
	IoODEWorld_assertHasWorldId(self, locals, m);

	dWorldGetGravity(WORLDID, gravity);
	
	vec3f v;
	v.x = gravity[0];
	v.y = gravity[1];
	v.z = gravity[2];

	return IoSeq_newVec3f(IOSTATE, v);
}

IoObject *IoODEWorld_step(IoODEWorld *self, IoObject *locals, IoMessage *m)
{
	const double stepSize = IoMessage_locals_doubleArgAt_(m, locals, 0);

	IoODEWorld_assertHasWorldId(self, locals, m);
	dWorldStep(WORLDID, stepSize);
	return self;
}

IoObject *IoODEWorld_quickStep(IoODEWorld *self, IoObject *locals, IoMessage *m)
{
	const double stepSize = IoMessage_locals_doubleArgAt_(m, locals, 0);

	IoODEWorld_assertHasWorldId(self, locals, m);
	dWorldQuickStep(WORLDID, stepSize);
	return self;
}

IoObject *IoODEWorld_stepFast1(IoODEWorld *self, IoObject *locals, IoMessage *m)
{
	const double stepSize = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const int maxIterations = IoMessage_locals_intArgAt_(m, locals, 1);

	IoODEWorld_assertHasWorldId(self, locals, m);
	dWorldStepFast1(WORLDID, stepSize, maxIterations);
	return self;
}

IoObject *IoODEWorld_bodies(IoODEWorld *self, IoObject *locals, IoMessage *m)
{
	IoODEWorld_assertHasWorldId(self, locals, m);
	{
		IoList *list = IoList_new(IOSTATE);
		IoList_rawAddBaseList_(list, DATA(self)->bodies);
		return list;
	}
}

IoObject *IoODEWorld_jointGroups(IoODEWorld *self, IoObject *locals, IoMessage *m)
{
	IoODEWorld_assertHasWorldId(self, locals, m);
	{
		IoList *list = IoList_new(IOSTATE);
		IoList_rawAddBaseList_(list, DATA(self)->jointGroups);
		return list;
	}
}

IoObject *IoODEWorld_erp(IoODEWorld *self, IoObject *locals, IoMessage *m)
{
	IoODEWorld_assertHasWorldId(self, locals, m);
	return IONUMBER(dWorldGetERP(WORLDID));
}

IoObject *IoODEWorld_setErp(IoODEWorld *self, IoObject *locals, IoMessage *m)
{
	const double erp = IoMessage_locals_doubleArgAt_(m, locals, 0);
	IoODEWorld_assertHasWorldId(self, locals, m);
	dWorldSetERP(WORLDID, erp);
	return self;
}

IoObject *IoODEWorld_cfm(IoODEWorld *self, IoObject *locals, IoMessage *m)
{
	IoODEWorld_assertHasWorldId(self, locals, m);
	return IONUMBER(dWorldGetCFM(WORLDID));
}

IoObject *IoODEWorld_setCfm(IoODEWorld *self, IoObject *locals, IoMessage *m)
{
	const double cfm = IoMessage_locals_doubleArgAt_(m, locals, 0);
	IoODEWorld_assertHasWorldId(self, locals, m);
	dWorldSetCFM(WORLDID, cfm);
	return self;
}
