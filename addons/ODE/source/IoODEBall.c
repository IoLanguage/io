//metadoc ODEBall copyright Jonathan Wright, 2006
//metadoc ODEBall license BSD revised
//metadoc ODEBall category Physics
/*metadoc ODEBall description
ODEBall binding
*/

#include "IoODEJoint_internal.h"
#include "IoODEBall.h"
#include "IoODEBody.h"
#include "IoState.h"
#include "IoSeq.h"
#include "GLIncludes.h"

/* ----------------------------------------------------------- */

IoTag *IoODEBall_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("ODEBall");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoODEBall_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoODEBall_mark);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoODEBall_rawClone);
	return tag;
}

IoODEBall *IoODEBall_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoODEBall_newTag(state));

	IoODEJoint_protoCommon(self);

	IoState_registerProtoWithFunc_(state, self, IoODEBall_proto);

	{
		IoMethodTable methodTable[] = {
				ODE_COMMON_JOINT_METHODS

		{"anchor", IoODEBall_anchor},
		{"anchor2", IoODEBall_anchor2},
		{"setAnchor", IoODEBall_setAnchor},
		//{"setAnchor2", IoODEBall_setAnchor2},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoODEBall *IoODEBall_rawClone(IoODEBall *proto)
{
	IoObject *self = IoODEJoint_rawClone(proto);

	if(DATA(proto)->jointGroup)
	{
		IoODEJointGroup *jointGroup = DATA(proto)->jointGroup;

		JOINTGROUP = jointGroup;
		IoODEJointGroup_addJoint(jointGroup, self);
		JOINTID = dJointCreateBall(WORLDID, JOINTGROUPID);
	}
	return self;
}

void IoODEBall_free(IoODEBall *self)
{
	IoODEJoint_free(self);
}

void IoODEBall_mark(IoODEBall *self)
{
	IoODEJoint_mark(self);
}

IoODEJoint *IoODEBall_newProto(void *state, IoODEJointGroup *jointGroup)
{
	return IoODEJoint_newProtoCommon(state, IoODEBall_proto, jointGroup);
}

/* ----------------------------------------------------------- */


IoObject *IoODEBall_setAnchor(IoODEBall *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_setVector3(self, locals, m, dJointSetBallAnchor);
}

/* Only in newer versions of ode
IoObject *IoODEBall_setAnchor2(IoODEBall *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_setVector3(self, locals, m, dJointSetBallAnchor2);
}
*/

IoObject *IoODEBall_anchor(IoODEBall *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getVector3(self, locals, m, dJointGetBallAnchor);
}

IoObject *IoODEBall_anchor2(IoODEBall *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getVector3(self, locals, m, dJointGetBallAnchor2);
}

