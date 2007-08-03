Regex do(
	docSlot("with(string, pattern)",
		"Returns a clone of the Regex object with the string set to the first argument
		and the pattern set to the second."
	)
	with := method(string, pattern,
		self clone setString(string) setPattern(pattern)
	)
	
	docSlot("asRegex",
		"Simply returns self."
	)
	asRegex := method(self)


	docSlot("substitute(name, substitutionMessage)",
		"Substitutes the result of substitutionMessage with each match, returning
		the substituted string."
	)
	substitute := method(
		name := call message argAt(0) name
		sub := call message argAt(1)
		parts := list
		eachInterval(i,
			call sender setSlot(name, i)
			parts append(call sender doMessage(sub))
		,
			parts append(i)
		)
		parts join
	)


	docSlot("qsub(formatString)",
		"Perform substitution using the provided format string.
		The format string replaces \\N with the Nth group."
	)
	SubstitutionRegex := Regex clone setPattern("(\\\\+)(\\d+)")
	qsub := method(format,
		parts := list
		SubstitutionRegex setString(format) eachInterval(i,
			extraSlashes := i at(1) slice(0, ((i at(1) size - 1) / 2) floor)
			if(extraSlashes size > 0, parts append(extraSlashes))

			# Append a string if it is a normal digit, a number if it is a capture reference
			parts append(
				if(i at(1) size % 2 == 0, i at(2), i at(2) asNumber)
			),
			parts append(i) # Append the inter-match string
		)

		self substitute(match,
			parts map(part,
				if(part hasProto(Number), match at(part) asString, part)
			) join
		)
	)


	docSlot("escape(aString)",
		"Escape the provided string such that
		<pre>Regex clone setString(string) setPattern(Regex escape(string))</pre>
		will match the entire string."
	)
	EscapeRegex := Regex clone setPattern("[-*+?.()\\[\\]\\{\\}\\\\]")
	escape := method(string,
		EscapeRegex setString(string) substitute(match, "\\" .. match)
	)


	docSlot("eachInterval(intervalName, matchMessage, interMatchMessage)",
		"Iterate each interval in the string. The first message is performed for
		each match; the second for each inter-match interval."
	)
	eachInterval := method(
		name := call message argAt(0) name
		cursor := 0
		eachMatch(match,
			if(match start > cursor,
				call sender setSlot(name, string slice(cursor, match start))
				call sender doMessage(call message argAt(2))
			)
			call sender setSlot(name, match)
			call sender doMessage(call message argAt(1))
			cursor := match end
		)

		# Perform the inter-match interval for the tail end
		tail := string slice(cursor)
		if(tail size > 0,
			call sender setSlot(name, tail)
			call sender doMessage(call message argAt(2))
		)
		self
	)
)
