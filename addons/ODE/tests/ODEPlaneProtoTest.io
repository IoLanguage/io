ODEPlaneProtoTest := UnitTest clone do(

	testPlaneId := method(
		assertEquals(ODEPlane geomId, 0)
	)

	testGetParams := method(
		assertRaisesException(plane params)
	)
) 
