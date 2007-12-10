context := method(name, ContextScope clone setName(name) doMessage(call argAt(1)))

doFile("addWordyMethods.io")

#Number do(
#    should := method(
#        shouldScope clone setActual(self)
#    )
#
#    shouldScope := Object clone do(
#        newSlot("actual")
#
#        be := method(expected,
#            if(expected == nil and call message arguments size == 0,
#                beScope clone setActual(actual)
#            ,
#                assertEquals(expected, actual)
#            )
#        )
#    )
#
#    shouldScope not := method(
#        shouldNotScope clone setActual(actual)
#    )
#
#    shouldScope shouldNotScope := Object clone do(
#        newSlot("actual")
#        be := method(expected,
#            if(expected == nil and call message arguments size == 0,
#                beScope clone setActual(actual)
#            ,
#                assertNotEquals(expected, actual)
#            )
#        )
#    )
#
#    shouldScope beScope := Object clone do(
#        newSlot("actual")
#        close := method(expected, tolerance,(
#            tolerance ifNil(tolerance = 0.0001)
#            assertCompare(expected, actual, call argAt(0), message(actual), (expected - actual) abs <= tolerance)
#        )
#    )
#
#    shouldScope shouldNotScope beScope := Object clone do(
#        newSlot("actual")
#        close := method(expected, tolerance,
#            tolerance ifNil(tolerance = 0.0001)
#            assertCompare(expected, actual, call argAt(0), message(actual), (expected - actual) abs > tolerance)
#        )
#    )
#)
#
#Object do(
#    should := method(
#        shouldScope clone setActual(self)
#    )
#
#    shouldScope := Object clone do(
#        newSlot("actual")
#
#        be := method(expected, assertSame(expected, actual))
#        equal := method(expected, assertEquals(expected, actual))
#    )
#
#    shouldScope not := method(
#        shouldNotScope clone setActual(actual)
#    )
#
#    shouldScope shouldNotScope := Object clone do(
#        newSlot("actual")
#        be := method(expected, assertNotSame(expected, actual))
#        equal := method(expected, assertNotEquals(expected, actual))
#    )
#)
#
#nil should := Object clone do(
#    raise := method(e,
#        AssertionFailure raise("Expected a '" .. e type .. "' exception.")
#    )
#)
#
#nil should not := Object clone do(
#    raise := nil
#)
#
#Exception do(
#    should := method(
#        shouldScope clone setException(self)
#    )
#
#    shouldScope := Object clone do(
#        newSlot("exception")
#
#        raise := method(e,
#            if(exception isKindOf(e) not,
#                AssertionFailure raise("Unexpected exception '" .. exception type .. "' instead of '" .. e type .. "'.", exception)
#            )
#        )
#    )
#
#    shouldScope not := method(
#        shouldNotScope clone setException(exception)
#    )
#
#    shouldScope shouldNotScope := Object clone do(
#        newSlot("exception")
#
#        raise := method(e,
#            if(e == nil or exception isKindOf(e),
#                AssertionFailure raise("Unexpected '" .. exception type .. "' exception.", exception)
#            )
#        )
#    )
#)

AssertionFailure := Exception clone

assertFail := method(AssertionFailure raise("fail"))

assertCompare := method(valueA, valueB, msgA, msgB, condition,
    if(condition not,
        d := msgA code .. " != " .. msgB code
        AssertionFailure raise("[" .. d .. "] [" .. valueA asSimpleString .. " != " .. valueB asSimpleString .. "]")
    )
    true
)

assertEquals := method(a, b, m,
    m ifNil(m = call message)
    assertCompare(a, b, m argAt(0), call argAt(1), a == b)
)

assertNotEquals := method(a, b, if(a == b, AssertionFailure raise(a asSimpleString .. " == " .. b asSimpleString)))

assertSame    := method(a, b, assertEquals(a uniqueId, b uniqueId, call message))
assertNotSame := method(a, b, assertNotEquals(a uniqueId, b uniqueId, call message))
assertNil     := method(a, assertEquals(a, nil, call message))
assertNotNil  := method(a, assertNotEquals(a, nil, call message))
assertTrue    := method(a, assertEquals(a, true, call message))
assertFalse   := method(a, assertEquals(a, false, call message))
assertKindOf := method(a, b,
    assertCompare(a, b, call argAt(0), call argAt(1), a isKindOf(b))
)

Object addWordyMethods(
    should equal := method(value, assertEquals(value, self))
    should not equal := method(value, assertNotEquals(value, self))
    should be := method(value, assertSame(value, self))
    should not be := method(value, assertNotSame(value, self))
    should forward := method(
        name := call message name
        if(self hasSlot(name), assertTrue(call delegateTo(self)))
        # handle the case where the slots don't exist as a failure
        # handle is stripping.
    )
    should not forward := method(
        name := call message name
        if(self hasSlot(name), assertFalse(call delegateTo(self) not))
        assertFail
        # handle the case where the slots don't exist as a failure
        # handle is stripping.
    )
)

Number addWordyMethods(
    should equals := method(expected, assertEquals(expected, self))
    should not equals := method(expected, assertNotEquals(expected, self))
    should be := method(expected, assertEquals(expected, self))
    should not be := method(expected, assertNotEquals(expected, self))
    should be close := method(expected, tolerance,
        tolerance ifNil(tolerance = 0.0001)
        assertCompare(expected, actual, call argAt(0), message(actual), (expected - actual) abs <= tolerance)
    )
    should not be close := method(expected, tolerance,
        tolerance ifNil(tolerance = 0.0001)
        assertCompare(expected, actual, call argAt(0), message(actual), (expected - actual) abs > tolerance)
    )
)

nil addWordyMethods(
    should raise := method(e,
        AssertionFailure raise("Expected a '" .. e type .. "' exception.")
    )

    should not raise := nil

    should be := method(expected, assertEquals(expected, self))
    should not be := method(expected, assertNotEquals(expected, self))
)

Exception addWordyMethods(
    should raise := method(e,
        if(self isKindOf(e) not,
            AssertionFailure raise("Unexpected exception '" .. self type .. "' instead of '" .. e type .. "'.", self)
        )
    )

    should not raise := method(
        if(e == nil or self isKindOf(e),
            AssertionFailure raise("Unexpected '" .. self type .. "' exception.", self)
        )
    )
)

List addWordyMethods(
    should contain := method(value, assertTrue(contains(value)))
    should not contain := method(value, assertFalse(contains(value)))
    should have := method(expectedSize, assertEquals(expectedSize, size); Object clone do (things := nil))
    should not have := method(expectedSize, assertNotEquals(expectedSize, size); Object clone do (things := nil))
    should have at least := method(minSize, assertTrue(size >= minSize); Object clone do (things := nil))
    should have at most := method(maxSize, assertTrue(size <= maxSize); Object clone do (things := nil))
)
