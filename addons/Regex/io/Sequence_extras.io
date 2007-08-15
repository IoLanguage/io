Sequence do(
	docSlot("asRegex",
		"Returns a new Regex with the reciever set as the pattern."
	)
	asRegex := method(
		Regex clone setPattern(self)
	)
	
	docSlot("matchesRegex(aStringOrRegex)",
		"Returns true if the receiver matches the given regex, false if not."
	)
	matchesRegex := method(aStringOrRegex,
		match := aStringOrRegex asRegex anchoredOn setString(self) match
		match and (match string size) == (self size)
	)
	
	docSlot("findRegex(aStringOrRegex"
		"Returns the first match of the given regex, or nil if no match was found."
	)
	findRegex := method(aStringOrRegex,
		aStringOrRegex asRegex setString(self) match
	)
	
	docSlot("matchesOfRegex(aStringOrRegex)",
		"Returns a List containing all matches of the given regex found in the receiver."
	)
	matchesOfRegex := method(aStringOrRegex,
		aStringOrRegex asRegex setString(self) allMatches
	)
)
