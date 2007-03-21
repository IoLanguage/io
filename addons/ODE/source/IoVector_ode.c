#include "IoVector_ode.h"

void IoVector_odeInit(IoState *state, IoObject *context) 
{
}

IoVector *IoVector_newWithODEPoint(IoState *state, const dReal *point)
{
	Vector *vector = Vector_new();
	Vector_setSize_(vector, 3);
	Vector_at_put_(vector, 0, point[0]);
	Vector_at_put_(vector, 1, point[1]);
	Vector_at_put_(vector, 2, point[2]);
	return IoVector_newWithRawVector_(state, vector);
}

IoVector *IoVector_newWithODEVector4(IoState *state, const dReal *point)
{
	Vector *vector = Vector_new();
	Vector_setSize_(vector, 4);
	Vector_at_put_(vector, 0, point[0]);
	Vector_at_put_(vector, 1, point[1]);
	Vector_at_put_(vector, 2, point[2]);
	Vector_at_put_(vector, 3, point[3]);
	return IoVector_newWithRawVector_(state, vector);
}
