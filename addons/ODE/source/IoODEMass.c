
//metadoc ODEMass copyright Jonathan Wright, 2006
//metadoc ODEMass license BSD revised
/*metadoc ODEMass description
ODEMass binding
*/

#include "IoODEMass.h"
#include "IoState.h"
#include "IoSeq.h"

#define DATA(self) ((IoODEMassData *)IoObject_dataPointer(self))

IoTag *IoODEMass_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("ODEMass");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoODEMass_free);
	IoTag_markFunc_(tag, (IoTagMarkFunc *)IoODEMass_mark);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoODEMass_rawClone);
	return tag;
}

IoODEMass *IoODEMass_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoODEMass_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODEMassData)));

	IoState_registerProtoWithFunc_(state, self, IoODEMass_proto);

	{
		IoMethodTable methodTable[] = {
		{"reset", IoODEMass_reset},
		{"mass", IoODEMass_mass},
		{"setMass", IoODEMass_setMass},
		{"centerOfGravity", IoODEMass_centerOfGravity},
		{"setCenterOfGravity", IoODEMass_setCenterOfGravity},
		{"inertiaTensor", IoODEMass_inertiaTensor},
		{"parameters", IoODEMass_parameters},
		{"setParameters", IoODEMass_setParameters},

		{"setSphereDensity", IoODEMass_setSphereDensity},
		{"setSphereMass", IoODEMass_setSphereMass},
		{"setCappedCylinderDensity", IoODEMass_setCappedCylinderDensity},
		{"setCappedCylinderMass", IoODEMass_setCappedCylinderMass},
		{"setBoxDensity", IoODEMass_setBoxDensity},
		{"setBoxMass", IoODEMass_setBoxMass},

		{NULL, NULL},
		};
		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoODEMass *IoODEMass_rawClone(IoODEMass *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, calloc(1, sizeof(IoODEMassData)));
	memcpy(DATA(self), DATA(proto), sizeof(IoODEMassData));
	return self;
}

void IoODEMass_free(IoODEMass *self)
{
	free(IoObject_dataPointer(self));
}

void IoODEMass_mark(IoODEMass *self)
{
}

IoODEMass *IoODEMass_new(void *state)
{
	IoODEMass *proto = IoState_protoWithInitFunction_(state, IoODEMass_proto);
	return IOCLONE(proto);
}


/* ----------------------------------------------------------- */

IoODEMass *IoMessage_locals_odeMassArgAt_(IoMessage *self, void *locals, int n)
{
	IoObject *m = IoMessage_locals_valueArgAt_(self, locals, n);

	if (!ISODEMASS(m))
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "ODEMass");
	}

	return m;
}

dMass *IoMessage_locals_odeMassStructArgAt_(IoMessage *self, void *locals, int n)
{
	return IoODEMass_dMassStruct(IoMessage_locals_odeMassArgAt_(self, locals, n));
}

dMass *IoODEMass_dMassStruct(IoODEMass *self)
{
	return DATA(self);
}

/* ----------------------------------------------------------- */

IoObject *IoODEMass_reset(IoODEMass *self, IoObject *locals, IoMessage *m)
{
	dMassSetZero(DATA(self));
	return self;
}

IoObject *IoODEMass_mass(IoODEMass *self, IoObject *locals, IoMessage *m)
{
	return IONUMBER(DATA(self)->mass);
}

IoObject *IoODEMass_setMass(IoODEMass *self, IoObject *locals, IoMessage *m)
{
	//DATA(self)->mass = IoMessage_locals_doubleArgAt_(m, locals, 0);
	dMassAdjust(DATA(self), IoMessage_locals_doubleArgAt_(m, locals, 0));
	return self;
}

IoObject *IoODEMass_centerOfGravity(IoODEMass *self, IoObject *locals, IoMessage *m)
{
	vec3f v;

	v.x = DATA(self)->c[0];
	v.y = DATA(self)->c[1];
	v.z = DATA(self)->c[2];

	return IoSeq_newVec3f(IOSTATE, v);
}

IoObject *IoODEMass_setCenterOfGravity(IoODEMass *self, IoObject *locals, IoMessage *m)
{
	const double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double y = IoMessage_locals_doubleArgAt_(m, locals, 1);
	const double z = IoMessage_locals_doubleArgAt_(m, locals, 2);

	DATA(self)->c[0] = x;
	DATA(self)->c[1] = y;
	DATA(self)->c[2] = z;
	return self;
}

IoObject *IoODEMass_inertiaTensor(IoODEMass *self, IoObject *locals, IoMessage *m)
{
	UArray *u = UArray_new();
	int i, j;

	UArray_setItemType_(u, CTYPE_float32_t);
	UArray_setSize_(u, 9);

	// I == vector(I11, I12, I13, _, I12, I22, I23, _, I13, I23, I33, _)


	for(i = 0, j = 0; i < 12; i++)
	{
		if ((i + 1) % 4)
		{
			UArray_at_putDouble_(u, j++, DATA(self)->I[i]);
		}
	}

	return IoSeq_newWithUArray_copy_(IOSTATE, u, 1);
}

