#include "tools.h"

IoSeq *IoSeq_newWithX_y_(void *state, double x, double y)
{
	double doubles[2] = {x, y};
	return IoSeq_newWithDoubles_count_(state, doubles, 2);
}

IoSeq *IoSeq_newWithDoubles_count_(void *state, double *doubles, int count)
{
	UArray *array = UArray_newWithData_type_size_copy_(doubles, CTYPE_float64_t, count, 1);
	return IoSeq_newWithUArray_copy_(state, array, 0);
}


const char *IoMessage_locals_UTF8ArgAt_(IoMessage *m, IoObject *locals, int index)
{
	UArray *array = IoSeq_rawUArray(IoMessage_locals_symbolArgAt_(m, locals, index));
	return (const char *)UArray_bytes(UArray_asUTF8(array));
}


void checkStatus_(void *state, IoMessage *m, cairo_status_t status)
{
	if (status != CAIRO_STATUS_SUCCESS)
		IoState_error_(state, m, cairo_status_to_string(status));
}
