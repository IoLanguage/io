
NumberTest := UnitTest clone do(

	testClone := method(
		number := 5
		assertEquals(number, number clone)
		assertSame(Number, number proto)
	)

	testEquality := method(
		a := 10
		b := a
		assertTrue(a == 10)
		assertTrue(a == b)
	)

	testPrecision := method(
		assertEquals(2.2469135782469135, 2 * 1.123456789123456789)
		assertFalse(2 * 1.123456789123456789 == 2.246913578246913)
		assertEquals(10, 10.49 round)
		assertEquals(11, 10.50 round)
		assertEquals(11, 10.51 round)
	)

	testPrecedence := method(
		assertEquals(2 + (3 * 4) - (5 * 6) / 7, 2 + 3 * 4 - 5 * 6 / 7)
	)

	testFunctions := method(
		assertEquals("31.6227766017", 1000 sqrt asString(0, 10))
		assertEqualsWithinDelta(31.6227766, 1000 sqrt, 0.00000001)
		n := -10
		assertEquals(10, n abs)
		//knownBug(assertEquals(10, -10 abs))
		assertEquals(10, (-10) abs)
		assertTrue(10 between(9.9999, 10.0001))
		assertEquals(1, 10 log10)
	)

	testDirectAssignment := method(
		a := 5
		a = 10
		assertEquals(10, a)
		assertFalse(a < -10)
		assertFalse(a <(-10))
	)

	testBitwiseOperations := method(
		assertEquals(4, 2 shiftLeft(1))
		assertEquals(1, 2 shiftRight(1))
		assertEquals(7, 3 | (4))
		assertEquals(7, 3 bitwiseOr(4))
		assertEquals(0, 3 & (4))
		assertEquals(0, 3 bitwiseAnd(4))
	)

	testCharacters := method(
		assertTrue(65 isLetter)
		assertFalse(30 isLetter)
		assertFalse(47 isDigit)
		assertTrue(48 isDigit)
		assertTrue(57 isDigit)
		assertFalse(58 isDigit)
	)

	testPerform := method(
		assertEquals(1, 10 perform("log10"))
	)

	/*
	what was this bug supposed to be?
	testFloatMaxBug := method(
		knownBug((Number floatMax) asString)
		knownBug("x = " .. (Number floatMax))
	)
	*/

	testAsString := method(
		# Test the no-argument form (try all the IoNumber_Double_intoCString_() paths)
		assertEquals("1", 1 asString)
		assertEquals("2147483647", 2147483647 asString)
		assertEquals("1.1000000000000001", 1.1 asString)
		assertEquals("1.2", 1.2 asString)

		# Windows formats exponents to 3 digits
		plat := System platform
		isOnWindows := plat beginsWithSeq("Windows") or plat beginsWithSeq("mingw")
		if(isOnWindows,
			mersenne8 := "2.147484e+009",
			mersenne8 := "2.147484e+09")
		assertEquals(mersenne8, 2147483648 asString)

		# Test the one argument form
		assertEquals("1.000000", 1 asString(0))
		assertEquals("1.000000", 1 asString(1))
		assertEquals("  1.000000", 1 asString(10))
		assertEquals("1.100000", 1.1 asString(0))
		assertEquals("1.100000", 1.1 asString(1))
		assertEquals("  1.100000", 1.1 asString(10))

		assertEquals("2147483647.000000", 2147483647 asString(0))
		assertEquals("2147483648.000000", 2147483648 asString(0))

		# Test the two argument form
		assertEquals("137846528820", 137846528820 asString(0, 0))
		assertEquals("137846528820.0", 137846528820 asString(0, 1))
		assertEquals("        137846528820", 137846528820 asString(20, 0))
		assertEquals("      137846528820.0", 137846528820 asString(20, 1))

		assertEquals("-137846528820", (-137846528820) asString(0, 0))
		assertEquals("-137846528820.0", (-137846528820) asString(0, 1))
		assertEquals("       -137846528820", (-137846528820) asString(20, 0))
		assertEquals("     -137846528820.0", (-137846528820) asString(20, 1))
	)

	testAsHex := method(
		assertEquals("0f", 0xf asHex)
		assertEquals("ff", 0xff asHex)
		assertEquals("00", 0x00 asHex)
		assertEquals("0100", 0x100 asHex)
		assertEquals("ffef", 0xffef asHex)
	)

	testAsBinary := method(
		assertEquals("00101010", 42 asBinary)
		assertEquals("0000000100000000", 256 asBinary)
	)

	testAsOctal := method(
		assertEquals("001", 1 asOctal)
		assertEquals("141", 97 asOctal)
		assertEquals("001000", 512 asOctal)
	)

	testToBase := method(
		assertEquals("101010", 42 toBase(2))
		assertEquals("f", 15 toBase(16))
	)
)
