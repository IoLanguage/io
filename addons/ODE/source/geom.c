// Copyright (c) 2006, Jonathan Wright

#include "geom.h"
#include "IoODEBox.h"
#include "IoODEPlane.h"
#include "IoODEContact.h"
#include "IoList.h"

#define DATA(self) ((IoODEGeomData *)IoObject_dataPointer(self))

int ISODEGEOM(IoObject *self)
{
	return ISODEBOX(self) || ISODEPLANE(self);
}

IoObject *IoMessage_locals_odeGeomArgAt_(IoMessage *self, void *locals, int n)
{
	IoObject *g = IoMessage_locals_valueArgAt_(self, locals, n);

	if (!ISODEGEOM(g) && !ISNIL(g))
	{
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "ODEGeom");
	}

	return g;
}

dGeomID IoMessage_locals_odeGeomIdArgAt_(IoMessage *self, void *locals, int n)
{
	IoObject *geom = IoMessage_locals_odeGeomArgAt_(self, locals, n);

	if (ISNIL(geom))
	{
		return 0;
	}
	else
	{
		return DATA(geom)->geomId;
	}
}

IoObject *IoODEGeom_geomFromId(void *state, dGeomID id)
{
	if (id == 0)
	{
		return ((IoState*)state)->ioNil;
	}
	else
	{
		return (IoObject*)dGeomGetData(id);
	}
}

IoObject *IoODEGeom_collide(IoObject *self, IoObject *locals, IoMessage *m)
{
	dGeomID g1 = DATA(self)->geomId;
	dGeomID g2 = IoMessage_locals_odeGeomIdArgAt_(m, locals, 0);

	int max = IoMessage_argCount(m) > 1 ? IoMessage_locals_doubleArgAt_(m, locals, 1) : 1;
	dContactGeom* contacts = calloc(max, sizeof(*contacts));

	int count = dCollide(g1, g2, max, contacts, sizeof(*contacts));

	IoList *result = IoList_new(IOSTATE);
	int i;

	for(i=0; i < count; i++)
	{
		IoODEContact *contact = IoODEContact_newContactGeom(IOSTATE, &(contacts[i]));
		IoList_rawAppend_(result, contact);
	}

	free(contacts);
	return result;
}
