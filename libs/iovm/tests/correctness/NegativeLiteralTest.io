NegativeLiteralTest := UnitTest clone do(
	// `-` written directly before a number is a negative literal when nothing
	// stands to its left, so `-1 acos` is `(-1) acos`. It used to parse as
	// `-(1 acos)`: the minus applied to the whole rest of the chain (#489).
	// With a left operand, or a space after the `-`, the meaning is unchanged.

	testMessageSentToNegativeLiteral := method(
		assertEquals(-1 abs, 1)
		assertEquals(-2.5 abs, 2.5)
		assertTrue((-1 acos - 3.14159265) abs < 0.0001)
	)

	testAfterOperator := method(
		assertEquals(2 * -1 abs, 2)
		assertEquals(2 + -1 abs, 3)
		assertEquals(1 - -1, 2)
		assertEquals(2 ** -1, 0.5)
	)

	testAfterAssignmentAndReturn := method(
		x := -3 abs
		assertEquals(x, 3)
		assertEquals(method(return -4 abs) call, 4)
	)

	testAsArgument := method(
		assertEquals(list(1, -2 abs) at(1), 2)
		assertEquals(list(1, 2) at(-1), 2)
	)

	testAtStatementStart := method(
		r := doString("1; -5 abs")
		assertEquals(r, 5)
	)

	testHexAndExponent := method(
		assertEquals(-0x10 abs, 16)
		assertEquals(-1.5e2 abs, 150)
	)

	testLeftOperandIsSubtraction := method(
		a := 3
		assertEquals(3 -1, 2)
		assertEquals(3-1, 2)
		assertEquals(a -1, 2)
		assertEquals(a-1, 2)
		assertEquals(1 -1 -1, -1)
		assertEquals(10 -2 * 3, 4)
		assertEquals(list(5) at(0) -1, 4)
		assertEquals(10 -1 abs, 9)
	)

	testSpaceAfterMinusIsNegation := method(
		assertEquals(- 1 abs, -1)
		b := 5
		assertEquals(-b, -5)
	)

	testMessageText := method(
		assertEquals(message(-1 abs) asString, "-1 abs")
		assertEquals(message(3 -1) asString, "3 -(1)")
		assertEquals(message(-1 abs) cachedResult, -1)
	)
)
