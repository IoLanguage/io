IfTest := UnitTest clone do(

	/*
	Object if := method(condition,
		index = if(condition, 1, 2)
		if(call message arguments size > index,
			call sender doMessage(call message argAt(index), call sender)
		,
			if(condition, true, false)
		)
	)
	*/

	testIfEval := method(
		v := false
		if(v := true)
		assertTrue(v)
	)

	testTrue := method(
		assertTrue(if(true))
	)

	testTrueExpression := method(
		assertEquals(if(true, 1), 1)
	)

	testTrue2Expressions := method(
		assertEquals(if(true, 1, 2), 1)
	)

	testFalse := method(
		assertFalse(if(false))
	)

	testFalseExpression := method(
		assertFalse(if(false, 1))
	)

	testFalse2Expressions := method(
		assertEquals(if(false, 1, 2), 2)
	)

	testNil := method(
		assertFalse(if)
		assertFalse(if())
		assertFalse(if(nil))
	)

	testOtherTrue := method(
		assertTrue(if(Object))
		assertTrue(if(13))
		assertTrue(if("foo"))
	)
)
