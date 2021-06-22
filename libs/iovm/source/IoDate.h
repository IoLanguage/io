
// metadoc Date copyright Steve Dekorte 2002
// metadoc Date license BSD revised

#ifndef IODATE_DEFINED
#define IODATE_DEFINED 1

#include "Common.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "Date.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISDATE(self)                                                           \
    IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoDate_rawClone)
#define IODATE(time) IoDate_newWithTime_((IoState *)IOSTATE, (time_t)time)

typedef IoObject IoDate;

IoDate *IoDate_proto(void *state);
IOVM_API IoDate *IoDate_new(void *state);
IOVM_API IoDate *IoDate_newWithTime_(void *state, time_t t);
IOVM_API IoDate *IoDate_newWithLocalTime_(void *state, struct tm *t);
IOVM_API IoDate *IoDate_newWithTimeval_(void *state, struct timeval tv);
IoDate *IoDate_rawClone(IoDate *self);
void IoDate_free(IoDate *self);
int IoDate_compare(IoDate *self, IoDate *other);

// void IoDate_writeToStream_(IoDate *self, BStream *stream);
// void IoDate_readFromStream_(IoDate *self, BStream *stream);

double IoDate_difference_(IoDate *self, IoDate *other);

// -----------------------------------------------------------

IO_METHOD(IoDate, asSerialization);
IO_METHOD(IoDate, fromSerialization);
IO_METHOD(IoDate, now);
IO_METHOD(IoDate, clock);
IO_METHOD(IoDate, copy);
IO_METHOD(IoDate, cpuSecondsToRun);
IO_METHOD(IoDate, year);
IO_METHOD(IoDate, setYear);
IO_METHOD(IoDate, month);
IO_METHOD(IoDate, setMonth);
IO_METHOD(IoDate, day);
IO_METHOD(IoDate, setDay);
IO_METHOD(IoDate, hour);
IO_METHOD(IoDate, setHour);
IO_METHOD(IoDate, minute);
IO_METHOD(IoDate, setMinute);
IO_METHOD(IoDate, second);
IO_METHOD(IoDate, setSecond);
IO_METHOD(IoDate, isDaylightSavingsTime);
IO_METHOD(IoDate, zone);
IO_METHOD(IoDate, isDST);
IO_METHOD(IoDate, setGmtOffset);
IO_METHOD(IoDate, gmtOffset);
IO_METHOD(IoDate, gmtOffsetSeconds);
IO_METHOD(IoDate, convertToUTC);
IO_METHOD(IoDate, convertToZone);
IO_METHOD(IoDate, convertToLocal);
IO_METHOD(IoDate, setToUTC);

IO_METHOD(IoDate, isValidTime);
IO_METHOD(IoDate, secondsSince_);
IO_METHOD(IoDate, secondsSinceNow);
IO_METHOD(IoDate, isPast);
// IO_METHOD(IoDate, dateAfterSeconds_);
IO_METHOD(IoDate, printDate);

// coversions

IO_METHOD(IoDate, asString);
IO_METHOD(IoDate, asNumber);

IO_METHOD(IoDate, fromNumber);
IO_METHOD(IoDate, fromString);

IO_METHOD(IoDate, setFormat);
IO_METHOD(IoDate, format);

// durations

IO_METHOD(IoDate, subtract);
IO_METHOD(IoDate, subtractInPlace);
IO_METHOD(IoDate, add);
IO_METHOD(IoDate, addInPlace);

#ifdef __cplusplus
}
#endif
#endif
