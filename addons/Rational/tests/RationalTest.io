Rational

RationalTest := UnitTest clone do(
	testWith := method(
		v := Rational with(1, 2)
		assertEquals(v, Rational clone setNumerator(1) setDenominator(2))
	)

	testReduce := method(
		v := Rational clone setNumerator(2) setDenominator(4) reduce
		assertEquals(v, Rational with(1, 2))
	)

	testAdd := method(
		v := Rational with(1, 2) + Rational with(2, 3)
		assertEquals(v, Rational with(7, 6))
	)

	testSubtract := method(
		v := Rational with(1, 2) - Rational with(1, 8)
		assertEquals(v, Rational with(3, 8))
	)

	testMultiply := method(
		v := Rational with(1, 2) * Rational with(4, 1)
		assertEquals(v, Rational with(2, 1))
	)

	testDivide := method(
		v := Rational with(1, 2) / Rational with(1, 8)
		assertEquals(v, Rational with(4, 1))
	)

	testDivmod := method(
		v := Rational with(3, 4) divmod(Rational with(1, 2))
		assertEquals(v, list(1, 0.25))
	)

	testModulus := method(
		v := Rational with(3, 4) % Rational with(1, 2)
		assertEquals(v, 0.25)
	)

	testPow := method(
		v := Rational with(1, 2) pow(Rational with(8, 1))
		assertEquals(v, Rational with(1, 256))
	)

	testAbs := method(
		v := Rational with(-1, 2) abs
		assertEquals(Rational with(-1, 2) abs, Rational with(1, 2))
	)

	testNegate := method(
		v := Rational with(1, 2) negate
		assertEquals(v, Rational with(-1, 2))
	)

	testEqual := method(
		assertTrue(Rational with(1, 2) == Rational with(1, 2))
	)

	testNotEqual := method(
		assertTrue(Rational with(1, 2) != Rational with(2, 1))
	)

	testCompare := method(
		v1 := Rational with(1, 2) compare(Rational with(2, 1))
		v2 := Rational with(1, 2) compare(Rational with(1, 2))
		v3 := Rational with(1, 2) compare(Rational with(1, 4))
		assertTrue(v1 == -1 and v2 == 0 and v3 == 1)
	)

	testAsNumber := method(
		assertEquals(0.5, Rational with(1, 2) asNumber)
	)

	testAsString := method(
		assertEquals("1/2", Rational with(1, 2) asString)
	)
)
