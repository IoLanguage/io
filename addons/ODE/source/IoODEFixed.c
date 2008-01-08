/*#io
ODEJoint ioDoc(
		 docCopyright("Jonathan Wright", 2006)
		 docLicense("BSD revised")
		 docDescription("ODEJoint binding")
		 */

#include "IoODEJoint_internal.h"
#include "IoODEFixed.h"
#include "IoODEBody.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoVector_ode.h"
#include "GLIncludes.h"

/* ----------------------------------------------------------- */

IoTag *IoODEFixed_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("ODEFixed");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoODEFixed_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoODEFixed_mark);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoODEFixed_rawClone);
	return tag;
}

IoODEFixed *IoODEFixed_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoODEFixed_newTag(state));

	IoODEJoint_protoCommon(self);

	IoState_registerProtoWithFunc_(state, self, IoODEFixed_proto);

	{
		IoMethodTable methodTable[] = {
				ODE_COMMON_JOINT_METHODS

		{"fix", IoODEFixed_fix},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoODEFixed *IoODEFixed_rawClone(IoODEFixed *proto)
{
	IoObject *self = IoODEJoint_rawClone(proto);

	if(DATA(proto)->jointGroup)
	{
		IoODEJointGroup *jointGroup = DATA(proto)->jointGroup;

		JOINTGROUP = jointGroup;
		IoODEJointGroup_addJoint(jointGroup, self);
		JOINTID = dJointCreateFixed(WORLDID, JOINTGROUPID);
	}
	return self;
}

void IoODEFixed_free(IoODEFixed *self)
{
	IoODEJoint_free(self);
}

void IoODEFixed_mark(IoODEFixed *self)
{
	IoODEJoint_mark(self);
}

IoODEJoint *IoODEFixed_newProto(void *state, IoODEJointGroup *jointGroup)
{
	return IoODEJoint_newProtoCommon(state, IoODEFixed_proto, jointGroup);
}

/* ----------------------------------------------------------- */


IoObject *IoODEFixed_fix(IoODEFixed *self, IoObject *locals, IoMessage *m)
{
	IoODEJoint_assertValidJoint(self, locals, m);
		dJointSetFixed(JOINTID);
		return self;
}

