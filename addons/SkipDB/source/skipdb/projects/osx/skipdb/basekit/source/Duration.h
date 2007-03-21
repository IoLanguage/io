/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#ifndef DURATION_DEFINED
#define DURATION_DEFINED 1

#include "Common.h"
#include "ByteArray.h"
#include "PortableGettimeofday.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
    double seconds;
} Duration;

BASEKIT_API Duration *Duration_new(void);
BASEKIT_API Duration *Duration_newWithSeconds_(double s);
BASEKIT_API Duration *Duration_clone(Duration *self);
BASEKIT_API void Duration_copy_(Duration *self, Duration *other);

BASEKIT_API void Duration_free(Duration *self);
BASEKIT_API int Duration_compare(Duration *self, Duration *other);

// components 

BASEKIT_API int Duration_years(Duration *self);
BASEKIT_API void Duration_setYears_(Duration *self, double y);

BASEKIT_API int Duration_days(Duration *self);
BASEKIT_API void Duration_setDays_(Duration *self, double d);

BASEKIT_API int Duration_hours(Duration *self);
BASEKIT_API void Duration_setHours_(Duration *self, double m);

BASEKIT_API int Duration_minutes(Duration *self);
BASEKIT_API void Duration_setMinutes_(Duration *self, double m);

BASEKIT_API double Duration_seconds(Duration *self);
BASEKIT_API void Duration_setSeconds_(Duration *self, double s);

// total seconds 

BASEKIT_API double Duration_asSeconds(Duration *self);
BASEKIT_API void Duration_fromSeconds_(Duration *self, double s);

// strings

BASEKIT_API ByteArray *Duration_asByteArrayWithFormat_(Duration *self, const char *format);
BASEKIT_API void Duration_print(Duration *self);

// math

BASEKIT_API void Duration_add_(Duration *self, Duration *other);
BASEKIT_API void Duration_subtract_(Duration *self, Duration *other);

#ifdef __cplusplus
}
#endif
#endif
