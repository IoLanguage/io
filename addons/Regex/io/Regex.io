Regex do(
	docSlot("matchesFor(aString)",
		"Returns a RegexMatches object that enumerates the matches of the receiver
		in the given string."
	)
	matchesFor := method(aString,
		RegexMatches clone setRegex(self) setString(aString)
	)

	docSlot("asRegex",
		"Returns self."
	)
	asRegex := method(
		self
	)
)
