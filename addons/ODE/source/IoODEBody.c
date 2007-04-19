/*#io
ODEBody ioDoc(
		 docCopyright("Jonathan Wright", 2006)
		 docLicense("BSD revised")
		 docDescription("ODEBody binding")
		 */

#include "IoODEBody.h"
#include "IoODEMass.h"
#include "IoState.h"
#include "IoSeq.h"
#include "IoVector_ode.h"
#include "GLIncludes.h"

#define DATA(self) ((IoODEBodyData *)IoObject_dataPointer(self))
#define BODYID (DATA(self)->bodyId)
#define WORLD (DATA(self)->world)

void IoODEBody_assertValidBody(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	IOASSERT(WORLD, "This ODE body belongs to an ODE world which has been freed.");
	IOASSERT(BODYID, "ODE World Body cannot be used directly. Clone the world and use the Body on the cloned world.");
}


IoODEBody *IoMessage_locals_odeBodyArgAt_(IoMessage *self, void *locals, int n)
{
	IoObject *b = IoMessage_locals_valueArgAt_(self, locals, n);
	
	if (!ISODEBODY(b) && !ISNIL(b)) 
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "ODEBody"); 
	}
	
	return b;
}

dBodyID IoMessage_locals_odeBodyIdArgAt_(IoMessage *self, void *locals, int n)
{
	IoObject *body = IoMessage_locals_odeBodyArgAt_(self, locals, n);
	if (ISNIL(body))
	{
		return 0;
	}
	else
	{
		return DATA(body)->bodyId;
	}
}


IoTag *IoODEBody_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("ODEBody");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoODEBody_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoODEBody_mark);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoODEBody_rawClone);
	return tag;
}

IoODEBody *IoODEBody_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoODEBody_newTag(state));
	
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODEBodyData)));

	BODYID = 0;
	WORLD = 0L;
	
	IoState_registerProtoWithFunc_(state, self, IoODEBody_proto);
	
	{
		IoMethodTable methodTable[] = {
		{"bodyId", IoODEBody_bodyId},
		{"world", IoODEBody_world},
		{"mass", IoODEBody_mass},
		{"setMass", IoODEBody_setMass},
		{"position", IoODEBody_position},
		{"setPosition", IoODEBody_setPosition},

		{"force", IoODEBody_force},
		{"setForce", IoODEBody_setForce},
		{"addForce", IoODEBody_addForce},
		{"addRelForce", IoODEBody_addRelForce},

		{"torque", IoODEBody_torque},
		{"setTorque", IoODEBody_setTorque},
		{"addTorque", IoODEBody_addTorque},
		{"addRelTorque", IoODEBody_addRelTorque},

		{"linearVelocity", IoODEBody_linearVelocity},
		{"setLinearVelocity", IoODEBody_setLinearVelocity},

		{"quaternion", IoODEBody_quaternion},
		//{"setQuaternion", IoODEBody_setQuaternion},
		{"rotation", IoODEBody_rotation},
		{"setRotation", IoODEBody_setRotation},
		{"glMultMatrix", IoODEBody_glMultMatrix},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoODEBody *IoODEBody_rawClone(IoODEBody *proto) 
{ 
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODEBodyData)));

	if(DATA(proto)->world)
	{
		IoODEWorld *world = DATA(proto)->world;

		WORLD = world;
		IoODEWorld_addBody(world, self);
		BODYID = dBodyCreate(IoODEWorld_rawWorldId(world));
                dBodySetData(BODYID, self);
	}
	return self; 
}

void IoODEBody_free(IoODEBody *self) 
{ 
	if(BODYID && WORLD)
	{
		IoODEWorld_removeBody(WORLD, self);
		dBodyDestroy(BODYID);
	}
	free(IoObject_dataPointer(self)); 
}

void IoODEBody_mark(IoODEBody *self) 
{ 
	if(WORLD)
	{
		IoObject_shouldMark((IoObject *)WORLD); 
	}
}

