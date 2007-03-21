ODEBoxTest := UnitTest clone do(
	setUp := method(
		self space := ODESimpleSpace clone
		self box := space box(1, 2, 3)
	)

	testGeomId := method(
		assertNotEquals(box geomId, 0)
	)

	testLengths := method(
		assertEquals(box lengths, vector(1,2,3))
	)

	testSetLengths := method(
		box setLengths(4,5,6)
		assertEquals(box lengths, vector(4,5,6))
	)

	testPointDepth := method(
		assertEquals(box pointDepth(0,0,0), 0.5)
	)

	testNoBody := method(
		assertNil(box body)
	)
)
