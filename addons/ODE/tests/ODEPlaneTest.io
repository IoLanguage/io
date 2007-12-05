ODEPlaneTest := UnitTest clone do(
	setUp := method(
		self space := ODESimpleSpace clone
		self plane := space plane(0, 1, 0, 0)
	)

	testPlaneId := method(
		assertNotEquals(plane geomId, 0)
	)

	testGetParams := method(
		assertEquals(plane params, vector(0, 1, 0, 0))
	)

	testSetParams := method(
		plane setParams(0, 0, 1, 0)
		assertEquals(plane params, vector(0, 0, 1, 0))
	)
) 
