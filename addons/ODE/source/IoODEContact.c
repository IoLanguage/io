
//metadoc ODEContact copyright Jonathan Wright, 2006
//metadoc ODEContact license BSD revised
/*metadoc ODEContact description
ODEContact binding
*/

#include "IoODEContact.h"
#include "IoState.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "IoSeq_ode.h"
#include "IoMessage.h"
#include "geom.h"

#define DATA(self) ((IoODEContactData *)IoObject_dataPointer(self))

IoTag *IoODEContact_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("ODEContact");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoODEContact_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoODEContact_mark);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoODEContact_rawClone);
	return tag;
}

IoODEContact *IoODEContact_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoODEContact_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODEContactData)));

	IoState_registerProtoWithFunc_(state, self, IoODEContact_proto);

	{
		IoMethodTable methodTable[] = {
		{"position", IoODEContact_position},
		{"setPosition", IoODEContact_setPosition},
		{"normal", IoODEContact_normal},
		{"setNormal", IoODEContact_setNormal},
		{"depth", IoODEContact_depth},
		{"setDepth", IoODEContact_setDepth},
		{"geom1", IoODEContact_geom1},
		{"setGeom1", IoODEContact_setGeom1},
		{"geom2", IoODEContact_geom2},
		{"setGeom2", IoODEContact_setGeom2},
		{"side1", IoODEContact_side1},
		{"setSide1", IoODEContact_setSide1},
		{"side2", IoODEContact_side2},
		{"setSide2", IoODEContact_setSide2},
		{"mu", IoODEContact_mu},
		{"setMu", IoODEContact_setMu},

		{"frictionDirection", IoODEContact_frictionDirection},
		{"setFrictionDirection", IoODEContact_setFrictionDirection},
		{"frictionDirectionEnabled", IoODEContact_frictionDirectionEnabled},
		{"disableFrictionDirection", IoODEContact_disableFrictionDirection},

		{"mu2", IoODEContact_mu2},
		{"setMu2", IoODEContact_setMu2},
		{"mu2Enabled", IoODEContact_mu2Enabled},
		{"disableMu2", IoODEContact_disableMu2},

		{"bounce", IoODEContact_bounce},
		{"setBounce", IoODEContact_setBounce},
		{"bounceEnabled", IoODEContact_bounceEnabled},
		{"disableBounce", IoODEContact_disableBounce},
		{"bounceVelocity", IoODEContact_bounceVelocity},
		{"setBounceVelocity", IoODEContact_setBounceVelocity},

		{"softErp", IoODEContact_softErp},
		{"setSoftErp", IoODEContact_setSoftErp},
		{"softErpEnabled", IoODEContact_softErpEnabled},
		{"disableSoftErp", IoODEContact_disableSoftErp},

		{"softCfm", IoODEContact_softCfm},
		{"setSoftCfm", IoODEContact_setSoftCfm},
		{"softCfmEnabled", IoODEContact_softCfmEnabled},
		{"disableSoftCfm", IoODEContact_disableSoftCfm},

		{"motion1", IoODEContact_motion1},
		{"setMotion1", IoODEContact_setMotion1},
		{"motion1Enabled", IoODEContact_motion1Enabled},
		{"disableMotion1", IoODEContact_disableMotion1},

		{"motion2", IoODEContact_motion2},
		{"setMotion2", IoODEContact_setMotion2},
		{"motion2Enabled", IoODEContact_motion2Enabled},
		{"disableMotion2", IoODEContact_disableMotion2},

		{"slip1", IoODEContact_slip1},
		{"setSlip1", IoODEContact_setSlip1},
		{"slip1Enabled", IoODEContact_slip1Enabled},
		{"disableSlip1", IoODEContact_disableSlip1},

		{"slip2", IoODEContact_slip2},
		{"setSlip2", IoODEContact_setSlip2},
		{"slip2Enabled", IoODEContact_slip2Enabled},
		{"disableSlip2", IoODEContact_disableSlip2},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoODEContact *IoODEContact_rawClone(IoODEContact *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODEContactData)));
	memcpy(DATA(self), DATA(proto), sizeof(IoODEContactData));
	return self;
}

void IoODEContact_free(IoODEContact *self)
{
	free(IoObject_dataPointer(self));
}

void IoODEContact_mark(IoODEContact *self)
{
	if (DATA(self)->geom.g1)
	{
		IoObject_shouldMark(IoODEGeom_geomFromId(IOSTATE, DATA(self)->geom.g1));
	}

	if (DATA(self)->geom.g2)
	{
		IoObject_shouldMark(IoODEGeom_geomFromId(IOSTATE, DATA(self)->geom.g2));
	}
}

