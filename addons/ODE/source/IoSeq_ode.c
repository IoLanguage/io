#include "IoSeq_ode.h"

void IoSeq_odeInit(IoState *state, IoObject *context)
{
}

IoSeq *IoSeq_newWithODEPoint(IoState *state, const dReal *point)
{
	IoSeq *vector = IoSeq_newFloatArrayOfSize_(state, 3);
	UArray *u = IoSeq_rawUArray(vector);
	
	UArray_at_putDouble_(u, 0, point[0]);
	UArray_at_putDouble_(u, 1, point[1]);
	UArray_at_putDouble_(u, 2, point[2]);
	return vector;
}

IoSeq *IoSeq_newWithODEVector4(IoState *state, const dReal *point)
{
	IoSeq *vector = IoSeq_newFloatArrayOfSize_(state, 4);
	UArray *u = IoSeq_rawUArray(vector);
	
	UArray_at_putDouble_(u, 0, point[0]);
	UArray_at_putDouble_(u, 1, point[1]);
	UArray_at_putDouble_(u, 2, point[2]);
	UArray_at_putDouble_(u, 3, point[3]);
	return vector;
}
