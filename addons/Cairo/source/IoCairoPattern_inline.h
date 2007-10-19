#ifndef IOCAIROPATTERN_INLINE
#define IOCAIROPATTERN_INLINE 1

#define PATTERN(self) ((cairo_pattern_t *)IoObject_dataPointer(self))
#define CHECK_STATUS(self) checkStatus_(IOSTATE, m, cairo_pattern_status(PATTERN(self)))

#endif
