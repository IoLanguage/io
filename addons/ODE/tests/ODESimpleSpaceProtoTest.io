ODESimpleSpaceProtoTest := UnitTest clone do(

	testODESimpleSpaceIdIs0 := method(
		assertEquals(ODESimpleSpace spaceId, 0)
	)
	
	testPlaneWorksOnProto := method(
		assertNotEquals(ODESimpleSpace plane(1, 2, 3, 4) geomId, 0)
	)

	testBoxWorksOnProto := method(
		assertNotEquals(ODESimpleSpace box(1, 2, 3) geomId, 0)
	)

	testCollide := method(
		o := Object clone
		o nearCallback := method(assertFail)
		assertRaisesException(ODESimpleSpace collide(o, message(nearCallback)))
	)
) 
