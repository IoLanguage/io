
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
        knownBug(assertEquals(10, -10 abs))
        assertEquals(10, (-10) abs)
        assertTrue(10 between(9.9999, 10.0001))
        assertEquals(-0.5440211108893698, 10 sin)
        assertEquals(-0.8390715290764524, 10 cos )
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
        assertEquals(-0.5440211108893698, 10 perform("sin"))
    )
    
    /*
    what was this bug supposed to be?
    testFloatMaxBug := method(
        knownBug((Number floatMax) asString)
        knownBug("x = " .. (Number floatMax))
    )
    */
    
)
