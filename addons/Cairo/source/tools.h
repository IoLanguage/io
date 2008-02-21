#ifndef TOOLS_DEFINED
#define TOOLS_DEFINED 1

#include "IoState.h"
#include "IoSeq.h"
#include <cairo.h>

IoSeq *IoSeq_newWithX_y_(void *state, double x, double y);
IoSeq *IoSeq_newWithDoubles_count_(void *state, double *doubles, int count);

const char *IoMessage_locals_UTF8ArgAt_(IoMessage *m, IoObject *locals, int index);

void checkStatus_(void *state, IoMessage *m, cairo_status_t status);

#endif
