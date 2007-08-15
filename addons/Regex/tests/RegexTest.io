RegexTest := UnitTest clone do(
	assertEqual := getSlot("assertEquals")

	testWithoutCaptures := method(
		r := Regex clone setString("Hello World") setPattern("\\w+")
		first := r nextMatch
		assertEqual("Hello", first string)
		assertEqual(list("Hello"), first captures)
		assertEqual(first, r currentMatch)
		assertTrue(first == r currentMatch)

		last := r nextMatch
		assertEqual("World", last string)
		assertTrue(last == r currentMatch)

		# Make sure they're not the same match
		assertTrue(first != last)
		assertEqual("Hello", first string)
	)

	testWithCaptures := method(
		r := Regex clone setString("37signals 43things 69pictures") setPattern("(\\d+)(\\w+)")

		s := r nextMatch
		assertEqual("37signals", s at(0))
		assertEqual("37", s at(1))
		assertEqual("signals", s at(2))

		t := r nextMatch
		assertEqual("43things", t at(0))
		assertEqual("43", t at(1))
		assertEqual("things", t at(2))

		p := r nextMatch
		assertEqual("69pictures", p at(0))
		assertEqual("69", p at(1))
		assertEqual("pictures", p at(2))
	)

	testHasMatch := method(
		r := Regex clone setString("Hey you!") setPattern("[A-Za-z]+")
		assertTrue(r hasMatch)
		r setString("12 34")
		assertFalse(r hasMatch)
	)

	testAllMatches := method(
		r := Regex clone setString("Hey you!") setPattern("\\w+")
		matches := r allMatches
		assertEqual(2, matches size)
		assertEqual("Hey", matches first string)
		assertEqual("you", matches last string)
	)
  
	testEachMatch := method(
		r := Regex clone setString("Hey you!") setPattern("\\w+")
		record := list
		r eachMatch(m, record append(m))
		assertEqual(2, record size)
		assertEqual("Hey", record first string)
		assertEqual("you", record last string)
	)

	testEachInterval := method(
		r := Regex clone setString("Hey you!") setPattern("\\w+")
		record := list
		r eachInterval(m, record append(m), record append(m))
		assertEqual(4, record size)
		assertEqual("Hey", record at(0) string)
		assertEqual(" ", record at(1))
		assertEqual("you", record at(2) string)
		assertEqual("!", record at(3))
	)

	testSubstitute := method(
		r := Regex clone setString("Hello32 World10") setPattern("\\d+")
		assertEqual("Hello42 World20", r substitute(m, m string asNumber + 10))
	)

	testEscape := method(
		assertEqual("Hello", Regex escape("Hello"))
		assertEqual("\\\\d", Regex escape("\\d"))
		assertEqual("\\(\\.\\+\\)", Regex escape("(.+)"))
	)

	testcaptureCount := method(
		assertEqual(0, Regex clone setPattern(".*") captureCount)
		assertEqual(0, Regex clone setPattern("\\d+.*\\w+") captureCount)
		assertEqual(1, Regex clone setPattern("(\\d+).*\\w+") captureCount)
		assertEqual(2, Regex clone setPattern("(\\d+).*(\\w+)") captureCount)
		assertEqual(3, Regex clone setPattern("(\\d+)(.*)(\\w+)") captureCount)
		assertEqual(4, Regex clone setPattern("(\\d+)(.*)()(\\w+)") captureCount)
	)

	testQuickSub := method(
		r := Regex clone setString("37signals 43things 69pictures") setPattern("(\\d+)(\\w+)")
		assertEqual("=signals-37= =things-43= =pictures-69=", r qsub("=\\2-\\1="))
		assertEqual("\\37 \\43 \\69", r qsub("\\\\\\1"))
		assertEqual("1 1 1", r qsub("\\\\1"))
	)
)

