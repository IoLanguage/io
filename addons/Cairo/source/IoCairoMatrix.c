//metadoc CairoMatrix copyright Daniel Rosengren, 2007
//metadoc CairoMatrix license BSD revised
//metadoc CairoMatrix category Graphics

#include "IoCairoMatrix.h"
#include "tools.h"
#include <stdlib.h>

#define MATRIX(self) ((cairo_matrix_t *)IoObject_dataPointer(self))


void *IoMessage_locals_cairoMatrixArgAt_(IoMessage *self, void *locals, int n)
{
	IoObject *arg = IoMessage_locals_valueArgAt_(self, locals, n);
	if (!ISCAIROMATRIX(arg))
		IoMessage_locals_numberArgAt_errorForType_(self, locals, n, "CairoMatrix");
	return arg;
}


static IoTag *IoCairoMatrix_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoMatrix");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoMatrix_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoMatrix_rawClone);
	return tag;
}

IoCairoMatrix *IoCairoMatrix_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoMatrix_newTag(state));

	IoObject_setDataPointer_(self, malloc(sizeof(cairo_matrix_t)));
	cairo_matrix_init_identity(MATRIX(self));

	IoState_registerProtoWithFunc_(state, self, IoCairoMatrix_proto);

	{
		IoMethodTable methodTable[] = {
			{"identity", IoCairoMatrix_identity},

			{"translate", IoCairoMatrix_translate},
			{"scale", IoCairoMatrix_scale},
			{"rotate", IoCairoMatrix_rotate},
			{"invert", IoCairoMatrix_invert},
			{"*", IoCairoMatrix_multiply},

			{"transformDistance", IoCairoMatrix_transformDistance},
			{"transformPoint", IoCairoMatrix_transformPoint},

			{NULL, NULL},
		};

		IoObject_addMethodTable_(self, methodTable);
	}

	return self;
}

IoCairoMatrix *IoCairoMatrix_rawClone(IoCairoMatrix *proto)
{
	IoCairoMatrix *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(IoObject_dataPointer(proto), sizeof(cairo_matrix_t)));
	return self;
}

IoCairoMatrix *IoCairoMatrix_new(void *state)
{
	return IoState_protoWithInitFunction_(state, IoCairoMatrix_proto);
}

IoCairoMatrix *IoCairoMatrix_newWithRawMatrix_(void *state, cairo_matrix_t *matrix)
{
	IoCairoMatrix *self = IOCLONE(IoState_protoWithInitFunction_(state, IoCairoMatrix_proto));
	memcpy(MATRIX(self), matrix, sizeof(cairo_matrix_t));
	return self;
}

void IoCairoMatrix_free(IoCairoMatrix *self)
{
	if (MATRIX(self))
		free(MATRIX(self));
}

cairo_matrix_t *IoCairoMatrix_rawMatrix(IoCairoMatrix *self)
{
	return MATRIX(self);
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoMatrix_identity(IoCairoMatrix *self, IoObject *locals, IoMessage *m)
{
	cairo_matrix_init_identity(MATRIX(self));
	return self;
}

IoObject *IoCairoMatrix_translate(IoCairoMatrix *self, IoObject *locals, IoMessage *m)
{
	double tx = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double ty = IoMessage_locals_doubleArgAt_(m, locals, 1);

	cairo_matrix_translate(MATRIX(self), tx, ty);
	return self;
}

IoObject *IoCairoMatrix_scale(IoCairoMatrix *self, IoObject *locals, IoMessage *m)
{
	double sx = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double sy = IoMessage_locals_doubleArgAt_(m, locals, 1);

	cairo_matrix_scale(MATRIX(self), sx, sy);
	return self;
}

IoObject *IoCairoMatrix_rotate(IoCairoMatrix *self, IoObject *locals, IoMessage *m)
{
	cairo_matrix_rotate(MATRIX(self), IoMessage_locals_doubleArgAt_(m, locals, 0));
	return self;
}

IoObject *IoCairoMatrix_invert(IoCairoMatrix *self, IoObject *locals, IoMessage *m)
{
	cairo_matrix_invert(MATRIX(self));
	return self;
}

IoObject *IoCairoMatrix_multiply(IoCairoMatrix *self, IoObject *locals, IoMessage *m)
{
	IoCairoMatrix *left = IoMessage_locals_valueArgAt_(m, locals, 0);
	IoCairoMatrix *right = IoMessage_locals_valueArgAt_(m, locals, 1);
	cairo_matrix_t *result = malloc(sizeof(cairo_matrix_t));

	cairo_matrix_multiply(result, IoCairoMatrix_rawMatrix(left), IoCairoMatrix_rawMatrix(right));
	return IoCairoMatrix_newWithRawMatrix_(IOSTATE, result);
}


IoObject *IoCairoMatrix_transformDistance(IoCairoMatrix *self, IoObject *locals, IoMessage *m)
{
	double dx = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double dy = IoMessage_locals_doubleArgAt_(m, locals, 1);

	cairo_matrix_transform_distance(MATRIX(self), &dx, &dy);
	return IoSeq_newWithX_y_(IOSTATE, dx, dy);
}

IoObject *IoCairoMatrix_transformPoint(IoCairoMatrix *self, IoObject *locals, IoMessage *m)
{
	double x = IoMessage_locals_doubleArgAt_(m, locals, 0);
	double y = IoMessage_locals_doubleArgAt_(m, locals, 1);

	cairo_matrix_transform_point(MATRIX(self), &x, &y);
	return IoSeq_newWithX_y_(IOSTATE, x, y);
}
