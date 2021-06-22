
// metadoc Date category Core
// metadoc Date copyright Steve Dekorte 2002
// metadoc Date license BSD revised
/*metadoc Date description A container for a date and time information.*/
// metadoc Date credits fromString method by Sean Perry

#include "IoDate.h"
#include "IoState.h"
#include "IoCFunction.h"
#include "IoObject.h"
#include "IoSeq.h"
#include "IoNumber.h"
#include "IoDuration.h"
#include "PortableStrptime.h"
#include <string.h>
#include <time.h>

static const char *protoId = "Date";

#define DATA(self) ((Date *)IoObject_dataPointer(self))

IoTag *IoDate_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoDate_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoDate_free);
    IoTag_compareFunc_(tag, (IoTagCompareFunc *)IoDate_compare);
    return tag;
}

IoDate *IoDate_proto(void *state) {
    IoMethodTable methodTable[] = {
        {"asSerialization", IoDate_asSerialization},
        {"fromSerialization", IoDate_fromSerialization},
        {"now", IoDate_now},
        {"clock", IoDate_clock},
        {"copy", IoDate_copy},
        {"cpuSecondsToRun", IoDate_cpuSecondsToRun},
        {"year", IoDate_year},
        {"setYear", IoDate_setYear},
        {"month", IoDate_month},
        {"setMonth", IoDate_setMonth},
        {"day", IoDate_day},
        {"setDay", IoDate_setDay},
        {"hour", IoDate_hour},
        {"setHour", IoDate_setHour},
        {"minute", IoDate_minute},
        {"setMinute", IoDate_setMinute},
        {"second", IoDate_second},
        {"setSecond", IoDate_setSecond},
        {"isDaylightSavingsTime", IoDate_isDaylightSavingsTime},
        {"zone", IoDate_zone},
        {"isDST", IoDate_isDST},
        {"setGmtOffset", IoDate_setGmtOffset},
        {"gmtOffset", IoDate_gmtOffset},
        {"gmtOffsetSeconds", IoDate_gmtOffsetSeconds},
        {"convertToUTC", IoDate_convertToUTC},
        {"convertToZone", IoDate_convertToZone},
        {"convertToLocal", IoDate_convertToLocal},
        {"setToUTC", IoDate_setToUTC},
        {"isValidTime", IoDate_isValidTime},
        {"secondsSince", IoDate_secondsSince_},
        {"secondsSinceNow", IoDate_secondsSinceNow},
        {"isPast", IoDate_isPast},
        //{"dateAfterSeconds", IoDate_dateAfterSeconds_},
        {"asString", IoDate_asString},
        {"asNumber", IoDate_asNumber},
        {"fromNumber", IoDate_fromNumber},
        {"fromString", IoDate_fromString},
        {"print", IoDate_printDate},
        {"+", IoDate_add},
        {"-", IoDate_subtract},
        {"+=", IoDate_addInPlace},
        {"-=", IoDate_subtractInPlace},
        {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);

    IoObject_tag_(self, IoDate_newTag(state));
    IoObject_setDataPointer_(self, Date_new());

    /*doc Date format
    Returns the format string for the receiver. The default is "%Y-%m-%d
    %H:%M:%S %Z".
    */

    IoObject_setSlot_to_(self, IOSYMBOL("format"),
                         IOSYMBOL("%Y-%m-%d %H:%M:%S %Z"));
    IoState_registerProtoWithId_((IoState *)state, self, protoId);

    IoObject_addMethodTable_(self, methodTable);
    return self;
}

IoDate *IoDate_rawClone(IoDate *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, Date_new());
    Date_copy_(DATA(self), DATA(proto));
    return self;
}

