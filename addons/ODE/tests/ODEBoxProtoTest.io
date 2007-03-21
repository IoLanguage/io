ODEBoxProtoTest := UnitTest clone do(
	
	testBoxId := method(
		assertEquals(ODEBox geomId, 0)
	)

	testLengths := method(
		assertRaisesException(ODEBox lengths)
	)

	testSetLengths := method(
		assertRaisesException(ODEBox setLengths(1,2,3))
	)

	testPointDepth := method(
		assertRaisesException(ODEBox pointDepth(0,0,0))
	)

	testBody := method(
		assertRaisesException(ODEBox body)
	)
)
