//metadoc ODEJoint Jonathan Wright", 2006)
//metadoc ODEJoint license BSD revised
/*metadoc ODEJoint description
ODEJoint binding
*/

#include "IoODEJoint_internal.h"
#include "IoODEBody.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoVector_ode.h"
#include "GLIncludes.h"

/* ----------------------------------------------------------- */

IoTag *IoODEJoint_newTag(void *state)
{
		// TODO: Get rid of IoODEJoint as an IoObject. Can't be manipulated directly through Io.
	IoTag *tag = IoTag_newWithName_("ODEJoint");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoODEJoint_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoODEJoint_mark);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoODEJoint_rawClone);
	return tag;
}

IoODEJoint *IoODEJoint_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoODEJoint_newTag(state));

	IoODEJoint_protoCommon(self);

	IoState_registerProtoWithFunc_(state, self, IoODEJoint_proto);

	{
		IoMethodTable methodTable[] = {
				ODE_COMMON_JOINT_METHODS
		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

void IoODEJoint_protoCommon(IoODEJoint *self)
{
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODEJointData)));

	JOINTID = 0;
	JOINTGROUP = 0L;
}


IoODEJoint *IoODEJoint_rawClone(IoODEJoint *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODEJointData)));
	return self;
}

void IoODEJoint_free(IoODEJoint *self)
{
	if(JOINTID && JOINTGROUP)
	{
		IoODEJointGroup_removeJoint(JOINTGROUP, self);
		dJointDestroy(JOINTID);
	}
	free(IoObject_dataPointer(self));
}

void IoODEJoint_mark(IoODEJoint *self)
{
	if(JOINTGROUP)
	{
		IoObject_shouldMark((IoObject *)JOINTGROUP);
	}
}

IoODEJoint *IoODEJoint_newProtoCommon(void *state, IoStateProtoFunc *func, IoODEJointGroup *jointGroup)
{
	IoODEJoint *proto = IoState_protoWithInitFunction_(state, func);
	IoODEJoint *self = IOCLONE(proto);
	JOINTGROUP = jointGroup;
	return self;
}

/* ----------------------------------------------------------- */

void IoODEJoint_worldDestoryed(IoODEJoint *self)
{
	JOINTGROUP = 0L;
	JOINTID = 0;
}

/* ----------------------------------------------------------- */


IoObject *IoODEJoint_jointId(IoODEJoint *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER((long)JOINTID);
}

IoObject *IoODEJoint_jointGroup(IoODEJoint *self, IoObject *locals, IoMessage *m)
{
	return JOINTGROUP ? JOINTGROUP : IONIL(self);
}

void IoODEJoint_assertValidJoint(IoODEJoint *self, IoObject *locals, IoMessage *m)
{
	IOASSERT(JOINTGROUP, "This ODE joint belongs to an ODE joint group which has been freed or emptied.");
	IOASSERT(JOINTID, "ODE Joint cannot be used directly. Clone the joint and use the Joint on the cloned joint.");
}

IoObject *IoODEJoint_attach(IoODEJoint *self, IoObject *locals, IoMessage *m)
{
	dBodyID body1 = IoMessage_locals_odeBodyIdArgAt_(m, locals, 0);
	dBodyID body2 = IoMessage_locals_odeBodyIdArgAt_(m, locals, 1);

	IoODEJoint_assertValidJoint(self, locals, m);
	dJointAttach(JOINTID, body1, body2);
	return self;
}

IoObject *IoODEJoint_attachedBody1(IoODEJoint *self, IoObject *locals, IoMessage *m)
{
	IoODEJoint_assertValidJoint(self, locals, m);
		return IoODEBody_bodyFromId(IOSTATE, dJointGetBody(JOINTID, 0));
}

IoObject *IoODEJoint_attachedBody2(IoODEJoint *self, IoObject *locals, IoMessage *m)
{
	IoODEJoint_assertValidJoint(self, locals, m);
		return IoODEBody_bodyFromId(IOSTATE, dJointGetBody(JOINTID, 1));
}

IoObject *IoODEJoint_getParam(IoODEJoint *self, IoObject *locals, IoMessage *m, int parameter, IoODEJointGetParamFunc func)
{
	IoODEJoint_assertValidJoint(self, locals, m);
	return IONUMBER(func(JOINTID, parameter));
}

IoObject *IoODEJoint_setParam(IoODEJoint *self, IoObject *locals, IoMessage *m, int parameter, IoODEJointSetParamFunc func)
{
	dReal value = IoMessage_locals_doubleArgAt_(m, locals, 0);
	IoODEJoint_assertValidJoint(self, locals, m);
	func(JOINTID, parameter, value);
	return self;
}

IoObject *IoODEJoint_getVector3(IoODEJoint *self, IoObject *locals, IoMessage *m, IoODEJointGetVector3Func func)
{
	dVector3 anchor;

	IoODEJoint_assertValidJoint(self, locals, m);
	func(JOINTID, anchor);
	return IoVector_newWithODEPoint(IOSTATE, anchor);
}

IoObject *IoODEJoint_setVector3(IoODEJoint *self, IoObject *locals, IoMessage *m, IoODEJointSetVector3Func func)
{
	dReal x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	dReal y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	dReal z = IoMessage_locals_doubleArgAt_(m, locals, 2);

	IoODEJoint_assertValidJoint(self, locals, m);
	func(JOINTID, x, y, z);
	return self;
}

IoObject *IoODEJoint_getReal(IoODEJoint *self, IoObject *locals, IoMessage *m, IoODEJointGetReal func)
{
	IoODEJoint_assertValidJoint(self, locals, m);
	return IONUMBER(func(JOINTID));
}

IoObject *IoODEJoint_setReal(IoODEJoint *self, IoObject *locals, IoMessage *m, IoODEJointSetReal func)
{
	dReal value = IoMessage_locals_doubleArgAt_(m, locals, 0);

	IoODEJoint_assertValidJoint(self, locals, m);
	func(JOINTID, value);
	return self;
}

IoObject *IoODEJoint_setReal2(IoODEJoint *self, IoObject *locals, IoMessage *m, IoODEJointSetReal2 func)
{
	dReal value1 = IoMessage_locals_doubleArgAt_(m, locals, 0);
	dReal value2 = IoMessage_locals_doubleArgAt_(m, locals, 1);

	IoODEJoint_assertValidJoint(self, locals, m);
	func(JOINTID, value1, value2);
	return self;
}
