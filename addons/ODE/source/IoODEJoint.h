//metadoc ODEJoint copyright Jonathan Wright, 2006
//metadoc ODEJoint license All rights reserved. See _BSDLicense.txt.

#ifndef IOODEJoint_DEFINED
#define IOODEJoint_DEFINED 1

#include "IoObject.h"
#include "IoODEJoint_typedef.h"
#include "IoNumber.h"
#include "IoODEWorld.h"

#include <ode/ode.h>

typedef struct
{
	dJointID jointId;
	IoODEJointGroup *jointGroup;
} IoODEJointData;

IoODEJoint *IoODEJoint_rawClone(IoODEJoint *self);
IoODEJoint *IoODEJoint_proto(void *state);
IoODEJoint *IoODEJoint_newJointWithJointId(void *state, IoODEJointGroup *jointGroup, dJointID jointId);

void IoODEJoint_free(IoODEJoint *self);
void IoODEJoint_mark(IoODEJoint *self);

/* ----------------------------------------------------------- */

void IoODEJoint_worldDestoryed(IoODEJoint *self);

/* ----------------------------------------------------------- */

#define ODE_COMMON_JOINT_METHODS \
		{"jointId", IoODEJoint_jointId}, \
		{"jointGroup", IoODEJoint_jointGroup}, \
		{"attach", IoODEJoint_attach}, \
		{"attachedBody1", IoODEJoint_attachedBody1}, \
		{"attachedBody2", IoODEJoint_attachedBody2},

IoObject *IoODEJoint_jointId(IoODEJoint *self, IoObject *locals, IoMessage *m);
IoObject *IoODEJoint_jointGroup(IoODEJoint *self, IoObject *locals, IoMessage *m);

IoObject *IoODEJoint_attachedBody1(IoODEJoint *self, IoObject *locals, IoMessage *m);
IoObject *IoODEJoint_attachedBody2(IoODEJoint *self, IoObject *locals, IoMessage *m);

typedef dReal (*IoODEJointGetParamFunc)(dJointID jointId, int parameter);
typedef void (*IoODEJointSetParamFunc)(dJointID jointId, int parameter, dReal value);

IoObject *IoODEJoint_getParam(IoODEJoint *self, IoObject *locals, IoMessage *m, int parameter, IoODEJointGetParamFunc func);
IoObject *IoODEJoint_setParam(IoODEJoint *self, IoObject *locals, IoMessage *m, int parameter, IoODEJointSetParamFunc func);

typedef void (*IoODEJointGetVector3Func)(dJointID, dVector3 result);
typedef void (*IoODEJointSetVector3Func)(dJointID, dReal x, dReal y, dReal z);

IoObject *IoODEJoint_getVector3(IoODEJoint *self, IoObject *locals, IoMessage *m, IoODEJointGetVector3Func func);
IoObject *IoODEJoint_setVector3(IoODEJoint *self, IoObject *locals, IoMessage *m, IoODEJointSetVector3Func func);

typedef dReal (*IoODEJointGetReal)(dJointID jointId);
typedef void (*IoODEJointSetReal)(dJointID jointId, dReal value);
typedef void (*IoODEJointSetReal2)(dJointID jointId, dReal value1, dReal value2);

IoObject *IoODEJoint_getReal(IoODEJoint *self, IoObject *locals, IoMessage *m, IoODEJointGetReal func);
IoObject *IoODEJoint_setReal(IoODEJoint *self, IoObject *locals, IoMessage *m, IoODEJointSetReal func);
IoObject *IoODEJoint_setReal2(IoODEJoint *self, IoObject *locals, IoMessage *m, IoODEJointSetReal2 func);

#endif
