Regex do(
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
)
