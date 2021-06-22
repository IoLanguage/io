
// metadoc Duration category Time
// metadoc Duration copyright Steve Dekorte 2002
// metadoc Duration license BSD revised
/*metadoc Duration description
A container for a duration of time.
*/

#include "IoDuration.h"
#include "IoState.h"
#include "IoCFunction.h"
#include "IoObject.h"
#include "IoNumber.h"
#include <time.h>

static const char *protoId = "Duration";

#define DATA(self) ((Duration *)IoObject_dataPointer(self))

// extend message object

IoDuration *IoMessage_locals_durationArgAt_(IoMessage *self, void *locals,
                                            int n) {
    IoObject *v = IoMessage_locals_valueArgAt_(self, (IoObject *)locals, n);
    if (!ISDURATION(v))
        IoMessage_locals_numberArgAt_errorForType_(self, (IoObject *)locals, n,
                                                   "Duration");
    return v;
}

// ---------------------------------------------

typedef struct tm tm;

IoTag *IoDuration_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoDuration_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoDuration_free);
    IoTag_compareFunc_(tag, (IoTagCompareFunc *)IoDuration_compare);
    return tag;
}

IoDuration *IoDuration_proto(void *state) {
    IoMethodTable methodTable[] = {
        {"years", IoDuration_years},
        {"setYears", IoDuration_setYears},
        {"days", IoDuration_days},
        {"setDays", IoDuration_setDays},
        {"hours", IoDuration_hours},
        {"setHours", IoDuration_setHours},
        {"minutes", IoDuration_minutes},
        {"setMinutes", IoDuration_setMinutes},
        {"seconds", IoDuration_seconds},
        {"setSeconds", IoDuration_setSeconds},
        {"totalSeconds", IoDuration_asNumber},

        {"asString", IoDuration_asString},
        {"asNumber", IoDuration_asNumber},

        {"fromNumber", IoDuration_fromNumber},
        /*Tag_addMethod(tag, "fromString", IoDuration_fromString),*/

        {"print", IoDuration_printDuration},
        {"+=", IoDuration_add},
        {"-=", IoDuration_subtract},
        {NULL, NULL},
    };

    IoObject *self = IoObject_new(state);

    IoObject_setDataPointer_(self, Duration_new());
    IoObject_tag_(self, IoDuration_newTag(state));
    IoState_registerProtoWithId_((IoState *)state, self, protoId);

    IoObject_addMethodTable_(self, methodTable);
    return self;
}

IoDuration *IoDuration_rawClone(IoDuration *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, Duration_new());
    Duration_copy_(DATA(self), DATA(proto));
    return self;
}

