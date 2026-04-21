# UnitTest

An object for organizing and running unit tests validated by assertions.

- **category**: Testing

## assertEquals(a,

b) Fail the running test if a != b.

## assertEqualsWithinDelta(expected, actual, delta)

Fail the running test if the expected value is not within delta of the actual value.

## assertFalse(a)

Fail the running test if a != false.

## assertNil(a)

Fail the running test if a != nil.

## assertNotEquals(a,

b) Fail the running test if a == b.

## assertNotNil(a)

Fail the running test if a == nil.

## assertNotSame(a,

b) Fail the running test if a != b.

## assertRaisesException(code)

Fail the running test if the code does not raise an Exception.

## assertTrue(a)

Fail the running test if a != true.

## fail(error)

Call to trigger a test failure with a given error message.

## setUp

Method called prior to each test.

## tearDown

Method called after each test.