IoODEContact *IoODEContact_new(void *state)
{
	IoODEContact *proto = IoState_protoWithInitFunction_(state, IoODEContact_proto);
	return IOCLONE(proto);
}

IoODEContact *IoODEContact_newContactGeom(void *state, dContactGeom *contact)
{
	IoODEContact *proto = IoState_protoWithInitFunction_(state, IoODEContact_proto);
	IoODEContact *self = IOCLONE(proto);

	memcpy(&(DATA(self)->geom), contact, sizeof(*contact));

	return self;
}

/* ----------------------------------------------------------- */

IoODEContact *IoMessage_locals_odeContactArgAt_(IoMessage *self, void *locals, int n)
{
	IoObject *m = IoMessage_locals_valueArgAt_(self, locals, n);

	if (!ISODECONTACT(m))
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "ODEContact");
	}

	return m;
}

dContact *IoMessage_locals_odeContactStructArgAt_(IoMessage *self, void *locals, int n)
{
	return IoODEContact_dContactStruct(IoMessage_locals_odeContactArgAt_(self, locals, n));
}

dContact *IoODEContact_dContactStruct(IoODEContact *self)
{
	return DATA(self);
}

/* ----------------------------------------------------------- */

IoObject *IoODEContact_position(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IoSeq_newWithODEPoint(IOSTATE, DATA(self)->geom.pos);
}

IoObject *IoODEContact_setPosition(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->geom.pos[0] = IoMessage_locals_doubleArgAt_(m, locals, 0);
	DATA(self)->geom.pos[1] = IoMessage_locals_doubleArgAt_(m, locals, 1);
	DATA(self)->geom.pos[2] = IoMessage_locals_doubleArgAt_(m, locals, 2);

	return self;
}

IoObject *IoODEContact_normal(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IoSeq_newWithODEPoint(IOSTATE, DATA(self)->geom.normal);
}

IoObject *IoODEContact_setNormal(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->geom.normal[0] = IoMessage_locals_doubleArgAt_(m, locals, 0);
	DATA(self)->geom.normal[1] = IoMessage_locals_doubleArgAt_(m, locals, 1);
	DATA(self)->geom.normal[2] = IoMessage_locals_doubleArgAt_(m, locals, 2);

	return self;
}

IoObject *IoODEContact_depth(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->geom.depth);
}

IoObject *IoODEContact_setDepth(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->geom.depth = IoMessage_locals_doubleArgAt_(m, locals, 0);

	return self;
}

IoObject *IoODEContact_geom1(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IoODEGeom_geomFromId(IOSTATE, DATA(self)->geom.g1);
}

IoObject *IoODEContact_setGeom1(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->geom.g1 = IoMessage_locals_odeGeomIdArgAt_(m, locals, 0);
	return self;
}

IoObject *IoODEContact_geom2(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IoODEGeom_geomFromId(IOSTATE, DATA(self)->geom.g2);
}

IoObject *IoODEContact_setGeom2(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->geom.g2 = IoMessage_locals_odeGeomIdArgAt_(m, locals, 0);
	return self;
}

IoObject *IoODEContact_side1(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->geom.side1);
}

IoObject *IoODEContact_setSide1(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->geom.side1 = IoMessage_locals_doubleArgAt_(m, locals, 0);

	return self;
}

IoObject *IoODEContact_side2(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->geom.side2);
}

IoObject *IoODEContact_setSide2(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->geom.side2 = IoMessage_locals_doubleArgAt_(m, locals, 0);

	return self;
}

IoObject *IoODEContact_mu(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->surface.mu);
}

IoObject *IoODEContact_setMu(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.mu = IoMessage_locals_doubleArgAt_(m, locals, 0);

	return self;
}

IoObject *IoODEContact_frictionDirection(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IoSeq_newWithODEPoint(IOSTATE, DATA(self)->fdir1);
}

IoObject *IoODEContact_setFrictionDirection(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->fdir1[0] = IoMessage_locals_doubleArgAt_(m, locals, 0);
	DATA(self)->fdir1[1] = IoMessage_locals_doubleArgAt_(m, locals, 1);
	DATA(self)->fdir1[2] = IoMessage_locals_doubleArgAt_(m, locals, 2);
	DATA(self)->surface.mode |= dContactFDir1;

	return self;
}

IoObject *IoODEContact_frictionDirectionEnabled(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IOBOOL(self, DATA(self)->surface.mode & dContactFDir1);
}

IoObject *IoODEContact_disableFrictionDirection(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.mode ^= dContactFDir1;
	return self;
}

