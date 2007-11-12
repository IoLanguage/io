Sequence do(
	docSlot("asRegex",
		"Returns a new Regex created from the receiver."
	)
	asRegex := method(
		Regex with(self)
	)

	docSlot("matchesOfRegex(aRegexOrString)",
		"Returns a RegexMatches object that enumerates all matches of the given regex in the receiver."
	)
	matchesOfRegex := method(aRegexOrString,
		aRegexOrString asRegex matchesFor(self)
	)
	
	docSlot("matchesRegex(aRegexOrString)",
		"Returns true if the receiver matches the given regex, false if not."
	)
	matchesRegex := method(aRegexOrString,
		match := matchesOfRegex(aRegexOrString) anchored
		match isNil not and match string size == self size
	)
	
	docSlot("hasMatchOfRegex(aRegexOrString)",
		"Returns true if the string contains one or more matches of the given regex."
	)
	hasMatchOfRegex := method(aRegexOrString,
		matchesOfRegex(aRegexOrString) next isNil not
	)
	
	docSlot("allMatchesOfRegex(aRegexOrString)",
		"Returns a List containing all matches of the given regex found in the receiver."
	)
	allMatchesOfRegex := method(aRegexOrString,
		matchesOfRegex(aRegexOrString) all
	)
	
	docSlot("splitAtRegex(aRegexOrString)",
		"""Splits the receiver into pieces using the given regex as the delimiter and
		returns the pieces as a list of strings."""
	)
	splitAtRegex := method(aRegexOrString,
		matchesOfRegex(aRegexOrString) nonMatches
	)
	
	docSlot("escapeRegexChars",
		"""Returns a clone of the receiver with all special regular expression characters
		("^", "$", etc) backslashed. Useful if you have a string that contains such characters,
		but want it to be treated as a literal string."""
	)
	EscapeRegex := Regex with("""[-*+?|.()\[\]\{\}\\]""")
	escapeRegexChars := method(
		matchesOfRegex(EscapeRegex) replace(m, "\\" .. m)
	)
)
