//metadoc ODEContactJoint copyright Jonathan Wright, 2006
//metadoc ODEContactJoint license BSD revised
//metadoc ODEContactJoint category Physics
/*metadoc ODEContactJoint description
ODEContactJoint binding
*/

#include "IoODEJoint_internal.h"
#include "IoODEContactJoint.h"
#include "IoODEBody.h"
#include "IoState.h"
#include "IoSeq.h"
#include "GLIncludes.h"

/* ----------------------------------------------------------- */

static const char *protoId = "ODEContactJoint";

IoTag *IoODEContactJoint_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_(protoId);
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoODEContactJoint_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoODEContactJoint_mark);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoODEContactJoint_rawClone);
	return tag;
}

IoODEContactJoint *IoODEContactJoint_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoODEContactJoint_newTag(state));

	IoODEJoint_protoCommon(self);

	IoState_registerProtoWithId_(state, self, protoId);

	{
		IoMethodTable methodTable[] = {
		ODE_COMMON_JOINT_METHODS

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoODEContactJoint *IoODEContactJoint_rawClone(IoODEContactJoint *proto)
{
	IoObject *self = IoODEJoint_rawClone(proto);

	if(DATA(proto)->jointGroup)
	{
		IoODEJointGroup *jointGroup = DATA(proto)->jointGroup;

		JOINTGROUP = jointGroup;
	}
	return self;
}

void IoODEContactJoint_free(IoODEContactJoint *self)
{
	IoODEJoint_free(self);
}

void IoODEContactJoint_mark(IoODEContactJoint *self)
{
	IoODEJoint_mark(self);
}

IoODEContactJoint *IoODEContactJoint_new(void *state, IoODEJointGroup *jointGroup, dJointID jointId)
{
	IoODEContactJoint *self = IoODEJoint_newProtoCommon(state, IoODEContactJoint_proto, jointGroup);

	JOINTID = jointId;
	IoODEJointGroup_addJoint(JOINTGROUP, self);

	return self;
}

/* ----------------------------------------------------------- */


