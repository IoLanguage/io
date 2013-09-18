#include "IoODEJoint.h"
#include "IoODEJointGroup.h"
#include <ode/ode.h>

#define DATA(self) ((IoODEJointData *)IoObject_dataPointer(self))
#define JOINTID (DATA(self)->jointId)
#define JOINTGROUP (DATA(self)->jointGroup)
#define WORLDID (IoODEJointGroup_rawWorldId(JOINTGROUP))
#define JOINTGROUPID (IoODEJointGroup_rawJointGroupId(JOINTGROUP))

void IoODEJoint_protoCommon(IoODEJoint *self);
IoODEJoint *IoODEJoint_newProtoCommon(void *state, const char *protoWithId, IoODEJointGroup *jointGroup);

void IoODEJoint_assertValidJoint(IoODEJoint *self, IoObject *locals, IoMessage *m);

IoObject *IoODEJoint_attach(IoODEJoint *self, IoObject *locals, IoMessage *m);
IoObject *IoODEJoint_jointId(IoODEJoint *self, IoObject *locals, IoMessage *m);
IoObject *IoODEJoint_jointGroup(IoODEJoint *self, IoObject *locals, IoMessage *m);
