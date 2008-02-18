RegexMatchTest := UnitTest clone do(
	setUp := method(
		super(setUp)
		self regex := Regex with("(?<number>\\d+)([ \t]+)?(?<word>\\w+)")
		self match := "37signals" matchesOfRegex(regex) next
	)

	testRange := method(
		assertEquals(0, match start)
		assertEquals(9, match end)
		assertEquals(0, match range first)
		assertEquals(9, match range last)
	)

	testCaptures := method(
		assertEquals(4, match size)
		
		assertEquals("37signals", match at(0))
		assertEquals("37", match at(1))
		assertEquals(nil, match at(2))
		assertEquals("signals", match at(3))

		assertEquals(nil, match at(4))
		assertEquals(nil, match at(666))
	)
	
	testNamedCaptures := method(
		assertEquals(list("number", "word"), match names)
		
		assertEquals("37", match at("number"))
		assertEquals("signals", match at("word"))
		assertEquals(nil, match at("humbug"))

		assertEquals(1, match indexOf("number"))
		assertEquals(3, match indexOf("word"))
		assertEquals(nil, match indexOf("humbug"))

		assertEquals("number", match nameOf(1))
		assertEquals("word", match nameOf(3))
		assertEquals(nil, match nameOf(666))
	)

	testCaptureRanges := method(
		assertEquals(0, match startOf(0))
		assertEquals(9, match endOf(0))

		assertEquals(0, match startOf(1))
		assertEquals(2, match endOf(1))

		assertEquals(2, match startOf(3))
		assertEquals(9, match endOf(3))

		assertEquals(nil, match startOf(666))
		assertEquals(nil, match endOf(666))
	)

	testNamedCaptureRanges := method(
		assertEquals(0, match startOf("number"))
		assertEquals(2, match endOf("number"))

		assertEquals(2, match startOf("word"))
		assertEquals(9, match endOf("word"))

		assertEquals(nil, match startOf("humbug"))
		assertEquals(nil, match endOf("humbug"))
	)
	
	testSlice := method(
		assertEquals(list("37", nil, "signals"), match slice(1))
		assertEquals(list("37signals", "37"), match slice(0, 2))
	)

	testForeach := method(
		out := list
		match foreach(capture, out append(capture))
		assertEquals(list("37signals", "37", nil, "signals"), out)	
	)

	testMap := method(
		out := match map(capture, capture)
		assertEquals(list("37signals", "37", nil, "signals"), out)	
	)

	testSelect := method(
		out := match select(capture, capture ?containsSeq("37"))
		assertEquals(list("37signals", "37"), out)	
	)

	testExpandTo := method(
		s := match expandTo("number of $3: ${number}") 
		assertEquals("number of signals: 37", s)

		s := match expandTo("number of $999: ${humbug}") 
		assertEquals("number of : ", s)
	)
)
