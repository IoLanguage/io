/*#io
docCopyright("Steve Dekorte", 2002)
docLicense("BSD revised")
*/

#include "Base.h"

#ifndef DATE_DEFINED
#define DATE_DEFINED 1

#include "Common.h"
#include "Duration.h"
#include "PortableGettimeofday.h"
#include <time.h>
#include "UTinstant.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
    UTinstant *ut;
    struct timezone tz;
} Date;

BASEKIT_API Date *Date_new(void);
BASEKIT_API void Date_copy_(Date *self, Date *other);
BASEKIT_API void Date_free(Date *self);
BASEKIT_API int Date_compare(Date *self, Date *other);

BASEKIT_API void Date_now(Date *self);
BASEKIT_API void Date_setTimevalue_(Date *self, struct timeval tv);
BASEKIT_API void Date_setToLocalTimeZone(Date *self);
BASEKIT_API double Date_Clock(void);

BASEKIT_API void Date_fromLocalTime_(Date *self, struct tm *t);
BASEKIT_API void Date_fromTime_(Date *self, time_t t);
BASEKIT_API time_t Date_asTime(Date *self);

/* --- Zone ----------------------------------------------------------- */
BASEKIT_API void Date_setToLocalTimeZone(Date *self);
struct timezone Date_timeZone(Date *self);
BASEKIT_API void Date_setTimeZone_(Date *self, struct timezone tz);
BASEKIT_API void Date_convertToTimeZone_(Date *self, struct timezone tz);

/* --- Components ----------------------------------------------------- */
BASEKIT_API void Date_setYear_(Date *self, long y);
BASEKIT_API long Date_year(Date *self);

BASEKIT_API void Date_setMonth_(Date *self, int m);
BASEKIT_API int Date_month(Date *self);

BASEKIT_API void Date_setDay_(Date *self, int d);
BASEKIT_API int Date_day(Date *self);

BASEKIT_API void Date_setHour_(Date *self, int h);
BASEKIT_API int Date_hour(Date *self);

BASEKIT_API void Date_setMinute_(Date *self, int m);
BASEKIT_API int Date_minute(Date *self);

BASEKIT_API void Date_setSecond_(Date *self, double s);
BASEKIT_API double Date_second(Date *self);

BASEKIT_API unsigned char Date_isDaylightSavingsTime(Date *self);
BASEKIT_API int Date_isLeapYear(Date *self);

/* --- Seconds -------------------------------------------------------- */
BASEKIT_API double Date_asSeconds(Date *self);
BASEKIT_API void Date_fromSeconds_(Date *self, double s);

BASEKIT_API void Date_addSeconds_(Date *self, double s);
BASEKIT_API double Date_secondsSince_(Date *self, Date *other);

/* --- Format --------------------------------------------------------- */
BASEKIT_API void Date_fromString_format_(Date *self, const char *s, const char *format);

/* --- Durations ------------------------------------------------------ */
BASEKIT_API Duration *Date_newDurationBySubtractingDate_(Date *self, Date *other);
BASEKIT_API void Date_addDuration_(Date *self, Duration *d);
BASEKIT_API void Date_subtractDuration_(Date *self, Duration *d);

/* -------------------------------------------------------------------- */
BASEKIT_API double Date_secondsSinceNow(Date *self);

BASEKIT_API ByteArray *Date_asString(Date *self, const char *format);

#ifdef __cplusplus
}
#endif
#endif
