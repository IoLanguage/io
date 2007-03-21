/*#io
ODEJoint ioDoc(
		 docCopyright("Jonathan Wright", 2006)
		 docLicense("BSD revised")
		 docDescription("ODEJoint binding")
		 */

#include "IoODEJoint_internal.h"
#include "IoODESlider.h"
#include "IoODEBody.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoVector_ode.h"
#include "GLIncludes.h"

/* ----------------------------------------------------------- */

IoTag *IoODESlider_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("ODESlider");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoODESlider_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoODESlider_mark);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoODESlider_rawClone);
	return tag;
}

IoODESlider *IoODESlider_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoODESlider_newTag(state));

	IoODEJoint_protoCommon(self);
	
	IoState_registerProtoWithFunc_(state, self, IoODESlider_proto);
	
	{
		IoMethodTable methodTable[] = {
                ODE_COMMON_JOINT_METHODS
#define PARAM(X, _N, _SETN) \
		{#_N, IoODESlider_##_N}, \
		{#_SETN, IoODESlider_##_SETN},
PARAMS
#undef PARAM

		{"axis", IoODESlider_axis},
                {"setAxis", IoODESlider_setAxis},
		{"position", IoODESlider_position},
		{"positionRate", IoODESlider_positionRate},
                {"addForce", IoODESlider_addForce},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoODESlider *IoODESlider_rawClone(IoODESlider *proto) 
{ 
	IoObject *self = IoODEJoint_rawClone(proto);

	if(DATA(proto)->jointGroup)
	{
		IoODEJointGroup *jointGroup = DATA(proto)->jointGroup;

		JOINTGROUP = jointGroup;
		IoODEJointGroup_addJoint(jointGroup, self);
		JOINTID = dJointCreateSlider(WORLDID, JOINTGROUPID);
	}
	return self;
}

void IoODESlider_free(IoODESlider *self) 
{ 
	IoODEJoint_free(self);
}

void IoODESlider_mark(IoODESlider *self) 
{ 
	IoODEJoint_mark(self);
}

IoODEJoint *IoODESlider_newProto(void *state, IoODEJointGroup *jointGroup)
{
	return IoODEJoint_newProtoCommon(state, IoODESlider_proto, jointGroup);
}

/* ----------------------------------------------------------- */


IoObject *IoODESlider_getParam(IoODESlider *self, IoObject *locals, IoMessage *m, int parameter)
{
	return IoODEJoint_getParam(self, locals, m, parameter, dJointGetSliderParam);
}

IoObject *IoODESlider_setParam(IoODESlider *self, IoObject *locals, IoMessage *m, int parameter)
{
	return IoODEJoint_setParam(self, locals, m, parameter, dJointSetSliderParam);
}

#define PARAM(X, _N, _SETN) \
IoObject *IoODESlider_##_N(IoODESlider *self, IoObject *locals, IoMessage *m) \
{ \
	return IoODESlider_getParam(self, locals, m, X); \
} \
 \
IoObject *IoODESlider_##_SETN(IoODESlider *self, IoObject *locals, IoMessage *m) \
{ \
	return IoODESlider_setParam(self, locals, m, X); \
}
PARAMS
#undef PARAM

IoObject *IoODESlider_axis(IoODESlider *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getVector3(self, locals, m, dJointGetSliderAxis);
}

IoObject *IoODESlider_position(IoODESlider *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getReal(self, locals, m, dJointGetSliderPosition);
}

IoObject *IoODESlider_positionRate(IoODESlider *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_getReal(self, locals, m, dJointGetSliderPositionRate);
}


IoObject *IoODESlider_setAxis(IoODESlider *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_setVector3(self, locals, m, dJointSetSliderAxis);
}

IoObject *IoODESlider_addForce(IoODESlider *self, IoObject *locals, IoMessage *m)
{
	return IoODEJoint_setReal(self, locals, m, dJointAddSliderForce);
}