IOVM_API IoDate *IoDate_new(void *state) {
    IoDate *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

IOVM_API IoDate *IoDate_newWithTime_(void *state, time_t t) {
    IoDate *self = IoDate_new(state);
    Date_fromTime_(DATA(self), t);
    return self;
}

IOVM_API IoDate *IoDate_newWithTimeval_(void *state, struct timeval tv) {
    IoDate *self = IoDate_new(state);
    DATA(self)->tv = tv;
    return self;
}

IOVM_API IoDate *IoDate_newWithLocalTime_(void *state, struct tm *t) {
    IoDate *self = IoDate_new(state);
    Date_fromLocalTime_(DATA(self), t);
    return self;
}

void IoDate_free(IoDate *self) { Date_free(DATA(self)); }

int IoDate_compare(IoDate *self, IoDate *date) {
    if (ISDATE(date))
        return Date_compare(DATA(self), DATA(date));
    return IoObject_defaultCompare(self, date);
}

// -----------------------------------------------------------

IoSeq *IoDate_asSerialization(IoDate *self, IoObject *locals, IoMessage *m) {
    /*doc Date asSerialization
    Returns a serialization (sequence) of the date that allows for perfect
    reconstruction of the timestamp.
    */

    return IoSeq_newWithUArray_copy_(
        IOSTATE, (UArray *)Date_asSerialization(DATA(self)), 0);
}

IoDate *IoDate_fromSerialization(IoDate *self, IoObject *locals, IoMessage *m) {
    /*doc Date fromSerialization
    Sets the date based on the serialization sequence.  Return self.
    */

    IoSeq *serializationSeq = IoMessage_locals_seqArgAt_(m, locals, 0);
    UArray *serialization = UArray_clone(IoSeq_rawUArray(serializationSeq));

    UArray_setItemType_(serialization, CTYPE_int32_t);
    if (UArray_size(serialization) != 4) {
        IoState_error_(
            IOSTATE, self,
            "Expected a serialization sequence comprising 4 int32 items.");
    }

    Date_fromSerialization(DATA(self), serialization);

    UArray_free(serialization);

    return self;
}

IO_METHOD(IoDate, now) {
    /*doc Date now
    Sets the receiver to the current time. Returns self.
    */

    Date_now(DATA(self));
    return self;
}

IO_METHOD(IoDate, copy) {
    /*doc Date copy(aDate)
    Sets the receiver to be the same date as aDate. Returns self.
    */

    IoDate *date = IoMessage_locals_dateArgAt_(m, locals, 0);

    Date_copy_(DATA(self), DATA(date));
    return self;
}

IO_METHOD(IoDate, clock) {
    /*doc Date clock
    Returns a number containing the number of seconds
    of processor time since the beginning of the program or -1 if unavailable.
    */

    return IONUMBER(Date_Clock());
}

IO_METHOD(IoDate, cpuSecondsToRun) {
    /*doc Date cpuSecondsToRun(expression)
    Evaluates message and returns a Number whose value
    is the cpu seconds taken to do the evaluation.
    */

    IoMessage_assertArgCount_receiver_(m, 1, self);

    {
        double t2, t1 = clock();
        IoMessage *doMessage = IoMessage_rawArgAt_(m, 0);
        IoMessage_locals_performOn_(doMessage, locals, locals);
        t2 = clock();
        return IONUMBER((t2 - t1) / ((double)CLOCKS_PER_SEC));
    }
}

IO_METHOD(IoDate, year) {
    /*doc Date year
    Returns a number containing the year of the receiver.
    */

    return IONUMBER(Date_year(DATA(self)));
}

IO_METHOD(IoDate, setYear) {
    /*doc Date setYear(aNumber)
    Sets the year of the receiver.
    */

    Date_setYear_(DATA(self), IoMessage_locals_intArgAt_(m, locals, 0));
    return self;
}

IO_METHOD(IoDate, month) {
    /*doc Date month
    Returns a number containing the month(1-12) of the year of the receiver.
    */

    return IONUMBER(Date_month(DATA(self)) + 1);
}

IO_METHOD(IoDate, setMonth) {
    /*doc Date setMonth(aNumber)
    Sets the month(1-12) of the receiver. Returns self.
    */

    int v = IoMessage_locals_intArgAt_(m, locals, 0);
    IOASSERT(v >= 1 && v <= 12, "month must be within range 1-12");
    Date_setMonth_(DATA(self), v - 1);
    return self;
}

IO_METHOD(IoDate, day) {
    /*doc Date day
    Returns a number containing the day of the month of the receiver.
    */

    return IONUMBER(Date_day(DATA(self)));
}

IO_METHOD(IoDate, setDay) {
    /*doc Date setDay(aNumber)
    Sets the day of the receiver. Returns self.
    */

    int v = IoMessage_locals_intArgAt_(m, locals, 0);
    int month = Date_month(DATA(self)) + 1;

    IOASSERT(v >= 1 && v <= 31, "day must be within range 1-31");

    if (month == 2) {
        if (Date_isLeapYear(DATA(self))) {
            IOASSERT(v >= 1 && v <= 29, "day must be within range 1-29");
        } else {
            IOASSERT(v >= 1 && v <= 28, "day must be within range 1-28");
        }
    } else if (month == 11) {
        IOASSERT(v >= 1 && v <= 30, "day must be within range 1-30");
    } else if (month == 12) {
        IOASSERT(v >= 1 && v <= 31, "day must be within range 1-31");
    }

    Date_setDay_(DATA(self), v);
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoDate, hour) {
    /*doc Date hour
    Returns a number containing the hour of the day(0-23) of the receiver.
    */

    return IONUMBER(Date_hour(DATA(self)));
}

IO_METHOD(IoDate, setHour) {
    /*doc Date setHour(aNumber)
    Sets the hour of the receiver. Returns self.
    */

    int v = IoMessage_locals_intArgAt_(m, locals, 0);
    IOASSERT(v >= 0 && v <= 23, "hour must be within range 0-23");
    Date_setHour_(DATA(self), v);
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoDate, minute) {
    /*doc Date minute
    Returns a number containing the minute of the hour(0-59) of the receiver.
    */

    return IONUMBER(Date_minute(DATA(self)));
}

IO_METHOD(IoDate, setMinute) {
    /*doc Date setMinute(aNumber)
    Sets the minute of the receiver. Returns self.
    */

    int v = IoMessage_locals_intArgAt_(m, locals, 0);
    IOASSERT(v >= 0 && v <= 59, "minute must be within range 0-59");
    Date_setMinute_(DATA(self), v);
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoDate, second) {
    /*doc Date second
    Returns a number containing the seconds of the minute(0-59) of the receiver.
    This number may contain fractions of seconds.
    */

    return IONUMBER(Date_second(DATA(self)));
}

IO_METHOD(IoDate, setSecond) {
    /*doc Date setSecond(aNumber)
    Sets the second of the receiver. Returns self.
    */

    int v = IoMessage_locals_intArgAt_(m, locals, 0);
    IOASSERT(v >= 0 && v <= 59, "second must be within range 0-59");
    Date_setSecond_(DATA(self), v);
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoDate, zone) {
    /*doc Date zone
    Returns a string containing the system's time zone code.
    */

    time_t t = time(NULL);
    const struct tm *tp = localtime(&t);
    char s[32];
    strftime(s, 32, "%Z", tp);
    return IOSYMBOL(s);
}

IO_METHOD(IoDate, isDST) {
    /*doc Date isDST
    Returns true if the Date is set to use DST.  Posix only.
    */

    struct timezone tz = Date_timeZone(DATA(self));
#if defined(__CYGWIN__) || defined(_WIN32)
    IoState_error_(IOSTATE, m, "Not implemented on Windows.");
    return IONIL(self);
#else
    return IOBOOL(self, tz.tz_dsttime);
#endif
}

IO_METHOD(IoDate, gmtOffsetSeconds) {
    /*doc Date gmtOffsetSeconds
    Returns the system's seconds east of UTC.
    */

    time_t t = time(NULL);
    const struct tm *tp = localtime(&t);
#if defined(__CYGWIN__) || defined(_WIN32)
    return IONUMBER(_timezone);
#else
    return IONUMBER(tp->tm_gmtoff);
#endif
}

IO_METHOD(IoDate, setGmtOffset) {
    /*doc Date setGmtOffset
    Set the number of minutes west of GMT for this Date's zone
    */

    struct timezone tz = Date_timeZone(DATA(self));
    tz.tz_minuteswest = IoMessage_locals_intArgAt_(m, locals, 0);
    Date_setTimeZone_(DATA(self), tz);
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoDate, gmtOffset) {
    /*doc Date gmtOffset
    Returns the system's timezone string. E.g., +1300 or -0500.
    */

    time_t t = time(NULL);
    const struct tm *tp = localtime(&t);

    char buf[6];
#if defined(__CYGWIN__) || defined(_WIN32)
    int minutes = _timezone / 60;
#else
    int minutes = (int)(tp->tm_gmtoff / 60);
#endif
    snprintf(buf, sizeof(buf), "%+03d%02d", minutes / 60, minutes % 60);

    return IOSYMBOL(buf);
}

IO_METHOD(IoDate, convertToUTC) {
    /*doc Date convertToUTC
    Converts self from a local date to the equivalent UTC date
    */

    struct timezone tz;
    tz.tz_minuteswest = 0;
    tz.tz_dsttime = 0;
    Date_convertToTimeZone_(DATA(self), tz);
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoDate, convertToZone) {
    /*doc Date convertToZone(offset, isDST)
    Converts self to an equivalent data in a zone with offset (minutes west) and
    DST (true, false).
    */

    struct timezone tz;

    int mw = IoMessage_locals_intArgAt_(m, locals, 0);
    int dst = IoMessage_locals_boolArgAt_(m, locals, 1);

    tz.tz_minuteswest = mw;
    tz.tz_dsttime = dst;
    Date_convertToTimeZone_(DATA(self), tz);
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoDate, convertToLocal) {
    /*doc Date convertToLocal
    Converts self date from a UTC date to the equivalent local date
    */

    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);

    Date_convertToTimeZone_(DATA(self), tz);
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoDate, setToUTC) {
    /*doc Date asUTC
    Changes the timezone of this date to utc
    */

    struct timezone tz;
    tz.tz_minuteswest = 0;
    tz.tz_dsttime = 0;

    Date_setTimeZone_(DATA(self), tz);
    IoObject_isDirty_(self, 1);

    return self;
}

