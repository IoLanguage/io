Regex do(
	/*doc Regex matchesFor(aString) 
		Returns a RegexMatches object that enumerates the matches of the receiver
		in the given string.
	*/
	matchesFor := method(aString,
		RegexMatches clone setRegex(self) setString(aString)
	)

	/*doc Regex asRegex
	Returns self.
	*/
	asRegex := method(self)
	
	/* doc Regex nameTable
	Returns a list with the name of each capture.
	The first element will always be nil, because it corresponds to the whole match.
	The second element will contain the name of the first capture, or nil if the first
	capture has no name.
	And so on.
	*/
	nameTable := lazySlot(
		names := list setSize(captureCount + 1)
		namedCaptures keys foreach(key,
			names atPut(namedCaptures at(key), key)
		)
		names
	)
	
	/* doc Regex names
	Returns a list of the name of each named capture.
	If there are no named captures, the list will be empty.
	*/
	names := method(
		nameTable select(isNil not)
	)
)
