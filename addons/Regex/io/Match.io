Range

Regex do(
	docSlot("Match",
		"Match instances hold the subject string, and the ranges of each capture.
		The ranges are used to extract the entire match and each capture."
	)

	Match do(
		docSlot("at(indexOrName)",
			"Returns the capture with the given index or name. match at(0) is the entire match."
		)
		at := method(indexOrName,
			captures at(indexOf(indexOrName))
		)
			
		docSlot("indexOf(name)",
			"Returns the index of the capture with the given name."
		)
		indexOf := method(name,
			if (name isKindOf(Sequence), nameToIndexMap at(name), name)
		)
		

		docSlot("names",
			"Returns a list containing the name of each capture."
		)
		names := lazySlot(
			names := List clone setSize(captures size)
			nameToIndexMap asList foreach(l, names atPut(l at(1), l at(0)))
			names
		)
		docSlot("nameOf(index",
			"Returns the name of the capture with the given index."
		)
		nameOf := method(index,
			names at(index)			
		)

		docSlot("string",
			"Returns the matched string."
		)
		asString := string := method(captures first)

		docSlot("asCode",
			"A poorly named inspect method"
		)
		asCode := method("<Regex Match: " .. match .. ">")


		docSlot("range",
			"Returns the range of the match in the subject."
		)
		range := method(ranges first)

		docSlot("start",
			"Returns the index into the subject at which the match starts."
		)
		start := method(if(range, range first, nil))

		docSlot("end",
			"Returns the index into the subject at which the match ends."
		)
		end := method(if(range, range last, nil))


		docSlot("rangeOf(indexOrName)",
			"Returns the range of the capture with the given index or name."
		)
		rangeOf := method(indexOrName,
			if (index := indexOf(indexOrName), ranges at(index), nil)
		)
		
		docSlot("startOf(indexOrName)",
			"Returns the index into the subject at which the capture with the given index or name starts."
		)
		startOf := method(indexOrName,
			if(range := rangeOf(indexOrName), range first, nil)
		)

		docSlot("endOf(indexOrName)",
			"Returns the index into the subject at which the capture with the given index or name ends."
		)
		endOf := method(indexOrName,
			if(range := rangeOf(indexOrName), range last, nil)
		)
	)


	docSlot("hasMatch",
		"Returns true if there are any matches or false otherwise."
	)
	hasMatch := method(currentMatch or nextMatch or false)

	docSlot("match",
		"Return the current match, or if there is none, finds the next match and returns it."
	)
	match := method(
		if(match := self currentMatch, match, self nextMatch)
	)

	docSlot("allMatches",
		"Returns a List of strings containing all matches of receiver's pattern within its string."
	)
	allMatches := method(
		matches := list
		resetSearch
		while(match := self nextMatch, matches append(match))
		matches
	)

	docSlot("eachMatch(aName, doSomethingWithAName)",
		"Perform the given message with aName set to each match."
	)
	eachMatch := method(
		name := call message argAt(0) name
		iterator := call message argAt(1)
		resetSearch
		while(match := nextMatch,
			call sender setSlot(name, match)
			call sender doMessage(iterator)
		)
	)
)
