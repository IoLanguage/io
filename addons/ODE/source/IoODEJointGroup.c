//metadoc ODEJointGroup copyright Jonathan Wright, 2006
//metadoc ODEJointGroup license BSD revised
//metadoc ODEJointGroup category Physics
/*metadoc ODEJointGroup description
ODEJointGroup binding
*/

#include "IoODEJointGroup.h"
#include "IoODEJoint.h"
#include "IoODEContact.h"
#include "IoState.h"
#include "IoList.h"

#include "IoODEBall.h"
#include "IoODEHinge.h"
#include "IoODESlider.h"
#include "IoODEHinge2.h"
#include "IoODEUniversal.h"
#include "IoODEFixed.h"
//#include "IoODEAMotor.h"
//#include "IoODELMotor.h"
#include "IoODEContactJoint.h"

#define DATA(self) ((IoODEJointGroupData *)IoObject_dataPointer(self))
#define JOINTGROUPID (DATA(self)->jointGroupId)
#define WORLD (DATA(self)->world)
#define WORLDID (IoODEWorld_rawWorldId(WORLD))

static const char *protoId = "ODEJointGroup";


IoTag *IoODEJointGroup_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoODEJointGroup_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoODEJointGroup_mark);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoODEJointGroup_rawClone);
	return tag;
}

IoODEJointGroup *IoODEJointGroup_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoODEJointGroup_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODEJointGroupData)));

	JOINTGROUPID = 0;
	WORLD = 0L;
	DATA(self)->joints = 0L;

	IoState_registerProtoWithFunc_(state, self, IoODEJointGroup_proto);

	{
		IoMethodTable methodTable[] = {
		{"jointGroupId", IoODEJointGroup_jointGroupId},
		{"world", IoODEJointGroup_world},
		{"empty", IoODEJointGroup_empty},
		{"joints", IoODEJointGroup_joints},
		{"createContact", IoODEJointGroup_createContact},
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoODEJointGroup *IoODEJointGroup_rawClone(IoODEJointGroup *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODEJointGroupData)));

	if(DATA(proto)->world)
	{
		IoODEWorld *world = DATA(proto)->world;
		WORLD = world;
		IoODEWorld_addJointGroup(world, self);
		JOINTGROUPID = dJointGroupCreate(0);
		DATA(self)->joints = List_new();

		IoObject_inlineSetSlot_to_(self, IOSYMBOL("Ball"), IoODEBall_newProto(IOSTATE, self));
		IoObject_inlineSetSlot_to_(self, IOSYMBOL("Hinge"), IoODEHinge_newProto(IOSTATE, self));
		IoObject_inlineSetSlot_to_(self, IOSYMBOL("Slider"), IoODESlider_newProto(IOSTATE, self));
		IoObject_inlineSetSlot_to_(self, IOSYMBOL("Hinge2"), IoODEHinge2_newProto(IOSTATE, self));
		IoObject_inlineSetSlot_to_(self, IOSYMBOL("Universal"), IoODEUniversal_newProto(IOSTATE, self));
		IoObject_inlineSetSlot_to_(self, IOSYMBOL("Fixed"), IoODEFixed_newProto(IOSTATE, self));
		//IoObject_inlineSetSlot_to_(self, IOSYMBOL("AMotor"), IoODEAMotor_newProto(IOSTATE, self));
		//IoObject_inlineSetSlot_to_(self, IOSYMBOL("LMotor"), IoODELMotor_newProto(IOSTATE, self));
	}
	return self;
}

void IoODEJointGroup_free(IoODEJointGroup *self)
{
	if(JOINTGROUPID && WORLD)
	{
		IoODEWorld_removeJointGroup(WORLD, self);
		dJointGroupDestroy(JOINTGROUPID);
	}
	if(DATA(self)->joints)
	{
		LIST_FOREACH(DATA(self)->joints, i, joint,
			IoODEJoint_worldDestoryed((IoODEJoint*)joint);
		)
		List_free(DATA(self)->joints);
	}
	free(IoObject_dataPointer(self));
}

