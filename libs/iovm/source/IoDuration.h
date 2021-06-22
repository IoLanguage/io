
// metadoc Duration copyright Steve Dekorte 2002
// metadoc Duration license BSD revised

#ifndef IODURATION_DEFINED
#define IODURATION_DEFINED 1

#include <sys/types.h>
#include "Common.h"
#include "Duration.h"
#include "IoObject.h"

#ifdef __cplusplus
extern "C" {
#endif

// extend message object

#define ISDURATION(self)                                                       \
    IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoDuration_rawClone)

typedef IoObject IoDuration;

IoDuration *IoMessage_locals_durationArgAt_(IoMessage *self, void *locals,
                                            int n);

IoDuration *IoDuration_proto(void *state);
IoDuration *IoDuration_rawClone(IoDuration *self);
IoDuration *IoDuration_new(void *state);
IoDuration *IoDuration_newWithSeconds_(void *state, double t);
void IoDuration_free(IoDuration *self);
int IoDuration_compare(IoDuration *self, IoDuration *other);
Duration *IoDuration_duration(IoDuration *self);

IoDuration *IoDuration_fromSeconds_(IoDuration *self, double s);
double IoDuration_asSeconds(IoDuration *self);

// getters/setters

IO_METHOD(IoDuration, years);
IO_METHOD(IoDuration, setYears);

IO_METHOD(IoDuration, days);
IO_METHOD(IoDuration, setDays);

IO_METHOD(IoDuration, hours);
IO_METHOD(IoDuration, setHours);

IO_METHOD(IoDuration, minutes);
IO_METHOD(IoDuration, setMinutes);

IO_METHOD(IoDuration, seconds);
IO_METHOD(IoDuration, setSeconds);
IO_METHOD(IoDuration, totalSeconds);

IO_METHOD(IoDuration, setFromTo);
IO_METHOD(IoDuration, printDuration);

// conversion

IO_METHOD(IoDuration, asString);
IO_METHOD(IoDuration, asNumber);

IO_METHOD(IoDuration, fromNumber);
IO_METHOD(IoDuration, fromString);

// math

IO_METHOD(IoDuration, add);
IO_METHOD(IoDuration, subtract);

#ifdef __cplusplus
}
#endif
#endif