IO_METHOD(IoDate, isDaylightSavingsTime) {
    /*doc Date isDaylightSavingsTime
    Returns self if Daylight Saving Time is in effect for the receiver,
    otherwise returns Nil.
    */

    return IOBOOL(self, Date_isDaylightSavingsTime(DATA(self)));
}

IO_METHOD(IoDate, isValidTime) {
    /*doc Date isValidTime(hour, min, sec)
    Returns self if the specified time is valid, otherwise returns Nil.
    A negative value will count back; i.e., a value of -5 for the hour,
    will count back 5 hours to return a value of 19. No adjustment is
    done for values above 24.
    */

    int hour = IoMessage_locals_intArgAt_(m, locals, 0);
    int min = IoMessage_locals_intArgAt_(m, locals, 1);
    int sec = IoMessage_locals_intArgAt_(m, locals, 2);

    if (hour < 0)
        hour += 24;
    if (min < 0)
        min += 60;
    if (sec < 0)
        sec += 60;

    return IOBOOL(self, ((hour >= 0) && (hour < 24)) &&
                            ((min >= 0) && (min < 60)) &&
                            ((sec >= 0) && (sec < 60)));
}

IO_METHOD(IoDate, secondsSince_) {
    /*doc Date secondsSince(aDate)
    Returns a number of seconds of between aDate and the receiver.
    */

    IoDate *date = IoMessage_locals_dateArgAt_(m, locals, 0);
    return IONUMBER(Date_secondsSince_(DATA(self), DATA(date)));
}