void IoODEJointGroup_mark(IoODEJointGroup *self)
{
	if(WORLD)
	{
		IoObject_shouldMark((IoObject *)WORLD);
	}
	if(DATA(self)->joints)
	{
		LIST_FOREACH(DATA(self)->joints, i, joint,
			IoObject_shouldMark(joint);
		)
	}
}

IoODEJointGroup *IoODEJointGroup_new(void *state)
{
	IoODEJointGroup *proto = IoState_protoWithId_(state, protoId);
	return IOCLONE(proto);
}

IoODEJointGroup *IoODEJointGroup_newJointGroupProtoWithWorld(void *state, IoODEWorld *world)
{
	IoODEJointGroup *proto = IoState_protoWithId_(state, protoId);
	IoODEJointGroup *self = IOCLONE(proto);
	WORLD = world;
	return self;
}


/* ----------------------------------------------------------- */

void IoODEJointGroup_worldDestoryed(IoODEJointGroup *self)
{
	WORLD = 0L;
	JOINTGROUPID = 0;
	if(DATA(self)->joints)
	{
		LIST_FOREACH(DATA(self)->joints, i, joint,
			IoODEJoint_worldDestoryed((IoODEJoint*)joint);
		)
		List_free(DATA(self)->joints);
		DATA(self)->joints = 0L;
	}
}

void IoODEJointGroup_rawEmpty(IoODEJointGroup *self)
{
	dJointGroupEmpty(JOINTGROUPID);

	LIST_FOREACH(DATA(self)->joints, i, joint,
		IoODEJoint_worldDestoryed((IoODEJoint*)joint);
	)
	List_removeAll(DATA(self)->joints);
}

dJointGroupID IoODEJointGroup_rawJointGroupId(IoODEJointGroup *self)
{
	return JOINTGROUPID;
}

dWorldID IoODEJointGroup_rawWorldId(IoODEJointGroup *self)
{
	return WORLDID;
}

void IoODEJointGroup_addJoint(IoODEJointGroup *self, IoODEJoint *joint)
{
	List_append_(DATA(self)->joints, joint);
}

void IoODEJointGroup_removeJoint(IoODEJointGroup *self, IoODEJoint *joint)
{
	List_remove_(DATA(self)->joints, joint);
}


/* ----------------------------------------------------------- */

void IoODEJointGroup_assertValidJointGroup(IoODEJointGroup *self, IoObject *locals, IoMessage *m)
{
	IOASSERT(WORLD, "This ODE JointGroup belongs to an ODE world which has been freed.");
	IOASSERT(JOINTGROUPID, "ODE World JointGroup cannot be used directly. Clone the world and use the JointGroup on the cloned world.");
}


IoObject *IoODEJointGroup_jointGroupId(IoODEJointGroup *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER((long)JOINTGROUPID);
}

IoObject *IoODEJointGroup_world(IoODEJointGroup *self, IoObject *locals, IoMessage *m)
{
	return WORLD ? WORLD : IONIL(self);
}

IoObject *IoODEJointGroup_empty(IoODEJointGroup *self, IoObject *locals, IoMessage *m)
{
	IoODEJointGroup_assertValidJointGroup(self, locals, m);
	IoODEJointGroup_rawEmpty(self);
	return self;
}

IoObject *IoODEJointGroup_joints(IoODEWorld *self, IoObject *locals, IoMessage *m)
{
	IoODEJointGroup_assertValidJointGroup(self, locals, m);
	{
		IoList *list = IoList_new(IOSTATE);
		IoList_rawAddBaseList_(list, DATA(self)->joints);
		return list;
	}
}

IoObject *IoODEJointGroup_createContact(IoODEWorld *self, IoObject *locals, IoMessage *m)
{
	dContact *contact = IoMessage_locals_odeContactStructArgAt_(m, locals, 0);
	dJointID jointId;

	IoODEJointGroup_assertValidJointGroup(self, locals, m);

	jointId = dJointCreateContact (WORLDID, JOINTGROUPID, contact);

	return IoODEContactJoint_new(IOSTATE, self, jointId);
}
