#ifdef IORANGE_C 
#define IO_IN_C_FILE
#endif
#include "Common_inline.h"
#ifdef IO_DECLARE_INLINES

#define RANGEDATA(self) ((IoRangeData *)IoObject_dataPointer(self))

IOINLINE IoObject *IoRange_getFirst(IoRange *self)
{
    return RANGEDATA(self)->start;
}

IOINLINE void IoRange_setFirst(IoRange *self, IoObject *v)
{
    RANGEDATA(self)->start = IOREF(v);
}

IOINLINE IoObject *IoRange_getLast(IoRange *self)
{
    return RANGEDATA(self)->end;
}

IOINLINE void IoRange_setLast(IoRange *self, IoObject *v)
{
    RANGEDATA(self)->end = IOREF(v);
}

IOINLINE IoObject *IoRange_getCurrent(IoRange *self)
{
    return RANGEDATA(self)->curr;
}

IOINLINE void IoRange_setCurrent(IoRange *self, IoObject *v)
{
    RANGEDATA(self)->curr = IOREF(v);
}

IOINLINE IoObject *IoRange_getIncrement(IoRange *self)
{
    return RANGEDATA(self)->increment;
}

IOINLINE void IoRange_setIncrement(IoRange *self, IoObject *v)
{
    RANGEDATA(self)->increment = IOREF(v);
}

IOINLINE IoObject *IoRange_getIndex(IoRange *self)
{
    return RANGEDATA(self)->index;
}

IOINLINE void IoRange_setIndex(IoRange *self, IoObject *v)
{
    RANGEDATA(self)->index = IOREF(v);
}

#undef IO_IN_C_FILE
#endif