IoDuration *IoDuration_new(void *state) {
    IoDuration *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

IoDuration *IoDuration_newWithSeconds_(void *state, double s) {
    IoDuration *self = IoDuration_new(state);
    IoDuration_fromSeconds_(self, s);
    return self;
}

int IoDuration_compare(IoDuration *self, IoDuration *other) {
    if (ISDURATION(other)) {
        return Duration_compare(DATA(self), DATA(other));
    }

    return IoObject_defaultCompare(self, other);
}

void IoDuration_free(IoDuration *self) { Duration_free(DATA(self)); }

Duration *IoDuration_duration(IoDuration *self) { return DATA(self); }

IoDuration *IoDuration_fromSeconds_(IoDuration *self, double s) {
    Duration_fromSeconds_(DATA(self), s);
    return self;
}

double IoDuration_asSeconds(IoDuration *self) {
    return Duration_asSeconds(DATA(self));
}

// years --------------------------------------------------------

IO_METHOD(IoDuration, years) {
    /*doc Duration years
    Returns a number containing the year of the receiver.
    */

    return IONUMBER(Duration_years(DATA(self)));
}

IO_METHOD(IoDuration, setYears) {
    /*doc Duration setYears(aNumber)
    Sets the year of the receiver. Returns self.
    */

    Duration_setYears_(DATA(self), IoMessage_locals_doubleArgAt_(m, locals, 0));
    return self;
}

// days --------------------------------------------------------

IO_METHOD(IoDuration, days) {
    /*doc Duration days
    Returns a number containing the day of the month of the receiver.
    */

    return IONUMBER(Duration_days(DATA(self)));
}

IO_METHOD(IoDuration, setDays) {
    /*doc Duration setDays(aNumber)
    Sets the day of the receiver. Returns self.
    */

    Duration_setDays_(DATA(self), IoMessage_locals_doubleArgAt_(m, locals, 0));
    return self;
}

// hours --------------------------------------------------------

IO_METHOD(IoDuration, hours) {
    /*doc Duration hours
    Returns a number containing the hour of the day(0-23) of the receiver.
    */

    return IONUMBER(Duration_hours(DATA(self)));
}

IO_METHOD(IoDuration, setHours) {
    /*doc Duration setHours(aNumber)
    Sets the hour of the receiver. Returns self.
    */

    Duration_setHours_(DATA(self), IoMessage_locals_doubleArgAt_(m, locals, 0));
    return self;
}

// minutes --------------------------------------------------------

IO_METHOD(IoDuration, minutes) {
    /*doc Duration minutes
    Returns a number containing the minute of the hour(0-59) of the receiver.
    */

    return IONUMBER(Duration_minutes(DATA(self)));
}

IO_METHOD(IoDuration, setMinutes) {
    /*doc Duration setMinutes(aNumber)
    Sets the minute of the receiver. Returns self.
    */

    Duration_setMinutes_(DATA(self),
                         IoMessage_locals_doubleArgAt_(m, locals, 0));
    return self;
}

// seconds --------------------------------------------------------

IO_METHOD(IoDuration, seconds) {
    /*doc Duration seconds
    Returns a number containing the seconds of the minute(0-59) of the receiver.
    This number may contain fractions of seconds.
    */

    return IONUMBER(Duration_seconds(DATA(self)));
}

IO_METHOD(IoDuration, setSeconds) {
    /*doc Duration setSeconds(aNumber)
    Sets the second of the receiver. Return self.
    */

    Duration_setSeconds_(DATA(self),
                         IoMessage_locals_doubleArgAt_(m, locals, 0));
    return self;
}

// conversion --------------------------------------------------------

IO_METHOD(IoDuration, asString) {
    /*doc Duration asString(formatString)
    Returns a string representation of the receiver. The formatString argument
    is optional. If present, the returned string will be formatted according to
    ANSI C date formating rules. <p> <pre> %y years without century as two-digit
    decimal number (00-99) %Y year with century as four-digit decimal number

    %d days
    %H hour as two-digit 24-hour clock decimal integer (00-23)
    %M minute as a two-digit decimal integer (00-59)
    %S second as a two-digit decimal integer (00-59)

    The default format is "%Y %d %H:%M:%S".
    </pre>
    */
    UArray *ba;
    char *format = NULL;

    if (IoMessage_argCount(m) == 1) {
        format = CSTRING(IoMessage_locals_symbolArgAt_(m, locals, 0));
    }

    ba = Duration_asUArrayWithFormat_(DATA(self), format);
    return IoState_symbolWithUArray_copy_convertToFixedWidth(IOSTATE, ba, 0);
}

IO_METHOD(IoDuration, printDuration) {
    /*doc Duration print
    Prints the receiver. Returns self.
    */

    Duration_print(DATA(self));
    return self;
}

/*doc Duration totalSeconds
Same as a asNumber.
*/

IO_METHOD(IoDuration, asNumber) {
    /*doc Duration asNumber
    Returns a number representation of the receiver.
    (where 1 is equal to one second)
    */

    return IONUMBER(Duration_asSeconds(DATA(self)));
}

IO_METHOD(IoDuration, fromNumber) {
    /*doc Duration fromNumber(aNumber)
    Sets the receiver to the Duration specified by
    aNumber(same format number as returned by Duration asNumber). Returns self.
    */

    Duration_fromSeconds_(DATA(self),
                          IoMessage_locals_doubleArgAt_(m, locals, 0));
    return self;
}

// math --------------------------------------------------------

IO_METHOD(IoDuration, add) {
    /*doc Duration +=(aDuration)
    Add aDuration to the receiver. Returns self.
    */

    IoDuration *d = IoMessage_locals_durationArgAt_(m, locals, 0);
    Duration_add_(DATA(self), DATA(d));
    return self;
}

IO_METHOD(IoDuration, subtract) {
    /*doc Duration -=(aDuration)
    Subtract aDuration to the receiver. Returns self.
    */

    IoDuration *d = IoMessage_locals_durationArgAt_(m, locals, 0);
    Duration_subtract_(DATA(self), DATA(d));
    return self;
}