IoODEBody *IoODEBody_new(void *state)
{
	IoODEBody *proto = IoState_protoWithInitFunction_(state, IoODEBody_proto);
	return IOCLONE(proto);
}

IoODEBody *IoODEBody_newBodyProtoWithWorld(void *state, IoODEWorld *world)
{
	IoODEBody *proto = IoState_protoWithInitFunction_(state, IoODEBody_proto);
	IoODEBody *self = IOCLONE(proto);
	WORLD = world;
	return self;
}

IoObject *IoODEBody_bodyFromId(void *state, dBodyID id)
{
        if (id == 0)
        {
                return ((IoState*)state)->ioNil;
        }
        else
        {
                return (IoODEBody*)dBodyGetData(id);
        }
}

/* ----------------------------------------------------------- */

void IoODEBody_worldDestroyed(IoODEBody *self)
{
	WORLD = 0L;
	BODYID = 0;
}

/* ----------------------------------------------------------- */


IoObject *IoODEBody_bodyId(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER((long)BODYID);
}

IoObject *IoODEBody_world(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	return WORLD ? WORLD : IONIL(self);
}

IoObject *IoODEBody_mass(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	IoODEBody_assertValidBody(self, locals, m);

	{
		IoODEMass *mass = IoODEMass_new(IOSTATE);
		dBodyGetMass(BODYID, IoODEMass_dMassStruct(mass));
		return mass;
	}
}

IoObject *IoODEBody_setMass(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	IoODEBody_assertValidBody(self, locals, m);

	{
		dMass *mass = IoMessage_locals_odeMassStructArgAt_(m, locals, 0);
		dBodySetMass(BODYID, mass);
	}

	return self;
}

IoObject *IoODEBody_position(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	IoODEBody_assertValidBody(self, locals, m);
	return IoVector_newWithODEPoint(IOSTATE, dBodyGetPosition(BODYID));
}

IoObject *IoODEBody_setPosition(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	const double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	const double z = IoMessage_locals_doubleArgAt_(m, locals, 2);

	IoODEBody_assertValidBody(self, locals, m);
	dBodySetPosition(BODYID, x, y, z);
	return self;
}

IoObject *IoODEBody_force(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	IoODEBody_assertValidBody(self, locals, m);
	return IoVector_newWithODEPoint(IOSTATE, dBodyGetForce(BODYID));
}

IoObject *IoODEBody_setForce(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	const double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	const double z = IoMessage_locals_doubleArgAt_(m, locals, 2);

	IoODEBody_assertValidBody(self, locals, m);
	dBodySetForce(BODYID, x, y, z);
	return self;
}

IoObject *IoODEBody_addForce(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	const double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	const double z = IoMessage_locals_doubleArgAt_(m, locals, 2);

	IoODEBody_assertValidBody(self, locals, m);
	dBodyAddForce(BODYID, x, y, z);
	return self;
}

IoObject *IoODEBody_addRelForce(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	const double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	const double z = IoMessage_locals_doubleArgAt_(m, locals, 2);

	IoODEBody_assertValidBody(self, locals, m);
	dBodyAddRelForce(BODYID, x, y, z);
	return self;
}

IoObject *IoODEBody_torque(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	IoODEBody_assertValidBody(self, locals, m);
	return IoVector_newWithODEPoint(IOSTATE, dBodyGetTorque(BODYID));
}

IoObject *IoODEBody_setTorque(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	const double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	const double z = IoMessage_locals_doubleArgAt_(m, locals, 2);

	IoODEBody_assertValidBody(self, locals, m);
	dBodySetTorque(BODYID, x, y, z);
	return self;
}

IoObject *IoODEBody_addTorque(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	const double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	const double z = IoMessage_locals_doubleArgAt_(m, locals, 2);

	IoODEBody_assertValidBody(self, locals, m);
	dBodyAddTorque(BODYID, x, y, z);
	return self;
}

IoObject *IoODEBody_addRelTorque(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	const double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	const double z = IoMessage_locals_doubleArgAt_(m, locals, 2);

	IoODEBody_assertValidBody(self, locals, m);
	dBodyAddRelTorque(BODYID, x, y, z);
	return self;
}

