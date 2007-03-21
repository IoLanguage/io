/*   copyright: Jonathan Wright, 2006
 *   All rights reserved. See _BSDLicense.txt.
 */

#ifndef IOODEBody_DEFINED
#define IOODEBody_DEFINED 1

#include "IoObject.h"
#include "IoODEBody_typedef.h"
#include "IoNumber.h"
#include "IoODEWorld.h"

#include <ode/ode.h>

#define ISODEBODY(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoODEBody_rawClone)

typedef struct
{
	dBodyID bodyId;
	IoODEWorld *world;
} IoODEBodyData;

IoODEBody *IoODEBody_rawClone(IoODEBody *self);
IoODEBody *IoODEBody_proto(void *state);
IoODEBody *IoODEBody_new(void *state);
IoODEBody *IoODEBody_newBodyProtoWithWorld(void *state, IoODEWorld *world);
IoObject *IoODEBody_bodyFromId(void *state, dBodyID id);

void IoODEBody_free(IoODEBody *self);
void IoODEBody_mark(IoODEBody *self);

/* ----------------------------------------------------------- */

void IoODEBody_worldDestoryed(IoODEBody *self);

/* ----------------------------------------------------------- */

IoODEBody *IoMessage_locals_odeBodyArgAt_(IoMessage *self, void *locals, int n);
dBodyID IoMessage_locals_odeBodyIdArgAt_(IoMessage *self, void *locals, int n);

/* ----------------------------------------------------------- */

IoObject *IoODEBody_bodyId(IoODEBody *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBody_world(IoODEBody *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBody_mass(IoODEBody *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBody_setMass(IoODEBody *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBody_position(IoODEBody *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBody_setPosition(IoODEBody *self, IoObject *locals, IoMessage *m);

IoObject *IoODEBody_force(IoODEBody *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBody_setForce(IoODEBody *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBody_addForce(IoODEBody *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBody_addRelForce(IoODEBody *self, IoObject *locals, IoMessage *m);

IoObject *IoODEBody_torque(IoODEBody *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBody_setTorque(IoODEBody *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBody_addTorque(IoODEBody *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBody_addRelTorque(IoODEBody *self, IoObject *locals, IoMessage *m);

IoObject *IoODEBody_linearVelocity(IoODEBody *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBody_setLinearVelocity(IoODEBody *self, IoObject *locals, IoMessage *m);

IoObject *IoODEBody_quaternion(IoODEBody *self, IoObject *locals, IoMessage *m);
//IoObject *IoODEBody_setQuaternion(IoODEBody *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBody_rotation(IoODEBody *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBody_setRotation(IoODEBody *self, IoObject *locals, IoMessage *m);
IoObject *IoODEBody_glMultMatrix(IoODEBody *self, IoObject *locals, IoMessage *m);

#endif
