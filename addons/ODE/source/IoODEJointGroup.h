/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IOODEJointGroup_DEFINED
#define IOODEJointGroup_DEFINED 1

#include "IoObject.h"
#include "IoODEJointGroup_typedef.h"
#include "IoODEJoint_typedef.h"
#include "IoNumber.h"
#include "IoODEWorld.h"

#include <ode/ode.h>

typedef struct
{
	dJointGroupID jointGroupId;
	IoODEWorld *world;
	List *joints;
} IoODEJointGroupData;

IoODEJointGroup *IoODEJointGroup_rawClone(IoODEJointGroup *self);
IoODEJointGroup *IoODEJointGroup_proto(void *state);
IoODEJointGroup *IoODEJointGroup_new(void *state);
IoODEJointGroup *IoODEJointGroup_newJointGroupProtoWithWorld(void *state, IoODEWorld *world);

void IoODEJointGroup_free(IoODEJointGroup *self);
void IoODEJointGroup_mark(IoODEJointGroup *self);

/* ----------------------------------------------------------- */

void IoODEJointGroup_worldDestoryed(IoODEJointGroup *self);
void IoODEJointGroup_rawEmpty(IoODEJointGroup *self);

dJointGroupID IoODEJointGroup_rawJointGroupId(IoODEJointGroup *self);
dWorldID IoODEJointGroup_rawWorldId(IoODEJointGroup *self);
void IoODEJointGroup_addJoint(IoODEJointGroup *self, IoODEJoint *joint);
void IoODEJointGroup_removeJoint(IoODEJointGroup *self, IoODEJoint *joint);

/* ----------------------------------------------------------- */

IoObject *IoODEJointGroup_jointGroupId(IoODEJointGroup *self, IoObject *locals, IoMessage *m);
IoObject *IoODEJointGroup_world(IoODEJointGroup *self, IoObject *locals, IoMessage *m);

IoObject *IoODEJointGroup_empty(IoODEJointGroup *self, IoObject *locals, IoMessage *m);
IoObject *IoODEJointGroup_joints(IoODEWorld *self, IoObject *locals, IoMessage *m);

IoObject *IoODEJointGroup_createContact(IoODEWorld *self, IoObject *locals, IoMessage *m);

#endif
