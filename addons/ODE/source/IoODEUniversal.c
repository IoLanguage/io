//metadoc ODEJoint copyright Jonathan Wright, 2006
//metadoc ODEJoint license BSD revised
//metadoc ODEJoint category Physics
/*metadoc ODEJoint description
ODEJoint binding
*/

#include "IoODEJoint_internal.h"
#include "IoODEUniversal.h"
#include "IoODEBody.h"
#include "IoState.h"
#include "IoSeq.h"
#include "GLIncludes.h"

/* ----------------------------------------------------------- */

static const char *protoId = "ODEUniversal";

IoTag *IoODEUniversal_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoODEUniversal_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoODEUniversal_mark);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoODEUniversal_rawClone);
	return tag;
}

IoODEUniversal *IoODEUniversal_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoODEUniversal_newTag(state));

	IoODEJoint_protoCommon(self);

	IoState_registerProtoWithFunc_(state, self, IoODEUniversal_proto);

	{
		IoMethodTable methodTable[] = {
				ODE_COMMON_JOINT_METHODS
#define PARAM(X, _N, _SETN) \
		{#_N, IoODEUniversal_##_N}, \
		{#_SETN, IoODEUniversal_##_SETN},
PARAMS
#undef PARAM

		{"anchor", IoODEUniversal_anchor},
				{"setAnchor", IoODEUniversal_setAnchor},
		{"anchor2", IoODEUniversal_anchor2},
		{"axis1", IoODEUniversal_axis1},
				{"setAxis1", IoODEUniversal_setAxis1},
		{"axis2", IoODEUniversal_axis2},
				{"setAxis2", IoODEUniversal_setAxis2},
		{"angle1", IoODEUniversal_angle1},
		{"angle2", IoODEUniversal_angle2},
		{"angle1Rate", IoODEUniversal_angle1Rate},
		{"angle2Rate", IoODEUniversal_angle2Rate},
				{"addTorques", IoODEUniversal_addTorques},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoODEUniversal *IoODEUniversal_rawClone(IoODEUniversal *proto)
{
	IoObject *self = IoODEJoint_rawClone(proto);

	if(DATA(proto)->jointGroup)
	{
		IoODEJointGroup *jointGroup = DATA(proto)->jointGroup;

		JOINTGROUP = jointGroup;
		IoODEJointGroup_addJoint(jointGroup, self);
		JOINTID = dJointCreateUniversal(WORLDID, JOINTGROUPID);
	}
	return self;
}

void IoODEUniversal_free(IoODEUniversal *self)
{
	IoODEJoint_free(self);
}

void IoODEUniversal_mark(IoODEUniversal *self)
{
	IoODEJoint_mark(self);
}

IoODEJoint *IoODEUniversal_newProto(void *state, IoODEJointGroup *jointGroup)
{
	return IoODEJoint_newProtoCommon(state, IoODEUniversal_proto, jointGroup);
}

/* ----------------------------------------------------------- */


IoObject *IoODEUniversal_getParam(IoODEUniversal *self, IoObject *locals, IoMessage *m, int parameter)
{
	return IoODEJoint_getParam(self, locals, m, parameter, dJointGetUniversalParam);
}

IoObject *IoODEUniversal_setParam(IoODEUniversal *self, IoObject *locals, IoMessage *m, int parameter)
{
	return IoODEJoint_setParam(self, locals, m, parameter, dJointSetUniversalParam);
}

#define PARAM(X, _N, _SETN) \
IoObject *IoODEUniversal_##_N(IoODEUniversal *self, IoObject *locals, IoMessage *m) \
{ \
	return IoODEUniversal_getParam(self, locals, m, X); \
} \
 \
IoObject *IoODEUniversal_##_SETN(IoODEUniversal *self, IoObject *locals, IoMessage *m) \
{ \
	return IoODEUniversal_setParam(self, locals, m, X); \
}
PARAMS
#undef PARAM

IoObject *IoODEUniversal_anchor(IoODEUniversal *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getVector3(self, locals, m, dJointGetUniversalAnchor);
}

IoObject *IoODEUniversal_anchor2(IoODEUniversal *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getVector3(self, locals, m, dJointGetUniversalAnchor2);
}

IoObject *IoODEUniversal_axis1(IoODEUniversal *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getVector3(self, locals, m, dJointGetUniversalAxis1);
}

IoObject *IoODEUniversal_axis2(IoODEUniversal *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getVector3(self, locals, m, dJointGetUniversalAxis2);
}

IoObject *IoODEUniversal_angle1(IoODEUniversal *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getReal(self, locals, m, dJointGetUniversalAngle1);
}

IoObject *IoODEUniversal_angle2(IoODEUniversal *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getReal(self, locals, m, dJointGetUniversalAngle2);
}

IoObject *IoODEUniversal_angle1Rate(IoODEUniversal *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getReal(self, locals, m, dJointGetUniversalAngle1Rate);
}

IoObject *IoODEUniversal_angle2Rate(IoODEUniversal *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getReal(self, locals, m, dJointGetUniversalAngle2Rate);
}


IoObject *IoODEUniversal_setAnchor(IoODEUniversal *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_setVector3(self, locals, m, dJointSetUniversalAnchor);
}

IoObject *IoODEUniversal_setAxis1(IoODEUniversal *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_setVector3(self, locals, m, dJointSetUniversalAxis1);
}

IoObject *IoODEUniversal_setAxis2(IoODEUniversal *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_setVector3(self, locals, m, dJointSetUniversalAxis2);
}

IoObject *IoODEUniversal_addTorques(IoODEUniversal *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_setReal2(self, locals, m, dJointAddUniversalTorques);
}

