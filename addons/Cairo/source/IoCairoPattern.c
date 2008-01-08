/*#io
CairoPattern ioDoc(
	docCopyright("Trevor Fancher", 2007)
	docCopyright("Daniel Rosengren", 2007)
	docLicense("BSD revised")
	docCategory("Graphics")
*/

#include "IoCairoPattern.h"
#include "IoCairoPattern_inline.h"
#include "IoCairoMatrix.h"
#include "tools.h"

#include "IoCairoSolidPattern.h"
#include "IoCairoLinearGradient.h"
#include "IoCairoRadialGradient.h"
#include "IoCairoSurfacePattern.h"

#define DATA(self) ((IoCairoPatternData *)IoObject_dataPointer(self))


IoTag *IoCairoPattern_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("CairoPattern");
	IoTag_state_(tag, state);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCairoPattern_rawClone);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCairoPattern_free);
	return tag;
}

IoCairoPattern *IoCairoPattern_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCairoPattern_newTag(state));

	IoState_registerProtoWithFunc_(state, self, IoCairoPattern_proto);

	IoCairoPattern_addMethods(self);
	return self;
}

void IoCairoPattern_addMethods(IoCairoPattern *self)
{
	IoMethodTable methodTable[] = {
		{"setMatrix", IoCairoPattern_setMatrix},
		{"getMatrix", IoCairoPattern_getMatrix},
		{NULL, NULL},
	};
	IoObject_addMethodTable_(self, methodTable);
}

IoCairoPattern *IoCairoPattern_rawClone(IoCairoPattern *proto)
{
	IoObject *self = IoObject_rawClonePrimitive(proto);
	if (PATTERN(proto))
		IoObject_setDataPointer_(self, cairo_pattern_reference(PATTERN(proto)));
	return self;
}

IoCairoPattern *IoCairoPattern_newWithRawPattern_(void *state, IoMessage *m, cairo_pattern_t *pattern)
{
	IoObject *self = 0;
	IoStateProtoFunc *initFunc = 0;

	checkStatus_(state, m, cairo_pattern_status(pattern));

	switch(cairo_pattern_get_type(pattern))
	{
		case CAIRO_PATTERN_TYPE_SOLID:
			initFunc = IoCairoSolidPattern_proto;
			break;
		case CAIRO_PATTERN_TYPE_LINEAR:
			initFunc = IoCairoLinearGradient_proto;
			break;
		case CAIRO_PATTERN_TYPE_RADIAL:
			initFunc = IoCairoRadialGradient_proto;
			break;
		case CAIRO_PATTERN_TYPE_SURFACE:
			initFunc = IoCairoSurfacePattern_proto;
			break;
		default:
			IoState_error_(state, 0, "Unsupported pattern type");
	}

	self = IOCLONE(IoState_protoWithInitFunction_(state, initFunc));
	IoObject_setDataPointer_(self, pattern);
	return self;
}

void IoCairoPattern_free(IoCairoPattern *self)
{
	if (PATTERN(self))
		cairo_pattern_destroy(PATTERN(self));
}

cairo_pattern_t *IoCairoPattern_rawPattern(IoCairoPattern *self)
{
	return PATTERN(self);
}


/* ------------------------------------------------------------------------------------------------*/

IoObject *IoCairoPattern_setMatrix(IoCairoPattern *self, IoObject *locals, IoMessage *m)
{
	IoCairoMatrix *matrix = IoMessage_locals_cairoMatrixArgAt_(m, locals, 0);
	cairo_pattern_set_matrix(PATTERN(self), IoCairoMatrix_rawMatrix(matrix));
	CHECK_STATUS(self);
	return self;
}

IoObject *IoCairoPattern_getMatrix(IoCairoPattern *self, IoObject *locals, IoMessage *m)
{
	cairo_matrix_t matrix;
	cairo_pattern_get_matrix(PATTERN(self), &matrix);
	return IoCairoMatrix_newWithRawMatrix_(IOSTATE, &matrix);
}