IoObject *IoODEMass_parameters(IoODEMass *self, IoObject *locals, IoMessage *m)
{
	// vector(theMass, cgx, cgy, cgz, I11, I22, I33, I12, I13, I23)

	UArray *u = UArray_new();
	int i, j = 0;

	UArray_setItemType_(u, CTYPE_float32_t);
	UArray_setSize_(u, 10);

	UArray_at_putDouble_(u, j++, DATA(self)->mass);

	for(i=0; i < 3; i++)
	{
		UArray_at_putDouble_(u, j++, DATA(self)->c[i]);
	}

	//              0    1    2   3   4    5    6   7   8    9   10  11
	// I == vector(I11, I12, I13, _, I12, I22, I23, _, I13, I23, I33, _)
	UArray_at_putDouble_(u, j++, DATA(self)->I[0 ]); // I11
	UArray_at_putDouble_(u, j++, DATA(self)->I[5 ]); // I22
	UArray_at_putDouble_(u, j++, DATA(self)->I[10]); // I33
	UArray_at_putDouble_(u, j++, DATA(self)->I[1 ]); // I12
	UArray_at_putDouble_(u, j++, DATA(self)->I[2 ]); // I13
	UArray_at_putDouble_(u, j++, DATA(self)->I[6 ]); // I23

	return IoSeq_newWithUArray_copy_(IOSTATE, u, 1);
}

IoObject *IoODEMass_setParameters(IoODEMass *self, IoObject *locals, IoMessage *m)
{
	const double theMass = IoMessage_locals_doubleArgAt_(m, locals, 0);

	const double cgx     = IoMessage_locals_doubleArgAt_(m, locals, 1);
	const double cgy     = IoMessage_locals_doubleArgAt_(m, locals, 2);
	const double cgz     = IoMessage_locals_doubleArgAt_(m, locals, 3);

	const double I11     = IoMessage_locals_doubleArgAt_(m, locals, 4);
	const double I22     = IoMessage_locals_doubleArgAt_(m, locals, 5);
	const double I33     = IoMessage_locals_doubleArgAt_(m, locals, 6);
	const double I12     = IoMessage_locals_doubleArgAt_(m, locals, 7);
	const double I13     = IoMessage_locals_doubleArgAt_(m, locals, 8);
	const double I23     = IoMessage_locals_doubleArgAt_(m, locals, 9);

	dMassSetParameters(DATA(self), theMass, cgx, cgy, cgz, I11, I22, I33, I12, I13, I23);
	return self;
}

IoObject *IoODEMass_setSphereDensity(IoODEMass *self, IoObject *locals, IoMessage *m)
{
	const double density = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double radius = IoMessage_locals_doubleArgAt_(m, locals, 1);

	dMassSetSphere(DATA(self), density, radius);
	return self;
}

IoObject *IoODEMass_setSphereMass(IoODEMass *self, IoObject *locals, IoMessage *m)
{
	const double totalMass = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double radius = IoMessage_locals_doubleArgAt_(m, locals, 1);

	dMassSetSphereTotal(DATA(self), totalMass, radius);
	return self;
}


IoObject *IoODEMass_setCappedCylinderDensity(IoODEMass *self, IoObject *locals, IoMessage *m)
{
	const double density   = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double direction = IoMessage_locals_doubleArgAt_(m, locals, 1);
	const double radius    = IoMessage_locals_doubleArgAt_(m, locals, 2);
	const double length    = IoMessage_locals_doubleArgAt_(m, locals, 3);

	dMassSetCappedCylinder(DATA(self), density, direction, radius, length);
	return self;
}

IoObject *IoODEMass_setCappedCylinderMass(IoODEMass *self, IoObject *locals, IoMessage *m)
{
	const double totalMass = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double direction = IoMessage_locals_doubleArgAt_(m, locals, 1);
	const double radius    = IoMessage_locals_doubleArgAt_(m, locals, 2);
	const double length    = IoMessage_locals_doubleArgAt_(m, locals, 3);

	dMassSetCappedCylinderTotal(DATA(self), totalMass, direction, radius, length);
	return self;
}

IoObject *IoODEMass_setBoxDensity(IoODEMass *self, IoObject *locals, IoMessage *m)
{
	const double density = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double lx      = IoMessage_locals_doubleArgAt_(m, locals, 1);
	const double ly      = IoMessage_locals_doubleArgAt_(m, locals, 2);
	const double lz      = IoMessage_locals_doubleArgAt_(m, locals, 3);

	dMassSetBox(DATA(self), density, lx, ly, lz);
	return self;
}

IoObject *IoODEMass_setBoxMass(IoODEMass *self, IoObject *locals, IoMessage *m)
{
	const double totalMass = IoMessage_locals_doubleArgAt_(m, locals, 0);
	const double lx        = IoMessage_locals_doubleArgAt_(m, locals, 1);
	const double ly        = IoMessage_locals_doubleArgAt_(m, locals, 2);
	const double lz        = IoMessage_locals_doubleArgAt_(m, locals, 3);

	dMassSetBoxTotal(DATA(self), totalMass, lx, ly, lz);
	return self;
}
