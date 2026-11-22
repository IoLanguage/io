InterpolateTest := UnitTest clone do(
	// interpolate turns the string into one message whose arguments are its
	// pieces, so each #{expression} runs as an ordinary frame and can wait
	// or yield. These cases pin down what it returns.

	testExpressions := method(
		x := 5
		assertEquals("a #{x} b #{x * 2} c" interpolate, "a 5 b 10 c")
	)

	testNoExpressions := method(
		assertEquals("plain" interpolate, "plain")
		assertEquals("" interpolate, "")
	)

	testEmptyAndUnterminated := method(
		assertEquals("#{}|#{1 + 1}" interpolate, "|2")
		assertEquals("open #{1 + 1" interpolate, "open #{1 + 1")
	)

	testAdjacentAndEdges := method(
		assertEquals("#{1}#{2}" interpolate, "12")
		assertEquals("#{1} end" interpolate, "1 end")
		assertEquals("start #{2}" interpolate, "start 2")
	)

	testValuesUseAsString := method(
		assertEquals("#{nil} #{list(1, 2)} #{\"q\"}" interpolate, "nil list(1, 2) q")
	)

	testContextArgument := method(
		o := Object clone
		o n := 3
		assertEquals("n=#{n}" interpolate(o), "n=3")
	)

	testSeveralStatements := method(
		assertEquals("#{y := 3; y + 1}" interpolate, "4")
	)

	testResultIsNotInterpolatedAgain := method(
		s := "#" .. "{1 + 1}"
		assertEquals("#{s}" interpolate, s)
	)

	testReturnsSymbol := method(
		assertEquals("#{1}" interpolate type, "Sequence")
		assertEquals("a#{1}" interpolate .. "b", "a1b")
	)

	testInPlace := method(
		s := "v=#{1 + 2}!" asMutable
		r := s interpolateInPlace
		assertEquals(s, "v=3!")
		assertTrue(r isIdenticalTo(s))
	)

	testInsideMethodAndMap := method(
		f := method(a, "got #{a}" interpolate)
		assertEquals(f(9), "got 9")
		assertEquals(list(1, 2) map(v, "<#{v}>" interpolate), list("<1>", "<2>"))
	)

	testExpressionCanYield := method(
		assertEquals("#{yield; 7} and #{yield; 8}" interpolate, "7 and 8")
	)

	testErrorInExpression := method(
		e := try("#{1 + nil}" interpolate)
		assertTrue(e error containsSeq("must be a Number"))
	)

	testNonAscii := method(
		assertEquals("é#{1}ü" interpolate, "é1ü")
		w := "ñandú"
		assertEquals("#{w} é" interpolate, "ñandú é")
	)
)
