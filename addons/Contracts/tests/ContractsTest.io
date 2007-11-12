Contracts

ContractsTest := UnitTest clone do(
	testPreCondition := method(
		c := contract(a, a * a) setPreCondition(a isKindOf(Number))
		v := nil
		try(v = c(5))
		assertEquals(v, 25)
	)

	testPostCondition := method(
		c := contract(a, a * a) setPostCondition(result == 25)
		v := nil
		try(v = c(5))
		assertEquals(v, 25)
	)

	testMethod := method(
		c := contract(a, a * a)
		v := c(5)
		assertEquals(v, 25)
	)
)
