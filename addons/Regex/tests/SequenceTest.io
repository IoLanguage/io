SequenceTest := UnitTest clone do(
	testAsRegex := method(
		assertEquals(".+", ".+" asRegex pattern)
	)
	
	testMatchesRegex := method(
		s := "37signals"
		assertEquals(true, s matchesRegex("37signals"))
		assertEquals(false, s matchesRegex("37"))
		assertEquals(false, s matchesRegex("signals"))

		assertEquals(true, s matchesRegex("37signals" asRegex))
		assertEquals(false, s matchesRegex("37" asRegex))
		assertEquals(false, s matchesRegex("signals" asRegex))
	)
	
	testHasMatchOfRegex := method(
		s := "37signals"
		assertEquals(true, s hasMatchOfRegex("signals"))
		assertEquals(true, s hasMatchOfRegex("signals" asRegex))
			
		assertEquals(false, s hasMatchOfRegex("uuh"))
		assertEquals(false, s hasMatchOfRegex("uuh" asRegex))
	)

	testAllMatchesOfRegex := method(
		matches := "one two three" allMatchesOfRegex("\\w+") map(string)
		assertEquals(list("one", "two", "three"), matches)

		matches := "one two three" allMatchesOfRegex("\\w+" asRegex) map(string)
		assertEquals(list("one", "two", "three"), matches)
	)
	
	testSplitAtRegex := method(
		s := "one!two!!three!!!"
		assertEquals(list("one", "two", "three"), s splitAtRegex("!+"))
	)

	testEscapeRegexChars := method(
		assertEquals("Hello", "Hello" escapeRegexChars)
		assertEquals("\\\\d", "\\d" escapeRegexChars)
		assertEquals("\\(\\.\\+\\)", "(.+)" escapeRegexChars)
	)
)
