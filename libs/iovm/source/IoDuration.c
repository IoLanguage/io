
// metadoc Duration category Time
// metadoc Duration copyright Steve Dekorte 2002
// metadoc Duration license BSD revised
/*metadoc Duration description
A container for a duration of time.
*/

/*cmetadoc Duration description
Thin IoObject wrapper around the plain-C Duration struct (years,
days, hours, minutes, seconds). All arithmetic and formatting live
in Duration.c; this file only bridges Io message sends to the C API,
manages lifecycle (tag / clone / free / compare), and registers the
proto. Durations interoperate with IoDate for +/- arithmetic — see
IoDate_add / IoDate_subtract, which read the C Duration via
IoDuration_duration — and with IoNumber through asNumber /
fromNumber, treating seconds as the canonical scalar form.
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

/*cdoc Duration IoMessage_locals_durationArgAt_(self, locals, n)
Argument coercion helper used from other files (notably IoDate.c)
to fetch the n-th argument as an IoDuration. Raises a type error
via IoMessage_locals_numberArgAt_errorForType_ when the value is
not a Duration, mirroring the pattern used by numberArgAt_ and
seqArgAt_ elsewhere in the VM.
*/
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

/*cdoc Duration IoDuration_newTag(state)
Builds the Duration tag and wires in clone / free / compare function
pointers. No mark func is needed since Duration's C payload holds
no IoObject references — it is a pure numeric struct.
*/
IoTag *IoDuration_newTag(void *state) {
    IoTag *tag = IoTag_newWithName_(protoId);
    IoTag_state_(tag, state);
    IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoDuration_rawClone);
    IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoDuration_free);
    IoTag_compareFunc_(tag, (IoTagCompareFunc *)IoDuration_compare);
    return tag;
}

/*cdoc Duration IoDuration_proto(state)
Creates the Duration proto with its full method table (field accessors
for years/days/hours/minutes/seconds, asString/asNumber conversion,
and +=/-= operators). Allocates a zeroed C Duration struct and
registers the proto under "Duration" on the VM state so IoDuration_new
can clone it later.
*/
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

/*cdoc Duration IoDuration_rawClone(proto)
Tag cloneFunc. Allocates a fresh C Duration struct on the clone
(never shared with the proto) and copies its fields via
Duration_copy_, so each IoDuration owns its own numeric state.
*/
IoDuration *IoDuration_rawClone(IoDuration *proto) {
    IoObject *self = IoObject_rawClonePrimitive(proto);
    IoObject_setDataPointer_(self, Duration_new());
    Duration_copy_(DATA(self), DATA(proto));
    return self;
}

/*cdoc Duration IoDuration_new(state)
Clones the registered proto. Preferred C-level constructor when a
blank Duration is needed before any field is set.
*/
IoDuration *IoDuration_new(void *state) {
    IoDuration *proto = IoState_protoWithId_((IoState *)state, protoId);
    return IOCLONE(proto);
}

/*cdoc Duration IoDuration_newWithSeconds_(state, s)
Factory for a Duration initialized from a double seconds value.
Used by IoDate_subtract when a Date-minus-Date difference is
returned as a Duration, and anywhere else the VM needs to surface
an elapsed time to Io code.
*/
IoDuration *IoDuration_newWithSeconds_(void *state, double s) {
    IoDuration *self = IoDuration_new(state);
    IoDuration_fromSeconds_(self, s);
    return self;
}

/*cdoc Duration IoDuration_compare(self, other)
Tag compareFunc. Delegates to Duration_compare when both sides are
Durations; otherwise falls back to IoObject_defaultCompare so that
cross-type comparisons still produce a stable ordering rather than
an error.
*/
int IoDuration_compare(IoDuration *self, IoDuration *other) {
    if (ISDURATION(other)) {
        return Duration_compare(DATA(self), DATA(other));
    }

    return IoObject_defaultCompare(self, other);
}

/*cdoc Duration IoDuration_free(self)
Tag freeFunc. Releases the heap-allocated C Duration struct; the
IoObject header itself is freed by the collector.
*/
void IoDuration_free(IoDuration *self) { Duration_free(DATA(self)); }

/*cdoc Duration IoDuration_duration(self)
Accessor exposed to other C code (IoDate in particular) so it can
reach the underlying Duration struct without duplicating the DATA
macro. Returns a borrowed pointer; the caller must not free it.
*/
Duration *IoDuration_duration(IoDuration *self) { return DATA(self); }

/*cdoc Duration IoDuration_fromSeconds_(self, s)
In-place mutator that rewrites every Duration field from a single
seconds value. Thin wrapper around Duration_fromSeconds_; the split
exists so IoDuration_newWithSeconds_ and the Io-level fromNumber
method share one path.
*/
IoDuration *IoDuration_fromSeconds_(IoDuration *self, double s) {
    Duration_fromSeconds_(DATA(self), s);
    return self;
}

/*cdoc Duration IoDuration_asSeconds(self)
Inverse of IoDuration_fromSeconds_: collapses the multi-field
Duration into a single double. Used by the Io-visible asNumber
method and by any C caller that needs a scalar.
*/
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
