Regex

RegexTest := UnitTest clone do(
	testcaptureCount := method(
		assertEquals(0, Regex with(".*") captureCount)
		assertEquals(0, Regex with("\\d+.*\\w+") captureCount)
		assertEquals(1, Regex with("(\\d+).*\\w+") captureCount)
		assertEquals(2, Regex with("(\\d+).*(\\w+)") captureCount)
		assertEquals(3, Regex with("(\\d+)(.*)(\\w+)") captureCount)
		assertEquals(4, Regex with("(\\d+)(.*)()(\\w+)") captureCount)
	)

	testCaseless := method(
		string := "something"

		re := "[A-Z]+" asRegex
		assertEquals(false, re isCaseless)
		assertEquals(false, string matchesRegex(re))

		re := re caseless
		assertEquals(true, re isCaseless)
		assertEquals(true, string matchesRegex(re))

		re := re notCaseless
		assertEquals(false, re isCaseless)
		assertEquals(false, string matchesRegex(re))
	)

	testDotAll := method(
		string := "A\nB"
		
		re := "^.+$" asRegex
		assertEquals(false, re isDotAll)
		assertEquals(false, string matchesRegex(re))

		re := re dotAll
		assertEquals(true, re isDotAll)
		assertEquals(true, string matchesRegex(re))

		re := re notDotAll
		assertEquals(false, re isDotAll)
		assertEquals(false, string matchesRegex(re))
	)
	
	testExtended := method(
		string := "Oh no!"

		re := """
		\w+     # a word
		[ \t]+  # whitespace
		\w+     # another word
		!       # exclamation mark
		""" asRegex
		assertEquals(false, re isExtended)
		assertEquals(false, string matchesRegex(re))

		re := re extended
		assertEquals(true, re isExtended)
		assertEquals(true, string matchesRegex(re))
		
		re := re notExtended
		assertEquals(false, re isExtended)
		assertEquals(false, string matchesRegex(re))
	)

	testMultiline := method(
		string := "uno\ndos\ntres"
	
		re := "^." asRegex
		matches := string matchesOfRegex(re)
		assertEquals(false, re isMultiline)
		assertEquals(list("u"), matches all map(string))
		
		re := re multiline
		matches := string matchesOfRegex(re)
		assertEquals(true, re isMultiline)
		assertEquals(list("u", "d", "t"), matches all map(string))
		
		re := re notMultiline
		matches := string matchesOfRegex(re)
		assertEquals(false, re isMultiline)
		assertEquals(list("u"), matches all map(string))
	)
)