IoObject *IoODEContact_mu2(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->surface.mu2);
}

IoObject *IoODEContact_setMu2(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.mu2 = IoMessage_locals_doubleArgAt_(m, locals, 0);
	DATA(self)->surface.mode |= dContactMu2;

	return self;
}

IoObject *IoODEContact_mu2Enabled(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IOBOOL(self, DATA(self)->surface.mode & dContactMu2);
}

IoObject *IoODEContact_disableMu2(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.mode ^= dContactMu2;
	return self;
}



IoObject *IoODEContact_bounce(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->surface.bounce);
}

IoObject *IoODEContact_setBounce(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.bounce = IoMessage_locals_doubleArgAt_(m, locals, 0);
	DATA(self)->surface.mode |= dContactBounce;

	return self;
}

IoObject *IoODEContact_bounceEnabled(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IOBOOL(self, DATA(self)->surface.mode & dContactBounce);
}

IoObject *IoODEContact_disableBounce(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.mode ^= dContactBounce;
	return self;
}

IoObject *IoODEContact_bounceVelocity(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->surface.bounce_vel);
}

IoObject *IoODEContact_setBounceVelocity(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.bounce_vel = IoMessage_locals_doubleArgAt_(m, locals, 0);

	return self;
}



IoObject *IoODEContact_softErp(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->surface.soft_erp);
}

IoObject *IoODEContact_setSoftErp(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.soft_erp = IoMessage_locals_doubleArgAt_(m, locals, 0);
	DATA(self)->surface.mode |= dContactSoftERP;

	return self;
}

IoObject *IoODEContact_softErpEnabled(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IOBOOL(self, DATA(self)->surface.mode & dContactSoftERP);
}

IoObject *IoODEContact_disableSoftErp(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.mode ^= dContactSoftERP;
	return self;
}



IoObject *IoODEContact_softCfm(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->surface.soft_cfm);
}

IoObject *IoODEContact_setSoftCfm(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.soft_cfm = IoMessage_locals_doubleArgAt_(m, locals, 0);
	DATA(self)->surface.mode |= dContactSoftCFM;

	return self;
}

IoObject *IoODEContact_softCfmEnabled(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IOBOOL(self, DATA(self)->surface.mode & dContactSoftCFM);
}

IoObject *IoODEContact_disableSoftCfm(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.mode ^= dContactSoftCFM;
	return self;
}


IoObject *IoODEContact_motion1(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->surface.motion1);
}

IoObject *IoODEContact_setMotion1(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.motion1 = IoMessage_locals_doubleArgAt_(m, locals, 0);
	DATA(self)->surface.mode |= dContactMotion1;

	return self;
}

IoObject *IoODEContact_motion1Enabled(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IOBOOL(self, DATA(self)->surface.mode & dContactMotion1);
}

IoObject *IoODEContact_disableMotion1(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.mode ^= dContactMotion1;
	return self;
}


IoObject *IoODEContact_motion2(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->surface.motion2);
}

IoObject *IoODEContact_setMotion2(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.motion2 = IoMessage_locals_doubleArgAt_(m, locals, 0);
	DATA(self)->surface.mode |= dContactMotion2;

	return self;
}

IoObject *IoODEContact_motion2Enabled(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IOBOOL(self, DATA(self)->surface.mode & dContactMotion2);
}

IoObject *IoODEContact_disableMotion2(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.mode ^= dContactMotion2;
	return self;
}



IoObject *IoODEContact_slip1(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->surface.slip1);
}

IoObject *IoODEContact_setSlip1(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.slip1 = IoMessage_locals_doubleArgAt_(m, locals, 0);
	DATA(self)->surface.mode |= dContactSlip1;

	return self;
}

IoObject *IoODEContact_slip1Enabled(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IOBOOL(self, DATA(self)->surface.mode & dContactSlip1);
}

IoObject *IoODEContact_disableSlip1(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.mode ^= dContactSlip1;
	return self;
}


IoObject *IoODEContact_slip2(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->surface.slip2);
}

IoObject *IoODEContact_setSlip2(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.slip2 = IoMessage_locals_doubleArgAt_(m, locals, 0);
	DATA(self)->surface.mode |= dContactSlip2;

	return self;
}

IoObject *IoODEContact_slip2Enabled(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	return IOBOOL(self, DATA(self)->surface.mode & dContactSlip2);
}

IoObject *IoODEContact_disableSlip2(IoODEContact *self, IoObject *locals, IoMessage *m)
{
	DATA(self)->surface.mode ^= dContactSlip2;
	return self;
}
