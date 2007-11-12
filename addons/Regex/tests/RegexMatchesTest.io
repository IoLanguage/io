RegexMatchesTest := UnitTest clone do(
	setUp := method(
		super(setUp)
		self wordRegex := Regex with("\\w+")
	)
	
	testNext := method(
		matches := "one two three" matchesOfRegex(wordRegex)
	 	assertEquals("one", matches next string)
	 	assertEquals("two", matches next string)
	 	assertEquals("three", matches next string)
	 	assertEquals(nil, matches next)
	)
	
	testLast := method(
		matches := "one two three" matchesOfRegex(wordRegex)
		assertEquals("three", matches last string)
	)
	
	testAnchored := method(
		matches := "one two three" matchesOfRegex(wordRegex)
		assertEquals("one", matches anchored string)
		assertEquals(nil, matches anchored)
	)
	
	testAll := method(
		matches := "one two three" matchesOfRegex(wordRegex)
		assertEquals(list("one", "two", "three"), matches all map(string))
	)
	
	testForeach := method(
		matches := "one two three" matchesOfRegex(wordRegex)
		output := list
	
		matches foreach(m, output append(m string))
		assertEquals(list("one", "two", "three"), output)
	)
	
	testForeachInterval := method(
		matches := "one two three" matchesOfRegex(wordRegex)
		output := list

		matches foreachInterval(m, output append(m string), output append(m))
		assertEquals(list("one", " ", "two", " ", "three"), output)
	)
	
	testMap := method(
		matches := "one two three" matchesOfRegex(wordRegex)

		qm := "?"
		assertEquals(list("one?", "two?", "three?"), matches map(m, m string .. qm))

		em := "!"
		assertEquals(list("one!", "two!", "three!"), matches map(string .. em))
	)
	
	testReplace := method(
		matches := "Say it ain't so!" matchesOfRegex(wordRegex)
		assertEquals("*** ** ***'* **!", matches replace(m, "*" repeated(m string size)))
	)
	
	testReplaceWith := method(
		string := "37signals 43things"
		matches := string matchesOfRegex("(\\d+)([a-z]+)")
		assertEquals("signals:37 things:43", matches replaceWith("$2:$1"))
	)
	
	testNonMatches := method(
		matches := "one two three" matchesOfRegex(" ")
		assertEquals(list("one", "two", "three"), matches nonMatches)

		matches := "one!two!!three!!!" matchesOfRegex("!+")
		assertEquals(list("one", "two", "three"), matches nonMatches)
	)
	
	testSetPosition := method(	
		string := "funkbuster"
		matches := string matchesOfRegex(wordRegex)
	
		assertEquals(0, matches position)

		matches setPosition(-8)
		assertEquals(0, matches position)

		matches setPosition(666)
		assertEquals(string size, matches position)
		
		matches setPosition(4)
		assertEquals("buster", matches next string)
	)
	
	testSetEndPosition := method(
		string := "funkadelic"
		matches := string matchesOfRegex(wordRegex)

		assertEquals(string size, matches endPosition)
		
		matches setEndPosition(-8)
		assertEquals(0, matches endPosition)

		matches setEndPosition(666)
		assertEquals(string size, matches endPosition)
		
		matches setEndPosition(4)
		assertEquals("funk", matches next string)

		matches setEndPosition(nil)
		assertEquals("adelic", matches next string)
	)
	
	testDisallowEmptyMatches := method(
		matches := "Io" matchesOfRegex("^")
		assertEquals("", matches next string)
		
		matches reset disallowEmptyMatches
		assertEquals(nil, matches next)

		matches reset allowEmptyMatches
		assertEquals("", matches next string)
	)
	
	testEmptyMatches := method(
		matches := "one\ntwo\nthree" matchesOfRegex("$" asRegex multiline)
		
		match := matches next
		assertEquals(3, match start)
		assertEquals(3, match end)

		match := matches next
		assertEquals(7, match start)
		assertEquals(7, match end)

		match := matches next
		assertEquals(13, match start)
		assertEquals(13, match end)
	)
)

