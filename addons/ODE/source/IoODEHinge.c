/*#io
ODEJoint ioDoc(
	docCopyright("Jonathan Wright", 2006)
	docLicense("BSD revised")
	docDescription("ODEJoint binding")
*/

#include "IoODEJoint_internal.h"
#include "IoODEHinge.h"
#include "IoODEBody.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoVector_ode.h"
#include "GLIncludes.h"

/* ----------------------------------------------------------- */

IoTag *IoODEHinge_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("ODEHinge");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoODEHinge_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoODEHinge_mark);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoODEHinge_rawClone);
	return tag;
}

IoODEHinge *IoODEHinge_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoODEHinge_newTag(state));

	IoODEJoint_protoCommon(self);

	IoState_registerProtoWithFunc_(state, self, IoODEHinge_proto);

	{
		IoMethodTable methodTable[] = {
				ODE_COMMON_JOINT_METHODS
#define PARAM(X, _N, _SETN) \
		{#_N, IoODEHinge_##_N}, \
		{#_SETN, IoODEHinge_##_SETN},
PARAMS
#undef PARAM

		{"anchor", IoODEHinge_anchor},
		{"setAnchor", IoODEHinge_setAnchor},
		{"anchor2", IoODEHinge_anchor2},
		{"axis", IoODEHinge_axis},
		{"setAxis", IoODEHinge_setAxis},
		{"angle", IoODEHinge_angle},
		{"angleRate", IoODEHinge_angleRate},
				{"addTorque", IoODEHinge_addTorque},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoODEHinge *IoODEHinge_rawClone(IoODEHinge *proto)
{
	IoObject *self = IoODEJoint_rawClone(proto);

	if(DATA(proto)->jointGroup)
	{
		IoODEJointGroup *jointGroup = DATA(proto)->jointGroup;

		JOINTGROUP = jointGroup;
		IoODEJointGroup_addJoint(jointGroup, self);
		JOINTID = dJointCreateHinge(WORLDID, JOINTGROUPID);
	}
	return self;
}

void IoODEHinge_free(IoODEHinge *self)
{
	IoODEJoint_free(self);
}

void IoODEHinge_mark(IoODEHinge *self)
{
	IoODEJoint_mark(self);
}

IoODEJoint *IoODEHinge_newProto(void *state, IoODEJointGroup *jointGroup)
{
	return IoODEJoint_newProtoCommon(state, IoODEHinge_proto, jointGroup);
}

/* ----------------------------------------------------------- */


IoObject *IoODEHinge_getParam(IoODEHinge *self, IoObject *locals, IoMessage *m, int parameter)
{
	return IoODEJoint_getParam(self, locals, m, parameter, dJointGetHingeParam);
}

IoObject *IoODEHinge_setParam(IoODEHinge *self, IoObject *locals, IoMessage *m, int parameter)
{
	return IoODEJoint_setParam(self, locals, m, parameter, dJointSetHingeParam);
}

#define PARAM(X, _N, _SETN) \
IoObject *IoODEHinge_##_N(IoODEHinge *self, IoObject *locals, IoMessage *m) \
{ \
	return IoODEHinge_getParam(self, locals, m, X); \
} \
 \
IoObject *IoODEHinge_##_SETN(IoODEHinge *self, IoObject *locals, IoMessage *m) \
{ \
	return IoODEHinge_setParam(self, locals, m, X); \
}
PARAMS
#undef PARAM

IoObject *IoODEHinge_anchor(IoODEHinge *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getVector3(self, locals, m, dJointGetHingeAnchor);
}

IoObject *IoODEHinge_anchor2(IoODEHinge *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getVector3(self, locals, m, dJointGetHingeAnchor2);
}

IoObject *IoODEHinge_axis(IoODEHinge *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getVector3(self, locals, m, dJointGetHingeAxis);
}

IoObject *IoODEHinge_angle(IoODEHinge *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getReal(self, locals, m, dJointGetHingeAngle);
}

IoObject *IoODEHinge_angleRate(IoODEHinge *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getReal(self, locals, m, dJointGetHingeAngleRate);
}


IoObject *IoODEHinge_setAnchor(IoODEHinge *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_setVector3(self, locals, m, dJointSetHingeAnchor);
}

IoObject *IoODEHinge_setAxis(IoODEHinge *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_setVector3(self, locals, m, dJointSetHingeAxis);
}

IoObject *IoODEHinge_addTorque(IoODEHinge *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_setReal(self, locals, m, dJointAddHingeTorque);
}

