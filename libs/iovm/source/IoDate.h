
//metadoc Date copyright Steve Dekorte 2002
//metadoc Date license BSD revised

#ifndef IODATE_DEFINED
#define IODATE_DEFINED 1

#include "Common.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "Date.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ISDATE(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoDate_rawClone)
#define IODATE(time) IoDate_newWithTime_((IoState*)IOSTATE, (time_t)time)

typedef IoObject IoDate;

IoDate *IoDate_proto(void *state);
IoDate *IoDate_new(void *state);
IoDate *IoDate_newWithTime_(void *state, time_t t);
IoDate *IoDate_newWithLocalTime_(void *state, struct tm *t);
IoDate *IoDate_newWithTimeval_(void *state, struct timeval tv);
IoDate *IoDate_rawClone(IoDate *self);
void IoDate_free(IoDate *self);
int IoDate_compare(IoDate *self, IoDate *other);

//void IoDate_writeToStream_(IoDate *self, BStream *stream);
//void IoDate_readFromStream_(IoDate *self, BStream *stream);

double IoDate_difference_(IoDate *self, IoDate *other);

// -----------------------------------------------------------

IoObject *IoDate_asSerialization(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_fromSerialization(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_now(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_clock(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_copy(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_cpuSecondsToRun(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_year(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_setYear(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_month(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_setMonth(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_day(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_setDay(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_hour(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_setHour(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_minute(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_setMinute(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_second(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_setSecond(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_isDaylightSavingsTime(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_zone(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_gmtOffset(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_gmtOffsetSeconds(IoDate *self, IoObject *locals, IoMessage *m);

IoObject *IoDate_isValidTime(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_secondsSince_(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_secondsSinceNow(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_isPast(IoDate *self, IoObject *locals, IoMessage *m);
//IoObject *IoDate_dateAfterSeconds_(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_printDate(IoDate *self, IoObject *locals, IoMessage *m);

// coversions

IoObject *IoDate_asString(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_asNumber(IoDate *self, IoObject *locals, IoMessage *m);

IoObject *IoDate_fromNumber(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_fromString(IoDate *self, IoObject *locals, IoMessage *m);

IoObject *IoDate_setFormat(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_format(IoDate *self, IoObject *locals, IoMessage *m);

// durations

IoObject *IoDate_subtract(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_subtractInPlace(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_add(IoDate *self, IoObject *locals, IoMessage *m);
IoObject *IoDate_addInPlace(IoDate *self, IoObject *locals, IoMessage *m);

#ifdef __cplusplus
}
#endif
#endif