IO_METHOD(IoDate, secondsSinceNow) {
    /*doc Date secondsSinceNow(aDate)
    Returns the number of seconds since aDate.
    */

    return IONUMBER(Date_secondsSinceNow(DATA(self)));
}

IO_METHOD(IoDate, isPast) {
    /*doc Date isPast
    Returns true if the receiver is a date in the past.
    */

    return IOBOOL(self, Date_secondsSinceNow(DATA(self)) > 0);
}

/*
IO_METHOD(IoDate, dateAfterSeconds_)
{
        // doc Date dateAfterSeconds(secondsNumber)
        Returns a new date that is secondsNumber seconds after the receiver.


        IoDate *newDate = IoDate_new(IOSTATE);
        Date_addSeconds_(DATA(newDate), IoMessage_locals_doubleArgAt_(m, locals,
0)); return newDate;
}
*/

IO_METHOD(IoDate, asString) {
    /*doc Date asString(optionalFormatString)
    Returns a string representation of the receiver using the
receivers format. If the optionalFormatString argument is present, the
receiver's format is set to it first. Formatting is according to ANSI C
date formatting rules.
<p>
<pre>
%a abbreviated weekday name (Sun, Mon, etc.)
%A full weekday name (Sunday, Monday, etc.)
%b abbreviated month name (Jan, Feb, etc.)
%B full month name (January, February, etc.)
%c full date and time string
%d day of the month as two-digit decimal integer (01-31)
%H hour as two-digit 24-hour clock decimal integer (00-23)
%I hour as two-digit 12-hour clock decimal integer (01-12)
%m month as a two-digit decimal integer (01-12)
%M minute as a two-digit decimal integer (00-59)
%p either "AM" or "PM"
%S second as a two-digit decimal integer (00-59)
%U number of week in the year as two-digit decimal integer (00-52)
with Sunday considered as first day of the week
%w weekday as one-digit decimal integer (0-6) with Sunday as 0
%W number of week in the year as two-digit decimal integer (00-52)
with Monday considered as first day of the week
%x full date string (no time); in the C locale, this is equivalent
to "%m/%d/%y".
%y year without century as two-digit decimal number (00-99)
%Y year with century as four-digit decimal number
%Z time zone name (e.g. EST);
null string if no time zone can be obtained
%% stands for '%' character in output string.
</pre>
*/

    char *format = "%Y-%m-%d %H:%M:%S %Z";

    if (IoMessage_argCount(m) == 1) {
        format = CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0));
    } else {
        IoObject *f = IoObject_getSlot_(self, IOSYMBOL("format"));
        if (ISSEQ(f)) {
            format = CSTRING(f);
        }
    }

    {
        UArray *ba = Date_asString(DATA(self), format);
        return IoState_symbolWithUArray_copy_convertToFixedWidth(IOSTATE, ba,
                                                                 0);
    }
}

