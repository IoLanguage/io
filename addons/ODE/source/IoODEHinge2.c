//metadoc ODEHinge2 copyright Jonathan Wright", 2006)
//metadoc ODEHinge2 license BSD revised
//metadoc ODEHinge2 category Physics
/*metadoc ODEHinge2 description
ODEJoint binding
*/

#include "IoODEJoint_internal.h"
#include "IoODEHinge2.h"
#include "IoODEBody.h"
#include "IoState.h"
#include "IoSeq.h"
#include "GLIncludes.h"

/* ----------------------------------------------------------- */

IoTag *IoODEHinge2_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("ODEHinge2");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoODEHinge2_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoODEHinge2_mark);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoODEHinge2_rawClone);
	return tag;
}

IoODEHinge2 *IoODEHinge2_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoODEHinge2_newTag(state));

	IoODEJoint_protoCommon(self);

	IoState_registerProtoWithFunc_(state, self, IoODEHinge2_proto);

	{
		IoMethodTable methodTable[] = {
				ODE_COMMON_JOINT_METHODS
#define PARAM(X, _N, _SETN) \
		{#_N, IoODEHinge2_##_N}, \
		{#_SETN, IoODEHinge2_##_SETN},
PARAMS
#undef PARAM

		{"anchor", IoODEHinge2_anchor},
				{"setAnchor", IoODEHinge2_setAnchor},
		{"anchor2", IoODEHinge2_anchor2},
		{"axis1", IoODEHinge2_axis1},
				{"setAxis1", IoODEHinge2_setAxis1},
		{"axis2", IoODEHinge2_axis2},
				{"setAxis2", IoODEHinge2_setAxis2},
		{"angle1", IoODEHinge2_angle1},
		{"angle1Rate", IoODEHinge2_angle1Rate},
		{"angle2Rate", IoODEHinge2_angle2Rate},
				{"addTorques", IoODEHinge2_addTorques},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoODEHinge2 *IoODEHinge2_rawClone(IoODEHinge2 *proto)
{
	IoObject *self = IoODEJoint_rawClone(proto);

	if(DATA(proto)->jointGroup)
	{
		IoODEJointGroup *jointGroup = DATA(proto)->jointGroup;

		JOINTGROUP = jointGroup;
		IoODEJointGroup_addJoint(jointGroup, self);
		JOINTID = dJointCreateHinge2(WORLDID, JOINTGROUPID);
	}
	return self;
}

void IoODEHinge2_free(IoODEHinge2 *self)
{
	IoODEJoint_free(self);
}

void IoODEHinge2_mark(IoODEHinge2 *self)
{
	IoODEJoint_mark(self);
}

IoODEJoint *IoODEHinge2_newProto(void *state, IoODEJointGroup *jointGroup)
{
	return IoODEJoint_newProtoCommon(state, IoODEHinge2_proto, jointGroup);
}

/* ----------------------------------------------------------- */


IoObject *IoODEHinge2_getParam(IoODEHinge2 *self, IoObject *locals, IoMessage *m, int parameter)
{
	return IoODEJoint_getParam(self, locals, m, parameter, dJointGetHinge2Param);
}

IoObject *IoODEHinge2_setParam(IoODEHinge2 *self, IoObject *locals, IoMessage *m, int parameter)
{
	return IoODEJoint_setParam(self, locals, m, parameter, dJointSetHinge2Param);
}

#define PARAM(X, _N, _SETN) \
IoObject *IoODEHinge2_##_N(IoODEHinge2 *self, IoObject *locals, IoMessage *m) \
{ \
	return IoODEHinge2_getParam(self, locals, m, X); \
} \
 \
IoObject *IoODEHinge2_##_SETN(IoODEHinge2 *self, IoObject *locals, IoMessage *m) \
{ \
	return IoODEHinge2_setParam(self, locals, m, X); \
}
PARAMS
#undef PARAM

IoObject *IoODEHinge2_anchor(IoODEHinge2 *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getVector3(self, locals, m, dJointGetHinge2Anchor);
}

IoObject *IoODEHinge2_anchor2(IoODEHinge2 *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getVector3(self, locals, m, dJointGetHinge2Anchor2);
}

IoObject *IoODEHinge2_axis1(IoODEHinge2 *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getVector3(self, locals, m, dJointGetHinge2Axis1);
}

IoObject *IoODEHinge2_axis2(IoODEHinge2 *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getVector3(self, locals, m, dJointGetHinge2Axis2);
}

IoObject *IoODEHinge2_angle1(IoODEHinge2 *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getReal(self, locals, m, dJointGetHinge2Angle1);
}

IoObject *IoODEHinge2_angle1Rate(IoODEHinge2 *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getReal(self, locals, m, dJointGetHinge2Angle1Rate);
}

IoObject *IoODEHinge2_angle2Rate(IoODEHinge2 *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getReal(self, locals, m, dJointGetHinge2Angle2Rate);
}


IoObject *IoODEHinge2_setAnchor(IoODEHinge2 *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_setVector3(self, locals, m, dJointSetHinge2Anchor);
}

IoObject *IoODEHinge2_setAxis1(IoODEHinge2 *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_setVector3(self, locals, m, dJointSetHinge2Axis1);
}

IoObject *IoODEHinge2_setAxis2(IoODEHinge2 *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_setVector3(self, locals, m, dJointSetHinge2Axis2);
}

IoObject *IoODEHinge2_addTorques(IoODEHinge2 *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_setReal2(self, locals, m, dJointAddHinge2Torques);
}

