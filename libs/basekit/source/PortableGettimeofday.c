#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include "PortableGettimeofday.h"

/* just to make compiler happy */
void PortableGettimeOfday(void) {}

double secondsSince1970(void) {
    double result;
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    result = tv.tv_sec;
    result += tv.tv_usec / 1000000.0;
    return result;
}