IO_METHOD(IoDate, printDate) {
    /*doc Date print
    Prints the receiver. Returns self.
    */

    IoSymbol *s = (IoSymbol *)IoDate_asString(self, locals, m);
    IoSeq_print(s, locals, m);
    return self;
}

IO_METHOD(IoDate, asNumber) {
    /*doc Date asNumber
    Returns the date as seconds since 1970 UTC.
    */

    return IONUMBER(Date_asSeconds(DATA(self)));
}

IO_METHOD(IoDate, fromNumber) {
    /*doc Date fromNumber(aNumber)
    Sets the receiver to be aNumber seconds since 1970.
    */

    Date_fromSeconds_(DATA(self), IoMessage_locals_doubleArgAt_(m, locals, 0));
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoDate, fromString) {
    /*doc Date fromString(aString, formatString)
    Sets the receiver to the date specified by aString as parsed according to
    the given formatString. See the Date asString method for formatting rules.
    Returns self.
    */

    IoMessage_assertArgCount_receiver_(m, 2, self);
    {
        IoSymbol *date_input = IoMessage_locals_seqArgAt_(m, locals, 0);
        IoSymbol *format = IoMessage_locals_seqArgAt_(m, locals, 1);
        Date_fromString_format_(DATA(self), CSTRING(date_input),
                                CSTRING(format));
    }
    IoObject_isDirty_(self, 1);
    return self;
}

/* --- Durations -------------------------------------------------------- */

IO_METHOD(IoDate, subtract) {
    /*doc Date -(aDurationOrDate)
    Return a new Date with the receiver's value minus an amount of time
    specified by aDuration to the receiver. Returns self.
    */

    IoObject *v = IoMessage_locals_valueArgAt_(m, locals, 0);

    if (ISDATE(v)) {
        double d = Date_secondsSince_(DATA(self), DATA(v));
        return IoDuration_newWithSeconds_(IOSTATE, d);
    } else if (ISDURATION(v)) {
        IoDate *newDate = IOCLONE(self);
        Date_subtractDuration_(DATA(newDate), IoDuration_duration(v));
        return newDate;
    }

    IOASSERT(1, "Date or Duration argument required");

    return IONIL(self);
}

IO_METHOD(IoDate, subtractInPlace) {
    /*doc Date -=(aDuration)
    Subtract aDuration from the receiver. Returns self.
    */

    IoDuration *d = IoMessage_locals_durationArgAt_(m, locals, 0);
    Date_subtractDuration_(DATA(self), IoDuration_duration(d));
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoDate, addInPlace) {
    /*doc Date +=(aDuration)
    Add aDuration to the receiver. Returns self.
    */

    IoDuration *d = IoMessage_locals_durationArgAt_(m, locals, 0);
    Date_addDuration_(DATA(self), IoDuration_duration(d));
    IoObject_isDirty_(self, 1);
    return self;
}

IO_METHOD(IoDate, add) {
    /*doc Date +(aDuration)
    Return a new Date with the receiver's value plus an amount
    of time specified by aDuration object to the receiver.
    */

    IoDate *newDate = IOCLONE(self);
    return IoDate_addInPlace(newDate, locals, m);
}
