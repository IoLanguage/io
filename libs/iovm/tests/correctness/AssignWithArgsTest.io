AssignWithArgsTest := UnitTest clone do(
	// `target name(args) := value` passes the left side to setSlot as a
	// message. The value used to be only the first message after `:=`,
	// taken before operator shuffling, and it was left in the chain a
	// second time: `a foo(1) := 5 + 2` became `a setSlot(foo(1), 5) 5 +(2)`.

	testValueIsWholeExpression := method(
		assertEquals(message(a foo(1) := 5 + 2) asString, "a setSlot(foo(1), 5 +(2))")
		assertEquals(message(a foo(1) := b c) asString, "a setSlot(foo(1), b c)")
	)

	testValueNotRepeated := method(
		assertEquals(message(a foo(1) := 5) asString, "a setSlot(foo(1), 5)")
		assertEquals(message(a foo(1) := b; d) asString, "a setSlot(foo(1), b) ;\nd")
	)

	testOperatorsInsideAreShuffled := method(
		assertEquals(message(a foo(1 + 2 * 3) := 5) asString, "a setSlot(foo(1 +(2 *(3))), 5)")
		assertEquals(message(a foo(1) := list(1 + 2 * 3)) asString, "a setSlot(foo(1), list(1 +(2 *(3))))")
	)

	testNoReceiver := method(
		assertEquals(message(list(a, b) := c d) asString, "setSlot(list(a, b), c d)")
	)

	testValueEvaluatedOnce := method(
		o := Object clone
		o count := 0
		o bump := method(count = count + 1; count)
		o seen := nil
		o setSlot := method(seen = call evalArgAt(1); nil)
		o at(1) := o bump
		assertEquals(o count, 1)
		assertEquals(o seen, 1)
	)

	testPlainAssignmentUnchanged := method(
		assertEquals(message(a b := 5 + 2; d) asString, "a setSlot(\"b\", 5 +(2)) ;\nd")
	)

	testMissingValueIsCompileError := method(
		e := try(doString("a foo(1) :="))
		assertTrue(e error containsSeq("must be followed by a value"))
	)
)
