# Date

An absolute point in time, represented as seconds since the epoch.

## -(aDurationOrDate)

Return a new Date with the receiver's value minus an amount of time
specified by aDuration to the receiver. Returns self.

## -=(aDuration)

Subtract aDuration from the receiver. Returns self.

## +(aDuration)

Return a new Date with the receiver's value plus an amount
of time specified by aDuration object to the receiver.

## +=(aDuration)

Add aDuration to the receiver. Returns self.

## asAtomDate

Returns the date formatted as a valid atom date (rfc4287) in the system's timezone.

## asNumber

Returns the date as seconds since 1970 UTC.

## asSerialization

Returns a serialization (sequence) of the date that allows for perfect
reconstruction of the timestamp.

## asString(optionalFormatString)

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

## asUTC

Changes the timezone of this date to utc

## clock

Returns a number containing the number of seconds
of processor time since the beginning of the program or -1 if unavailable.

## convertToLocal

Converts self date from a UTC date to the equivalent local date

## convertToUTC

Converts self from a local date to the equivalent UTC date

## convertToZone(offset, isDST)

Converts self to an equivalent data in a zone with offset (minutes west) and
DST (true, false).

## copy(aDate)

Sets the receiver to be the same date as aDate. Returns self.

## cpuSecondsToRun(expression)

Evaluates message and returns a Number whose value
is the cpu seconds taken to do the evaluation.

## day

Returns a number containing the day of the month of the receiver.

## format

Returns the format string for the receiver. The default is "%Y-%m-%d
%H:%M:%S %Z".

## fromNumber(aNumber)

Sets the receiver to be aNumber seconds since 1970.

## fromSerialization

Sets the date based on the serialization sequence.  Return self.

## fromString(aString, formatString)

Sets the receiver to the date specified by aString as parsed according to
the given formatString. See the Date asString method for formatting rules.
Returns self.

## gmtOffset

Returns the system's timezone string. E.g., +1300 or -0500.

## gmtOffsetSeconds

Returns the system's seconds east of UTC.

## hour

Returns a number containing the hour of the day(0-23) of the receiver.

## isDaylightSavingsTime

Returns self if Daylight Saving Time is in effect for the receiver,
otherwise returns Nil.

## isDST

Returns true if the Date is set to use DST.  Posix only.

## isPast

Returns true if the receiver is a date in the past.

## isToday

Returns true if the receiver's date is today's date.

## isValidTime(hour, min, sec)

Returns self if the specified time is valid, otherwise returns Nil.
A negative value will count back; i.e., a value of -5 for the hour,
will count back 5 hours to return a value of 19. No adjustment is
done for values above 24.

## minute

Returns a number containing the minute of the hour(0-59) of the receiver.

## month

Returns a number containing the month(1-12) of the year of the receiver.

## now

Sets the receiver to the current time. Returns self.

## print

Prints the receiver. Returns self.

## second

Returns a number containing the seconds of the minute(0-59) of the receiver.
This number may contain fractions of seconds.

## secondsSince(aDate)

Returns a number of seconds of between aDate and the receiver.

## secondsSinceNow(aDate)

Returns the number of seconds since aDate.

## secondsToRun(expression)

Evaluates message and returns a Number whose value is the number of seconds taken to do the evaluation

## setDay(aNumber)

Sets the day of the receiver. Returns self.

## setGmtOffset

Set the number of minutes west of GMT for this Date's zone

## setHour(aNumber)

Sets the hour of the receiver. Returns self.

## setMinute(aNumber)

Sets the minute of the receiver. Returns self.

## setMonth(aNumber)

Sets the month(1-12) of the receiver. Returns self.

## setSecond(aNumber)

Sets the second of the receiver. Returns self.

## setYear(aNumber)

Sets the year of the receiver.

## today

Set the receiver to the current date, no time information
is included. See `now' for the current date and time.

## year

Returns a number containing the year of the receiver.

## zone

Returns a string containing the system's time zone code.