RegexOptionTest := UnitTest clone do(
	assertEqual := getSlot("assertEquals")

	testCaseless := method(
		r := Regex clone setString("37signals") setPattern("37SIGNALS")
		r caselessOn
		assertEqual(r match string, "37signals")
		r caselessOff resetSearch
		assertEqual(r match, nil)
	)

	testDotAll := method(
		r := Regex clone setString("37\nsignals") setPattern(".+")
		r dotAllOn
		assertEqual(r match string, "37\nsignals")
		r dotAllOff resetSearch
		assertEqual(r match string, "37")
	)
	
	testExtended := method(
		r := Regex clone setString("37signals") setPattern("\\d+ \\w+  # comment")
		r extendedOn
		assertEqual(r match string, "37signals")
		r extendedOff resetSearch
		assertEqual(r match, nil)
	)

	testMultiline := method(
		r := Regex clone setString("37\nsignals") setPattern("^.+$")
		r multilineOn
		assertEqual(r match string, "37")
		r multilineOff resetSearch
		assertEqual(r match, nil)
	)
	
	testMatchEmpty := method(
		r := Regex clone setString("") setPattern(".*")
		r matchEmptyOn
		assertEqual(r match string, "")
		r matchEmptyOff resetSearch
		assertEqual(r match, nil)
	)
)

RegexMatchTest := UnitTest clone do(
	setUp := method(
		super(setUp)
		self regex := Regex clone setString("37signals") setPattern("(?<number>\\d+)(?<word>\\w+)")
		self match := regex match
	)

	testRange := method(
		assertEquals(0, match start)
		assertEquals(9, match end)
		assertEquals(0, match range first)
		assertEquals(9, match range last)
	)

	testCaptures := method(
		assertEquals("37", match at(1))
		assertEquals("signals", match at(2))
		assertEquals("37signals", match at(0))
	)
	
	testNamedCaptures := method(
		assertEquals("37", match at("number"))
		assertEquals("signals", match at("word"))

		assertEquals(1, match indexOf("number"))
		assertEquals(2, match indexOf("word"))

		assertEquals("number", match nameOf(1))
		assertEquals("word", match nameOf(2))
	)

	testCaptureRanges := method(
		assertEquals(0, match startOf(0))
		assertEquals(9, match endOf(0))

		assertEquals(0, match startOf(1))
		assertEquals(2, match endOf(1))

		assertEquals(2, match startOf(2))
		assertEquals(9, match endOf(2))
	)

	testNamedCaptureRanges := method(
		assertEquals(0, match startOf("number"))
		assertEquals(2, match endOf("number"))

		assertEquals(2, match startOf("word"))
		assertEquals(9, match endOf("word"))
	)
)

RegexSequenceTest := UnitTest clone do(
	assertEquals := getSlot("assertEquals")
	
	testAsRegex := method(
		r := ".+" asRegex		
		assertEquals(".+", r pattern)
		assertEquals("", r string)
	)
	
	testMatchesRegex := method(
		s := "37signals"
		assertEquals(true, s matchesRegex("37signals"))
		assertEquals(false, s matchesRegex("37"))
		assertEquals(false, s matchesRegex("signals"))

		r := Regex clone 
		assertEquals(true, s matchesRegex(r setPattern("37signals")))
		assertEquals(false, s matchesRegex(r setPattern("37")))
		assertEquals(false, s matchesRegex(r setPattern("signals")))
	)
	
	testFindRegex := method(
		s := "37signals"
		assertEquals("signals", s findRegex("signals") string)
			
		r := Regex clone setPattern("signals")
		assertEquals("signals", s findRegex(r) string)
	)

	testMatchesOfRegex := method(
		m := "one two three" matchesOfRegex("\\w+")
		assertEquals(list("one", "two", "three"), m map(string))
		
		r := Regex clone setPattern("\\w+")
		m = "one two three" matchesOfRegex(r)
		assertEquals(list("one", "two", "three"), m map(string))
	)
)

RegexTest run
RegexOptionTest run
RegexMatchTest run
RegexSequenceTest run
"Tests finished" println
