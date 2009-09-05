Date do(
	/*doc Date today 
	Set the receiver to the current date, no time information
	is included. See `now' for the current date and time.
	*/
	
	today := method(Date now setHour(0) setMinute(0) setSecond(0))

	//doc Date isToday Returns true if the receiver's date is today's date.
	isToday := method(
		now := Date clone now
		now year == year and now month == month and now day == day
	)

	//doc Date secondsToRun(expression) Evaluates message and returns a Number whose value is the number of seconds taken to do the evaluation
	secondsToRun := method(
		t1 := Date clone now
		call relayStopStatus(call evalArgAt(0))
		dt := Date clone now secondsSince(t1)
	)

	//doc Date asAtomDate Returns the date formatted as a valid atom date (rfc4287) in the system's timezone.
	Date asAtomDate := method(
		asString("%Y-%m-%dT%H:%M:%S") .. gmtOffset asMutable atInsertSeq(3, ":")
	)
	
	asJson := method(asString asJson)
			
	justSerialized := method(stream,
		stream write("Date clone do(",
			"setYear(", self year, ") ",
			"setMonth(", self month, ") ",
			"setDay(", self day, ") ",
			"setHour(", self hour, ") ",
			"setMinute(", self minute, ") ",
			"setSecond(", self second, ")",
			");")
	)
	
	asNumberString := method(
		self asNumber asString alignLeft(27, "0")
	)
	
	timeStampString := method(
		Date clone now asNumber asString alignLeft(27, "0")
	)
)

Duration do(
	//doc Duration + Returns a new Duration of the two added.
	setSlot("+", method(d, self clone += d))

	//doc Duration - Returns a new Duration of the two subtracted.
	setSlot("-", method(d, self clone -= d))
)
