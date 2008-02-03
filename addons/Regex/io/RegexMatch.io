Range

RegexMatch do(
	//doc RegexMatch captures Returns a list of captured strings. The first element is the whole match.
	captures := lazySlot(
		ranges map(range,
			if (range, subject slice(range first, range last), nil)
		)
	)

	//doc RegexMatch at(indexOrName) Returns the capture with the given index or name. at(0) is the entire match.
	at := method(indexOrName,
		if (index := indexOf(indexOrName), captures at(index), nil)
	)

	//doc RegexMatch indexOf(name) Returns the index of the capture with the given name.
	indexOf := method(name,
		if (name isKindOf(Sequence), regex nameToIndexMap at(name), name)
	)


	//doc RegexMatch names Returns a list containing the name of each capture.
	names := lazySlot(
		names := List clone setSize(captures size)
		regex nameToIndexMap asList foreach(l, names atPut(l at(1), l at(0)))
		names
	)
	//doc RegexMatch nameOf(index) Returns the name of the capture with the given index.
	nameOf := method(index,
		names at(index)
	)

	//doc RegexMatch string Returns the matched string.
	asString := string := method(captures first)


	//doc RegexMatch range Returns the range of the match in the subject.
	range := method(ranges first)

	//doc RegexMatch start Returns the index into the subject at which the match starts.
	start := method(if(range, range first, nil))

	//doc RegexMatch end Returns the index into the subject at which the match ends.
	end := method(if(range, range last, nil))


	//doc RegexMatch rangeOf(indexOrName) Returns the range of the capture with the given index or name.
	rangeOf := method(indexOrName,
		if (index := indexOf(indexOrName), ranges at(index), nil)
	)

	/*doc RegexMatch startOf(indexOrName) 
	Returns the index into the subject at which the capture with the given index or name starts.
	*/
	startOf := method(indexOrName,
		if(range := rangeOf(indexOrName), range first, nil)
	)

	/*doc RegexMatch endOf(indexOrName)
	Returns the index into the subject at which the capture with the given index or name ends."
	*/
	endOf := method(indexOrName,
		if(range := rangeOf(indexOrName), range last, nil)
	)


	/*doc RegexMatch expandTo(templateString)
	Returns <em>templateString</em> with capture placeholders replaced with what they represent.
	code>$0</code> is replaced with the whole match, <code>$1</code> is replaced with the first
	sub capture, etc. <code>${name}</code> is replaced with the capture of that name.
	*/
	ExpansionRegex := Regex with("""\$ (?> (\d+) | \{ (\w+) \})""") extended
	expandTo := method(templateString,
		templateString matchesOfRegex(ExpansionRegex) replace(m,
			cap := if(m at(1), m at(1) asNumber, m at(2))
			if(string := at(cap), string, "")
		)
	)
)
