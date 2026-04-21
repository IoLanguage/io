# Duration

A container for a duration of time.

## -

Returns a new Duration of the two subtracted.

## -=(aDuration)

Subtract aDuration to the receiver. Returns self.

## +

Returns a new Duration of the two added.

## +=(aDuration)

Add aDuration to the receiver. Returns self.

## asNumber

Returns a number representation of the receiver.
(where 1 is equal to one second)

## asString(formatString)

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

## days

Returns a number containing the day of the month of the receiver.

## fromNumber(aNumber)

Sets the receiver to the Duration specified by
aNumber(same format number as returned by Duration asNumber). Returns self.

## hours

Returns a number containing the hour of the day(0-23) of the receiver.

## minutes

Returns a number containing the minute of the hour(0-59) of the receiver.

## print

Prints the receiver. Returns self.

## seconds

Returns a number containing the seconds of the minute(0-59) of the receiver.
This number may contain fractions of seconds.

## setDays(aNumber)

Sets the day of the receiver. Returns self.

## setHours(aNumber)

Sets the hour of the receiver. Returns self.

## setMinutes(aNumber)

Sets the minute of the receiver. Returns self.

## setSeconds(aNumber)

Sets the second of the receiver. Return self.

## setYears(aNumber)

Sets the year of the receiver. Returns self.

## totalSeconds

Same as a asNumber.

## years

Returns a number containing the year of the receiver.

