OperatorTableTest := UnitTest clone do(
	// Source that is run (doString, doFile) is operator-shuffled one
	// statement at a time, just before each statement runs. It used to be
	// shuffled whole before any of it ran, so an operator added by one
	// statement did not apply to the statements after it in the same
	// source (#494, #438).

	testOperatorAddedMidSource := method(
		r := doString("OperatorTable addOperator(\"plusTest\", 3)
			Number plusTest := method(n, self + n)
			1 plusTest 2 plusTest 3")
		OperatorTable operators removeAt("plusTest")
		Number removeSlot("plusTest")
		assertEquals(r, 6)
	)

	testAssignOperatorAddedMidSource := method(
		r := doString("OperatorTable addAssignOperator(\"<~\", \"storeTest\")
			o := Object clone
			o got := list()
			o storeTest := method(k, v, got append(k) append(v); self)
			o a <~ 1 + 2
			o got")
		OperatorTable assignOperators removeAt("<~")
		assertEquals(r, list("a", 3))
	)

	testPrecedenceChangedMidSource := method(
		r := doString("a := 1 + 2 * 3
			OperatorTable addOperator(\"*\", 3)
			b := 1 + 2 * 3
			OperatorTable addOperator(\"*\", 2)
			c := 1 + 2 * 3
			list(a, b, c)")
		assertEquals(r, list(7, 9, 7))
	)

	testMethodBodyKeepsOperatorsFromDefinition := method(
		r := doString("f := method(1 + 2 * 3)
			OperatorTable addOperator(\"*\", 3)
			x := f
			OperatorTable addOperator(\"*\", 2)
			x")
		assertEquals(r, 7)
	)

	testStatementsOnOneLine := method(
		assertEquals(doString("a := 1 + 2 * 3; b := a * 2 + 1; b"), 15)
	)

	testLaterCompileErrorIsCatchable := method(
		seen := list()
		e := try(doString("seen append(1); y :=; seen append(2)"))
		assertTrue(e error containsSeq("must be followed by a value"))
		assertEquals(seen, list(1))
	)

	testParsedMessageIsFullyShuffled := method(
		m := "x := 2 + 3 * 4; y := x - 1" asMessage
		assertEquals(m asString, "setSlot(\"x\", 2 +(3 *(4))) ;\nsetSlot(\"y\", x -(1))")
	)
)