IoObject *IoODEBody_linearVelocity(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	IoODEBody_assertValidBody(self, locals, m);
	return IoVector_newWithODEPoint(IOSTATE, dBodyGetLinearVel(BODYID));
}

IoObject *IoODEBody_setLinearVelocity(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	const double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	const double z = IoMessage_locals_doubleArgAt_(m, locals, 2);

	IoODEBody_assertValidBody(self, locals, m);
	dBodySetLinearVel(BODYID, x, y, z);
	return self;
}

IoObject *IoODEBody_glMultMatrix(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	IoODEBody_assertValidBody(self, locals, m);
	{
		dBodyID bodyId = BODYID;

		const dReal *pos = dBodyGetPosition(bodyId);
		const dReal *R = dBodyGetRotation(bodyId);

		GLfloat matrix[16] = {R[0], R[4], R[8], 0, R[1], R[5], R[9], 0, R[2], R[6], R[10], 0, pos[0], pos[1], pos[2], 1};
		glMultMatrixf(matrix);
		return self;
	}
}

IoObject *IoODEBody_rotation(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	IoODEBody_assertValidBody(self, locals, m);
	{
		const dReal *R = dBodyGetRotation(BODYID);

		IoSeq *v = IoSeq_makeFloatArrayOfSize_(IOSTATE, 9);
		UArray *u = IoSeq_rawUArray(v);
		
		UArray_at_put_(u, 0, R[0]);
		UArray_at_put_(u, 1, R[4]);
		UArray_at_put_(u, 2, R[8]);

		UArray_at_put_(u, 3, R[1]);
		UArray_at_put_(u, 4, R[5]);
		UArray_at_put_(u, 5, R[9]);

		UArray_at_put_(u, 6, R[2]);
		UArray_at_put_(u, 7, R[6]);
		UArray_at_put_(u, 8, R[10]);
		
		return v;
	}
}

IoObject *IoODEBody_setRotation(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	IoODEBody_assertValidBody(self, locals, m);
	{
		dMatrix3 R;
        R[0] = IoMessage_locals_doubleArgAt_(m, locals, 0);
        R[1] = IoMessage_locals_doubleArgAt_(m, locals, 1);
        R[2] = IoMessage_locals_doubleArgAt_(m, locals, 2);
        R[3] = 0;
        R[4] = IoMessage_locals_doubleArgAt_(m, locals, 3);
        R[5] = IoMessage_locals_doubleArgAt_(m, locals, 4);
        R[6] = IoMessage_locals_doubleArgAt_(m, locals, 5);
        R[7] = 0;
        R[8] = IoMessage_locals_doubleArgAt_(m, locals, 6);
        R[9] = IoMessage_locals_doubleArgAt_(m, locals, 7);
        R[10] = IoMessage_locals_doubleArgAt_(m, locals, 8);
        R[11] = 0;

		dBodySetRotation(BODYID, R);
		return self;
	}
}

IoObject *IoODEBody_quaternion(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	IoODEBody_assertValidBody(self, locals, m);
	{
		const dReal *q = dBodyGetQuaternion(BODYID);
		IoSeq *v = IoSeq_makeFloatArrayOfSize_(IOSTATE, 4);
		UArray *u = IoSeq_rawUArray(v);
		UArray_at_put_(u, 0, q[0]);
		UArray_at_put_(u, 1, q[1]);
		UArray_at_put_(u, 2, q[2]);
		UArray_at_put_(u, 3, q[3]);
		return v;
	}
}

/*
IoObject *IoODEBody_setQuaternion(IoODEBody *self, IoObject *locals, IoMessage *m)
{
	const double w = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double x = IoMessage_locals_doubleArgAt_(m, locals, 1);
	const double y = IoMessage_locals_doubleArgAt_(m, locals, 2);
	const double z = IoMessage_locals_doubleArgAt_(m, locals, 3);

	IoODEBody_assertValidBody(self, locals, m);
	dBodySetQuaternion(BODYID, w, x, y, z);
	return self;
}
*/
