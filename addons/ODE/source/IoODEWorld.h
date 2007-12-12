/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IOODEWorld_DEFINED
#define IOODEWorld_DEFINED 1

#include "IoObject.h"
#include "IoODEBody_typedef.h"
#include "IoODEJointGroup_typedef.h"
#include "IoNumber.h"

#include <ode/ode.h>

typedef IoObject IoODEWorld;

typedef struct
{
	dWorldID worldId;
	List *bodies;
	List *jointGroups;
} IoODEWorldData;

IoODEWorld *IoODEWorld_rawClone(IoODEWorld *self);
IoODEWorld *IoODEWorld_proto(void *state);
IoODEWorld *IoODEWorld_new(void *state);

void IoODEWorld_free(IoODEWorld *self);
void IoODEWorld_mark(IoODEWorld *self);

/* ----------------------------------------------------------- */

dWorldID IoODEWorld_rawWorldId(IoODEWorld *self);
void IoODEWorld_addBody(IoODEWorld *self, IoODEBody *body);
void IoODEWorld_removeBody(IoODEWorld *self, IoODEBody *body);
void IoODEWorld_addJointGroup(IoODEWorld *self, IoODEJointGroup *jointGroup);
void IoODEWorld_removeJointGroup(IoODEWorld *self, IoODEJointGroup *jointGroup);

void IoODEWorld_emptyJointGroups(IoODEWorld *self);

/* ----------------------------------------------------------- */

IoObject *IoODEWorld_worldId(IoODEWorld *self, IoObject *locals, IoMessage *m);
IoObject *IoODEWorld_bodies(IoODEWorld *self, IoObject *locals, IoMessage *m);
IoObject *IoODEWorld_jointGroups(IoODEWorld *self, IoObject *locals, IoMessage *m);

IoObject *IoODEWorld_gravity(IoODEWorld *self, IoObject *locals, IoMessage *m);
IoObject *IoODEWorld_setGravity(IoODEWorld *self, IoObject *locals, IoMessage *m);
IoObject *IoODEWorld_erp(IoODEWorld *self, IoObject *locals, IoMessage *m);
IoObject *IoODEWorld_setErp(IoODEWorld *self, IoObject *locals, IoMessage *m);
IoObject *IoODEWorld_cfm(IoODEWorld *self, IoObject *locals, IoMessage *m);
IoObject *IoODEWorld_setCfm(IoODEWorld *self, IoObject *locals, IoMessage *m);

IoObject *IoODEWorld_step(IoODEWorld *self, IoObject *locals, IoMessage *m);
IoObject *IoODEWorld_quickStep(IoODEWorld *self, IoObject *locals, IoMessage *m);
IoObject *IoODEWorld_stepFast1(IoODEWorld *self, IoObject *locals, IoMessage *m);

#endif
