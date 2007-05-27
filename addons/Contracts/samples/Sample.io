#!/usr/bin/env io

/* Example usages of Contracts */
Contracts

/* This method only accepts numbers */
onlyNumbers := contract(a, b,
	a + b
) setPreCondition(a isKindOf(Number) and b isKindOf(Number))

onlyNumbers(1, 2)
onlyNumbers("foo", "bar")

/* This method must return 5 */
five := contract(a, b,
	a + b
) setPostCondition(result == 5)

five(2, 3)
five(5, 0)
five(1, 2)

/* This method uses both pre-conditions and post-conditions */
ensureNumbersAndFive := contract(a, b,
	a + b
) setPreCondition(
	a isKindOf(Number) and b isKindOf(Number)
) setPostCondition(
	result == 5
)
