#ifndef IOCAIROPATTERN_INLINE
#define IOCAIROPATTERN_INLINE 1

#define PATTERN(self) ((cairo_pattern_t *)IoObject_dataPointer(self))

#define STATUS(cairo_type) (cairo_status_to_string(cairo_pattern_status(cairo_type)))
#define CHECK_STATUS(obj) \
				cairo_status_t status = cairo_pattern_status(PATTERN(obj)); \
				if (status != CAIRO_STATUS_SUCCESS) { \
					IoState_error_(IOSTATE, m, "%s: cairo: %s", __func__, STATUS(PATTERN(obj))); \
				}

#endif
