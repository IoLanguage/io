ODESimpleSpaceTest := UnitTest clone do(

	setUp := method(
		self space := ODESimpleSpace clone
	)

	testSimpleSpaceIdIsNot0 := method(
		assertNotEquals(space spaceId, 0)
	)

	testEmptyCollide := method(
		o := Object clone
		o nearCallback := method(assertFail)
		space collide(o, message(nearCallback))
	)
) 
